/**
 * @file soft_timer.c
 *
 * @brief Implementation of Software Timer.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "soft_timer.h"
#include "hmcu_timer.h"

#include "inttypes.h"
#include "time.h"

/*****************************************************************************
 * Private constants.
 *****************************************************************************/

//constants to support menu in soft_timer_init()
 #define NEW_SOFT 1
 #define ALL_SOFT 2
 #define ERASE_ONE 3
 #define SETUP 4
 #define DONE 5

/*****************************************************************************
 * Private macros.
 *****************************************************************************/

 /*****************************************************************************
 * Private types.
 *****************************************************************************/

//type of node in timers' linked-list
typedef struct node{
    soft_timer_t* timer;
    struct node * next;
    uint32_t cnt_n;
    uint32_t reload_ms_n;
    bool repeat_n;
    bool is_active;
    soft_timer_callback_t callback_n;
} node_t;

/*****************************************************************************
 * Prototypes for private functions.
 *****************************************************************************/

 //update all timers' CNTs
 void update_timers(void);

 //soft timers callback functions
 void callback(node_t * timer);

/*****************************************************************************
 * Global variables.
 *****************************************************************************/

//set of timers, implemented as a linked list, initially empty
node_t * head = NULL;

//number of active timers, initially none is active
int active = 0;

//number of allocated timers, initially there are none
int num_timers = 0;

//MCU interface
uint16_t timer_ctrl;
uint16_t timer_cnt;
uint16_t timer_rld;

/*****************************************************************************
 * Bodies of public functions.
 *****************************************************************************/

void soft_timer_init(void)
{
    printf("Initializing soft timer module \n");
    bool opt = true;
    int t = 0;
    while(opt)
    {
        printf("Please select what to do next:\n");
        printf("1 - Create soft timer\n");
        printf("2 - See all soft timers\n");
        printf("3 - Delete one soft timer\n");
        printf("4 - Setup soft timer");
        printf("5 - Setup done\n");
        scanf("%d", &t);
        if (t == NEW_SOFT)
        {
            soft_timer_t * new_timer;
            soft_timer_create(&new_timer);
        }
        if (t == ALL_SOFT)
        {
            if (head == NULL)
            {
                printf("No timer created yet\n");
            }
            else
            {
                node_t * current = head;
                int i = 1;
                while (current!=NULL)
                {
                    printf("Timer ");
                    printf("%d", i);
                    if (current->is_active == true)
                    {
                        printf(": active\n");
                    }
                    else
                    {
                        printf(": not active\n");
                    }
                    current = current->next;
                    i++;
                }
            }
        }
        if (t == ERASE_ONE)
        {

        }
        if (t == SETUP)
        {

        }
        if (t == DONE)
        {
            opt = false;
            printf("DONE CONFIGURING SOFT TIMERS\n");
        }
    }
}

void soft_timer_create(soft_timer_t **pp_timer)
{
    //soft_timer_set(*ptimer);
    if (num_timers < SOFT_TIMER_MAX_INSTANCES)
    {
        //Allocates the timer created into the timers' set
        //softTimers[active] = pp_timer;
        if (head == NULL)
        {
            head = malloc(sizeof(node_t));
            head->timer = *pp_timer;
            head->next = NULL;
            head->is_active = false;
        }
        else
        {
            node_t * current = head;
            while (current->next!=NULL)
            {
                current = current->next;
            }
            current->next = malloc(sizeof(node_t));
            current->next->timer = *pp_timer;
            current->next->next = NULL;
            current->next->is_active = false;
        }
        active++;
    }
    else
    {
        printf("Timers' number limit achieved!!!\n\n");
    }
}

soft_timer_status_t soft_timer_set(soft_timer_t *p_timer, soft_timer_callback_t  timeout_cb, uint32_t reload_ms, bool repeat)
{
    soft_timer_status_t status;
    if (reload_ms > SOFT_TIMER_MAX_RELOAD_MS)
    {
        status = SOFT_TIMER_STATUS_INVALID_PARAMETER;
    }
    else
    {
        status = SOFT_TIMER_STATUS_SUCCESS;
        node_t * current = head;
        while (current->timer!= p_timer)
        {
            current = current->next;
        }
        current->reload_ms_n = reload_ms;
        current->repeat_n = repeat;
    }
    return status;
}

soft_timer_status_t soft_timer_start(soft_timer_t *p_timer)
{
    soft_timer_status_t status;

    node_t * current = head;
    while (current->timer!= p_timer)
    {
        current = current->next;
    }
    current->cnt_n = current->reload_ms_n;
    current->is_active = true;
    return status;
}

soft_timer_status_t soft_timer_stop(soft_timer_t *p_timer)
{
    soft_timer_status_t status;

    node_t * current = head;
    while (current->timer!= p_timer)
    {
        current = current->next;
    }

    current->is_active = false;

    return status;
}

void soft_timer_destroy(soft_timer_t **pp_timer)
{
    //basically remove timer from linked list
    if (head->timer == *pp_timer)
    {
        if (active == 1)
        {
            head = NULL;
        }
        else
        {
            head = head->next;
        }
    }
    else
    {
        node_t * current = head;
        node_t * last = malloc(sizeof(node_t));
        while (current->timer!= *pp_timer)
        {
            last = current;
            current = current->next;
        }
        current->next = NULL;
    }
}


/*****************************************************************************
 * Bodies of private functions.
 *****************************************************************************/

void update_timers(void)
{
    //update physical timer
    if (timer_cnt > 0)
    {
        timer_cnt --;
    }
    else
    {
        // checks RPT bit
        if ((timer_ctrl >> 2) & 1 == 1)
        {
            timer_cnt = timer_rld;
        }
    }
    //update soft timers
    node_t * current = head;
    while (current!=NULL)
    {
        if (current->is_active == true)
        {
            if (current->cnt_n > 0)
            {
                current->cnt_n --;
            }
            else if (current->cnt_n == 0)
            {
                if (current->repeat_n == true)
                {
                    current->cnt_n = current->reload_ms_n;
                }
            }
        }
        current = current->next;
    }
}

void callback(node_t * timer)
{
    printf("Finished timer");
    if (timer->repeat_n == true)
    {
        timer->cnt_n = timer_rld;
    }
    else
    {
        timer->is_active = false;
    }
}
