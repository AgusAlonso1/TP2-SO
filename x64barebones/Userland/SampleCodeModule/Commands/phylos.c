// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>
#define MAX_PHILOSOPHERS 20
#define MIN_PHILOSOPHERS 5
#define THINKING  0
#define EATING  1
#define HUNGRY  2
#define CONTINUE 1
#define FINISH 0

typedef struct {
    int state;
    uint64_t leftFork;
    uint64_t rightFork;
} Philosopher;

Philosopher philosophers[MAX_PHILOSOPHERS];
int num_philosophers; 
int philosophersPids[MAX_PHILOSOPHERS];
int64_t mutex;
int64_t change_sem;


int canEat(int i) {
    int left = (i + num_philosophers - 1) % num_philosophers;
    int right = (i + 1) % num_philosophers;
    return philosophers[left].state != EATING && philosophers[right].state != EATING;
}

void displayState() {
    printf("\t\t\t\t");
    for (int i = 0; i < num_philosophers; i++) {
        if (philosophers[i].state == EATING) { 
            printChar('E');
        } else {
            printChar('.');
        }
        printChar(' ');
    }
    printf("\n");
}

void think(int i) {
    call_sleep_seconds(1);
}

void eat(int i) {
    call_sleep_seconds(2);
}

void takeForks(int i) {
    philosophers[i].state = HUNGRY;
    if (canEat(i)) {
        philosophers[i].state = EATING;
        call_sem_wait(philosophers[i].leftFork);
        call_sem_wait(philosophers[i].rightFork);
    }
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
        if (philosophers[i].state == EATING) {
            eat(i);
            call_sem_wait(mutex);
            putForks(i);
            call_sem_post(mutex);
        }
        displayState();
    }
    return 0;
}

void addPhilosopher() {
    call_sem_wait(change_sem);
    if (num_philosophers < MAX_PHILOSOPHERS) {
        int i = num_philosophers;
        int idForkLeft = call_get_new_sem_id();
        int idForkRight = call_get_new_sem_id();
        philosophers[i].state = THINKING; 
        philosophers[i].leftFork = call_sem_open(1, idForkLeft);
        philosophers[i].rightFork = call_sem_open(1, idForkRight);
        char philoNumber[4];
        itoa(i, philoNumber, 10);
        char *argv[] = {philoNumber, NULL};
        num_philosophers++;
        int fileDescriptors[CANT_FILE_DESCRIPTORS] = {DEV_NULL, STDOUT, STDERR};
        call_create_process_background("philosopher", &philosopher, argv, call_get_pid(), fileDescriptors);
    }
    call_sem_post(change_sem);
}

void removePhilosopher() {
    call_sem_wait(change_sem);
        num_philosophers--;
        call_sem_close(philosophers[num_philosophers].leftFork);
        call_sem_close(philosophers[num_philosophers].rightFork);
        call_kill_process(philosophersPids[num_philosophers]);

    call_sem_post(change_sem);
}

int handleKeyboard(char key) {
    if (key == 'a') {
        addPhilosopher();
        return CONTINUE;
    } else if (key == 'r') {
        if(num_philosophers > MIN_PHILOSOPHERS)
            removePhilosopher();
        return CONTINUE;
    } else if (key == 'e') {
        return FINISH;
    }
    return CONTINUE;
}

void displayHeader() {
    printf("\n\t\t\tInstructions:\n");
    printf("\t\t\tPress - a - Add philosopher\n");
    printf("\t\t\tPress - r - Remove philosopher\n");
    printf("\t\t\tPress - e - Exit\n");
    printf("\t\t\tState of philosophers:\n");
    printf("\t\t\t - E - Comiendo\n");
    printf("\t\t\t - . - Pensando\n\n\n");
}

int phylos(int argc, char ** argv) {
    num_philosophers = 0;
    mutex = call_get_new_sem_id();
    call_sem_open(1, mutex);
    change_sem = call_get_new_sem_id();
    call_sem_open(1, change_sem);

    displayHeader();  

    for (int i = 0; i < MIN_PHILOSOPHERS; i++) {
        addPhilosopher();
    }

    int flag = 1;
    while (flag) {
        char key = getChar();
        flag = handleKeyboard(key);
    }

    while (num_philosophers > 0) {
        removePhilosopher();    
    }
    call_sem_close(mutex);
    call_sem_close(change_sem);
    
    return 0;
}
