/**
*
*   This file contains all necessary definitions, and LINUX SYSTEM V
*   semaphore implementations to run the H2O production problem.
*
**/

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <time.h>

//Semaphore Defintion
#define MUTEX       0
#define O           1
#define H           2
#define B           3
#define NUM_SEMS    4

//Error Defintions
#define FORK_FAILURE 1
#define SEMAPHORE_FAILURE 2
#define MEMORY_FAILURE 3
#define MAP_FAILURE 4
#define WAIT_FAILURE 5
#define SIGNAL_FAILURE 6


//Shared Memory Values
typedef struct {
    int HCount;
    int OCount;
    int BCount;
} shared_mem;

//Wait semaphore implementation
void P(int semid, int semaphore){
    struct sembuf psembuf;
    psembuf.sem_op = -1;
    psembuf.sem_flg = 0;
    psembuf.sem_num = semaphore;
    if(semop(semid, &psembuf, 1) < 0) {
        perror("Could not wait semaphore");
        exit(WAIT_FAILURE);
    }    
    else {
        return;
    }    
}

//Signal semaphore implementation
void V(int semid, int semaphore){
    struct sembuf psembuf;
    psembuf.sem_op = 1;
    psembuf.sem_flg = 0;
    psembuf.sem_num = semaphore;
    if(semop(semid, &psembuf, 1) < 0) {
        perror("Could not signal sempahore");
        exit(SIGNAL_FAILURE);
    }    
    else {
        return;
    }
}
