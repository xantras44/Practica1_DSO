#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include "my_io.h"

//#include "mythread.h"
#include "interrupt.h"

#include "queue.h"

TCB* scheduler();
void activator();
void timer_interrupt(int sig);
void disk_interrupt(int sig);

struct queue *t_queue;
struct queue *t_queue_high;

/* Array of state thread control blocks: the process allows a maximum of N threads */
static TCB t_state[N];

/* Current running thread */
static TCB* running;

/* Previous thread running*/
static TCB* prev;

static int current = 0;

/* Variable indicating if the library is initialized (init == 1) or not (init == 0) */
static int init=0;

/* Thread control block for the idle thread */
static TCB idle;

static void idle_function()
{
  while(1);
}

void function_thread(int sec)
{
    //time_t end = time(NULL) + sec;
    while(running->remaining_ticks)
    {
      //do something
    }
    mythread_exit();
}


/* Initialize the thread library */
void init_mythreadlib()
{
  int i;

  /* Create context for the idle thread */
  if(getcontext(&idle.run_env) == -1)
  {
    perror("*** ERROR: getcontext in init_thread_lib");
    exit(-1);
  }

  idle.state = IDLE;
  idle.priority = SYSTEM;
  idle.function = idle_function;
  idle.run_env.uc_stack.ss_sp = (void *)(malloc(STACKSIZE));
  idle.tid = -1;

  if(idle.run_env.uc_stack.ss_sp == NULL)
  {
    printf("*** ERROR: thread failed to get stack space\n");
    exit(-1);
  }

  idle.run_env.uc_stack.ss_size = STACKSIZE;
  idle.run_env.uc_stack.ss_flags = 0;
  idle.ticks = QUANTUM_TICKS;
  makecontext(&idle.run_env, idle_function, 1);

  t_state[0].state = INIT;
  t_state[0].priority = LOW_PRIORITY;
  t_state[0].ticks = QUANTUM_TICKS;

  if(getcontext(&t_state[0].run_env) == -1)
  {
    perror("*** ERROR: getcontext in init_thread_lib");
    exit(5);
  }

  for(i=1; i<N; i++)
  {
    t_state[i].state = FREE;
  }

  t_queue = queue_new();
  t_queue_high = queue_new();

  t_state[0].tid = 0;
  running = &t_state[0];

  /* Initialize disk and clock interrupts */
  init_disk_interrupt();
  init_interrupt();
}

/* Create and intialize a new thread with body fun_addr and one integer argument */
int mythread_create (void (*fun_addr)(),int priority,int seconds)
{
  int i;

  if (!init) { init_mythreadlib(); init=1;}

  for (i=0; i<N; i++)
    if (t_state[i].state == FREE) break;

  if (i == N) return(-1);

  if(getcontext(&t_state[i].run_env) == -1)
  {
    perror("*** ERROR: getcontext in my_thread_create");
    exit(-1);
  }
  t_state[i].ticks = QUANTUM_TICKS;
  t_state[i].state = INIT;
  t_state[i].priority = priority;
  t_state[i].function = fun_addr;
  t_state[i].execution_total_ticks = seconds_to_ticks(seconds);
  t_state[i].remaining_ticks = t_state[i].execution_total_ticks;
  t_state[i].run_env.uc_stack.ss_sp = (void *)(malloc(STACKSIZE));

  if(t_state[i].run_env.uc_stack.ss_sp == NULL)
  {
    printf("*** ERROR: thread failed to get stack space\n");
    exit(-1);
  }

  t_state[i].tid = i;
  t_state[i].run_env.uc_stack.ss_size = STACKSIZE;
  t_state[i].run_env.uc_stack.ss_flags = 0;
  makecontext(&t_state[i].run_env, fun_addr,2,seconds);

  TCB *t = &t_state[i];
  if (t->priority == LOW_PRIORITY){
    disable_interrupt();
    disable_disk_interrupt();
    enqueue(t_queue, t);
    enable_disk_interrupt();
    enable_interrupt();
  }
  else if (t->priority == HIGH_PRIORITY && running->priority == LOW_PRIORITY){
    running->state = INIT; //listo para ejecutar
    running->ticks = QUANTUM_TICKS;  //Reestablezco los ticks
    disable_interrupt();
    disable_disk_interrupt();
    enqueue(t_queue, running);
    enable_disk_interrupt();
    enable_interrupt();
    prev = running;
    running = t;
    printf("*** THREAD %d PREEMTED: SETCONTEXT  OF %d\n", prev->tid, running->tid);
    activator(running);
  }
  else if (running->priority == HIGH_PRIORITY && t->priority == HIGH_PRIORITY && t->remaining_ticks < running->remaining_ticks){
      running->state = INIT; //listo para ejecutar

      disable_interrupt();
      disable_disk_interrupt();
      sorted_enqueue(t_queue_high, running, running->remaining_ticks); //encola en lista de listos
      enable_disk_interrupt();
      enable_interrupt();

      prev = running; //hilo que ha estado corriendo hasta este momento
      running = t; //llamada a la funcion scheduler
      printf("*** SWAPCONTEXT FROM %d TO %d\n", prev->tid, running->tid);
      activator(running); //llamada a la funcion activator 
    }
  
else if (t->priority == HIGH_PRIORITY && running->priority == HIGH_PRIORITY && t->remaining_ticks >= running->remaining_ticks){
    t->state = INIT; //listo para ejecutar

    disable_interrupt();
    disable_disk_interrupt();
    sorted_enqueue(t_queue_high, t, t->remaining_ticks); //encola en lista de listos
    enable_disk_interrupt();
    enable_interrupt();
  }

  return i;
}
/****** End my_thread_create() ******/


/* Read disk syscall */
int read_disk()
{
   return 1;
}

/* Disk interrupt  */
void disk_interrupt(int sig)
{

}


/* Free terminated thread and exits */
void mythread_exit() {
  //int tid = mythread_gettid();     
                                        /*Hemos decidido no hacer uso de la función gettid en este caso, pues nos resulta
                                        facil usar el tid del hilo que sabemos que va  asalir de ejecucion*/

  prev = running;         //Guardo el anterior hilo ejecutado en prev
  printf("*** THREAD %d FINISHED\n", prev->tid);
  t_state[prev->tid].state = FREE;
  free(t_state[prev->tid].run_env.uc_stack.ss_sp);
  running = scheduler();  //Llamo al scheduler para que me de el hilo a ejecutar
 

  printf("*** THREAD %d TERMINATED : SETCONTEXT OF %d\n", prev->tid, running->tid);
  activator(running);     //Llamo al activador para realizar el setcontext
}


void mythread_timeout(int tid) {

    printf("*** THREAD %d EJECTED\n", tid);
    t_state[tid].state = FREE;
    free(t_state[tid].run_env.uc_stack.ss_sp);

    prev = running;          //Guardamos el anterior hilo ejecutado en prev
    running = scheduler();   //Llamamos al scheduler para que me de el hilo a ejecutar
    activator(running);      //Llamamos al activador para realizar el setcontext
}


/* Sets the priority of the calling thread */
void mythread_setpriority(int priority)
{
  int tid = mythread_gettid();
  t_state[tid].priority = priority;
  if(priority ==  HIGH_PRIORITY || priority == LOW_PRIORITY){
    t_state[tid].remaining_ticks = 195;
  }
}

/* Returns the priority of the calling thread */
int mythread_getpriority(int priority)
{
  int tid = mythread_gettid();
  return t_state[tid].priority;
}


/* Get the current thread id.  */
int mythread_gettid(){
  if (!init) { init_mythreadlib(); init=1;}
  return current;
}


/* SJF para alta prioridad, RR para baja*/

TCB* scheduler()
{
  TCB* next; //almacena el proximo hilo a ejecutar
  if (!queue_empty(t_queue_high)){ //si la cola de alta prioridad no esta vacia.
	  disable_interrupt(); //para proteger el acceso a la cola
    disable_disk_interrupt();
    next = dequeue(t_queue_high); //proximo hilo a correr
    enable_disk_interrupt();
    enable_interrupt();
	  return next;
  }
  else {
    if (!queue_empty(t_queue)){ //si la cola de baja prioridad no esta vacia.
  	  disable_interrupt(); //para proteger el acceso a la cola
      disable_disk_interrupt();
      next = dequeue(t_queue); //proximo hilo a correr
      enable_disk_interrupt();
      enable_interrupt();
  	  return next;
    }
  }
  printf("*** FINISH\n");
  exit(1);
}


/* Timer interrupt */
void timer_interrupt(int sig)
{
  if (running->priority == LOW_PRIORITY){
     running->ticks = running->ticks-1; //restar un tick al hilo en ejcucion
  }
  running->remaining_ticks = running->remaining_ticks - 1;
  // Si el proceso ejecutándose ya ha terminado de ejecutarse, se llama a mythread_timeout.
  if (running->remaining_ticks < 0){
    mythread_timeout(running->tid);
  }
  //Si los ticks han llegado a 0 (por lo tanto prioridad baja).
  if (running->ticks == 0 && running->priority == LOW_PRIORITY){
    //Si no hay hilos listos, se pueden reiniciar los ticks del hilo actual para que se siga ejecutando
    running->state = INIT; //listo para ejecutar
    running->ticks = QUANTUM_TICKS; //reinicia ticks

    disable_interrupt();
    disable_disk_interrupt();
    enqueue(t_queue, running); //encola en lista de listos
    enable_disk_interrupt();
    enable_interrupt();

    prev = running; //hilo que ha estado corriendo hasta este momento
    running = scheduler(); //llamada a la funcion scheduler
    if (prev != running){
    printf("*** SWAPCONTEXT FROM %d TO %d\n", prev->tid, running->tid);
    activator(running); //llamada a la funcion activator con el hilo que ha estado corriendo
          }
  }
  
}

/* Activator */
void activator(TCB* next)
{
    if(prev->state == FREE){       //Si el hilo anterior en ejecucion ha acabado, es decir su estado es FREE
      if(setcontext (&(next->run_env)) == -1){ //hago un setcontext y pongo el contexto del nuevo hilo
        perror("Fallo en el setcontext, no se debería llegar a este punto");
        exit(-1);
      }  

    }else{
      if(swapcontext(&(prev->run_env), &(next->run_env)) == -1){  //Si aun no ha acabado guardo el contexto del hilo anterior y cambio contexto
        perror("Fallo en el swapcontext, no se debería llegar a este punto");
        exit(-1);
      }
    }
}
