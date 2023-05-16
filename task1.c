#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>

const int N = 5;

const int THINKING = 0;
const int HUNGRY = 1;
const int EATING = 2;

sem_t s[N];
int state[N];


pthread_mutex_t lock;

pthread_mutex_t fork[N];

void print_states() {
	int i;
	for (int i=0; i<N; i++) {
		if (state[i] == THINKING) printf("THINKING ");
		else if (state[i] == HUNGRY) printf("HUNGRY ");
		else printf("EATING ");
	}
	printf("\n");
}

void get_fork(int fid) {
	pthread_mutex_lock(&fork[fid]);
}

void put_fork(int fid) {
	pthread_mutex_unlock(&fork[fid]);	
}

void test(int id) {
	int left = (id - 1 + N) % N;
	int right = (id + 1) % N;
	if (state[left] != EATING && state[right] != EATING && state[id] == HUNGRY) {
		state[id] = EATING;
		print_states();
		sem_post(&s[id]);
	}
}

void get_forks(int id) {
	pthread_mutex_lock(&lock);
	state[id] = HUNGRY;
	print_states();
	test(id);
	pthread_mutex_unlock(&lock);
	sem_wait(&s[id]);
}

void put_forks(int id) {
	int left = (id - 1 + N) % N;
	int right = (id + 1) % N;
	pthread_mutex_lock(&lock);
	state[id] = THINKING;
	print_states();
	test(left);
	test(right);
	pthread_mutex_unlock(&lock);
}

void *life(void *tid) {
	int myid = *(int *)tid;

	struct timespec t;

	while (true) {
		
		// Thinking phase
		printf("Philosopher %d: I'm thinking!\n", myid);

		t.tv_sec = 0;
		t.tv_nsec = (300 + rand() % 500) * 10;
		nanosleep(&t, NULL);

		get_forks(myid);

		get_fork(myid);
		get_fork((myid + 1) % N);
		
		// Eating phase
		printf("Philosopher %d: I'm eating!\n", myid);

		t.tv_sec = 0;
		t.tv_nsec = (300 + rand() % 500) * 10;
		nanosleep(&t, NULL);

		put_fork(myid);
		put_fork((myid + 1) % N);
		
		put_forks(myid);

	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t thread[N];	
	int ids[N];

	srand(time(NULL));

	pthread_mutex_init(&lock, NULL);

	for (int i=0; i<N; i++) {
		pthread_mutex_init(&fork[i], NULL);
		sem_init(&s[i], 0, 0);
		state[i] = THINKING;
	}

	for (int i=0; i<N; i++) {
		ids[i] = i;
		pthread_create(&thread[i], NULL, life, &ids[i]);
	}

	for (int i=0; i<N; i++) {
		pthread_join(thread[i], NULL);
	}

	for (int i=0; i<N; i++) {
		pthread_mutex_destroy(&fork[i]);
	}
	pthread_mutex_destroy(&lock);

	return 0;
}
