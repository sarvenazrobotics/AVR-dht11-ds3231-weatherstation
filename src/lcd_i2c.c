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


unsigned char i2c_write(unsigned char data)
{  unsigned char i,ack;
//send 8 bits
for(i=0;i<8;i++){
 if(data & 0x80)
     I2C_SDA_HIGH();
 else
     I2C_SDA_LOW();
     i2c_delay();
     I2C_SCL_HIGH();
     i2c_delay();
     I2C_SDA_HIGH();
     data<<=1;} //move to next bit
     //get ack
     
     I2C_SDA_HIGH();//releases SDA for ack
     i2c_delay();
     I2C_SCL_HIGH();//slaves look at SDA just when SCL is high
     i2c_delay();
     ack=I2C_SDA_READ();//read slave answer
     I2C_SCL_LOW();//prepares for next operation
     i2c_delay();
     
     return ack; //ack means received and nack means 1
 }   
 
 
//=========LCD functions======

void lcd_send_byte(unsigned char data,unsigned char cmd)
{
 unsigned char high_nibble,low_nibble,byte_to_send;
 
 //split into two nibbles 4bit mode 
 high_nibble=(data & 0xF0);
 low_nibble=((data<<4)& 0xF0);
 
 //send high nibble 
 byte_to_send=high nibble;
 if(cmd){
  byte_to_send |= 0x00; //RS=0  COMMAND 
  }else{
  byte_to_send |= 0x01; //RS=1 DATA
  }
  byte_to_send |= 0x04; //en=1 
  
  
  i2c_start();
  i2c_write(LCD_ADDR);
  i2c_write(byte_to_send);
  i2c_stop();
  delay_us(1);
  
  
  
  

}
 
void main(void)
{
while (1)
    {
    // Please write your application code here

    }
}
