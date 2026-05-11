/*
 * Weather Station - ATmega328P
 * I2C LCD (PCF8574 P3-P6 wiring) + DS3231 + DHT11
 * FIXED FOR YOUR EXACT SCHEMATIC
 */

#include <mega328p.h>
#include <delay.h>

// ===================== DEBUG LEDs =====================
#define LED1_ON()   (PORTB |= (1<<0))
#define LED1_OFF()  (PORTB &= ~(1<<0))
#define LED2_ON()   (PORTB |= (1<<1))
#define LED2_OFF()  (PORTB &= ~(1<<1))

// ===================== I2C BIT-BANG (Open-Drain) =====================
#define SDA_HIGH()  do { DDRC &= ~(1<<4); PORTC |= (1<<4); } while(0)
#define SDA_LOW()   do { DDRC |=  (1<<4); PORTC &= ~(1<<4); } while(0)
#define SCL_HIGH()  do { DDRC &= ~(1<<5); PORTC |= (1<<5); } while(0)
#define SCL_LOW()   do { DDRC |=  (1<<5); PORTC &= ~(1<<5); } while(0)
#define SDA_READ()  ((PINC & (1<<4)) ? 1 : 0)

void my_i2c_delay(void){ delay_us(50); }

void my_i2c_start(void){
    SDA_HIGH(); delay_us(5);
    SCL_HIGH(); delay_us(5);
    SDA_LOW();  my_i2c_delay();
    SCL_LOW();  my_i2c_delay();
}

void my_i2c_stop(void){
    SDA_LOW();  my_i2c_delay();
    SCL_HIGH(); delay_us(5);
    SDA_HIGH(); my_i2c_delay();
}

unsigned char my_i2c_write(unsigned char data){
    unsigned char i, ack;
    for(i=0; i<8; i++){
        if(data & 0x80) SDA_HIGH(); else SDA_LOW();
        my_i2c_delay();
        SCL_HIGH(); my_i2c_delay();
        SCL_LOW();  my_i2c_delay();
        data <<= 1;
    }
    SDA_HIGH(); my_i2c_delay();
    SCL_HIGH(); delay_us(5);
    ack = SDA_READ();
    SCL_LOW(); my_i2c_delay();
    return ack; // 0=ACK, 1=NACK
}

unsigned char my_i2c_read(unsigned char ack){
    unsigned char i, data=0;
    SDA_HIGH();
    for(i=0; i<8; i++){
        SCL_HIGH(); delay_us(5);
        data <<= 1;
        if(SDA_READ()) data |= 1;
        SCL_LOW(); my_i2c_delay();
    }
    if(ack) SDA_LOW(); else SDA_HIGH();
    my_i2c_delay();
    SCL_HIGH(); delay_us(5);
    SCL_LOW(); my_i2c_delay();
    SDA_HIGH();
    return data;
}

// ===================== LCD VIA PCF8574 =====================
// YOUR WIRING: P0=RS, P1=RW, P2=EN, P3=D4, P4=D5, P5=D6, P6=D7
// A0=A1=A2=GND ? 7-bit address = 0x20

#define LCD_ADDR 0x20

#define LCD_RS (1<<0)
#define LCD_EN (1<<2)

void pcf8574_write(unsigned char data){
    my_i2c_start();
    my_i2c_write((LCD_ADDR << 1) | 0);
    my_i2c_write(data);
    my_i2c_stop();
}

void lcd_pulse_enable(unsigned char data){
    // EN high
    pcf8574_write(data | LCD_EN);
    delay_us(5);
    // EN low (LCD latches on falling edge)
    pcf8574_write(data & ~LCD_EN);
    delay_us(100); // HD44780 requires >37us
}

void lcd_send_nibble(unsigned char nibble, unsigned char rs){
    // Shift 4-bit nibble to P3-P6
    unsigned char data = ((nibble & 0x0F) << 3); 
    data |= rs;        // P0 = RS
    data &= ~(1<<1);   // FORCE P1 (RW) = 0 (Write Mode)
    lcd_pulse_enable(data);
}

void lcd_send_byte(unsigned char value, unsigned char is_cmd){
    unsigned char rs = is_cmd ? 0 : LCD_RS;
    
    // High nibble first
    lcd_send_nibble((value >> 4) & 0x0F, rs);
    delay_us(100);
    // Low nibble
    lcd_send_nibble(value & 0x0F, rs);
    delay_us(100);
}

void lcd_init(void){
    delay_ms(50); // LCD power-up
    
    // HD44780 4-bit initialization sequence
    lcd_send_nibble(0x03, 0); delay_ms(5);
    lcd_send_nibble(0x03, 0); delay_ms(1);
    lcd_send_nibble(0x03, 0); delay_ms(1);
    lcd_send_nibble(0x02, 0); delay_ms(1); // Switch to 4-bit

    lcd_send_byte(0x28, 1); delay_ms(1); // 4-bit, 2 lines
    lcd_send_byte(0x0C, 1); delay_ms(1); // Display ON, cursor OFF
    lcd_send_byte(0x01, 1); delay_ms(2); // Clear
    lcd_send_byte(0x06, 1); delay_ms(1); // Entry mode
}

void lcd_clear(void){
    lcd_send_byte(0x01, 1); delay_ms(2);
}

void lcd_set_cursor(unsigned char row, unsigned char col){
    unsigned char addr = (row == 0 ? 0x80 : 0xC0) + col;
    lcd_send_byte(addr, 1); delay_us(100);
}

void lcd_print_char(char c){
    lcd_send_byte(c, 0); delay_us(50);
}

void lcd_print_string(const char *s){
    while(*s) lcd_print_char(*s++);
}

// ===================== DHT11 =====================
unsigned char dht11_data[5];
unsigned char dht11_read(void){
    unsigned char i, j;
    unsigned int t;
    DDRD |= (1<<2); PORTD &= ~(1<<2); delay_ms(18);
    PORTD |= (1<<2); delay_us(30); DDRD &= ~(1<<2);
    
    t=10000; while((PIND&(1<<2)) && t--) delay_us(1); if(!t) return 0;
    t=10000; while(!(PIND&(1<<2)) && t--) delay_us(1); if(!t) return 0;
    t=10000; while((PIND&(1<<2)) && t--) delay_us(1); if(!t) return 0;
    
    for(j=0;j<5;j++){
        unsigned char res=0;
        for(i=0;i<8;i++){
            t=10000; while(!(PIND&(1<<2)) && t--) delay_us(1); if(!t) return 0;
            delay_us(30);
            if(PIND & (1<<2)) res |= (1<<(7-i));
            t=10000; while((PIND&(1<<2)) && t--) delay_us(1); if(!t) return 0;
        }
        dht11_data[j] = res;
    }
    return 1;
}

// ===================== DS3231 =====================
unsigned char bcd_to_dec(unsigned char v){ return ((v>>4)*10)+(v&0x0F); }
void ds3231_read(unsigned char *h, unsigned char *m){
    my_i2c_start(); my_i2c_write(0xD0); my_i2c_write(0x00); my_i2c_stop();
    my_i2c_start(); my_i2c_write(0xD1);
    my_i2c_read(1); // skip seconds
    *m = bcd_to_dec(my_i2c_read(1));
    *h = bcd_to_dec(my_i2c_read(0));
    my_i2c_stop();
}

// ===================== MAIN =====================
void main(void){
    unsigned char h=12, m=30;

    // LEDs
    DDRB |= (1<<0)|(1<<1); PORTB &= ~((1<<0)|(1<<1));
    
    // I2C: INPUT + PULL-UPS (CRITICAL FIX)
    DDRC &= ~((1<<4)|(1<<5));
    PORTC |= ((1<<4)|(1<<5)); // ? ENABLED PULL-UPS
    
    // Buttons
    DDRD &= ~((1<<3)|(1<<4));
    PORTD &= ~((1<<3)|(1<<4));

    LED1_ON(); delay_ms(300);
    lcd_init(); delay_ms(200);
    lcd_clear(); delay_ms(100);
    LED1_OFF();

    // TEST: Print static text first to verify LCD
    lcd_set_cursor(0,0); lcd_print_string("LCD WORKS!");
    lcd_set_cursor(1,0); lcd_print_string("P3-P6 WIRING OK");
    delay_ms(2000);
    lcd_clear();

    while(1){
        LED2_ON();
        if(dht11_read()){
            ds3231_read(&h,&m);
            
            lcd_set_cursor(0,0);
            lcd_print_string("H:");
            lcd_print_char((dht11_data[0]/10)+'0');
            lcd_print_char((dht11_data[0]%10)+'0');
            lcd_print_char('%');
            
            lcd_set_cursor(0,8);
            lcd_print_string("T:");
            lcd_print_char((dht11_data[2]/10)+'0');
            lcd_print_char((dht11_data[2]%10)+'0');
            lcd_print_char(223); lcd_print_char('C');
            
            lcd_set_cursor(1,0);
            lcd_print_string("TIME ");
            lcd_print_char((h/10)+'0');
            lcd_print_char((h%10)+'0');
            lcd_print_char(':');
            lcd_print_char((m/10)+'0');
            lcd_print_char((m%10)+'0');
        } else {
            lcd_set_cursor(0,0); lcd_print_string("DHT11 ERROR   ");
        }
        LED2_OFF();
        delay_ms(1500);
    }
}