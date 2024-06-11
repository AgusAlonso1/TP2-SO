#include <commands.h>

void man(){
    printf("Welcome to MATV\n");
    printf("NAME\n");
    printf("\t help - the manual to the system reference\n\n");
    printf("DESCRIPTION\n");
    printf("\t This is an assignment for a Computer Architecture Course at ITBA.\n");
    printf("\t Created by Agustin Alonso, Magdalena Cullen, Valentina Marti Reta , and Tomas Becerra.\n\n");
    printf("PROGRAMS AVAILABLE FOR USER\n");
    printf("\t -- Do not include space after command. --\n");
    printf("\t help - Access the manual.\n");
    printf("\t time - Retrieve the current time.\n");
    printf("\t snake - Play the Snake game, available for both single-player and two-player modes. Follow the command with the number \n");
    printf("\t\t of players you want to play with.\n");
    printf("\t clear - Clear the screen.\n");
    printf("\t zoomin - Increase text size. Clears screen after change.\n");
    printf("\t zoomout - Decrease text size. Clears screen after change.\n");
    printf("\t registers - View the registers in use after pressing Shift + Alt.\n");
    printf("\t settheme - Choose terminal theme. Clears screen after change.\n");
    printf("\t mem - Prints the status of the memory.\n");
    printf("\t loop - Prints its ID with a greeting every specified number of seconds. Argument is the number of seconds.\n");
    printf("\t ps - Prints a list of all processes with their properties: name, ID, priority, stack and base pointer, foreground status, and any other relevant variables.\n");
    printf("\t kill - Terminates a process given its ID. Argument is the ID.\n");
    printf("\t nice - Changes the priority of a process given its ID and the new priority. Arguments are ID then new priority.\n");
    printf("\t block - Toggles the state of a process between blocked and ready given its ID. Arguments is the ID.\n");
    printf("\t cat - Prints the stdin exactly as it receives it.\n");
    printf("\t wc - Counts the number of characters, words and lines in the input.\n");
    printf("\t filter - Filters out vowels from the input.\n");
    printf("\t testmm - Runs the memory manager test. Argument is the quantity of MB.\n");
    printf("\t testprocesses - Runs the processes test. Argument is the maximum amount of processes \n");
    printf("\t testprio - Runs the priority test.\n");
    printf("\t testsync - Runs the synchronized test. Arguments are the number of increments, the number of processes, and whether to use semaphores or not.\n");
    printf("\t phylo - Simulates the philosophers problem.\n");
    printf("\t EstoesBoca - BOCA.\n\n");
    printf("EXCEPTIONS\n");
    printf("\t inval op - Indicates an invalid operation.\n");
    printf("\t div0 - Signals division by zero.\n");
}



