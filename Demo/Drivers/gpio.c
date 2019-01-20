/*
    FreeRTOS V7.2.0 - Copyright (C) 2012 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/**
 *	Quick and very Dirty GPIO API.
 *
 **/

#include "gpio.h"
#include <stdint.h>


static inline void mmio_write(uint32_t reg, uint32_t data);
static inline uint32_t mmio_read(uint32_t reg);

typedef struct {
	unsigned long	GPFSEL[6];	///< Function selection registers.
	unsigned long	Reserved_1;
	unsigned long	GPSET[2];
	unsigned long	Reserved_2;
	unsigned long	GPCLR[2];
	unsigned long	Reserved_3;
	unsigned long	GPLEV[2];
	unsigned long	Reserved_4;
	unsigned long	GPEDS[2];
	unsigned long	Reserved_5;
	unsigned long	GPREN[2];
	unsigned long	Reserved_6;
	unsigned long	GPFEN[2];
	unsigned long	Reserved_7;
	unsigned long	GPHEN[2];
	unsigned long	Reserved_8;
	unsigned long	GPLEN[2];
	unsigned long	Reserved_9;
	unsigned long	GPAREN[2];
	unsigned long	Reserved_A;
	unsigned long	GPAFEN[2];
	unsigned long	Reserved_B;
	unsigned long	GPPUD[1];
	unsigned long	GPPUDCLK[2];
	//Ignoring the reserved and test bytes
} BCM2835_GPIO_REGS;

volatile BCM2835_GPIO_REGS * const pRegs = (BCM2835_GPIO_REGS *) GPIO_BASE_ADDR;


void SetGpioFunction(unsigned int pinNum, unsigned int funcNum) {

	int offset = pinNum / 10;

	unsigned long val = pRegs->GPFSEL[offset];	// Read in the original register value.

	int item = pinNum % 10;
	val &= ~(0x7 << (item * 3));
	val |= ((funcNum & 0x7) << (item * 3));
	pRegs->GPFSEL[offset] = val;
}

void SetGpioDirection(unsigned int pinNum, enum GPIO_DIR dir) {
	SetGpioFunction(pinNum,dir);
}

void SetGpio(unsigned int pinNum, unsigned int pinVal) {
	unsigned long offset=pinNum/32;
	unsigned long mask=(1<<(pinNum%32));

	if(pinVal) {
		pRegs->GPSET[offset]|=mask;
	} else {
		pRegs->GPCLR[offset]|=mask;
	}
}

int ReadGpio(unsigned int pinNum) {
	return ((pRegs->GPLEV[pinNum/32])>>(pinNum%32))&1;
}

void EnableGpioDetect(unsigned int pinNum, enum DETECT_TYPE type)
{
	unsigned long mask=(1<<pinNum);
	unsigned long offset=pinNum/32;
	
	switch(type) {
	case DETECT_RISING:
		pRegs->GPREN[offset]|=mask;
		break;
	case DETECT_FALLING:
		pRegs->GPFEN[offset]|=mask;
		break;
	case DETECT_HIGH:
		pRegs->GPHEN[offset]|=mask;
		break;
	case DETECT_LOW:
		pRegs->GPLEN[offset]|=mask;
		break;
	case DETECT_RISING_ASYNC:
		pRegs->GPAREN[offset]|=mask;
		break;
	case DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[offset]|=mask;
		break;
	case DETECT_NONE:
		break;
	}
}

void DisableGpioDetect(unsigned int pinNum, enum DETECT_TYPE type)
{
	unsigned long mask=~(1<<(pinNum%32));
	unsigned long offset=pinNum/32;
	
	switch(type) {
	case DETECT_RISING:
		pRegs->GPREN[offset]&=mask;
		break;
	case DETECT_FALLING:
		pRegs->GPFEN[offset]&=mask;
		break;
	case DETECT_HIGH:
		pRegs->GPHEN[offset]&=mask;
		break;
	case DETECT_LOW:
		pRegs->GPLEN[offset]&=mask;
		break;
	case DETECT_RISING_ASYNC:
		pRegs->GPAREN[offset]&=mask;
		break;
	case DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[offset]&=mask;
		break;
	case DETECT_NONE:
		break;
	}
}
static inline void delay(int32_t count)
{
		asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
						 : "=r"(count): [count]"0"(count) : "cc");
}
void uart_init()
{
	mmio_write(UART_BASE_ADDR + 0x30, 0x00000000);
	mmio_write(GPIO_BASE_ADDR + 0x94, 0x00000000);
	delay(150);
	mmio_write(GPIO_BASE_ADDR + 0x98, (1 << 14) | (1 << 15));
	delay(150);
	mmio_write(GPIO_BASE_ADDR + 0x98, 0x00000000);
	mmio_write(UART_BASE_ADDR + 0x44, 0x7FF);
	mmio_write(UART_BASE_ADDR + 0x24, 1);
	mmio_write(UART_BASE_ADDR + 0x28, 40);
	mmio_write(UART_BASE_ADDR + 0x2C, (1 << 4) | (1 << 5) | (1 << 6));
	mmio_write(UART_BASE_ADDR + 0x38, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
						       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
	mmio_write(UART_BASE_ADDR + 0x30, (1 << 0) | (1 << 8) | (1 << 9));
}
void ClearGpioInterrupt(unsigned int pinNum)
{
	unsigned long mask=(1<<(pinNum%32));
	unsigned long offset=pinNum/32;

	pRegs->GPEDS[offset]=mask;
}
static inline void mmio_write(uint32_t reg, uint32_t data)
{
		*(volatile uint32_t*)reg = data;
}
 
// Memory-Mapped I/O input
static inline uint32_t mmio_read(uint32_t reg){
 	return *(volatile uint32_t*)reg;
}

void uart_putc(unsigned char c){
	// Wait for UART to become ready to transmit.
 	while ( mmio_read(UART_BASE_ADDR + 0x18) & (1 << 5) ) { }
 		mmio_write(UART_BASE_ADDR, c);
}		 
