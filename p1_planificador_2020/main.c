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
    //printf ("Thread %d with priority %d\t from fun2 a = %d\tb = %d\n", mythread_gettid(), mythread_getpriority(), a, b);
    for (b=0; b<25; ++b);
  }

  for (a=0; a<10; ++a) {
    //printf ("Thread %d with priority %d\t from fun2 a = %d\tb = %d\n", mythread_gettid(), mythread_getpriority(), a, b);
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

    mythread_setpriority(LOW_PRIORITY);
    //test0 o por defecto
    if ((strcmp(argv[1], "test0")) == 0){
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
    // test1 RR  Comprobar que ocurre si acaba la ejecucion del hilo antes de que acabe su rodaja
    if ((strcmp(argv[1], "test1")) == 0){
      mythread_setpriority(LOW_PRIORITY);
      if((a =  mythread_create(function_thread,LOW_PRIORITY,0.1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,LOW_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((m =  mythread_create(function_thread,LOW_PRIORITY,0.1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((f =  mythread_create(function_thread,LOW_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((l =  mythread_create(function_thread,LOW_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test2 RR  Comprobar que si el hilo acaba la ejecución junto a la rodaja sale del sistema y no reestablece la rodaja y sigue en ejecucion
    else if ((strcmp(argv[1], "test2")) == 0){
      mythread_setpriority(LOW_PRIORITY);
      if((a =  mythread_create(function_thread,LOW_PRIORITY,0.2)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test3 RRS Comprobar que se respetan las prioridades y que el SJF y el RR se realizan correctamente
    else if ((strcmp(argv[1], "test3")) == 0){
      mythread_setpriority(HIGH_PRIORITY);
      if((a =  mythread_create(function_thread,LOW_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,LOW_PRIORITY,2)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((f =  mythread_create(function_thread,HIGH_PRIORITY,3)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((l =  mythread_create(function_thread,HIGH_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test4  RRS Comprobar que un hilo de prioridad alta es sustituido en la ejecucion por otro de prioridad alta y menor tiempo de ejecucion que llegue
    else if ((strcmp(argv[1], "test4")) == 0){
      mythread_setpriority(HIGH_PRIORITY);
      if((a =  mythread_create(function_thread,HIGH_PRIORITY,0.1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,HIGH_PRIORITY,0.1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test5  RRS Comprobar que si hay un hilo de prioridad baja ejecutando y llega uno de alta es expulsado y si solo llegan de baja prioridad se cumple el RR
    else if ((strcmp(argv[1], "test5")) == 0){
      mythread_setpriority(LOW_PRIORITY);
      if((a =  mythread_create(function_thread,LOW_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,LOW_PRIORITY,2)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((f =  mythread_create(function_thread,HIGH_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((l =  mythread_create(function_thread,HIGH_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test6
    else if ((strcmp(argv[1], "test6")) == 0){
      mythread_setpriority(HIGH_PRIORITY);
      if((a =  mythread_create(function_thread,HIGH_PRIORITY,3)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,HIGH_PRIORITY,5)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test7
    else if ((strcmp(argv[1], "test7")) == 0){
      mythread_setpriority(HIGH_PRIORITY);
      if((a =  mythread_create(function_thread,HIGH_PRIORITY,3)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,HIGH_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }

    }
    // test8
    else if ((strcmp(argv[1], "test8")) == 0){
      if((a =  mythread_create(function_thread,LOW_PRIORITY,10)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,HIGH_PRIORITY,3)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test9
    else if ((strcmp(argv[1], "test9")) == 0){
      if((a =  mythread_create(function_thread,LOW_PRIORITY,41)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,LOW_PRIORITY,3)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }
    // test10
    else if ((strcmp(argv[1], "test10")) == 0){
      if((a =  mythread_create(function_thread,LOW_PRIORITY,1)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
      if((b =  mythread_create(function_thread,HIGH_PRIORITY,3)) == -1){
        printf("thread failed to initialize\n");
        exit(-1);
      }
    }// test11
    else if ((strcmp(argv[1], "test11")) == 0){

    }// test12
    else if ((strcmp(argv[1], "test12")) == 0){

    }// test13
    else if ((strcmp(argv[1], "test13")) == 0){

    }else {
      //printf("El paramentro introducido no es correcto\n");
    }
  }
  else{
    printf("introduce el argumento test\n");
    return 0;
  }

  mythread_exit();

  printf("This program should never come here\n");

  return 0;
} /****** End main() ******/
