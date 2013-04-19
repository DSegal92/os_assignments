#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define MAX_LENGTH 128

void print_agent_info(){
  //Find and assign hostname to variable
  char hostName[MAX_LENGTH];
  gethostname(hostName, sizeof(hostName));
  
  //Define and assign current time to variable
  time_t now;
  time(&now);

  printf("-----AGENT INFO------ \n");
  printf("HOSTNAME: \t %s \n", hostName);
  printf("PID: \t\t %d \n", getpid());
  printf("UID: \t\t %d \n", getuid());
  printf("TIME: \t\t %s \n", ctime(&now));

}

void update_quantities(int quantities[]){
   for (int i = 0; i < 3; i++){
      char temp[MAX_LENGTH];
      sprintf(temp, "%d", quantities[i]);
      if (i == 0){
       setenv("PAPER", temp, 1);
      }
      if (i == 1){
       setenv("TOBACCO", temp, 1);
      }
      if (i == 2){
       setenv("MATCHES", temp, 1);
      }     
  }
}

int check_quantity(char* material){
  return atoi(getenv(material));
}

int has_materials(){
  if (check_quantity("PAPER") != 0 && check_quantity("TOBACCO") != 0 && check_quantity("MATCHES") != 0){
    return (1);
  }
  else {
    return (0);
  }
}

void create_materials(int p, int t, int m){
  int quantities[] = {p,t,m};
  update_quantities(quantities);
 
  printf("-----MATERIALS------ \n");
  printf("PAPER:   \t %d \n", check_quantity("PAPER"));
  printf("TOBACCO: \t %d \n", check_quantity("TOBACCO"));
  printf("MATCHES: \t %d \n\n", check_quantity("MATCHES"));
}

void smoke_cycle(){
  pid_t s1, s2, s3;
  printf("FORKING THREE SMOKERS!\n");
  int count = 0;
  while (has_materials && count != 5){
    pid_t s1 = fork();
      count ++;
    

      
  }

}


int main(int argc, char **argv)
{
  print_agent_info();
  create_materials(1, 2, 3);
  smoke_cycle();
  /*

  createMaterials(2, 1, 3);
    smokeMaterials();

  createMaterials(1, 1, 1);
    smokeMaterials();
  */
  return(0);

}
