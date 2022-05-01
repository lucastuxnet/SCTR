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

/* Curso de introdução a systemas em tempo real
    Lucas Albino Martins
	Tarefa Assignment 1
*/

#include <stdio.h>
#include <stdlib.h>
/* Incluindo o kernel do FreeRTOS*/
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>
#include <math.h>
#define mainREGION_1_SIZE	3001
#define mainREGION_2_SIZE	18105
#define mainREGION_3_SIZE	1107

/*
 Iniciando a função.
*/
static void  prvInitialiseHeap(void);

/*
Função de retorno.
*/
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/*
 Gravando o processo em um arquivo quando é interrompida.
*/
static void prvSaveTraceFile(void);

/* Iniciando a gravação a cada tick */
traceLabel xTickTraceUserEvent;
static portBASE_TYPE xTraceRunning = pdTRUE;

/* Tarefa 1 */
void task1Code(void * pvParameters)
{
	/* Laço para iniciar o criador de tarefas */
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 ); 

	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;

	for( ;; )
	{
	/* Imprimindo o código da tarefa. */
		printf("This is task 1\n");
		fflush(stdout);

		vTaskDelay(xDelay);
	}
}

void task2Code(void * pvParameters)
{
	/* Laço para chamar a criação de tarefa */
	configASSERT(((uint32_t)pvParameters) == 1);

	const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	for (;; )
	{
		/* Código da tarefa. */
		printf("This is task 2\n");
		fflush(stdout);

		vTaskDelay(xDelay);
	}
}

/* Função de criaçao da tarefa. */
BaseType_t createTask(const char* const task_name,
					  const unsigned short stack_size,
	                  const UBaseType_t priority,
					  void(*callback)(void*),
					  TaskHandle_t task_handle)
{

	/* Criando a tarefa. */
	return xTaskCreate(
		callback,       /* Função para implementar a tarefa. */
		task_name,      /* Nome do cabeçario da tarefa. */
		stack_size,     /* Tamanho da pilha em caracteres e não em bytes. */
		(void *)1,      
		priority,       /* Determina a prioridade da tarefa criada. */
		&task_handle);      /* Identificador de tarefas criadas. */
}


int main(void)
{
	/* Utilizando o heap_5.c para definir regioes.  */
	prvInitialiseHeap();

	/* Inicializa a gravação. */
	vTraceInitTraceData();
	xTickTraceUserEvent = xTraceOpenLabel("tick");

	// Criando tarefas.
	TaskHandle_t task1_handle = NULL;
	TaskHandle_t task2_handle = NULL;

	(void)createTask("Task1", 1000U, 3U, task1Code, task1_handle);
	(void)createTask("Task2", 500U,  1U, task2Code, task2_handle);
	
	// Inicia o sistema de agendamento em tempo real
	vTaskStartScheduler();
	for (;; );
	return 0;
}

void vApplicationMallocFailedHook(void)
{
	/* Função criada para quando for criada uma tarefa ou existir uma fila
	   para consultar tamanho do espaço livre que resta.
	*/
	vAssertCalled(__LINE__, __FILE__);
}

void vApplicationIdleHook(void)
{

}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Verificação para checar se houve bufferoverflow */
	vAssertCalled(__LINE__, __FILE__);
}

void vApplicationTickHook(void)
{

}

void vAssertCalled(unsigned long ulLine, const char * const pcFileName)
{
	static portBASE_TYPE xPrinted = pdFALSE;
	volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

	/* Parametos não utilizados. */
	(void)ulLine;
	(void)pcFileName;

	printf("Linha %d, Arquivo %s\r\n", ulLine, pcFileName);

	taskENTER_CRITICAL();
	{
		/* Para a gravação */
		if (xPrinted == pdFALSE)
		{
			xPrinted = pdTRUE;
			if (xTraceRunning == pdTRUE)
			{
				vTraceStop();
				prvSaveTraceFile();
			}
		}

		/* Função de saida usando o debugger para um valor diferente de 0 */
		while (ulSetToNonZeroInDebuggerToContinue == 0)
		{
			__asm { NOP };
			__asm { NOP };
		}
	}
	taskEXIT_CRITICAL();
}

static void prvSaveTraceFile(void)
{
	FILE* pxOutputFile;

	pxOutputFile = fopen("Trace.dump", "wb");

	if (pxOutputFile != NULL)
	{
		fwrite(RecorderDataPtr, sizeof(RecorderDataType), 1, pxOutputFile);
		fclose(pxOutputFile);
		printf("\r\nSaida salva no Trace.dump\r\n");
	}
	else
	{
		printf("\r\nFalha na criação do arquivo\r\n");
	}
}

static void  prvInitialiseHeap(void)
{
  	static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
	volatile uint32_t ulAdditionalOffset = 19; 
	const HeapRegion_t xHeapRegions[] =
	{
		/* Inicia o endereço com o tamanho dos deslocamentos */
		{ ucHeap + 1,											mainREGION_1_SIZE },
		{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
		{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
		{ NULL, 0 }
	};

	/* Verifica se os tamanhos e deslocamentos definidos realmente se encaixam no array
    */
	configASSERT((ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE) < configTOTAL_HEAP_SIZE);

	vPortDefineHeapRegions(xHeapRegions);
}


