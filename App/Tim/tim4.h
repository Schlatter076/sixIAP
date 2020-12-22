/*
 * tim4.h
 *
 *  Created on: 2020��7��20��
 *      Author: loyer
 */

#ifndef TIM_TIM4_H_
#define TIM_TIM4_H_

#include "iap.h"
#include "bit_band.h"
#include "usart_4G.h"

#define LED_Status  PA_out(15)
#define BRIGHT  1
#define DARK    0

void Init_led(void);

void TIM4_Init(u16 per,u16 psc);

#endif /* TIM_TIM4_H_ */
