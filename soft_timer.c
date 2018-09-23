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

#include "/usr/include/inttypes.h"
#include "/usr/include/time.h"
#include "/usr/include/math.h"

/*****************************************************************************
 * Private constants.
 *****************************************************************************/

//constants to support menu in soft_timer_init()
#define NEW_SOFT 1
#define DONE 2

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
    uint16_t cnt_n;
    uint16_t reload_ms_n;
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
void init_MCU_timer(void);

//Update MCU's timer
void update_MCU_timer(void);

//setup time passing variables
void time_setup(void);

//all timers working
void timers_working(void);

//find a certain timer in the linked list
void activate_timers(void);

//stop all timers
void stop_timers(void);

//destroy soft timers that havent already stopped
void destroy_soft_timers(void);

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

//Timer clock frequency
float frq;

/*****************************************************************************
 * Bodies of public functions.
 *****************************************************************************/
void hmcu_timer_irq_handler(void)
{
    printf("irq requested\n");
}

void soft_timer_init(void)
{
    printf("Initializing soft timer module \n");
    bool opt = true;
    int t = 0;
    while(opt)
    {
        printf("Please select what to do next:\n");
        printf("1 - Create soft timer\n");
        printf("2 - Setup done\n");
        scanf("%d", &t);
        if (t == NEW_SOFT)
        {
            soft_timer_t * new_timer;
            soft_timer_create(&new_timer);

            printf("SETTING CREATED TIMER\n");
            uint32_t reload, r;
            bool repeat;
            printf("Please type reload value: \n");
            scanf("%u", &reload);
            printf("Should it repeat? 0-no 1-yes\n");
            scanf("%u", &r);
            if (r == 0) repeat = false;
            else repeat = true;
            soft_timer_status_t c = soft_timer_set(new_timer, callback , reload, repeat);
            if (c == SOFT_TIMER_STATUS_SUCCESS)
            {
                printf("Soft timer set success\n");
            }
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
        printf("SOFT TIMER CREATE SUCCES\n");
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
    return status = SOFT_TIMER_STATUS_SUCCESS;
}

soft_timer_status_t soft_timer_stop(soft_timer_t *p_timer)
{
    soft_timer_status_t status = SOFT_TIMER_STATUS_SUCCESS;

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

int main()
{
    //freopen("output.txt","w",stdout);

    //Initializes and sets the MCU's timer
    init_MCU_timer();

    time_setup();

    soft_timer_init();

    timers_working();

    stop_timers();

    destroy_soft_timers();

    return 0;
}

void update_timers(void)
{
    //update physical timer
    timer_cnt --;
    printf("HMCU CNT: %u\n", timer_cnt);
    //update soft timers
    node_t * current = head;
    int i = 1;
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
                else
                {
                    soft_timer_status_t c = soft_timer_stop(current->timer);
                    if (c == SOFT_TIMER_STATUS_SUCCESS)
                    {
                        printf("TIMER %d STOPPED\n",i);
                    }
                }
                callback(current->timer);
            }
            printf("TIMER %d CNT: %u\n", i, current->cnt_n);
        }
        current = current->next;
        i++;
    }
}

void callback(soft_timer_t * timer)
{
    printf("Finished timer\n");
}

void init_MCU_timer(void)
{
    //Configuring the MCU's physical timer
    printf("Please type PRSC value: \n");
    scanf("%f", &PSC);
    printf("Should it repeat? 0-no 1-yes\n");
    int rep;
    scanf("%d", &rep);
    printf("Enable Timer IRQ?\n");
    int irq;
    scanf("%d", &irq);

    //defining TIMER_CTRL
    timer_ctrl = (uint16_t)irq + ((uint16_t)rep << 2) + ((uint16_t)PSC << 3);

    //defining TIMER_RLD
    printf("Please type TIMER_RLD Register's value: \n");
    scanf("%" SCNd16, &timer_rld);

    //sets TIMER_CNT Register
    timer_cnt = timer_rld;
}

void time_setup(void)
{
    printf("SETTING UP TIME CHRACTERISTICS\n");
    frq = TIMER_CLK_HZ/pow(10,PSC);
    printf("FREQUENCY: %lf\n", frq);
    clock_time = 1/frq;
    printf("TIME IN A CLOCK CYCLE: %lf seconds \n", clock_time);
    start = clock();
    end = clock();
    time1 = (float)(end - start)*clock_time;
}

void timers_working(void)
{
    activate_timers();
    while(timer_cnt > 0)
    {
        //keeps tracking time at every clock_time
        end = clock();
        time1 = (float)(end - start)*clock_time;
        printf("Current time: %lf \n", time1);

        //if clock_time achieved, update timers
        if (time1 > clock_time)
        {
            update_timers();
            if (timer_cnt == 0)
            {
                if ((timer_ctrl >> 2) & 1)
                {
                    timer_cnt = timer_rld;
                }
                if (timer_ctrl & 1)
                {
                    hmcu_timer_irq_handler();
                }
            }
            start = clock();
        }
    }
}

void activate_timers(void)
{
    printf("ACTIVATING ALL SOFT TIMERS CREATED\n");
    node_t * node = head;
    int i = 1;
    while (node != NULL)
    {
        soft_timer_status_t c = soft_timer_start(node->timer);
        if (c == SOFT_TIMER_STATUS_SUCCESS)
        {
            printf("SOFT TIMER %d STARTED \n", i);
        }
        i++;
        node = node->next;
    }
}

void stop_timers()
{
    //stop hmcu
    timer_ctrl = timer_ctrl - 2;

    //stop soft timers
    int i = 0;
    node_t * current = head;
    while (i < active)
    {
        soft_timer_stop(current->timer);
        i++;
    }
}

void destroy_soft_timers(void)
{
    int i = 0;
    node_t * current = head;
    while (i < active)
    {
        soft_timer_destroy(&current->timer);
        i++;
    }
    printf("ALL SOFT TIMERS DESTROYED\n");
}
