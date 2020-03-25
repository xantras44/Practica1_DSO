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
    printf ("Thread %d with priority %d\t from fun2 a = %d\tb = %d\n", mythread_gettid(), mythread_getpriority(), a, b);
    for (b=0; b<25000000; ++b);
  }

  for (a=0; a<10; ++a) { 
    printf ("Thread %d with priority %d\t from fun2 a = %d\tb = %d\n", mythread_gettid(), mythread_getpriority(), a, b);
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

  if (argc > 1){
    //test0 o por defecto
    if ((strcmp(argv[1], "test0")) == 0){
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
    }
    // test1
    if ((strcmp(argv[1], "test1")) == 0){
      if((a =  mythread_create(fun1,HIGH_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(fun1,HIGH_PRIORITY, 1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    } 
    // test2
    else if ((strcmp(argv[1], "test2")) == 0){
      
    }
    // test3 
    else if ((strcmp(argv[1], "test3")) == 0){
      
    } 
    // test4
    else if ((strcmp(argv[1], "test4")) == 0){
      
    } 
    // test5
    else if ((strcmp(argv[1], "test5")) == 0){
      
    } 
    // test6
    else if ((strcmp(argv[1], "test6")) == 0){
      
    } 
    // test7
    else if ((strcmp(argv[1], "test7")) == 0){
      
    } 
    // test8
    else if ((strcmp(argv[1], "test8")) == 0){
      
    } 
    // test9
    else if ((strcmp(argv[1], "test9")) == 0){
      
    }
    // test10
    else if ((strcmp(argv[1], "test9")) == 0){
      
    }// test11
    else if ((strcmp(argv[1], "test9")) == 0){
      
    }// test12
    else if ((strcmp(argv[1], "test9")) == 0){
      
    }// test13
    else if ((strcmp(argv[1], "test9")) == 0){
      
    }else {
      printf("El paramentro introducido no es correcto\n");
    }
  }

  mythread_exit();	
  
  printf("This program should never come here\n");
  
  return 0;
} /****** End main() ******/


