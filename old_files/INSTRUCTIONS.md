# Soft Timer Exercise

This is a problem proposed to exercise embedded C software development skills.

## Problem

We have a hypothetical MCU with a single available physical timer. Our embedded application, however,
will need to employ many timers to perform its intended tasks.

## Goal

The main goal of this exercise is to develop a software timer (a.k.a. soft timer) component that
will multiplex our hypothetical MCU's single physical timer.
By making use of soft timer, our embedded application must be able to have more than one virtual timer at its disposal.

## Requirements

Our embedded application may need to schedule timers with periods ranging from 1 ms to 100,000,000 ms.

To achieve that, it is acceptable that timers configured with periods longer than 999 ms will occasionally
suffer from incremental imprecisions of up to 1000 ms (e.g. in case a timer gets started because of an
asynchronous event, such as a button interrupt).

## What is provided

Under **inc/** folder, soft_timer.h header file is provided with all the types, constants,
and function prototypes that must be publicly available for our soft timer component.
This file should not be changed.

Under **inc/** folder, hmcu_timer.h header file is provided with constants describing the addresses of
timer registers in our hypothetical MCU, as well as the prototype for the timer IRQ handler function
called by the interrupt vector whenever timer countdown reaches zero. This file should not be changed.

Under **src/** folder, file soft_timer.c header file is provided with a skeleton for implementing the
soft timer component.

## Our hypothetical MCU

The interface for our hypothetical MCU's single physical timer is comprised by the following three 16-bit registers.

### TIMER_CTRL Register

| Bit  | Name  | Function                                                                                                                                           |
|------|-------|----------------------------------------------------------------------------------------------------------------------------------------------------|
| 0    | IRQE  | Enable timer IRQ. When set to 1, timer interrupts are enabled.                                                                                     |
| 1    | START | Start / stop timer. When set to 1, timer is running. When set to 0, timer is stopped.                                                              |
| 2    | RPT   | Repeat timer. If set to 1, TIMER_CNT register will be reloaded with contents of TIMER_RLD register
               | when contents of TIMER_CNT register reach 0.    |
| 3:4  | PRSC  | Timer prescaler. Timer clock frequency is 1000 Hz. Prescaler bits 3:4 can be set to values
               | 0, 1, 2, and 3. Timer clock will be divided by 10^PRSC. |
| 5:15 | RFU   | Reserved for future use.                                                                                                                           |

### TIMER_CNT Register

| Bit  | Name  | Function                                |
|------|-------|-----------------------------------------|
| 0:15 | COUNT | Timer counter. Current countdown value. |


### TIMER_RLD Register

| Bit  | Name   | Function                               |
|------|--------|----------------------------------------|
| 0:15 | RELOAD | Timer reload. Initial countdown value. |

## Additional instructions

For testing purposes, it is expected that the implemented library will compile using the GNU C Compiler (
GCC: https://gcc.gnu.org/) for the x86-64 architecture.

All code and comments should be written in English.

There is no need to follow any particular standard for coding style, but employing a consistent style
for coding will be much appreciated.

## Questions

For questions related to this exercise, please contact Henrique (henrique@devtecnologia.com.br).
