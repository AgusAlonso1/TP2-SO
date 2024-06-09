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
            putchar('E');
        } else {
            putchar('.');
        }
     //   putchar(' ');
    }
    putchar('\n');
}

void think(int i) {
    call_sleep(1500);
}

void eat(int i) {
    call_sleep(1500);
}

void takeForks(int i) {
    semWait(philosophers[i].leftFork);
    semWait(philosophers[i].rightFork);
    philosophers[i].state = EATIING;
}

void putForks(int i) {
    philosophers[i].state = THINKING; 
    semPost(philosophers[i].leftFork);
    semPost(philosophers[i].rightFork);
}

void philosopher(uint64_t argc, char ** argv) {
    int i = satoi(argv[0]);
    philosophersPids[i] = call_get_pid();
    while (1) {
        think(i);
        semWait(mutex);
        takeForks(i);
        semPost(mutex);
        eat(i);
        semWait(mutex);
        putForks(i);
        semPost(mutex);
        displayState();
    }
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
        semClose(philosophers[num_philosophers].leftFork);
        semClose(philosophers[num_philosophers].rightFork);
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
    mutex = semOpen(1, MUTEX_ID);
    for (int i = 0; i < MIN_PHILOSOPHERS; i++) {
        addPhilosopher();
    }

    int flag = 1;
    while (flag) {
        char key = getchar();
        flag = handleKeyboard(key);
    }

    while (num_philosophers!=0){
        removePhilosopher();    
    }
    
    return 0;
}