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

	#pragma GCC diagnostic ignored "-Wint-conversion"
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

	static int16_t	*sens[5];  //buffer sensors for testing
	int16_t dht22_hum = 45;
	int16_t dht22_t = 201; //20.1 C
	int16_t ds18b20_t[] = { 364, 63, -74 };
	sens[0] = ds18b20_t;
	sens[1] = ds18b20_t + 1;
	sens[2] = ds18b20_t + 2;
	sens[3] = &dht22_t;
	sens[4] = &dht22_hum;

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