#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define A 1
#define B 2

sem_t mutex, ToB, ToA;

typedef enum {None, AToB, BToA} dir_t;
dir_t XingDirection = None;
int XingCount = 0;
int XedCount = 0;

int ToAWaitCount = 0;
int ToBWaitCount = 0;
int* baboons;
void* status;


