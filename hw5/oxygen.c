/**
*
*	Models the oxygen process in the H2O production problem.
*	Based on pseudocode found in Assignment 3.
*
*
**/


#include "hw5.h"

int main(int argc, char** argv){
	//Either creates a new semaphore if one does not exist (which should not happen) or retrieves the semaphore key
	int semid = semget(ftok(__FILE__, 1), NUM_SEMS, IPC_CREAT | 0666);
    	if(semid < 0) {
    		perror("Couldn't create semaphore");
    	exit(SEMAPHORE_FAILURE);
    }
    //Retrieves key for accessing shared memory	
    int shm_key = shmget(ftok(__FILE__, 1), sizeof(shared_mem), IPC_CREAT | 0666);
    	if(shm_key < 0) {
    		perror("Couldn't get a memory key");
    	exit(MEMORY_FAILURE);
    }
    //Uses above key to map memory to an accessible address
    void* addr = shmat(shm_key, NULL, 0);
    	if((int*)addr < 0) {
    		perror("Couldn't map memory to address");
    	exit(MAP_FAILURE);
    }

    shared_mem* shared = (shared_mem*)addr;

    P(semid, MUTEX);
    if(shared->HCount >= 2) {
		shared->BCount = 2;
		V(semid, H);
		V(semid, H);	
    } 
    else {
		shared->OCount++;
		V(semid, MUTEX);
		P(semid, O);
		P(semid, B);
		shared->OCount = shared->OCount - 1;
		shared->BCount = shared->BCount -1;
		if(shared->BCount != 0) {
	    	V(semid, B);
		} 
		else {
	    	V(semid, B);
	    	V(semid, MUTEX);
		}
    }


    return 0;
}
