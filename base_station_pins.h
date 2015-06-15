/* WHCS Base Station
   Master Pin-out file
*/
#ifndef BASE_STATION_PINS_H
#define BASE_STATION_PINS_H

#if !defined(__AVR_ATmega32__)
 #error "Board type unsupported / not recognized"
#endif

/* PORT A
   [3:0] LCD Resistive Touch (IN/OUT)
   [4]   Status LED (OUT)
   [5]   BlueTooth Status (IN)
   [6]   Software UART TX (OUT)
   [7]   Software UART RX (IN)
*/
#define PORT_TOUCH_YM PORTA
#define PORT_TOUCH_YM_MSK _BV(0)

#define PORT_TOUCH_XM PORTA
#define PORT_TOUCH_XM_MSK _BV(1)

#define PORT_TOUCH_YP PORTA
#define PORT_TOUCH_YP_MSK _BV(2)

#define PORT_TOUCH_XP PORTA
#define PORT_TOUCH_XP_MSK _BV(3)

#define PORT_STATUS_LED PORTA
#define PORT_STATUS_LED_MSK _BV(4)

#define PORT_BT_STATUS PORTA
#define PORT_BT_STATUS_MSK _BV(5)

#define PORT_TX_SOFT PORTA
#define PORT_TX_SOFT_MSK _BV(6)

#define PORT_RX_SOFT PORTA
#define PORT_RX_SOFT_MSK _BV(7)

/* PORT B
   [0] NRF Chip Enable (OUT)
   [1] HC-05 Key (OUT)
   [2] HC-05 Power Enable (OUT)
   [3] LCD Backlight (OUT PWM)
   [4] NRF Chip Select (SPI_SS)
   [5] NRF MOSI (SPI_MOSI)
   [6] NRF MISO (SPI_MISO)
   [7] NRF SCK (SPI_SCK)
*/
#define PORT_NRF_CE PORTB
#define PORT_NRF_CE_MSK _BV(0)

#define PORT_HC05_KEY PORTB
#define PORT_HC05_KEY_MSK _BV(1)

#define PORT_HC05_ENABLE PORTB
#define PORT_HC05_ENABLE_MSK _BV(2)

#define PORT_LCD_BACKLIGHT PORTB
#define PORT_LCD_BACKLIGHT_MSK _BV(3)

#define PORT_NRF_CS PORTB
#define PORT_NRF_CS_MSK _BV(4)

#define PORT_NRF_MOSI PORTB
#define PORT_NRF_MOSI_MSK _BV(5)

#define PORT_NRF_MISO PORTB
#define PORT_NRF_MISO_MSK _BV(6)

#define PORT_NRF_SCK PORTB
#define PORT_NRF_SCK_MSK _BV(7)

/* PORT C
   [7:0] LCD 8-bit Parallel Data In (IN/OUT)
*/
#define PORT_LCD_DIN PORTC
#define PORT_LCD_DIN_MSK 0b11111111

/* PORT D
   [0] HC-05 RX (UART_RX)
   [1] HC-05 TX (UART_TX)
   [2] NRF Interrupt Request (IN)
   [3] LCD Reset (OUT)
   [4] LCD Read (OUT)
   [5] LCD Write (OUT)
   [6] LCD Command/Data (OUT)
   [7] LCD Chip Select (OUT)
*/
#define PORT_HC05_RX PORTD
#define PORT_HC05_RX_MSK _BV(0)

#define PORT_HC05_TX PORTD
#define PORT_HC05_TX_MSK _BV(1)

#define PORT_NRF_IRQ PORTD
#define PORT_NRF_IRQ_MSK _BV(2)

#define PORT_LCD_RESET PORTD
#define PORT_LCD_RESET_MSK _BV(3)

#define PORT_LCD_RD PORTD
#define PORT_LCD_RD_MSK _BV(4)

#define PORT_LCD_WR PORTD
#define PORT_LCD_WR_MSK _BV(5)

#define PORT_LCD_CD PORTD
#define PORT_LCD_CD_MSK _BV(6)

#define PORT_LCD_CS PORTD
#define PORT_LCD_CS_MSK _BV(7)

#endif // BASE_STATION_PINS_H
