#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N

int state[N];
int phil[N] = {0, 1, 2, 3, 4};
sem_t mutex, S[N];

void test(int phnum) {
    if (state[phnum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) 
    {
        state[phnum] = EATING;
        sleep(2);
        printf("Philosopher %d takes fork %d and %d\n", phnum + 1, LEFT + 1, phnum + 1);
        printf("Philosopher %d is Eating\n", phnum + 1);
        sem_post(&S[phnum]);
    }
}

void take_fork(int phnum) {
    sem_wait(&mutex);
    state[phnum] = HUNGRY;
    printf("Philosopher %d is Hungry\n", phnum + 1);
    test(phnum);
    sem_post(&mutex);
    sem_wait(&S[phnum]);
    sleep(1);
}

void put_fork(int phnum) {
    sem_wait(&mutex);
    state[phnum] = THINKING;
    printf("Philosopher %d putting fork %d and %d down\n", phnum + 1, LEFT + 1, phnum + 1);
    printf("Philosopher %d is thinking\n", phnum + 1);
    test(LEFT);
    test(RIGHT);
    sem_post(&mutex);
    sleep(1);
}

void* philosopher(void* num) {
    while (1) {
        int* i = num;
        sleep(1);
        take_fork(*i);
        sleep(0);
        put_fork(*i);
    }
}

int main() {
    pthread_t thread_id[N];
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < N; i++)
        sem_init(&S[i], 0, 0);

    for (int i = 0; i < N; i++) {
        pthread_create(&thread_id[i], NULL, philosopher, &phil[i]);
        printf("Philosopher %d is thinking\n", i + 1);
    }

    for (int i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);

    return 0;
}

This code solves the Dining Philosophers Problem using semaphores and threads. The problem involves a number of philosophers sitting around a table, thinking and eating, and they must share a limited number of forks. The solution needs to ensure that no philosopher starves, and there is no deadlock (i.e., philosophers should not wait forever for forks).

Key Concepts Used:

Semaphore:
Semaphores are used for synchronization. There are two semaphores:
mutex: A semaphore that controls access to the shared state (ensuring that only one philosopher can change their state at a time).
S[]: An array of semaphores, one for each philosopher. This is used to block and unblock philosophers based on whether they can start eating.
Threads:
There is one thread for each philosopher. Each philosopher runs in their own thread and alternates between thinking, getting hungry, eating, and putting the forks down.
Critical Section:
The critical section is the part of the code where philosophers change their state (from thinking to hungry or from hungry to eating). This must be synchronized to prevent race conditions where multiple philosophers change their state at the same time, which could lead to inconsistencies.
Code Breakdown:
Global Variables:
state[]: An array to track the state of each philosopher. The possible states are:
HUNGRY (1): The philosopher wants to eat but hasn't started yet.
EATING (0): The philosopher is currently eating.
THINKING (2): The philosopher is thinking.
phil[]: An array that holds the philosopher numbers, used to identify them in the threads.
mutex: A semaphore that ensures mutual exclusion when modifying the philosopher's state.
S[]: An array of semaphores for each philosopher. These are used to block or unblock philosophers based on whether they can start eating.
Functions:
test(int phnum):
This function is used to check if a philosopher (given by phnum) can start eating. A philosopher can start eating if they are hungry and neither of their neighbors is eating.
If both conditions are met, the philosopher’s state is changed to EATING, and the philosopher is allowed to start eating (indicated by a sem_post on S[phnum]).
take_fork(int phnum):
This function is called by a philosopher when they want to take forks to eat.
The philosopher first locks the mutex to ensure no other philosopher changes their state while they are doing so.
The philosopher's state is set to HUNGRY, and they attempt to eat by calling test(phnum). If the philosopher is allowed to eat, the test function will unblock them.
The philosopher waits for their corresponding semaphore (sem_wait(&S[phnum])) before starting to eat.
The mutex is unlocked after this operation.
put_fork(int phnum):
This function is called by a philosopher when they finish eating and put the forks back down.
The philosopher locks the mutex again to modify the shared state.
The philosopher’s state is set to THINKING, and they notify their neighbors by calling test(LEFT) and test(RIGHT) to check if they can start eating.
The philosopher then unlocks the mutex.
philosopher(void* num):
This is the main function for each philosopher's thread.
The philosopher repeatedly alternates between thinking, becoming hungry, eating, and putting the forks down by calling take_fork() and put_fork() in an infinite loop.
The sleep functions simulate thinking and eating times.
Main Function (main):
Initialize semaphores:
The mutex semaphore is initialized to 1, meaning it’s initially unlocked, allowing philosophers to access the shared state.
Each semaphore in S[] is initialized to 0, which means each philosopher is initially blocked and cannot start eating.
Create philosopher threads:
A thread is created for each philosopher using pthread_create. Each philosopher runs the philosopher() function.
Each philosopher is initially in the thinking state, as indicated by the print statement.
Wait for threads to finish:
The pthread_join function ensures the main program waits for all philosopher threads to finish before exiting (though in this case, the threads run indefinitely).
Philosophers' Thinking and Eating:
Philosophers alternate between thinking and eating.
They pick up forks when both the forks they need are available (i.e., the neighbors are not eating).
Philosophers cannot eat if the forks are not available or if their neighbors are eating.
When done eating, philosophers put the forks down and resume thinking.
Theory and OS Concepts:
Concurrency:
Philosophers are executing concurrently. Each philosopher runs in their own thread, and the operating system manages the scheduling of these threads. This means multiple philosophers can attempt to pick up forks or eat at the same time, and proper synchronization is needed to prevent conflicts.
Synchronization:
Semaphores are used to synchronize the philosophers' actions, ensuring that only one philosopher can modify the state[] array at a time (using the mutex semaphore). This prevents race conditions where two philosophers might simultaneously try to pick up forks or change their state.
The semaphores in S[] ensure that a philosopher waits until both forks are available. They are blocked (via sem_wait) until they can proceed with eating.
Condition Synchronization: The test function implements condition synchronization. A philosopher checks if they can eat (if their neighbors aren't eating). If they can, they proceed, else they wait.
Deadlock Prevention:
The main issue in the Dining Philosophers Problem is deadlock, where all philosophers could end up waiting for forks, leading to a situation where no one can eat.
In this solution, deadlock is avoided by ensuring that a philosopher only starts eating if both adjacent philosophers are not eating. This prevents the scenario where all philosophers simultaneously wait for a fork, as at least one philosopher will always be able to proceed with eating.
Starvation:
Starvation occurs when a philosopher never gets a chance to eat because the forks are constantly being taken by others.
In this solution, starvation is avoided because philosophers are allowed to eat as long as both of their neighbors are not eating, and the semaphores ensure that philosophers take turns fairly.
Critical Section:
The shared resource in this problem is the state[] array, which tracks the state of each philosopher. The critical section is the part where philosophers modify their state (e.g., from HUNGRY to EATING or THINKING). The mutex ensures that only one philosopher can modify their state at any given time, preventing race conditions.
Summary:
This code implements a solution to the Dining Philosophers Problem using semaphores for synchronization and threads for concurrent execution. The semaphores ensure that philosophers can only eat when both forks are available, and they are synchronized to prevent race conditions, deadlock, and starvation. The program models the thinking, hunger, eating, and thinking cycle of philosophers, demonstrating key OS concepts like concurrency, synchronization, and mutual exclusion
