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

#define DHT11_PIN 2
#define BUTTON_UP 3
#define BUTTON_DOWN 4

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
 byte_to_send=high_nibble;
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
  
//pulse EN low LCD reads on falling edge
  byte_to_send &= ~0x04;
  i2c_start();
  i2c_write(LCD_ADDR);
  i2c_write(byte_to_send);
  i2c_stop();
  delay_us(50);
  
//===send low nibble===
  
  byte_to_send=low_nibble;
  if(cmd){
    byte_to_send |= 0x00; //   RS=0
    }else{
    byte_to_send |= 0x01; //RS=1}
    }
    byte_to_send |= 0x04; //en=1 
    
    i2c_start();
    i2c_write(LCD_ADDR); 
    i2c_write(byte_to_send);
    i2c_stop();
    delay_us(50);
    
    i2c_start();
    i2c_write(LCD_ADDR);
    i2c_write(byte_to_send);
    i2c_stop();
    delay_ms(2);
    } 
    
    
 //intialize the lcd display 
 
 void lcd_init(void){
 delay_ms(50);
 
 //intialization by HD44780 datasheet
 
 lcd_send_byte(0x30,1); delay_ms(5);//8bit
 lcd_send_byte(0x30,1); delay_ms(200);
 lcd_send_byte(0x30,1); delay_ms(5);
 lcd_send_byte(0x20,1);//4bit
 
 lcd_send_byte(LCD_FUNCTION_4BIT,1); //4BIT,2 LINES
 lcd_send_byte(LCD_DISPLAY_ON,1);
 lcd_send_byte(LCD_CLEAR,1);
 lcd_send_byte(LCD_ENTRY_MODE,1);
 }
 
 void lcd_clear(void){
 lcd_send_byte(LCD_CLEAR,1);
 delay_ms(2);
 }
 
 void lcd_set_cursor(unsigned char row,unsigned char col){
 unsigned char address;
 if(row==0){
 address=LCD_LINE1+col;} //0x80+col
 else{address=LCD_LINE2+col;}//0xC0+col
 lcd_send_byte(address,1);}
 
 //single charactor &string on LCD
 void lcd_print_char(char c){
 lcd_send_byte(c,0); }//0=data mode
 
 void lcd_print_string(const char* str)
 {while(*str){
     lcd_print_char(*str++);
     }}
     
 //=====DHT11 SENSOR FUNCTIONS====
 //READ TEMPERATURE AND HUMADITY
 //RETURN  SUCCESS=0,NO RES=1,CHECKSUM ERROR=2
 unsigned char dht11_read(unsigned int *temp,unsigned int *humi)
 { 
   unsigned char i,j,checksum;
   unsigned char data[5]={0,0,0,0,0}; 
   //SEND START SIGNAL TO DHT11
   DDRD |= (1<<DHT11_PIN); //SET AS OUTPUT
   PORTD &= ~(1<<DHT11_PIN);//PULL LOW
   delay_ms(18); 
   PORTD |= (1<<DHT11_PIN); // PULL HIGH
   delay_us(40);
   DDRD &= ~(1<< DHT11_PIN); //SET AS INPUT
   PORTD |= (1<<DHT11_PIN);//ENABLE PULL-UP
   
   delay_us(40);
   if(PIND & (1<<DHT11_PIN)) return 1;//no response as pin is high
   delay_us(80);
   if(!(PIND & (1<<DHT11_PIN))) return 1;
   delay_us(80);
   
   //Read 40 bits (5 bytes) from DHT11
   for(i=0;i<5;i++){
        for(j=0;j<8;j++){
            while(!(PIND & (1<<DHT11_PIN)));//WAIT FOR START OF PIN
            delay_us(30);
            data[i] <<= 1;
            if (PIND & (1<<DHT11_PIN)){
                data[i] |= 1;
            }
          while(PIND & (1<<DHT11_PIN));//wait for end of bit 
        }
   }
   
   checksum=data[0]+data[1]+data[2]+data[3];
   if(checksum != data[4]) return 2;//checksum error
   
   *humi = data[0]*10 + data[1];
   *temp = data[2]*10 + data[3];
    return 0; //success
   
 }  
 
 
 //=====DS3231 rtc ===== 
 
 #define DS3231_ADDR 0xD0 //??WRITE ADDRESS (0x68 << 1)
 
 //convert BCD TO decimal
 unsigned char bcd_to_dec(unsigned char bcd)
 {
    return((bcd >> 4)*10)+(bcd & 0x0F);
  }
 //convert decimal to bcd 
  unsigned char dec_to_bcd(unsigned char dec)
 {
    return(dec/10)|(dec%10);
  }
  
  
  //READ CURRENT TIME FROM DS3231
  void ds3231_read_time(unsigned char *hour,unsigned char *minute,unsigned char *second)
  {
  i2c_start();
  i2c_write(DS3231_ADDR); //send write address
  i2c_write(0x00); //start at second register
  i2c_start(); //repeated start for read
  i2c_write(DS3231_ADDR | 0x01);//send read address
  
  *second = bcd_to_dec(i2c_write(0xFF)& 0x7F); //READ SECONDS,SEND ACK
  *minute = bcd_to_dec(i2c_write(0xFF)& 0x7F);
  *hour = bcd_to_dec(i2c_write(0xFF)& 0x3F);
  
  
  i2c_stop();
  }    
  
  
  void ds3231_set_time(unsigned char hour,unsigned char minute,unsigned char second)
   {
  i2c_start();
  i2c_write(DS3231_ADDR); //send write address
  i2c_write(0x00); //start at second register
  i2c_write(dec_to_bcd(second)); //repeated start for read
  i2c_write(dec_to_bcd(minute));
  i2c_write(dec_to_bcd(hour));
  i2c_stop();
   } 
   
 //====BUTTON FUNCTION====
 // Check if a button is currently pressed
unsigned char button_is_pressed(unsigned char pin) {
    return (PIND & (1 << pin)) ? 1 : 0;
}

// Check for button press with debouncing
// Returns 1 only once per physical press
unsigned char button_get_press(unsigned char pin) {
    static unsigned char up_last = 0;
    static unsigned char down_last = 0;
    unsigned char current;
    unsigned char *last;
    
    // Select which button's state to track
    if(pin == BUTTON_UP) {
        last = &up_last;
        } else {
        last = &down_last;
    }
    
    current = button_is_pressed(pin);
    
    // Detect rising edge (off -> on)
    if(current && !(*last)) {
        *last = 1;
        delay_ms(50);  // Debounce delay
        return 1;
    }
    // Reset when button is released
    if(!current) {
        *last = 0;
    }
    
    return 0;
}


//===Menu Main===

void display_main_menu(unsigned char selected) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Weather Station");
    lcd_set_cursor(1, 0);
   
   if(selected == 0) {
        lcd_print_string("> 1. Temp & Hum");
    } else {
        lcd_print_string("  1. Temp & Hum");
    }
    
    lcd_set_cursor(1, 14);
    if(selected == 1) {
        lcd_print_string(">");
    } else {
        lcd_print_string(" ");
    }
}
  
void main(void)
{
while (1)
    {
    // Please write your application code here

    }
}
