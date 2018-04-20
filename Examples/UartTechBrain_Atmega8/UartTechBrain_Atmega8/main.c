/*
* UartTechBrain_Atmega8.c
*
* Created: 20-Apr-18 09:29:18
* Author : yahor.halubchyk
*/

#define F_CPU 8000000UL

#include <avr/io.h>

//***************** soft uart
#define USOFT_BAUD 4800
#define USOFT_BUFFER_EN false
#define USOFT_AUTOLISTEN true
#define USOFT_IO_RX C, 1
#define USOFT_IO_TX C, 0
#include <uart_soft.h>

//***************** techBrain
#include <uart_techBrain.h>

time_s rtcTime = {hour: 1, min: 5, sec: 36, dayWeek: 1};
portValue_s portVal; //

int main(void)
{
	usoft_init();

	static int16_t	*sens[5]; //buffer sensors for testing
	#pragma GCC diagnostic ignored "-Wint-conversion"
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
	sens[0] = (int16_t *)52; //5.2 
	sens[1] = 94;
	sens[2] = 127;
	sens[3] = 65;
	sens[4] = 45; 
	utb_init2(&rtcTime, &portVal, sens, 5);

	utb_cmd_e cmd;

	usoft_putStringf("test uart_tb");
	usoft_putCharf(0x0D); //end line
	while (1)
	{
		cmd = utb_getCmd(); //check command
	}
}

USOFT_ISR_newByte(b)
{
	utb_byteReceived(b);
}

UTB_UART_SEND(str, num)
{
	usoft_putBytes(str, num);
}