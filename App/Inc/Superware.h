#ifndef _SUPERWARE_H_
#define _SUPERWARE_H_

#include "common.h"
#include "include.h"
extern uint16 distance;

void PORTC_IRQHandler(void);
void Superware_Init();
void Superware_Get_Distance();

extern uint32 viameter;//·�̼�����
extern uint8 viameter_on_flag;//·�̼�����������־λ

#endif