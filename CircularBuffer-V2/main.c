#include "stm32f10x.h"
#include "cmsis_os.h"
#include "uart.h"

void producer_thread (void const *argument);
void consumer_thread(void const *argument);

// Define threads
osThreadDef(producer_thread, osPriorityNormal,1,0);
osThreadDef(consumer_thread, osPriorityNormal,1,0);

// Define the semaphores
osSemaphoreId item;
osSemaphoreDef(item);
osSemaphoreId empty;
osSemaphoreDef(empty);
osSemaphoreId sem1;
osSemaphoreDef(sem1);
osSemaphoreId sem2;
osSemaphoreDef(sem2);

// Define the mutex
osMutexId buffMutex;
osMutexDef(buffMutex);

osThreadId T_uart1;
osThreadId T_uart2;

unsigned int t;
unsigned int t1=0;
unsigned int t2=0;
const unsigned int N = 8;
unsigned char buffer[N];
unsigned int cbufferHead = 0;
unsigned int cbufferTail = 0;

void test(){
  t=0+t1-t2;
	if(t==0) {  //Empty
		SendChar('E'); 
		SendChar('\n'); SendChar('\r');
	}
	else if(t==8) {  //Full
		SendChar('F'); 
		SendChar('\n'); SendChar('\r');
	}
	else if(t>=9) {  //Error
		SendChar('e'); SendChar('r'); SendChar('r');
		SendChar('\n'); SendChar('\r');
	}
	else { 
	}	
}


void insert(unsigned char item1){
	osSemaphoreWait(empty, osWaitForever);
	osMutexWait(buffMutex, osWaitForever);
	buffer[cbufferTail] = item1;
	cbufferTail = (cbufferTail + 1) % N;
	osMutexRelease(buffMutex);
	osSemaphoreRelease(item);
}

void producer_thread (void const *argument)
{
	unsigned char item = 0x01;
	for(;;){
	long int i=0;
	for(i=0; i<5; i++){
		test();
		insert(item++);
		t1++;
  }
	osSemaphoreWait(sem1, osWaitForever);
	osSemaphoreRelease(sem2);
	}
}


void remove(unsigned char data1){
	osSemaphoreWait(item, osWaitForever);
	osMutexWait(buffMutex, osWaitForever);
	buffer[cbufferHead] = data1;
	cbufferHead = (cbufferHead + 1) % N;
	osMutexRelease(buffMutex);
	osSemaphoreRelease(empty);
}

void consumer_thread (void const *argument) 
{ 
	for(;;){
	unsigned char data = 0x00;
	long int j=0;
	osSemaphoreRelease(sem1);
	for(j=0; j<4; j++){
		test();
		remove(data);
		t2++;
	}
	osSemaphoreWait(sem2, osWaitForever);
  }
}


int main (void)
{
	osKernelInitialize ();   // initialize CMSIS-RTOS

	USART1_Init ();
	item = osSemaphoreCreate(osSemaphore(item), 0);	
	empty = osSemaphoreCreate(osSemaphore(empty), 8);	
	sem1 = osSemaphoreCreate(osSemaphore(sem1), 0);	
	sem2 = osSemaphoreCreate(osSemaphore(sem2), 0);	
	buffMutex = osMutexCreate(osMutex(buffMutex));
	T_uart1 = osThreadCreate(osThread(producer_thread), NULL);  //producer
	T_uart2 = osThreadCreate(osThread(consumer_thread), NULL);  //consumer
	
	osKernelStart ();	  // start thread execution 
}

