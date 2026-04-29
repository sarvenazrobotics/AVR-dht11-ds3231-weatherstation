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


#define I2C_SDA_HIGH() PORTC |= (1<<4)
#define I2C_SDA_LOW()  PORTC &= ~(1<<4)
#define I2C_SCL_HIGH() PORTC |= (1<<5)
#define I2C_SCL_LOW() PORTC &= ~(1<<5)
#define I2C_SDA_READ() (PINC & (1<<4))


void i2c_delay(void){
  delay_us(5);
}

void i2c_start(void){
I2C_SDA_HIGH();
I2C_SCL_HIGH();
i2c_delay();
I2C_SDA_LOW();
i2c_delay();
I2C_SCL_LOW();
i2c_delay();
}


void i2c_stop(void){
I2C_SDA_LOW();
I2C_SCL_HIGH();
i2c_delay();
I2C_SDA_HIGH();
i2c_delay();
}

void main(void)
{
while (1)
    {
    // Please write your application code here

    }
}
