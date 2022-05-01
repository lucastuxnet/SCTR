/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * This project provides two demo applications.  A simple blinky style project,
 * and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.
 * The simply blinky demo is implemented and described in main_blinky.c.  The
 * more comprehensive test and demo application is implemented and described in
 * main_full.c.
 *
 * This file implements the code that is not demo specific, including the
 * hardware setup and FreeRTOS hook functions.
 *
 *******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *
 *
 *******************************************************************************
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <intrin.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	1

#define mainREGION_1_SIZE	8201
#define mainREGION_2_SIZE	29905
#define mainREGION_3_SIZE	7607

/*-----------------------------------------------------------*/
/*
 * main_blinky() é usado quando mainCREATE_SIMPLE_BLINKY_DEMO_ONLY esta configurado para 1.
 * main_full() é usado quando mainCREATE_SIMPLE_BLINKY_DEMO_ONLY esta configurado para 0.
 */
extern void main_blinky( void );
extern void main_full( void );


void vFullDemoTickHookFunction( void );
void vFullDemoIdleFunction( void );

static void  prvInitialiseHeap( void );

void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

static void prvSaveTraceFile( void );

StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

/* Observa se o rastreamento está em execução ou não. */
static BaseType_t xTraceRunning = pdTRUE;

/*-----------------------------------------------------------*/
//Task 1-------------------------------------------------------

void Task1()
{
	while (1)
	{
		printf("This is task 1 \n"); // Imprime a Task1
		fflush(stdout);				
		vTaskDelay(100);			 // Definido o delay para 100mS

	}
}

//Task 2--------------------------------------------------------

void Task2()
{
	while (1)
	{
		printf("This is task 2 \n");  // Imprime a Task 2
		fflush(stdout);
		vTaskDelay(500);			  // Definido o delay para 500mS

	}
}



/*-----------------------------------------------------------*/
int main( void )
{
	
	prvInitialiseHeap();

	vTraceEnable( TRC_START );
	
	
	xTaskHandle TaskHandler;                                    // Criada a Task Handler com o nome de TaskHandler

	xTaskCreate(Task1, "Task1", 1000, NULL, 3, &TaskHandler);   // Pilha com tamanho de 1000 e prioridade 3 
	xTaskCreate(Task2, "Task2", 100, NULL, 1, &TaskHandler);	// Pilha com tamanho 100 e prioridade 1
	vTaskStartScheduler();										// Iniciando o Scheduler

	for (;;);

	return 0;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
	size of the	heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains
	(although it does not provide information on how the remaining heap might be
	fragmented).  See http://www.freertos.org/a00111.html for more
	information. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	
	#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{

		vFullDemoIdleFunction();
	}
	#endif
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{

	#if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		vFullDemoTickHookFunction();
	}
	#endif /* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
	/* Esta função será chamada apenas uma vez, quando a tarefa daemon começa a
	executar (às vezes chamada de tarefa de cronômetro). Isso é útil se o
	aplicativo inclui código de inicialização que se beneficiaria com a execução
	depois que o programador foi iniciado. */
}
/*-----------------------------------------------------------*/

void vAssertCalled(unsigned long ulLine, const char* const pcFileName)
{
	static BaseType_t xPrinted = pdFALSE;
	volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

		
	(void)ulLine;
	(void)pcFileName;

	printf("ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError());

	taskENTER_CRITICAL();
	{
		
		if (xPrinted == pdFALSE)
		{
			xPrinted = pdTRUE;
			if (xTraceRunning == pdTRUE)
			{
				vTraceStop();
				prvSaveTraceFile();
			}
		}

		/* Cause debugger break point if being debugged. */
		__debugbreak();

		/* Você pode sair desta função para depurar a asserção usando
		o depurador para definir ulSetToNonZeroInDebuggerToContinue para um valor diferente de zero */
		
		while (ulSetToNonZeroInDebuggerToContinue == 0)
		{
			__asm { NOP };
			__asm { NOP };
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile( void )
{
FILE* pxOutputFile;

	fopen_s( &pxOutputFile, "Trace.dump", "wb");

	if( pxOutputFile != NULL )
	{
		fwrite( RecorderDataPtr, sizeof( RecorderDataType ), 1, pxOutputFile );
		fclose( pxOutputFile );
		printf( "\r\nTrace output saved to Trace.dump\r\n" );
	}
	else
	{
		printf( "\r\nFailed to create trace dump file\r\n" );
	}
}
/*-----------------------------------------------------------*/

static void  prvInitialiseHeap(void)
{
	
	static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
	volatile uint32_t ulAdditionalOffset = 19; /* Apenas para evitar avisos de 'condição é sempre verdadeira' em configASSERT (). */
	const HeapRegion_t xHeapRegions[] =
	{
		/* endreço inicial com dummy offsets						tamanho */
		{ ucHeap + 1,											mainREGION_1_SIZE },
		{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
		{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
		{ NULL, 0 }
	};

	/* Verifique se os tamanhos e deslocamentos definidos realmente se encaixam no
	array. */
	configASSERT((ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE) < configTOTAL_HEAP_SIZE);

	/* Evita avisos do compilador quando configASSERT () não está definido. */
	(void)ulAdditionalOffset;

	vPortDefineHeapRegions(xHeapRegions);
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION é definido como 1, então o aplicativo deve fornecer um
implementação de vApplicationGetIdleTaskMemory () para fornecer a memória que é
usado pela tarefa ociosa. */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize)
{
	/* Se os buffers a serem fornecidos para a tarefa inativa são declarados dentro deste 
	função, então eles devem ser declarados estáticos - caso contrário, eles serão alocados dentro
	a pilha e, portanto, não existe após a saída desta função. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	/* Passe um ponteiro para a estrutura StaticTask_t na qual a tarefa Idle
	estado será armazenado. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Distribua a matriz que será usada como pilha da tarefa ociosa. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Passe o tamanho do array apontado por * ppxIdleTaskStackBuffer.
	Observe que, como a matriz é necessariamente do tipo StackType_t,
	configMINIMAL_STACK_SIZE é especificado em palavras, não em bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION e configUSE_TIMERS são definidos como 1, então o
o aplicativo deve fornecer uma implementação de vApplicationGetTimerTaskMemory ()
para fornecer a memória que é usada pela tarefa de serviço de Timer. */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize)
{
	/* Se os buffers a serem fornecidos para a tarefa Timer forem declarados dentro deste
	função, então eles devem ser declarados estáticos - caso contrário, eles serão alocados dentro
	a pilha e, portanto, não existe após a saída desta função. */
	static StaticTask_t xTimerTaskTCB;

	/* Passe um ponteiro para a estrutura StaticTask_t na qual o Timer
	o estado da tarefa será armazenado. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Distribua o array que será usado como pilha da tarefa do Timer. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Passe o tamanho do array apontado por * ppxTimerTaskStackBuffer.
	Observe que, como a matriz é necessariamente do tipo StackType_t,
	configMINIMAL_STACK_SIZE é especificado em palavras, não em bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

