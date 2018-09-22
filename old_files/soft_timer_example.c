#include "soft_timer.c"

static bool m_time_is_up;

void soft_timer_callback(soft_timer_t *p_timer)
{
    (void) p_timer;

    m_time_is_up = true;
}

void main(void)
{
    m_time_is_up = false;
    
    soft_timer_init();

    soft_timer_t *p_timer = NULL;

    soft_timer_create(&p_timer);

    soft_timer_set(p_timer,
                   soft_timer_callback,
                   10000,
                   true);

    soft_timer_start(p_timer);

    while (!m_time_is_up);

    soft_timer_stop(p_timer);

    soft_timer_destroy(&p_timer);
}
