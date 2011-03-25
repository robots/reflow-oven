
#include "main.h"

#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#include "lcd.h"

#define  LCD_DATA(x) (PORTC = (PORTC & 0xF0) | ((x) & 0x0F))

#define SET_LCD_E  PORTC |= _BV(PC4);
#define CLR_LCD_E  PORTC &= ~_BV(PC4);
#define SET_LCD_RS PORTC |= _BV(PC5);
#define CLR_LCD_RS PORTC &= ~_BV(PC5);


static void LCD_data(unsigned char dat);
static void LCD_cmd(unsigned char cmd);
static void LCD_clock(void);



void LCD_init(void)
{
   unsigned char i;

   for(i = 0; i < 10; i++)
   {
      _delay_ms(10);
   }

   LCD_DATA(0x03);     // enable 4-bit LCD mode
   LCD_clock();
   _delay_ms(5);
   LCD_DATA(0x03);
   LCD_clock();
   _delay_ms(5);
   LCD_DATA(0x03);
   LCD_clock();
   _delay_ms(5);
   LCD_DATA(0x02);
   LCD_clock();
   _delay_ms(5);

   if(LCD_COLS > 1)    // configuration
      LCD_cmd(0x28);
   else
      LCD_cmd(0x20);
   _delay_us(40);

   LCD_cmd(0x04);      // entry mode
   _delay_us(40);

   LCD_cmd(0x01);      // clear display
   _delay_ms(2);

   LCD_cmd(0x0C);      // Display ON
   _delay_us(40);

   LCD_cursor(0, 0);   // goto position (0,0)
}


void LCD_text(const char *buf)
{
   while(*buf)
   {
      LCD_data(*buf);
      buf++;
   }
}


void LCD_char(char c)
{
   LCD_data(c);
}


void LCD_cursor(unsigned char line, unsigned char column)
{
   unsigned char l = 0x00;
   unsigned char offset = column;

   if((line == 1) || (line == 3))
   {
      l = 0x40;
   }
   if((line == 2) || (line == 3))
   {
      offset += LCD_COLS;
   }
   LCD_cmd(0x80 | l | offset);
}



void LCD_specialchars(void)
{
   unsigned char i, k;

   for(i = 0; i < 8; i++)
   { 
      LCD_cmd(0x40 | (i*8));
      for(k = 0; k < (7-i); k++)
      {
         LCD_data(0x00);
      }
      for(; k < 8; k++)
      {
         LCD_data(0x1F);
      }
   }
}


void LCD_hex(unsigned char value)
{
   unsigned char temp;

   temp = value >> 4;
   if(temp > 9)
      temp += 55;
   else
      temp += '0';
   LCD_char(temp);

   temp = value & 0x0F;
   if(temp > 9)
      temp += 55;
   else
      temp += '0';
   LCD_char(temp);
}



static void LCD_clock(void)
{
   _delay_us(40);
   SET_LCD_E;
   _delay_ms(1);
   CLR_LCD_E;
   _delay_us(40);
}


static void LCD_data(unsigned char dat)
{
   SET_LCD_RS;
   LCD_DATA(dat >> 4);
   LCD_clock();
   LCD_DATA(dat & 0x0F);
   LCD_clock();
   _delay_us(40);
}


static void LCD_cmd(unsigned char cmd)
{
   CLR_LCD_RS;
   LCD_DATA(cmd >> 4);
   LCD_clock();
   LCD_DATA(cmd & 0x0F);
   LCD_clock();
   _delay_ms(1);
}
