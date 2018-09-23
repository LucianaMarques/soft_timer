# soft_timer

This program was written to simulate a multiplexed timer from an hypotetical MCU, for part of the interviewing process for an embbed programming internship at Dev Tecnologia. 

It consists on four main files:

* hmcu_timer.h
 Contains the hypotetical MCU's general characteristics, registers and methods.
* soft_timer.h
 Contains the soft timers' general characteristics, registers and methods.
* soft_timer.c
 Contains both private and global methods (these defined previously in the soft_timer.h header file).

The first two files were provided by the company and not modified by the candidate. 

The second two were used for implementation and testing. 

There is also a simple Makefile provided, created by the candidate. 

## Instructions

To run the project, save all files in a directory. 
With the command line, go to the created directory.
Then, type "make". This should create a soft_timer binary file.
Then execute the created file.
To exclude the binary file after execution, just type make clean.

$cd "directory"

$make

$./soft_timer

$make clean

## Project's General Overview

This section discusses the implementation of the hypotetical MCU's behavior and which functions were used to do so. For a detailed description of all the functions in the project, please refer to the next section.

### MCU's behavior

The MCU's behavior is defined accordingly to the main() function defined in the source file soft_timer.c. It can be described in the following simple steps:

* Define the physical timer characteristics, that is, define the Prescaler, the reload value, whether it repeats or not and if IRQ handler if Timeout IRQ handler is required. 

* Setup the system's clock frequency

* Initialize the soft timer module (enables the creation of soft timers)

* Initialize all timers (both physical and soft) 

* Stop all timers

* Destroy all soft timers

All these steps are defined in the main() function. 

### Project's Functions and files 

In the header file soft_timer.h, there were some declared function which were expected to be written in the source file.

The sorce file soft_timer.c contains all the required functions in its header, plus some private functions used to implement the hypotetical MCU's behavior.

Below there are the body and the description of all these functions:

### Global functions

### Private functions


