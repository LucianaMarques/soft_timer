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

cd "directory"

make

./soft_timer

make clean


