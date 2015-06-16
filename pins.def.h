/* PORT A
   [3:0] LCD Resistive Touch (IN/OUT)
   [4]   Status LED (OUT)
   [5]   BlueTooth Status (IN)
   [6]   Software UART TX (OUT)
   [7]   Software UART RX (IN)
*/

#define TOUCH_YM_PORT PORTA
#define TOUCH_YM_DDR  DDRA
#define TOUCH_YM_MSK  _BV(0)

#define TOUCH_XM_PORT PORTA
#define TOUCH_XM_DDR  DDRA
#define TOUCH_XM_MSK  _BV(1)

#define TOUCH_YP_PORT PORTA
#define TOUCH_YP_DDR  DDRA
#define TOUCH_YP_MSK  _BV(2)

#define TOUCH_XP_PORT PORTA
#define TOUCH_XP_DDR  DDRA
#define TOUCH_XP_MSK  _BV(3)

#define STATUS_LED_PORT PORTA
#define STATUS_LED_DDR  DDRA
#define STATUS_LED_MSK  _BV(4)

#define BT_STATUS_PORT PORTA
#define BT_STATUS_DDR  DDRA
#define BT_STATUS_MSK  _BV(5)

#define TX_SOFT_PORT PORTA
#define TX_SOFT_DDR  DDRA
#define TX_SOFT_MSK  _BV(6)

#define RX_SOFT_PORT PORTA
#define RX_SOFT_DDR  DDRA
#define RX_SOFT_MSK  _BV(7)


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

#define NRF_CE_PORT PORTB
#define NRF_CE_DDR  DDRB
#define NRF_CE_MSK  _BV(0)

#define HC05_KEY_PORT PORTB
#define HC05_KEY_DDR  DDRB
#define HC05_KEY_MSK  _BV(1)

#define HC05_ENABLE_PORT PORTB
#define HC05_ENABLE_DDR  DDRB
#define HC05_ENABLE_MSK  _BV(2)

#define LCD_BACKLIGHT_PORT PORTB
#define LCD_BACKLIGHT_DDR  DDRB
#define LCD_BACKLIGHT_MSK  _BV(3)

#define NRF_CS_PORT PORTB
#define NRF_CS_DDR  DDRB
#define NRF_CS_MSK  _BV(4)

#define NRF_MOSI_PORT PORTB
#define NRF_MOSI_DDR  DDRB
#define NRF_MOSI_MSK  _BV(5)

#define NRF_MISO_PORT PORTB
#define NRF_MISO_DDR  DDRB
#define NRF_MISO_MSK  _BV(6)

#define NRF_SCK_PORT PORTB
#define NRF_SCK_DDR  DDRB
#define NRF_SCK_MSK  _BV(7)


/* PORT C
   [7:0] LCD 8-bit Parallel Data In (IN/OUT)
*/

#define LCD_DIN_PORT PORTC
#define LCD_DIN_DDR  DDRC
#define LCD_DIN_MSK  0xff


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

#define HC05_RX_PORT PORTD
#define HC05_RX_DDR  DDRD
#define HC05_RX_MSK  _BV(0)

#define HC05_TX_PORT PORTD
#define HC05_TX_DDR  DDRD
#define HC05_TX_MSK  _BV(1)

#define NRF_IRQ_PORT PORTD
#define NRF_IRQ_DDR  DDRD
#define NRF_IRQ_MSK  _BV(2)

#define LCD_RESET_PORT PORTD
#define LCD_RESET_DDR  DDRD
#define LCD_RESET_MSK  _BV(3)

#define LCD_RD_PORT PORTD
#define LCD_RD_DDR  DDRD
#define LCD_RD_MSK  _BV(4)

#define LCD_WR_PORT PORTD
#define LCD_WR_DDR  DDRD
#define LCD_WR_MSK  _BV(5)

#define LCD_CD_PORT PORTD
#define LCD_CD_DDR  DDRD
#define LCD_CD_MSK  _BV(6)

#define LCD_CS_PORT PORTD
#define LCD_CS_DDR  DDRD
#define LCD_CS_MSK  _BV(7)


