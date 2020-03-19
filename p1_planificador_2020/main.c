#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <string.h>

#include "mythread.h"

void fun1 (int global_index)
{
  int a=0, b=0;
read_disk();
  for (a=0; a<10; ++a) { 
//    printf ("Thread %d with priority %d\t from fun2 a = %d\tb = %d\n", mythread_gettid(), mythread_getpriority(), a, b);
    for (b=0; b<25000000; ++b);
  }

  for (a=0; a<10; ++a) { 
//    printf ("Thread %d with priority %d\t from fun2 a = %d\tb = %d\n", mythread_gettid(), mythread_getpriority(), a, b);
    for (b=0; b<25000000; ++b);
  }
  mythread_exit(); 
  return;
}

//Each thread executes this function
extern void function_thread(int sec);



int main(int argc, char *argv[])
{
  int j,k,l,m,a,b,f;


  mythread_setpriority(LOW_PRIORITY);
  if((f = mythread_create(function_thread,HIGH_PRIORITY,2)) == -1){
      printf("thread failed to initialize\n");
      exit(-1);
  }
  
  read_disk();
  read_disk();

  if((j = mythread_create(function_thread,HIGH_PRIORITY, 2)) == -1){
    printf("thread failed to initialize\n");
    exit(-1);
  }
  if((k = mythread_create(function_thread,HIGH_PRIORITY, 2)) == -1){
    printf("thread failed to initialize\n");
    exit(-1);
  }  
  if((l = mythread_create(function_thread,LOW_PRIORITY, 2)) == -1){
    printf("thread failed to initialize\n");
    exit(-1);
  }

  if((m = mythread_create(function_thread,HIGH_PRIORITY, 1)) == -1){
    printf("thread failed to initialize\n");
    exit(-1);
  }
      
     
  for (a=0; a<10; ++a) {
    for (b=0; b<30000000; ++b);
  }	

  if (argc > 1){
    // TEST1
    if ((strcmp(argv[1], "test1")) == 0){
      /*if((a =  mythread_create(fun1,HIGH_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(fun1,HIGH_PRIORITY, 1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }*/
    } 
    // TEST2
    else if ((strcmp(argv[1], "test2")) == 0){
      
    } else {
      printf("El paramentro introducido no es correcto");
    }
  }

  mythread_exit();	
  
  printf("This program should never come here\n");
  
  return 0;
} /****** End main() ******/


