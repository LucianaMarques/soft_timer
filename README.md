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

The MCU's behavior is defined accordingly to the main() function defined in the source file soft_timer.c. It can be defined with the following simple steps:

* Define the physical timer characteristics, that is, define the Prescaler, the reload value, whether it repeats or not and if IRQ handler if Timeout IRQ handler is required. 

* Setup the system's clock frequency

* Initialize the soft timer module (enables the creation of soft timers)

* Initialize all timers (both physical and soft) 

* Stop all timers

* Destroy all soft timers

All these steps are defined in the main() function. 

### Project's Internal Organization

This project consists in the physical timer and the possible software timers. 

Some assumptions adopted:

1. All timers (both physical and soft) should start together;
2. Soft timers will be updated at the same time as the physical timer (following the system's clock frequency);

#### Physical timer

To implement and test the physical timer behavior, there are three extern variables (provided in the header file hmcu_timer.h) that simulate its main registers (CTRL, CNT and RELOAD). 

In order to set CTRL's value, the user inputs the PRSC value, whether the timer should repeat or not and whether the timer's IRQ Handler should be enabled or not. This set is done in the init_ MCU _ timer() function (private functions defined in source file soft_timer.c).

#### Soft timer

The program allows the user to create and set a certain amount of timers, defined in SOFT_ TIMER _ MAX _ INSTANCES in header file soft_timer.h 

To keep track of how many timers should run in the program and their characteristics, there is a structure defined with all relevant features (cnt, reload value, callback function, etc) that behaves as a node in a linked list. The system has a head node (initially NULL) that is later redefined as the user creates the first soft timer, and as the following soft timers are creted, this list increases. 

The soft timer behavior is also defined with the global function that required to be defined, like soft_ timer _ init(), soft _ timer _ create (), among others (better described in the next section). 

The cycle of life of a soft timer consists on:

* soft timer is created
* soft timer is activated
* soft timer runs
* soft timer stops
* soft timer is destroyed

#### Time Management

To keep track of time, the program uses the c library "time.h".

When the PRSC value is inputed by the user, the physical timer's frequency is calculated and thus also the clock cycle time.

The function that keeps the timers working is timers_ working() in source file soft_timer.c. It keeps calculating how much time has passed since the last update. Once this time is equal or greater than the clock cycle time, all timer are updated (with update _timer() function) and time counting restarts.   
 

#### Some Engineering Trade-Offs

Since using c programming language was a requirement, this solution was made using a linked list to store all the soft timers created. 
This decision was made due to implementation issues: a linked list can be easily implemented using structs.  
However, it is noted that a solution using hash tables would possibly be better as for complexity, but the linked lists were prefered in terms of deadlines (there aren't much already implemented hash table resourses in c language, differently from c++). 

## Project's Functions and files 

In the provided header files, there were some declared function which were expected to be written in the source file.

The sorce file soft_timer.c contains all the required functions in its header, plus some private functions used to implement the hypotetical MCU's behavior.

The bodies and the description of all these functions can be found below.

### Global functions

For the complete body functions, please refer to lines 114-271 in soft_ timer.c file. 

The global functions that required defining were:

* void hmcu_timer_irq_handler(void);

Function used by physical timer when the countdown reaches zero. Can be enabled or not.

* void soft_timer_init(void)

This function initialized the soft timers module. It has a running menu in which the user can create and set new soft timers. 

* void soft_timer_create(soft_timer_t **pp_timer)

Basically creates another soft timer. This is done by creating a new node (if head is already defined) in the general soft timer linked list. 

* soft_timer_status_t soft_timer_set(soft_timer_t *p_timer, soft_timer_callback_t  timeout_cb, uint32_t reload_ms, bool repeat)

This function is called upon creating a new soft timer. It is used to set it, defining the callback function, the reload value and if it should repeat or not. As for the callback function, there is only one avaiable in the whole system, but it is noted that there could be more callback functions avaiable depending on the soft timer's part in the MCU's behavior, and the user could be given the choice to select one of these callback functions.

* soft_timer_status_t soft_timer_start(soft_timer_t *p_timer)

Activates an specific soft timer. It searches on the linked list for this specific timer and changes the boolean value of "is_active" to true.

* soft_timer_status_t soft_timer_stop(soft_timer_t *p_timer)

Stops an specific soft timer. It searches on the linked list for this specific timer and changes the boolean value of "is_active" to false.

* void soft_timer_destroy(soft_timer_t **pp_timer)

Destroys a soft timer, basically removing it from the linked list.

### Private functions

The following functions were created to support the MCU's behavior. For the functions full bodies please refer to lines 277-448 in source file soft_ timer.c

* void update_timers(void)

This function basically updates the CNT values of all timers, both physical and soft. It is called when the system detected that a clock cycle time has passed.

* void callback(soft_timer_t * timer)

The callback function used by soft timers. As said before, there is only one callback function provided. However, the system could provide more callback functions and the user could choose which one to use in the soft timer set function.

* void init_MCU_timer(void)

Takes input for PRSC, whether the physical timer shoudl restart or not, RELOAD value and if TIMER IRQ should be enabled.

* void time_setup(void)

Calculates the system's frequency and clock cycle time. Initiates the time passing trackdown.

* void timers_working(void)

Enables timers to work. First, it activates them, and while CNT is not zero, it keeps updating all timers' cnts when a clock cycle ends. If the physical timer is set to reload, once CNT is zero it sets CNT to RELOAD value and continues the loop.

* void activate_timers(void)

Starts all soft timers using soft_timer_start(* p_timer) function.

* void stop_timers()

Stops all soft timers using soft_timer_stop(* p_timer) function.

* void destroy_soft_timers(void)

Destroy all soft timers using the soft_timer_destroy(** p_timer) function.

## A simple program test

A simple program test can be seen on output.txt. The program was run on the commandline and the outputs were written on the file with the following:

$ ./soft_timer >outuput.txt

The user's input still needed to be done on the command line, though. 

The PRSC value was 3, RELOAD value was 10 and Timer IRQ was enabled. Only one soft timer was created, with reload value of 5. None of the timers were setted to repeat.


