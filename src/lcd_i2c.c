/*
 * lcd_i2c.c
 *
 * Created: 4/29/2026 12:09:05 PM
 * Author: Sarve
 */

#include <io.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>


#define LCD_ADDR 0x27

#define DHT11_PIN
#define BUTTON_UP
#define BUTTON_DOWN

//DEFINE LCD MODES
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_FUNCTION_4BIT 0x28
#define LCD_LINE1 0x80
#define LCD_LINE2 0xC0

#define MENU_MAIN
#define MENU_TEMP_HUM
#define MENU_TIME
#define MENU_ABOUT


static void lcd_send_data(uint8_t data,uint8_t cmd)
{

//extract high and low nibbles
uint8_t high_nibble=(0xF0);   //mask lower 4 bits
uint8_t low_nibble=((data<<4)& 0xF0);   //shift lower 4 bits to upper position 

}

void main(void)
{
while (1)
    {
    // Please write your application code here

    }
}
