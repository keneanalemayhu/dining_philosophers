#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

enum { THINKING, HUNGRY, EATING } state[N];
pthread_mutex_t mutex;
pthread_cond_t cond[N];

void test(int i) {
    if (state[i] == HUNGRY &&
        state[(i + 4) % N] != EATING &&
        state[(i + 1) % N] != EATING) {
        state[i] = EATING;
        pthread_cond_signal(&cond[i]);
    }
}

void pickup_forks(int i) {
    pthread_mutex_lock(&mutex);
    state[i] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", i);
    test(i);
    while (state[i] != EATING)
        pthread_cond_wait(&cond[i], &mutex);
    printf("Philosopher %d takes forks and starts EATING\n", i);
    pthread_mutex_unlock(&mutex);
}

void return_forks(int i) {
    pthread_mutex_lock(&mutex);
    state[i] = THINKING;
    printf("Philosopher %d puts down forks and starts THINKING\n", i);
    test((i + 4) % N);
    test((i + 1) % N);
    pthread_mutex_unlock(&mutex);
}

void* philosopher(void* num) {
    int i = *(int*)num;
    while (1) {
        printf("Philosopher %d is THINKING\n", i);
        sleep(rand() % 3 + 1);
        pickup_forks(i);
        sleep(rand() % 3 + 1);
        return_forks(i);
    }
}

int main() {
    pthread_t threads[N];
    int ids[N];

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < N; i++)
        pthread_cond_init(&cond[i], NULL);

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < N; i++)
        pthread_cond_destroy(&cond[i]);

    return 0;
}
