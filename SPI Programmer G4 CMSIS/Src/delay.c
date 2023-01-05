/*
 * delay.c
 *
 *  Created on: 3 gru 2020
 *      Author: bga-service
 */

#include "delay.h"

void delayms(uint32_t n)
	{
	    delayvalue=n;
	    while(delayvalue!=0);
	}
void delay()
    {
    if(delayvalue>0)
    	{
    	delayvalue--;
    	}
        else
    	{
    	delayvalue=0;
    	}
    }
