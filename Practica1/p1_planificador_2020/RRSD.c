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

//Definimos la cola para procesos de baja prioridad
struct queue *t_queue;
//Definimos la cola para procesos de alta prioridad
struct queue *t_queue_high;
//Definimos la cola para procesos bloqueados
struct queue *t_queue_wait;

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

//Funcion usada en algunas pruebas
void fun1(int sec)
    {
    //time_t end = time(NULL) + sec;
    while(running->remaining_ticks)
    {

    }
    read_disk();
    mythread_exit();
  }

//Funcion usada en algunas pruebas
void fun2(int sec)
    {
    //time_t end = time(NULL) + sec;
    while(running->remaining_ticks)
    {
        for(int i = 0; i < 1000000000000000000; i++){
      
    }
    }
    
    mythread_exit();
    }

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

  //Creamos las colas que vamos a usar
  t_queue = queue_new();
  t_queue_high = queue_new();
  t_queue_wait = queue_new();

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

  //Si el nuevo hilo es de baja prioridad se encola en la cola correspondiente
  if (t->priority == LOW_PRIORITY){
    disable_interrupt();
    disable_disk_interrupt();   //Antes de acceder a las colas se deshabilitan las interrupciones
    enqueue(t_queue, t);
    enable_disk_interrupt();    //Después de acceder a las colas se habilitan las interrupciones
    enable_interrupt();
  }

  //Si el nuevo hilo es de alta prioridad y el que esta ejecutando de baja se debe encolar este ultimo y poner el nuevo a ejecutar
  else if (t->priority == HIGH_PRIORITY && running->priority == LOW_PRIORITY){
    running->state = INIT;            //listo para ejecutar
    running->ticks = QUANTUM_TICKS;   //Reestablezco los ticks
    disable_interrupt();
    disable_disk_interrupt();       //Antes de acceder a las colas se deshabilitan las interrupciones
    enqueue(t_queue, running);
    enable_disk_interrupt();        //Despues de acceder a las colas se habilitan las interrupciones
    enable_interrupt();
    prev = running;                 //hilo que ha estado corriendo hasta este momento
    running = t;                    //el hilo a ejecutar sera el recien creado
    printf("*** THREAD %d PREEMTED: SETCONTEXT  OF %d\n", prev->tid, running->tid);
    activator(running);             //llamada a la funcion activator 
  }

  /*Si ambos hilos (el nuevo y el que esta en ejecucion) son de alta prioridad se debe encolar al que tenga mayor 
  valor de remaining_ticks, mientras que el otro ejecutara, en este caso ejecutara el nuevo*/
  else if (running->priority == HIGH_PRIORITY && t->priority == HIGH_PRIORITY && t->remaining_ticks < running->remaining_ticks){
      running->state = INIT;          //listo para ejecutar

      disable_interrupt();
      disable_disk_interrupt();       //Antes de acceder a las colas se deshabilitan las interrupciones
      sorted_enqueue(t_queue_high, running, running->remaining_ticks); //encola en cola de procesos de alta prioridad
      enable_disk_interrupt();        //Despues de acceder a las colas se habilitan las interrupciones
      enable_interrupt();

      prev = running;                 //hilo que ha estado corriendo hasta este momento
      running = t;                    //el hilo a ejecutar sera el recien creado
      printf("*** SWAPCONTEXT FROM %d TO %d\n", prev->tid, running->tid);
      activator(running);             //llamada a la funcion activator 
    }
  
  /*Si ambos hilos (el nuevo y el que esta en ejecucion) son de alta prioridad se debe encolar al que tenga mayor 
  valor de remaining_ticks, mientras que el otro ejecutara, en este caso se encolara el nuevo*/
  else if (t->priority == HIGH_PRIORITY && running->priority == HIGH_PRIORITY && t->remaining_ticks >= running->remaining_ticks){
    t->state = INIT;                //listo para ejecutar

    disable_interrupt();
    disable_disk_interrupt();       //Antes de acceder a las colas se deshabilitan las interrupciones
    sorted_enqueue(t_queue_high, t, t->remaining_ticks); //encola en cola de procesos de alta prioridad
    enable_disk_interrupt();        //Despues de acceder a las colas se habilitan las interrupciones
    enable_interrupt();
  }

  return i;
}
/****** End my_thread_create() ******/


/* Read disk syscall */
int read_disk()
{
  //Si la informacion no esta en cache encolara el proceso en la cola de bloqueados y llamara al siguiente
  if(data_in_page_cache() != 0){
    running->state = WAITING;

    //Si es de baja prioridad restablece la rodaja
    if(running->priority == LOW_PRIORITY){
      running->ticks = QUANTUM_TICKS;
    }

    disable_interrupt();
    disable_disk_interrupt();         //Antes de acceder a las colas se deshabilitan las interrupciones
    enqueue(t_queue_wait, running);
    enable_disk_interrupt();          //Despues de acceder a las colas se habilitan las interrupciones
    enable_interrupt();

    prev = running;                   //Guardamos el anterior hilo ejecutado en prev
    printf("*** THREAD %d READ FROM DISK\n", prev->tid);
    running = scheduler();            //Llamamos al scheduler para que me de el hilo a ejecutar
    printf("*** SWAPCONTEXT FROM %d TO %d\n", prev->tid, running->tid);
    activator(running);               //Llamamos al activador para realizar el setcontext
  }
   return 1;
}

/* Disk interrupt  */
void disk_interrupt(int sig)
{
  //Si hay procesos bloqueados los desbloquea y encola en su cola correspondiente
  if(!queue_empty(t_queue_wait)){
    disable_interrupt();
    disable_disk_interrupt();             //Antes de acceder a las colas se deshabilitan las interrupciones
    TCB* listo = dequeue(t_queue_wait);
    if(listo->priority == LOW_PRIORITY){
      enqueue(t_queue, listo);
      printf("*** THREAD %d READY\n",listo->tid);
    }
    else{
      sorted_enqueue(t_queue_high, listo, listo->remaining_ticks);
      printf("*** THREAD %d READY\n",listo->tid);
    }
    enable_disk_interrupt();            //Despues de acceder a las colas se habilitan las interrupciones
    enable_interrupt();
  }
}


/* Free terminated thread and exits */
void mythread_exit() {
  //int tid = mythread_gettid();     
                                        /*Hemos decidido no hacer uso de la función gettid en este caso, pues nos resulta
                                        facil usar el tid del hilo que sabemos que va a salir de ejecucion*/

  prev = running;                       //Guardo el anterior hilo ejecutado en prev
  printf("*** THREAD %d FINISHED\n", prev->tid);
  t_state[prev->tid].state = FREE;
  free(t_state[prev->tid].run_env.uc_stack.ss_sp);
  running = scheduler();                //Llamo al scheduler para que me de el hilo a ejecutar
  
  printf("*** THREAD %d TERMINATED : SETCONTEXT OF %d\n", prev->tid, running->tid);
  activator(running);                   //Llamo al activador para realizar el setcontext
}

//Expulsa a un hilo si excede su tiempo de ejecucion asignado y libera su espacio
void mythread_timeout(int tid) {

    printf("*** THREAD %d EJECTED\n", tid);
    t_state[tid].state = FREE;
    free(t_state[tid].run_env.uc_stack.ss_sp);

    prev = running;                 //Guardamos el anterior hilo ejecutado en prev
    running = scheduler();          //Llamamos al scheduler para que me de el hilo a ejecutar
    activator(running);             //Llamamos al activador para realizar el setcontext
}


/* Sets the priority of the calling thread */
void mythread_setpriority(int priority)
{
  int tid = mythread_gettid();
  t_state[tid].priority = priority;
  if(priority ==  HIGH_PRIORITY || priority ==  LOW_PRIORITY){
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
  TCB* next;        //almacena el proximo hilo a ejecutar

  //Si estan todas las colas vacias menos la de bloqueados devuelve el idle
  if(queue_empty(t_queue_high) && queue_empty(t_queue) && !queue_empty(t_queue_wait)){
    return &idle;
  }

  //Si hay hilos en la cola de alta prioridad
  if (!queue_empty(t_queue_high)){ 
	  disable_interrupt(); 
    disable_disk_interrupt();               //Antes de acceder a las colas se deshabilitan las interrupciones
    next = dequeue(t_queue_high);           //proximo hilo a correr
    enable_disk_interrupt();                //Despues de acceder a las colas se habilitan las interrupciones
    enable_interrupt();
	  return next;
  }
  else {
    //si la cola de baja prioridad no esta vacia.
    if (!queue_empty(t_queue)){ 
  	  disable_interrupt(); 
      disable_disk_interrupt();             //Antes de acceder a las colas se deshabilitan las interrupciones
      next = dequeue(t_queue);              //proximo hilo a correr
      enable_disk_interrupt();              //Despues de acceder a las colas se habilitan las interrupciones
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
  //Si es de prioridad baja decrementa los ticks 
  if (running->priority == LOW_PRIORITY){
     running->ticks = running->ticks-1;
  }

  running->remaining_ticks = running->remaining_ticks - 1;

  // Si el proceso ejecutándose ya ha terminado de ejecutarse, se llama a mythread_timeout.
  if (running->remaining_ticks < 0 && running->priority != SYSTEM){
    mythread_timeout(running->tid);
  }

  //Si esta ejecutando el idle y ya hay algun otro hilo listo
  if (running->tid == -1 && (!queue_empty(t_queue_high) || !queue_empty(t_queue))){
    prev = running;                     //hilo que ha estado corriendo hasta este momento
    running = scheduler();              //llamo al scheduler para sacar al hilo que toca
    printf("*** THREAD READY: SET CONTEXT TO %d\n", running->tid);
    activator(running);                 //llamada a la funcion activator
  } 

  //Si hay hilos de alta prioridad listos y el que esta ejecutando es de alta compruebo si debe hacer cambio de contexto
  if(!queue_empty(t_queue_high) && running->priority == HIGH_PRIORITY){
    TCB * listo = t_queue_high->head->data;

    //Si los remaining_ticks del hilo en la cola son menores que el de ejecucion cambia de contexto
    if(listo->remaining_ticks < running->remaining_ticks){
      running->state = INIT;            //listo para ejecutar

      disable_interrupt();
      disable_disk_interrupt();         //Antes de acceder a las colas se deshabilitan las interrupciones
      sorted_enqueue(t_queue_high, running, running->remaining_ticks); //encola en cola de procesos de alta prioridad
      enable_disk_interrupt();          //Despues de acceder a las colas se habilitan las interrupciones
      enable_interrupt();
      prev = running;                   //hilo que ha estado corriendo hasta este momento
      running = scheduler();            //llamo al scheduler para sacar al hilo de alta prioridad que toca
      printf("*** SWAPCONTEXT FROM %d TO %d\n", prev->tid, running->tid);
      activator(running);               //llamada a la funcion activator
      }
  }

  //Si hay hilos de alta prioridad listos y el que hay en ejecucion es de baja cambia de contexto
  else if(!queue_empty(t_queue_high) && running->priority == LOW_PRIORITY){
    running->state = INIT;              //listo para ejecutar
    running->ticks = QUANTUM_TICKS;     //Reestablezco los ticks
    disable_interrupt();
    disable_disk_interrupt();           //Antes de acceder a las colas se deshabilitan las interrupciones
    enqueue(t_queue, running);
    enable_disk_interrupt();            //Despues de acceder a las colas se habilitan las interrupciones
    enable_interrupt();
    prev = running;                     //hilo que ha estado corriendo hasta este momento
    running = scheduler();              //llamo al scheduler para sacar al hilo de alta prioridad que toca
    printf("*** THREAD %d PREEMTED: SETCONTEXT  OF %d\n", prev->tid, running->tid);
    activator(running);                 //llamada a la funcion activator 
  }

  //Si los ticks han llegado a 0 (por lo tanto prioridad baja).
  if (running->ticks == 0 && running->priority == LOW_PRIORITY){
  
    running->state = INIT;                //listo para ejecutar
    running->ticks = QUANTUM_TICKS;       //reinicia ticks

    disable_interrupt();
    disable_disk_interrupt();             //Antes de acceder a las colas se deshabilitan las interrupciones
    enqueue(t_queue, running);            //encola en cola de procesos de baja prioridad
    enable_disk_interrupt();              //Despues de acceder a las colas se habilitan las interrupciones
    enable_interrupt();

    prev = running;                      //hilo que ha estado corriendo hasta este momento
    running = scheduler();               //llamada a la funcion scheduler
    if (prev != running){
    printf("*** SWAPCONTEXT FROM %d TO %d\n", prev->tid, running->tid);
    activator(running);                  //llamada a la funcion activator con el hilo que ha estado corriendo
          }
  }
}

/* Activator */
void activator(TCB* next)
{
    //Si el hilo anterior en ejecucion ha acabado, es decir su estado es FREE
    if(prev->state == FREE){       
      if(setcontext (&(next->run_env)) == -1){      //hago un setcontext y pongo el contexto del nuevo hilo
        perror("Fallo en setcontext, no debería llegar hasta aquí\n");
      }
    }else{
      //Si aun no ha acabado guardo el contexto del hilo anterior y cambio contexto
      if(swapcontext(&(prev->run_env), &(next->run_env)) == -1){  
        perror("Fallo en swapcontext, no debería llegar hasta aquí\n");
      }
    }
}
