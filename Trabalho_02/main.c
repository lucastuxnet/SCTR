/*
FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
All rights reserved

VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

This file is part of the FreeRTOS distribution.

FreeRTOS is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (version 2) as published by the
Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

***************************************************************************
>>!   NOTE: The modification to the GPL is included to allow you to     !<<
>>!   distribute a combined work that includes FreeRTOS without being   !<<
>>!   obliged to provide the source code for proprietary components     !<<
>>!   outside of the FreeRTOS kernel.                                   !<<
***************************************************************************

FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  Full license text is available on the following
link: http://www.freertos.org/a00114.html

***************************************************************************
*                                                                       *
*    FreeRTOS provides completely free yet professionally developed,    *
*    robust, strictly quality controlled, supported, and cross          *
*    platform software that is more than just the market leader, it     *
*    is the industry's de facto standard.                               *
*                                                                       *
*    Help yourself get started quickly while simultaneously helping     *
*    to support the FreeRTOS project by purchasing a FreeRTOS           *
*    tutorial book, reference manual, or both:                          *
*    http://www.FreeRTOS.org/Documentation                              *
*                                                                       *
***************************************************************************

http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
the FAQ page "My application does not run, what could be wrong?".  Have you
defined configASSERT()?

http://www.FreeRTOS.org/support - In return for receiving this top quality
embedded software for free we request you assist our global community by
participating in the support forum.

http://www.FreeRTOS.org/training - Investing in training allows your team to
be as productive as possible as early as possible.  Now you can receive
FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
Ltd, and the world's leading authority on the world's leading RTOS.

http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
including FreeRTOS+Trace - an indispensable productivity tool, a DOS
compatible FAT file system, and our tiny thread aware UDP/IP stack.

http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
licenses offer ticketed support, indemnification and commercial middleware.

http://www.SafeRTOS.com - High Integrity Systems also provide a safety
engineered and independently SIL3 certified version for use in safety and
mission critical applications that require provable dependability.

1 tab == 4 spaces!
*/


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>


/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>
#include <math.h>


#define mainREGION_1_SIZE	3001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	1107

/* Defina algumas constantes para a tarefa de cálculos de matriz */
#define SIZE 10
#define ROW SIZE
#define COL SIZE


static void  prvInitialiseHeap(void);


void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);


static void prvSaveTraceFile(void);


traceLabel xTickTraceUserEvent;
static portBASE_TYPE xTraceRunning = pdTRUE;

/* Protótipos para cada função da task */
static void matrix_task();
static void communication_task();
static void prioritysettask();

/* Handlers para as tasks */
TaskHandle_t matrix_handle = NULL;
TaskHandle_t communication_handle = NULL;
TaskHandle_t priority_handle = NULL;

/* CronOmetro para as tasks */
long matrix_ticks = 0;
long comm_ticks = 0;
boolean matrix_active = FALSE;
boolean comm_active = FALSE;

/*-----------------------------------------------------------*/

int main(void)
{
	
	prvInitialiseHeap();

	/* Inicialize o gravador de rastreamento e cria o rótulo usado para postar o usuário
eventos para a gravação de rastreamento em cada interrupção de tick. */
	vTraceInitTraceData();
	xTickTraceUserEvent = xTraceOpenLabel("tick");

	/* Cria as tasks */
	xTaskCreate((pdTASK_CODE)matrix_task, (signed char *)"Matrix", 1000, NULL, 3,
		&matrix_handle);
	xTaskCreate((pdTASK_CODE)communication_task, (signed char *)"Communication",
		configMINIMAL_STACK_SIZE, NULL, 4, &communication_handle);
	xTaskCreate((pdTASK_CODE)prioritysettask, (signed char *)"Priority",
		configMINIMAL_STACK_SIZE, NULL, 5, &priority_handle);
	
	// Inicia o real-time-scheduler
	vTaskStartScheduler();
	

	for ( ;; );

}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	
	vAssertCalled(__LINE__, __FILE__);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{

}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	
	vAssertCalled(__LINE__, __FILE__);
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
	/* O tickhook apenas incrementa os temporizadores se as tarefas estiverem ativas */
	if (matrix_active == TRUE)
		matrix_ticks++;

	if (comm_active == TRUE)
		comm_ticks++;
}
/*-----------------------------------------------------------*/

void vAssertCalled(unsigned long ulLine, const char * const pcFileName)
{
	static portBASE_TYPE xPrinted = pdFALSE;
	volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

	/* Pare o valor de gravação de rastreamento. */
	(void)ulLine;
	(void)pcFileName;

	printf("ASSERT! Line %d, file %s\r\n", ulLine, pcFileName);

	taskENTER_CRITICAL();
	{
		/* Stop the trace recording. */
		if (xPrinted == pdFALSE)
		{
			xPrinted = pdTRUE;
			if (xTraceRunning == pdTRUE)
			{
				vTraceStop();
				prvSaveTraceFile();
			}
		}

		/* Você pode sair desta função para debugar a asserção usando
		o debugger para setar ulSetToNonZeroInDebuggerToContinue para um valor diferente de zero. */

		while (ulSetToNonZeroInDebuggerToContinue == 0)
		{
			__asm { NOP };
			__asm { NOP };
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile(void)
{
	FILE* pxOutputFile;

	pxOutputFile = fopen("Trace.dump", "wb");

	if (pxOutputFile != NULL)
	{
		fwrite(RecorderDataPtr, sizeof(RecorderDataType), 1, pxOutputFile);
		fclose(pxOutputFile);
		printf("\r\nTrace output saved to Trace.dump\r\n");
	}
	else
	{
		printf("\r\nFailed to create trace dump file\r\n");
	}
}
/*-----------------------------------------------------------*/


static void  prvInitialiseHeap(void)
{
	
	static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
	volatile uint32_t ulAdditionalOffset = 19; /* Apenas para evitar avisos de 'condição é sempre verdadeira' em configASSERT(). */
	const HeapRegion_t xHeapRegions[] =
	{
		/* Endereço inicial com deslocamentos fictícios					Tamanho */
		{ ucHeap + 1,											mainREGION_1_SIZE },
		{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
		{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
		{ NULL, 0 }
	};

	/* Verifique se os tamanhos e deslocamentos definidos realmente se encaixam no
variedade. */
	configASSERT((ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE) < configTOTAL_HEAP_SIZE);

	vPortDefineHeapRegions(xHeapRegions);
}
/*-----------------------------------------------------------*/

/* Implementação das task */

static void matrix_task()
{
	int i;
	double **a = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) a[i] = (double *)pvPortMalloc(COL * sizeof(double));
	double **b = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) b[i] = (double *)pvPortMalloc(COL * sizeof(double));
	double **c = (double **)pvPortMalloc(ROW * sizeof(double*));
	for (i = 0; i < ROW; i++) c[i] = (double *)pvPortMalloc(COL * sizeof(double));

	double sum = 0.0;
	int j, k, l;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			a[i][j] = 1.5;
			b[i][j] = 2.6;
		}
	}

	while (1) {
		/*
		* Em um sistema embarcado, a multiplicação da matriz bloquearia a CPU por um longo tempo
		* mas como este é um simulador de PC, devemos adicionar um atraso fictício adicional.
		*/

		matrix_active = TRUE;
		matrix_ticks = 0;

		long simulationdelay;
		for (simulationdelay = 0; simulationdelay<1000000000; simulationdelay++)
			;
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				c[i][j] = 0.0;
			}
		}

		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				sum = 0.0;
				for (k = 0; k < SIZE; k++) {
					for (l = 0; l<10; l++) {
						sum = sum + a[i][k] * b[k][j];
					}
				}
				c[i][j] = sum;
			}
		}
		matrix_active = FALSE;
		printf("MATRIX TASK TIME		:  %i ms \n", matrix_ticks * portTICK_PERIOD_MS);
		fflush(stdout);
		vTaskDelay(100);
	}
}

static void communication_task()
{
	while (1) {
		// Para começar, suspendemos o controle de prioridade para que não interfira
		vTaskSuspend(priority_handle);
		comm_active = TRUE;
		comm_ticks = 0;
		// Em seguida, executamos a task de comunicação
		printf("Sending data...\n");
		fflush(stdout);
		vTaskDelay(100);
		printf("Data sent!\n");
		fflush(stdout);
		vTaskDelay(100);
		printf("COMMUNICATION TASK TIME		: %i ms\n", comm_ticks * portTICK_PERIOD_MS);
		fflush(stdout);
		// Após a execução, retomamos o tratamento de prioridade e marcamos a task como inativa
		comm_active = FALSE;
		vTaskResume(priority_handle);
	}
}


static void prioritysettask()
{
	static int comm_pri = 4;		// Usando uma variável local para ter menos chamadas de API
	static int comm_time = 0;		// Variável para calcular ticks/tempo em ms
	printf("Priority set task has been initialized...\n");
	fflush(stdout);
	while (1) {
		// Calculando primeiro o tempo de comunicação em ms
		comm_time = comm_ticks * portTICK_PERIOD_MS;
		
		//printf("PriSetTask getting COMTIME		: %i ms...\n", comm_time);
		//fflush(stdout);
		/* Logica : Na execução desta tarefa verifica os ticks ativos
		 * para a tarefa de comunicação, se mais de 1000 aumenta a prioridade,
		 * em contraponte caso abaixo de 200ms, reduz a prioridade */
		if (comm_time > 1000 && comm_pri != 4) {
			comm_pri = 4;
			printf("Communication task running slowly at %i ms, increasing priority to 4\n", comm_time);
			fflush(stdout);
			vTaskPrioritySet(communication_handle, comm_pri);
		}
		else if (comm_time < 200 && comm_pri != 2) {
			comm_pri = 2;
			printf("Communication task running fast enough at %i ms, reducing priority to 2\n", comm_time);
			fflush(stdout);
			vTaskPrioritySet(communication_handle, comm_pri);
		}
		// -- Bloquea a tarefa por alguns ticks antes de fazer o loop --
		vTaskDelay(1000);
	}
}
