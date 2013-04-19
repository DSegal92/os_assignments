#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "Smoking.h"

#define EXIT_FAILURE 1
#define ARGUMENT_FAILURE 2
#define RPC_FAILURE 3

#define AVAILABLE 1
#define UNAVAILABLE 0

int tobacco, paper, matches;
CLIENT* clnt;
char *server;

/**
*     Attempt to get however many more of a material
*     the smoker wants. Returns unavailable if not
*     able to get material.
**/
int request(int* value, data* shared, CLIENT* clnt){
  int* result = get_me_my_supply_1(shared, clnt);
  // Should never have negative materials
  if(result < 0) {
    exit(RPC_FAILURE);
  }
  //Give smoker the materials
  if(*result) {
    *value += shared->amount;
    return AVAILABLE;
  } 
  else {
    return UNAVAILABLE;
  }
}

/**
*     Allocates memory of each of shared materials.
**/
data* setup(int type, int amount){
    data* shared = (data*) calloc(sizeof(data), 1);
    shared->type = type;
    shared->amount = amount;
    shared->smoker = getpid();
    return shared;
}

int main(int argc, char** argv){
  char* host;
  int p, t, m;    
  int resources_available = AVAILABLE;

  if(argc != 5) {
    printf("./smoker [hostname] [tobacco_requested] [paper_requested] [matches_requested] \n");
    exit(ARGUMENT_FAILURE);
  } 

  t = atoi(argv[2]);
  p = atoi(argv[3]);
  m = atoi(argv[4]);
  server = argv[1];

  if((clnt = clnt_create(server, SMOKE, SMOKERS, "udp") ) == (CLIENT *) NULL ) {
    clnt_pcreateerror( server );
    exit( 2 );
  }

  data* tobacco_shared = setup(TOBACCO, t);
  data* paper_shared = setup(PAPER, p);
  data* matches_shared = setup(MATCHES, m);


  //Keep looping using local materials, request new materials if smoker runs out
  while(resources_available == AVAILABLE) {
    printf("Resources Left: P=%d, T=%d, M=%d\n", tobacco, paper, matches);
    if(paper == 0) {
      printf("REQUESTING %d PAPER \n", p);
      //Returns false if unable to obtain paper
      if(!request(&paper, paper_shared, clnt)) {
		    printf("PAPER GONE, NOW I WILL KILL MYSELF \n");
		    resources_available = UNAVAILABLE;
	    }
    } 
    else if(tobacco == 0) {
      printf("REQUESTING %d TOBACCO \n", t);
      //Returns false if unable to obtain tobacco
	    if(!request(&tobacco, tobacco_shared, clnt)) {
		    printf("TOBACCO GONE, NOW I WILL KILL MYSELF \n");
		    resources_available = UNAVAILABLE;
	    }
    } 
    else if(matches == 0) {
      printf("REQUESTING %d MATCHES \n", m);
      //Returns false if unable to obtain matches
	    if(!request(&matches, matches_shared, clnt)) {
		    printf("MATCHES GONE, NOW I WILL KILL MYSELF \n");
		    resources_available = UNAVAILABLE;
	    }
    } 
    else {
      printf("-------- Smoking! -------- \n");
        paper = paper - 1;
        tobacco = tobacco - 1;
        matches = matches - 1;        
        sleep(1);
	   }
    }

    free(tobacco_shared);
    free(paper_shared);
    free(matches_shared);
    return 0;

}
