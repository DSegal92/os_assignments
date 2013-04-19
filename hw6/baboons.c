#include "hw6.h"

/*
*   Prints thread information formatted to make tracing easy.
*   Parameters:
*       id: thread id
*       action: Arriving/ Waiting/ Crossing/ Sucess(ing)
*       status: Either side arriving on, sides crossing, or finished  
*/
void print_info(void* id, char* action, char* status)
{   
    char timeBuf  [256]; 
        struct timeval tv;
        struct timezone tz;
        struct tm *tm;
    gettimeofday(&tv, &tz);
    tm=localtime(&tv.tv_sec);
    sprintf (timeBuf, "%02d:%02d:%02d:%03ld",tm->tm_hour, tm->tm_min, tm->tm_sec, (tv.tv_usec/1000) );

    int* val = (int*)id;

    //This is just for my own satisfaction to prevent formatting issues
    if (XedCount < 10){
        printf("  %3d   |  %s  |  %s  |  %s  |      %d      |     %d \n", *val, action, timeBuf, status, XedCount, XingCount);
        fflush(stdout);
    }   
    else{
        printf("  %3d   |  %s  |  %s  |  %s  |      %d     |     %d \n", *val, action, timeBuf, status, XedCount, XingCount);
         fflush(stdout);
    }

}

/*
*   Based on solution given in homework 3
*   Controls baboons crossing from side A to B
*/
void* BaboonAToB(void* pthread_id){
    print_info(pthread_id, "Arrival ", "New Baboon on Side A");
    sleep(rand()%3);
    print_info(pthread_id, "Waiting ", "        -           ");
    sem_wait(&mutex);
    if ((XingDirection == AToB || XingDirection == None) && (XingCount < 5) && ((XedCount + XingCount) < 10)){
        XingDirection = AToB;
        XingCount++;
        sem_post(&mutex);
    }
    else {
        ToBWaitCount++;
        sem_post(&mutex);
        sem_wait(&ToB);
        ToBWaitCount--;
        XingCount++;
        XingDirection = AToB;
        sem_post(&mutex);
    }
    print_info(pthread_id, "Crossing", "Crossing from A -> B");
    sleep(rand()%3);
    print_info(pthread_id, "Success ", " Finished crossing  ");

    XedCount++;
    XingCount--;
    if (ToBWaitCount != 0 && (((XedCount + XingCount) < 10) || ((XedCount + XingCount) >= 10 && ToAWaitCount == 0))){
        sem_post(&ToB);
    }
    else if (XingCount == 0 && ToAWaitCount != 0 && (ToBWaitCount == 0 || (XedCount + XingCount) >= 10)){
        XingDirection = BToA;
        XedCount = 0;
        sem_post(&ToA);
    }
    else if (XingCount == 0 && ToBWaitCount == 0 && ToAWaitCount == 0){
        XingDirection = None;
        XedCount = 0;
        sem_post(&mutex);
    }
    else {
        sem_post(&mutex);
    }
    
    return NULL;
}

/*
*   Based on solution given in homework 3
*   Controls baboons crossing from side B to A
*/
void* BaboonBToA(void* pthread_id){
    print_info(pthread_id, "Arrival ", "New Baboon on Side B");
    sleep(rand()%3);
    print_info(pthread_id, "Waiting ", "        -           ");
    sem_wait(&mutex);
    if ((XingDirection == BToA || XingDirection == None) && (XingCount < 5) && ((XedCount + XingCount) < 10)){
        XingDirection = BToA;
        XingCount++;
        sem_post(&mutex);
    }
    else {
        ToAWaitCount++;
        sem_post(&mutex);
        sem_wait(&ToA);
        ToAWaitCount--;
        XingCount++;
        XingDirection = BToA;
        sem_post(&mutex);
    }
    print_info(pthread_id, "Crossing", "Crossing from B -> A");
    sleep(rand()%3);
    print_info(pthread_id, "Success ", " Finished crossing  ");

    XedCount++;
    XingCount--;
    if (ToAWaitCount != 0 && (((XedCount + XingCount) < 10) || ((XedCount + XingCount) >= 10 && ToBWaitCount == 0))){
        sem_post(&ToA);
    }
    else if (XingCount == 0 && ToBWaitCount != 0 && (ToAWaitCount == 0 || (XedCount + XingCount) >= 10)){
        XingDirection = AToB;
        XedCount = 0;
        sem_post(&ToB);
    }
    else if (XingCount == 0 && ToAWaitCount == 0 && ToBWaitCount == 0){
        XingDirection = None;
        XedCount = 0;
        sem_post(&mutex);
    }
    else {
        sem_post(&mutex);
    }
    
    return NULL;
}

int main(int argc, char** argv){
    printf("\n ** NOTE: This printout is only a visual guide, due to the discrepancy \n between printing to output and actual computation this may not be accurate ** \n");
    printf("\nTHREAD  |  Action    |  Current Time  |    \tStatus \t       |   Crossed   | Pending \n");
    printf("_____________________________________________________________________________________ \n");
    srandom(time(NULL));

    //Creates random number of baboons [18, 23] on both sides of passing
    int sideA = 18 + rand() % 5;
    int sideB = 18 + rand() % 5;
    int number_of_baboons = sideA + sideB;
    int i;


    pthread_t sideA_baboons[sideA];
    pthread_t sideB_baboons[sideB];

    pthread_attr_t attr;
   
    baboons = (int*)calloc(number_of_baboons, 4);  

     pthread_attr_init(&attr);


    sem_init(&mutex, 0, 1);
    sem_init(&ToA, 0, 0);
    sem_init(&ToB, 0, 0);

    for(i = 0; i < sideA; i++) {
       
	    baboons[i] = i;
	    pthread_create(&sideA_baboons[i], &attr, BaboonAToB, (void*)&baboons[i]);
    }


    for(i = sideA; i < number_of_baboons; i++) {
        
	    baboons[i] = i;
	    pthread_create(&sideB_baboons[i], &attr, BaboonBToA, (void*)&baboons[i]);
    }

    //There is a memory leak somewhere around here, it runs fine on my laptop but segfaults on lab computers. I tried. 
  for(i = 0; i < sideA; i++) {
   
        pthread_join(sideA_baboons[i], &status);
    }

    for(i = sideA; i < number_of_baboons; i++) {
     
        pthread_join(sideA_baboons[i], &status);

    }


    free(baboons);
    return 0;
}

