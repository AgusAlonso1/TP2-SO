#include <stdio.h>
#include <stdint.h>
#include <libc.h>
#include <syscallFunctions.h>
#include <libString.h>

#define MAX_PHILOSOPHERS 20
#define MIN_PHILOSOPHERS 5
#define MUTEX_ID 75
#define THINKING  0
#define EATIING  1
#define HUNGRY  2
#define CONTINUE 1
#define FINISH 0

typedef struct {
    int state;
    uint64_t leftFork;
    uint64_t rightFork;
} Philosopher;

static Philosopher philosophers[MAX_PHILOSOPHERS];
static int num_philosophers; 
static int philosophersPids[MAX_PHILOSOPHERS];
static uint64_t mutex;

void displayState() {
    for (int i = 0; i < num_philosophers; i++) {
        if (philosophers[i].state == EATIING) { 
            printChar('E');
        } else {
            printChar('.');
        }
     //   printChar(' ');
    }
    printChar('\n');
}

void think(int i) {
    call_sleep(1500);
}

void eat(int i) {
    call_sleep(1500);
}

void takeForks(int i) {
    call_sem_wait(philosophers[i].leftFork);
    call_sem_wait(philosophers[i].rightFork);
    philosophers[i].state = EATIING;
}

void putForks(int i) {
    philosophers[i].state = THINKING; 
    call_sem_post(philosophers[i].leftFork);
    call_sem_post(philosophers[i].rightFork);
}

int philosopher(int argc, char ** argv) {
    int i = atoi(argv[0]);
    philosophersPids[i] = call_get_pid();
    while (1) {
        think(i);
        call_sem_wait(mutex);
        takeForks(i);
        call_sem_post(mutex);
        eat(i);
        call_sem_wait(mutex);
        putForks(i);
        call_sem_post(mutex);
        displayState();
    }
    return 0;
}

void addPhilosopher() {
    if (num_philosophers < MAX_PHILOSOPHERS) {
        int i = num_philosophers;
        philosophers[i].state = THINKING; 
        philosophers[i].leftFork = call_sem_open(1, (MUTEX_ID + i));
        philosophers[i].rightFork = call_sem_open(1, (MUTEX_ID + i + 1) % (num_philosophers + 1));
        char philoNumber[4];
        itoa(i, philoNumber, 10);
        char *argv[] = {philoNumber, NULL};
        num_philosophers++;
        int fileDescriptors[CANT_FILE_DESCRIPTORS] = {STDIN, STDOUT, STDERR};
        call_create_process_foreground("philosopher", &philosopher, argv, call_get_pid(), fileDescriptors);
    }
}

void removePhilosopher() {
    if (num_philosophers > 1) {
        num_philosophers--;
        call_sem_close(philosophers[num_philosophers].leftFork);
        call_sem_close(philosophers[num_philosophers].rightFork);
        call_kill_process(philosophersPids[num_philosophers]);
    }
}

int handleKeyboard(char key) {
    if (key == 'a') {
        addPhilosopher();
        return CONTINUE;
    } else if (key == 'r') {
        removePhilosopher();
        return CONTINUE;
    } else if (key ==  'e'){
        return FINISH;
    }
    return CONTINUE;
}

int execute() {
    num_philosophers = 0;
    mutex = call_sem_open(1, MUTEX_ID);
    for (int i = 0; i < MIN_PHILOSOPHERS; i++) {
        addPhilosopher();
    }

    int flag = 1;
    while (flag) {
        char key = getChar();
        flag = handleKeyboard(key);
    }

    while (num_philosophers!=0){
        removePhilosopher();    
    }
    call_sem_close(mutex);
    
    return 0;
}