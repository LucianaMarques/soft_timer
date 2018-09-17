#include <stdio.h>
#include <stdlib.h>
#include "hmcu_timer.h"
#include "soft_timer.h"
#include <stdbool.h>
#include "time.h"

int main()
{
    //Initializes and sets the MCU's timer
    init_MCU_timer();

    soft_timer_init();

    time_setup();

    timers_working();

    return 0;
}
