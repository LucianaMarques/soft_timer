#include <stdio.h>
#include <stdlib.h>
#include "hmcu_timer.h"
#include "soft_timer.h"
#include <stdbool.h>
#include "time.h"

int main()
{
    //freopen("output.txt","w",stdout);

    //Initializes and sets the MCU's timer
    init_MCU_timer();

    time_setup();

    soft_timer_init();

    timers_working();

    return 0;
}
