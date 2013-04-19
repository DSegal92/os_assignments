#include <rpc/rpc.h>
#include <stdio.h>
#include "Smoking.h"

#define EXIT_FAILURE 1

static int paper = 10;
static int matches = 10;
static int tobacco = 10;

static int result;
 
int *get_me_my_supply_1_svc(data* shared, struct svc_req *req){
  int amount = shared->amount;
  int* value;

  //Determine what material smoker is requesting
  if (shared->type == TOBACCO){
      value = &tobacco;
  }
  else if (shared->type == PAPER){
      value = &paper;
  }
  else {
      value = &matches;
  }

  //Decrement agent's material accordingly if available
  if(*value >= shared->amount) {
    result = *value - shared->amount;
    *value = *value - shared->amount;
  } 
  else {
    result = -1;  
  }
  
  return &result;
}

