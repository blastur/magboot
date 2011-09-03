/* Target configuration */
#define F_CPU (16000000L)
#define JUMP_ADDR (0x00)

/* SWUART pin-configuration */
#define CONFIG_SWUART_RX_BIT	(PB3)
#define CONFIG_SWUART_RX_PORT	(PORTB)
#define CONFIG_SWUART_RX_PIN	(PINB)
#define CONFIG_SWUART_RX_DIR	(DDRB)

#define CONFIG_SWUART_TX_BIT	(PB4)
#define CONFIG_SWUART_TX_PORT	(PORTB)
#define CONFIG_SWUART_TX_DIR	(DDRB)

