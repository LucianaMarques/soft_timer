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
#include "math.h"

/*****************************************************************************
 * Private constants.
 *****************************************************************************/

//constants to support menu in soft_timer_init()
#define NEW_SOFT 1
#define ALL_SOFT 2
#define SETUP 3
#define DONE 4

//MCU registers
uint16_t timer_ctrl;
uint16_t timer_cnt;
uint16_t timer_rld;

//MCU timer's PSC
float PSC;

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
void callback(soft_timer_t * timer);

//MCU's timer setup
void init_MCU_timer();

//Update MCU's timer
void update_MCU_timer();

//setup time passing variables
void time_setup();

//all timers working
void timers_working();

//find a certain timer in the linked list
void activate_timers();

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

//Variables and methods used to control time (major reference is "time.h" library
clock_t start, end;
float time1 = 0;

//Time to complete a clock cycle
float clock_time;

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
        printf("3 - Setup soft timer");
        printf("4 - Setup done\n");
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
        if (t == SETUP)
        {
            int i, reload, repeat;
            printf("Please type the number of timer to be setted: \n");
            scanf("%d", &i);
            printf("Please type reload value: \n");
            scanf("%d", &reload);
            printf("Should it repeat? 0-no 1-yes\n");
            scanf("%d", &repeat);
            node_t* current = head;
            //suppose that user won't try to access a timer that's not on the linked list
            for (int j = 1; j < i; j++)
            {
                current = current->next;
            }
            soft_timer_callback_t callb = callback(current->timer);
            soft_timer_status_t c = soft_timer_set(&current->timer, callb , reload, repeat);
            printf("%s",c);
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
        current->callback_n = timeout_cb;
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

soft_timer_status_t soft_timer_stop(soft_timer_t * p_timer)
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
        last->next = current->next;
        current = NULL;
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

void callback(soft_timer_t * timer)
{
    printf("Finished timer");
    node_t * current = head;
    //assuming user won't try to access a timer that doesn't exists
    while (current->timer != timer)
    {
        current = current->next;
    }
    if (current->repeat_n == true)
    {
        current->cnt_n = timer_rld;
    }
    else
    {
        current->is_active = false;
    }
}

void init_MCU_timer()
{
    //Configuring the MCU's physical timer
    printf("Please type TIMER_CTRL Register: \n");
    scanf("%" SCNd16, &timer_ctrl);
    printf("Please type TIMER_RLD Register: \n");
    scanf("%" SCNd16, &timer_rld);

    //sets TIMER_CNT Register
    timer_cnt = timer_rld;

    //sets MCU Timer's PSC
    PSC = (timer_ctrl >> 2) & 1;
}

void time_setup()
{
    float frq = TIMER_CLK_HZ/pow(10,PSC);
    clock_time = 1/frq;
    start = clock();
    end = clock();
    time1 = (float)(end - start)*clock_time;
}

void timers_working()
{
    activate_timers();
    while(timer_cnt > 0)
    {
        //keeps tracking time at every clock_time
        end = clock();
        time1 = (float)(end - start)*clock_time;

        //if clock_time achieved, update timers
        if (time1 > clock_time)
        {
            update_timers();
            start = clock();
        }
    }
}

void activate_timers()
{
    node_t * node = head;
    while (node != NULL)
    {
        node->is_active = true;
        node = node->next;
    }
}
