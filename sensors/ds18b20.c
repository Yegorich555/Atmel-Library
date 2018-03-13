//Example *****
// ds1820_readt();
// sprintf(lcd_buffer,"%d",T_ds18b20[0]); - input temperature 1 with index 0
// lcd_gotoxy(0,0); lcd_puts(lcd_buffer);

#include "extensions.h"
#include "ds18b20.h"

#define _DS18B20_SEARCHROM 0xF0
#define _DS18B20_READROM 0x33
#define _DS18B20_MATCHROM 0x55
#define _DS18B20_SKIPROM 0xCC
#define _DS18B20_CONVERT_T 0x44
#define _DS18B20_Write 0x4E
#define _DS18B20_Read 0xBE

#define _DS18B20_Resolution12bit 0b1111 & (1 << 4) & (1 << 5) //time convert 750ms
#define _DS18B20_Resolution11bit 0b1111 & (0 << 4) & (1 << 5) //time convert 375ms
#define _DS18B20_Resolution10bit 0b1111 & (1 << 4) & (0 << 5) //time convert 187.5ms
#define _DS18B20_Resolution9bit  0b1111 & (0 << 4) & (0 << 5) //time convert 93.75ms

//ds.reset(); // rest 1-Wire
//ds.select(addr); // select DS18B20
//ds.write(0x4E); // write on scratchPad
//ds.write(0x00); // User byte 0 - Unused
//ds.write(0x00); // User byte 1 - Unused
//ds.write(0x7F); // set up en 12 bits (0x7F)
//ds.reset(); // reset 1-Wire

//***************************************************************************
// CRC8
// Для серийного номера вызывать 8 раз
// Для данных вызвать 9 раз
// Если в результате crc == 0, то чтение успешно
//***************************************************************************
unsigned char sens_ds18b20_CRC8_byte(unsigned char data, unsigned char crc)
{
	#define _SENS_DS18B20_CRC8INIT   0x00
	#define _SENS_DS18B20_CRC8POLY   0x18  //0X18 = X^8+X^5+X^4+X^0 without last bit

	unsigned char bit_counter = 8;
	unsigned char feedback_bit;
	do
	{
		feedback_bit = (crc ^ data) & 0x01;
		if ( feedback_bit == 0x01 ) crc ^= _SENS_DS18B20_CRC8POLY;
		crc = (crc >> 1) & 0x7F;
		if ( feedback_bit == 0x01 ) crc |= 0x80;
		data = data >> 1;
		bit_counter--;
	} while (bit_counter > 0);

	return crc;
}

unsigned char sens_ds18b20_calcCRC8(unsigned char *data, unsigned char numBytes)
{
	unsigned char crc = 0;
	for (uint8_t n = 0; n < numBytes; ++n)
	{
		crc = sens_ds18b20_CRC8_byte(data[n], crc);
	}
	return crc;
}

char sens_ds18b20_reset()
{
	//asm_cli();

	io_set(DDR, SENS_DS18B20_IO);
	io_setPort(SENS_DS18B20_IO);
	delay_ms(10);

	io_resetPort(SENS_DS18B20_IO);
	_delay_us(480);

	io_reset(DDR, SENS_DS18B20_IO);
	_delay_us(10);
	//while (io_getPin(SENS_DS18B20_IO)) {++count; _delay_us(2); if (count>60) { asm_sei(); return 1;}}
	//_delay_us(10); count=0;
	//while (!io_getPin(SENS_DS18B20_IO)) {++count; _delay_us(2); if (count>100) {asm_sei(); return 2;}}
	if (!io_waitPin(SENS_DS18B20_IO, 0, 120)) return 1;
	_delay_us(10);
	if (!io_waitPin(SENS_DS18B20_IO, 1, 200)) return 2;

	//asm_sei();
	return 0;
}

void sens_ds18b20_sendByte(uint8_t b)
{
	unsigned char bitc = 0;
	asm_cli();
	// запрещаем прерывания, что бы не было сбоев при передачи
	for (bitc = 0; bitc < 8; bitc++)
	{
		if (b & 0x01) // сравниваем младший бит
		{  //запись 1
			io_set(DDR, SENS_DS18B20_IO);
			io_resetPort(SENS_DS18B20_IO);
			_delay_us(10); // 1 - 15 мкс
			io_setPort(SENS_DS18B20_IO);
			_delay_us(100); //> 61
			io_reset(DDR, SENS_DS18B20_IO);
			_delay_us(10);// > 1 мкс
		}
		else //запись 0
		{
			io_set(DDR, SENS_DS18B20_IO);
			io_resetPort(SENS_DS18B20_IO);
			_delay_us(100); // 60 - 120 мкс
			io_reset(DDR, SENS_DS18B20_IO);
			_delay_us(10); // > 1 мкс
		};
		b = b >> 1; //сдвигаем передаваемый байт данных на 1 в сторону младших разрядов
	};
	asm_sei();
}

unsigned char sens_ds18b20_readByte()
{
	unsigned char bitc=0;// счетчик принятых байт
	unsigned char res=0; // принятый байт

	asm_cli();

	for (bitc = 0; bitc < 8; bitc++)
	{
		io_set(DDR, SENS_DS18B20_IO);  // порт на вывод
		io_resetPort(SENS_DS18B20_IO); // начинаем тайм слот записью 0
		_delay_us(10);  // 1 - 15 мкс
		io_reset(DDR, SENS_DS18B20_IO);         // порт на ввод
		_delay_us(20);     // ждем завершения переходных процессов (не менее 15 мкс)

		if (io_getPin(SENS_DS18B20_IO))
		res|= (1 << bitc);     // если на ходе 1 то запишем ее в текущий бит

		_delay_us(90); // ждем до завершения тайм слота
	};
	_delay_us(7);
	asm_sei();
	return res;
}

void sens_ds18b20_writeRom(uint8_t *romCode)
{
	for (uint8_t n = 0; n < 8; ++n)
	{
		sens_ds18b20_sendByte(romCode[n]);
	}
}

bool sens_ds1820_readByRom(int16_t *t_values, uint8_t romCodes[][8])
{
	int16_t r = 0, rf = 0;
	uint8_t data[9];
	bool good = true;
	//asm_cli();
	if (sens_ds18b20_reset()==0)
	{
		sens_ds18b20_sendByte(_DS18B20_SKIPROM); //обратиться ко всем устройствам на шине
		sens_ds18b20_sendByte(_DS18B20_CONVERT_T);  // посылаем команду старта преобразования
	};
	//asm_sei();
	delay_ms(760); // ждем пока идет преобразование;
	//asm_cli();
	if (sens_ds18b20_reset() == 0) // если на шине есть откликнувшееся устройство то продолжаем
	{
		for (uint8_t dsNum = 0; dsNum < SENS_DS18B20_NUM; ++dsNum)
		{
			bool minus = false;
			sens_ds18b20_sendByte(_DS18B20_MATCHROM); // обратиться по ром коду
			sens_ds18b20_writeRom(romCodes[dsNum]);   // выдаем ром код выбранного датчика на шину
			sens_ds18b20_sendByte(_DS18B20_Read);             // считать память

			for (uint8_t n = 0; n < 9; ++n)
			{
				data[n] = sens_ds18b20_readByte();
			}
			uint8_t crc = sens_ds18b20_calcCRC8(data,8);
			if (crc == data[8])
			{

				if ( (data[1]&0b11110000) == 0b11110000)
				{
					r = ~(data[1]<<8 | data[0]);
					++r;
					data[0] = r&0b11111111;
					data[1] = r>>8;
					minus = true;
				}

				r = data[0];
				rf = (r & 0b1111) * 10 / 16; ///0.625
				r = r >> 4;
				r = ((data[1] << 4) + r) * 10 + rf;

				if (r>-300 && r<1250)
				{
					t_values[dsNum] = r;
					if (minus) t_values[dsNum]*=-1;
				}
			}

			sens_ds18b20_reset(); // по правилам DS'ки прервать дальнейшее чтение стоит командой RESET
			_delay_us(100);
		} // for (DSnum=0; DSnum<Number_ds18b20; ++DSnum)
	}
	else
	{
		good = false;
	}
	//asm_sei();
	return good;
}

//bool sens_ds18b20_init()
//{
//return true;
//}