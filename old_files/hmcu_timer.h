/**
 * @file hmcu_timer.h
 *
 * @brief Hypothetical MCU Timer register addresses and IRQ handler.
 */

#ifndef __HMCU_TIMER_H__
#define __HMCU_TIMER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*****************************************************************************
 * Timer clock frequency.
 *****************************************************************************/

#define TIMER_CLK_HZ            (1000)

/*****************************************************************************
 * Timer register addresses.
 *****************************************************************************/

#define TIMER_CTRL_REG_ADDR (&timer_ctrl)
#define TIMER_CNT_REG_ADDR  (&timer_cnt)
#define TIMER_RLD_REG_ADDR  (&timer_rld)

/*****************************************************************************
 * Timer IRQ handler prototype.
 *****************************************************************************/

/**
 * @brief Timeout IRQ handler.
 *
 * @details This function will be called by our hypothetical MCU interrupt
 *          vector when timer countdown reaches zero.
 */
void hmcu_timer_irq_handler(void);

/*****************************************************************************
 * For test purposes: variables abstracting hypothetical MCU registers.
 *****************************************************************************/

extern uint16_t timer_ctrl;
extern uint16_t timer_cnt;
extern uint16_t timer_rld;

#endif /** __HMCU_TIMER_H__ */
