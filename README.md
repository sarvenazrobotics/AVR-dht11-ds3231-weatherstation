# Weather Station for ATmega328P

A complete weather station implementation for ATmega328P microcontroller with I2C LCD (PCF8574), DS3231 RTC, and DHT11 sensor.

## Hardware Components

- **Microcontroller**: ATmega328P
- **Display**: LCD 16x2 with I2C backpack (PCF8574)
- **RTC**: DS3231 (I2C)
- **Sensor**: DHT11 (temperature & humidity)
- **Debug LEDs**: 2x LEDs with current-limiting resistors

## Wiring Connections

### I2C Bus (Bit-banged on PORTC)

| Signal | ATmega328P Pin |
|--------|----------------|
| SDA    | PC4 (Pin 27)    |
| SCL    | PC5 (Pin 28)    |

**Important**: Pull-up resistors (4.7kΩ) required on both SDA and SCL lines.

### LCD (PCF8574) - P3-P6 Wiring

The LCD backpack is wired with 4-bit data mode:

| PCF8574 Pin | LCD Pin | Function |
|-------------|---------|----------|
| P0          | RS      | Register Select |
| P1          | RW      | Read/Write (GND - Write mode) |
| P2          | EN      | Enable |
| P3          | D4      | Data bit 4 |
| P4          | D5      | Data bit 5 |
| P5          | D6      | Data bit 6 |
| P6          | D7      | Data bit 7 |
| P7          | -       | Backlight (optional) |

**I2C Address**: 0x20 (when A0=A1=A2=GND)




https://github.com/user-attachments/assets/444faf32-9ff2-4503-8abb-51b203d28e6e

<img width="400" height="600" alt="5818793737138671768" src="https://github.com/user-attachments/assets/fb5482c5-c998-4445-b536-a5108759933e" />






### DHT11

| DHT11 Pin | ATmega328P Pin |
|-----------|----------------|
| Data      | PD2 (Pin 4)     |
| VCC       | +5V             |
| GND       | GND             |

**Note**: Add 10kΩ pull-up resistor between Data and VCC.

### DS3231 RTC

| DS3231 Pin | ATmega328P Pin |
|------------|----------------|
| SDA        | PC4 (Pin 27)    |
| SCL        | PC5 (Pin 28)    |
| VCC        | +5V             |
| GND        | GND             |

### Debug LEDs

| LED  | ATmega328P Pin | Active |
|------|----------------|--------|
| LED1 | PB0 (Pin 14)    | High   |
| LED2 | PB1 (Pin 15)    | High   |

**Resistors**: 220Ω in series with each LED.

### Buttons (Optional - not used in main loop)

| Button | ATmega328P Pin |
|--------|----------------|
| BTN1   | PD3 (Pin 5)     |
| BTN2   | PD4 (Pin 6)     |

Both buttons have internal pull-ups disabled - external pull-ups (10kΩ) or use with pull-down configuration.

## Features

- Reads temperature and humidity from DHT11 sensor
- Reads current time from DS3231 RTC
- Displays data on 16x2 LCD:
  - **Line 1**: Humidity (H:XX%) and Temperature (T:XX°C)
  - **Line 2**: Current Time (TIME HH:MM)
- Debug LEDs indicate sensor read cycles

## Software Implementation Details

### I2C Bit-Banging

The code implements software I2C master with open-drain configuration:
- SDA and SCL pins configured for open-drain operation
- 50µs delays for ~100kHz bus speed
- Proper start/stop condition generation
- NACK/ACK handling


  

https://github.com/user-attachments/assets/4017180c-4144-40f9-af74-8a646eed561c



### LCD Communication

- 4-bit mode interface through PCF8574 I/O expander
- Custom nibble-sending functions
- Proper initialization sequence for HD44780 controller
- Support for custom characters (degree symbol)

### DHT11 Protocol

- Single-wire communication with precise timing
- 18ms start pulse to wake sensor
- 30µs timing for bit detection
- Checksum validation (not implemented - recommended addition)

### DS3231 RTC

- I2C communication at device address 0xD0 (write) / 0xD1 (read)
- BCD to decimal conversion for time data
- Only reads hours and minutes (seconds skipped)

## Building and Programming

### Requirements

- CodeVision AVR compiler (or AVR-GCC with porting)
- AVR programmer (USBasp, AVRISP, etc.)

### Compilation (CodeVision AVR)

1. Create new project for ATmega328P
2. Set clock frequency to 16MHz (or adjust delay.h accordingly)
3. Add all source code
4. Compile to generate HEX file

### License
This code is open-source and free to use for educational and personal projects.

## Project Author
Sarvenaz Ashoori

GitHub: @sarvenazrobotics

Email: sarvenazrobotics@gmail.com

LinkedIn: Sarvenaz Ashoori
