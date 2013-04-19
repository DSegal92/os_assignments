/**
*
* Main method controlling dispatching of oxygen & hydrogen atoms.
* Attempts to create at least 5 bonds, but may fail depending on
* order of atoms created.
*
**/

#include "hw5.h"

//Atom creation is random 0 or 1, mapped to Oxygen and Hydrogen
#define OXYGEN 0
#define HYDROGEN 1

void initialize_atoms(shared_mem* shared){
    shared->OCount = 0;
    shared->HCount = 0;
    shared->BCount = 0;
}

void decide_path(pid_t pid, int decide){  
  if(pid < 0) {
    perror("Could not fork process");
    exit(FORK_FAILURE);
  }
  else if(pid == 0) {
    if (decide == HYDROGEN){
      printf("PID %d: Creating Hydrogen\n", getpid());
      execl("./hydrogen", "", (char*) NULL);
    }
    else if (decide == OXYGEN){
      printf("PID %d: Creating Oxygen\n", getpid());
      execl("./oxygen", "", (char*) NULL);
    }
  } 
  else {
    sleep(1);
  }
}

int main(int argc, char** argv){
  printf("Attempting to Create 5 Bonds \n"); fflush(stdout);
  int bonds = 5;
  int hydrogen_needed, oxygen_needed, next;
  
  //See Oxygen.c for comments on lines 39-53
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
  initialize_atoms(shared);
  
  //Set the initial values for semaphores
  semctl(MUTEX, semid, SETVAL, 1);
  semctl(O, semid, SETVAL, 0);
  semctl(H, semid, SETVAL, 0);
  semctl(B, semid, SETVAL, 1);

  hydrogen_needed = bonds*2;
  oxygen_needed = bonds;

  //In 15 atom creations, should theoretically bond 5 times
  for (int i = 0; i < 15; i++) {
    time_t now;
    srandom(time(&now));
    next = rand() / (RAND_MAX / 2 + 1);
    if(next == HYDROGEN) {
      hydrogen_needed = hydrogen_needed - 1;
      pid_t pid = fork();
      decide_path(pid, next);       
    } 
    else if(next == OXYGEN) {
      oxygen_needed = oxygen_needed - 1;
      pid_t pid = fork();
      decide_path(pid, next);       
      }            
    else {
      printf("No processes remaining\n");
    } 
  }

wait(NULL);
sleep(1);

printf("Removing unused atoms \n");

//Clear out memory utilized by program
semctl(semid, 0, IPC_RMID, NULL);
shmctl(semid, IPC_RMID, NULL);

return 0;
}
