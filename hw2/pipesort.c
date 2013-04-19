#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Set a maximum size for string operations
#define BUFFER 256

//Things that could go wrong
#define PIPE_FAIL 1
#define EXEC_FAIL 2
#define FORK_FAIL 3

//Inner pipes between processes
#define PIPE_IN 0
#define PIPE_OUT 1
#define PIPE_ERR 2

//Pipe Ends
#define WRITE_END 1
#define READ_END 0


char* read_in(int pipe)
{
  //Set up for pipe reading
  char input[BUFFER];
  FILE *stream = fdopen(pipe, "r"); 
  int contentSize = 0;        
  char *content;    

  //While characters remain in the stream, concatenate them to the output
  while(fgets(input, BUFFER, stream)) {     
    contentSize += strlen(input); 
    content = malloc(contentSize);
    strncat(content, input, contentSize);    
  }

  return content;
}



void set_child_pipes(int *IN, int *OUT){
  //Hook children pipes up to correspond stdio pipes
  dup2(IN[READ_END], PIPE_IN);   
  dup2(OUT[WRITE_END], PIPE_OUT); 
  dup2(OUT[WRITE_END], PIPE_ERR); 
    
  //Information goes from child to parent
  close(IN[WRITE_END]); 
  close(OUT[READ_END]);
}

//Prevent wrong pipes from being read
void set_parent_pipes(int *IN, int *OUT){
  close(IN[READ_END]);  
  close(OUT[WRITE_END]); 
}

void create_subprocesses(int *IN, int *OUT)
{
  //Check that pipes were properly initialized
  if(pipe(IN)) {
      perror("Input pipe not created");
    exit(PIPE_FAIL);
  }
  if(pipe(OUT)) {
      perror("Output pipe not created");
    exit(PIPE_FAIL);
  }  

  //Check if process was properly forked
  pid_t process = fork();
  if(process < 0) {
    perror("Failed to fork");
    exit(FORK_FAIL);
  }

  //If process is child, set up properly then recursively call pipesort again
  if(process == 0) {
    set_child_pipes(IN, OUT);
    execl("./pipesort", "", (char*) NULL); 
    perror("REACHED UNREACHABLE POINT IN CODE");  //Process should have terminated by this point
    exit(EXEC_FAIL);
  } 
  //If process is parent, set up properly
  else {
    set_parent_pipes(IN, OUT);
    
  }
}



//Reads and writes necessary information from either right or left side of split substring
char* process_side(int *input, int *output, char *side){
  create_subprocesses(input, output);
    int side_size = strlen(side);
    write(input[WRITE_END], side, side_size); //Put child's information into the pipe and close
    close(input[WRITE_END]);
    char* content = read_in(output[READ_END]);  //Read new information into the pipe and close
    close(output[READ_END]);
  return content;
}

//Standard, run of the mill merge algorithm
void merge(int left_size, int right_size, char *left, char *right, char *result){  
  int l= 0, r= 0, counter= 0;

  int max_size = left_size + right_size;

  while(counter < max_size) {
    if (r >= right_size) {
      result[counter] = left[l];
      l++;
    }
    else if(l >= left_size) {
      result[counter] = right[r];
      r++;
    }  
    else if(left[l] < right[r]) {
      result[counter] = left[l];
      l++;
    } 
    else {
      result[counter] = right[r];
      r++;
    }
    counter++;
  }

  result[counter] = '\0';
}

//Break the current working string into two parts for Divide & Conquer
void partition(char* content, char* left, char* right)
{
  int mid = strlen(content)/2;

  strncpy(left, content, mid);
  strncpy(right, content + mid, strlen(content));
}

int main(int argc, char** argv){  
  //Initial variables
  char *input_string = read_in(PIPE_IN);
  int input_length = strlen(input_string);
  char left[BUFFER], right[BUFFER];

  if(input_length <= 1) {    
    printf("%s", input_string);
  } 
  else {  
    partition(input_string, left, right);
    //Create pipes for left child
    int leftIN[2], leftOUT[2];    
    char* left_partition = process_side(leftIN, leftOUT, left);

    //Create pipes for right child
    int rightIN[2], rightOUT[2];
    char* right_partition = process_side(rightIN, rightOUT, right);

    //Output the merged left and right child
    char result[BUFFER];
    int left_size = strlen(left);
    int right_size = strlen(right);
    merge(left_size, right_size, left_partition, right_partition, result);   
    printf("%s", result);
  }

  return 0;
}