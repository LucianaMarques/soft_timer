#include <stdio.h>
#include <stdlib.h>
#include "hmcu_timer.h"
#include "soft_timer.h"
#include <stdbool.h>
#include "time.h"

//MCU registers
uint16_t timer_ctrl = 0;
uint16_t timer_cnt = 100;
uint16_t timer_rld = 100;

void init_MCU_timer();

void update_MCU_timer();

int main()
{
    init_MCU_timer();
    //soft_timer_init();
    clock_t start, end;
    float time1 = 0, clock_time = 1/(float)CLOCKS_PER_SEC;
    start = clock();
    end = clock();
    time1 = (float)(end - start)*clock_time;
    while(timer_cnt > 0)
    {
        end = clock();
        time1 = (float)(end - start)*clock_time;
        if (time1 > clock_time)
        {
            update_MCU_timer();
            start = clock();
        }
        if ((timer_cnt == 0) && ((timer_ctrl >> 2) & 1))
        {
            timer_cnt = timer_rld;
        }
    }
    //printf("%f\n",time1);
    //float time2 = (float)clock()*clock_time;
    //printf("%f\n",time2);
    return 0;
}

void init_MCU_timer()
{
    //Configuring the MCU's physical timer
    printf("Please type the ")

}

void update_MCU_timer()
{
    printf("updated timer\n");
    timer_cnt--;
}
