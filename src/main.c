/*
 * main.c
 *
 *  Created on: Oct 8, 2021
 *      Author: Mohamed Ezzat
 */


#include <stdlib.h>
#include <stdio.h>

#include "ax25.h"

int main()
{
	TX_FRAME frame;
	for(int i=0; i<10;i++)
	{
	AX25_prepareIFrame(&frame);
	printf("\n--------------------- %d ------------------------\n", i);
	printTxFrame(&frame);
	printf("\n--------------------------------------------------------\n");
	}

}
