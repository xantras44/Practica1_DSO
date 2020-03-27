#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>

#include "mythread.h"
#include "interrupt.h"

#include "queue.h"

TCB* scheduler();
void activator();
void timer_interrupt(int sig);
void disk_interrupt(int sig);

struct queue *t_queue;

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
    //printf("hola");
    while(running->remaining_ticks)
    {
      //printf("Hola%d\n", running->remaining_ticks);
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

  /*Encola el nuevo thread*/
  TCB *t = &t_state[i];
  disable_interrupt();
  disable_disk_interrupt();
  enqueue(t_queue, t);
  enable_disk_interrupt();
  enable_interrupt();


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
  running = scheduler();  //Llamo al scheduler para que me de el hilo a ejecutar

  t_state[prev->tid].state = FREE;
  free(t_state[prev->tid].run_env.uc_stack.ss_sp);
  
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
  if(priority ==  HIGH_PRIORITY){
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


/* Planificador RR*/

TCB* scheduler()
{
  TCB* next; //almacena el proximo hilo a ejecutar
  if (!queue_empty(t_queue)){ //si la cola no esta vacia se desencola
	  disable_interrupt(); //para proteger el acceso a la cola
    disable_disk_interrupt();
    next = dequeue(t_queue); //proximo hilo a correr
	  enable_disk_interrupt();
    enable_interrupt();
	  current = next->tid;
	  return next;
  }
  printf("*** FINISH\n");
  exit(1);
}


/* Timer interrupt */
void timer_interrupt(int sig)
{
  running->ticks = running->ticks-1; //restar un tick al hilo en ejcucion
  running->remaining_ticks = running->remaining_ticks - 1;  //restar 1 al tiempo que le queda al proceso
  // Si el proceso ejecutándose ya ha terminado de ejecutarse, se llama a mythread_timeout.
  if (running->remaining_ticks < 0){
    mythread_timeout(running->tid);
  }

  if (running->ticks <= 0){ //si los ticks han llegado a 0 se restablecen los ticks y se encola de nuevo el hilo
    running->state = INIT; //listo para ejecutar
    running->ticks = QUANTUM_TICKS; //reinicia ticks

    disable_interrupt();
    disable_disk_interrupt();
    enqueue(t_queue, running); //encola en lista de listos
    enable_disk_interrupt();
    enable_interrupt();

	  prev = running; //hilo que ha estado corriendo hasta este momento
	  running = scheduler(); //llamada a la funcion scheduler para obtener el hilo a ejecutar
    if (prev != running){
	  printf("*** SWAPCONTEXT FROM %d TO %d\n", prev->tid, running->tid);
    
    activator(running); //llamada a la funcion activator para el cambio de contexto
          }
	    }
  
}

/* Activator */
void activator(TCB* next)
{
    if(prev->state == FREE){       //Si el hilo anterior en ejecucion ha acabado, es decir su estado es FREE
      setcontext (&(next->run_env));  //hago un setcontext y pongo el contexto del nuevo hilo
      printf("mythread_free: After setcontext, should never get here!!...\n");
    }else{
      swapcontext(&(prev->run_env), &(next->run_env));  //Si aun no ha acabado guardo el contexto del hilo anterior y cambio contexto
    }
}
