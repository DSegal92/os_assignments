/**
*
*	Models the hydrogen process in the H2O production problem.
*	Based on pseudocode found in Assignment 3.
*
*
**/


#include "hw5.h"

int main(int argc, char** argv){
	//See Oxygen.c for comments on lines 5-20
	int semid = semget(ftok(__FILE__, 1), NUM_SEMS, IPC_CREAT | 0666);
    	if(semid < 0) {
    		perror("Couldn't create semaphore");
    	exit(SEMAPHORE_FAILURE);
    }	
    int shm_key = shmget(ftok(__FILE__, 1), sizeof(shared_mem), IPC_CREAT | 0666);
    	if(shm_key < 0) {
    		perror("Couldn't get a memory key");
    	exit(MEMORY_FAILURE);
    }

    void* addr = shmat(shm_key, NULL, 0);
    	if((int*)addr < 0) {
    		perror("Couldn't map memory to address");
    	exit(MAP_FAILURE);
    }
    
    shared_mem* shared = (shared_mem*)addr;

    P(semid, MUTEX);
    if(shared->HCount >= 1 && shared->OCount >= 1) {
		shared->BCount = 2;
		V(semid, H);
		V(semid, O);	
		printf("----- Bonding! ----- \n"); fflush(stdout);
    } 
    else {
		shared->HCount = shared->HCount + 1;
		V(semid, MUTEX);
		P(semid, H);
		P(semid, B);
		shared->HCount = shared->HCount - 1;
		shared->BCount = shared->BCount - 1;
		if(shared->BCount != 0) {
	    	V(semid, MUTEX);
		} 
		else {
	    	V(semid, B);
	    	V(semid, MUTEX);
		}
    }
   

    
    return 0;
}
