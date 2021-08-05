#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"

#include "driverlib/uart.h"
#include "driverlib/pin_map.h" 
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
//#include "inc/hw_gpio.h" 

#include "inc/tm4c123gh6pm.h"

#define BAUD_RATE 95550

#define SWITCH_1 GPIO_PIN_4
#define SWITCH_2 GPIO_PIN_0

//struct receive_packet{
//	uint8_t red: 1;
//	uint8_t green: 1;
//	uint8_t blue: 1;
//	uint8_t off: 1;
//	uint8_t on: 1;
//};

struct receive_packet{
	uint8_t red: 1;
	uint8_t green: 1;
	uint8_t blue: 1;
};

struct transmit_packet {
	uint8_t red: 1;
	uint8_t green: 1;
	uint8_t blue: 1;
	uint8_t switch1: 1;
	uint8_t switch2: 1;
};

void board_init()
{
}

void GPIOF_Handler(void) {
	
	uint32_t status;
	status = GPIOIntStatus(GPIO_PORTF_BASE, true);
	GPIOIntClear(GPIO_PORTF_BASE, status);
	
	struct transmit_packet packet = {0, 0, 0, 0, 0};
	packet.red = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)& GPIO_PIN_1) >> 1;
	packet.blue = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)& GPIO_PIN_2) >> 2;
	packet.green = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) & GPIO_PIN_3) >> 3;
	
//	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) & GPIO_PIN_1) packet.green = 1;
//	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) & GPIO_PIN_2) packet.blue = 1;
//	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) & GPIO_PIN_3) packet.red = 1;

	switch(status) {
	case SWITCH_1:
		packet.switch1 = 1;
		break;
	case SWITCH_2: 
		packet.switch2 = 1;
		break;

	}
	
	UARTCharPutNonBlocking(UART0_BASE, *((uint8_t*)&packet));
}

void UART0_Handler(void)
{
	uint32_t status;
	status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, status); 

	while(UARTCharsAvail(UART0_BASE))
	{
		int32_t casted_character = UARTCharGetNonBlocking(UART0_BASE);
		uint8_t character = ((uint8_t*)&casted_character)[0];
		
		struct receive_packet packet = *((struct receive_packet*)&character);
		
		if(packet.red) led_toggle(LED_RED);
		if(packet.green) led_toggle(LED_GREEN);
		if(packet.blue) led_toggle(LED_BLUE);
		
//		if(packet.on) {
//			if(packet.red) led_on(LED_RED);
//			if(packet.green) led_on(LED_GREEN);
//			if(packet.blue) led_on(LED_BLUE);
//		} else if(packet.off) {
//			if(packet.red) led_off(LED_RED);
//			if(packet.green) led_off(LED_GREEN);
//			if(packet.blue) led_off(LED_BLUE);
//		}
		
		
//		UARTCharPutNonBlocking(UART0_BASE, character);
//		switch(character) {
//		case 'e':
//			led_on(LED_GREEN);
////			led_on(LED_BLUE);
////			led_on(LED_RED);
//			break;
//			
//		case 'd':
//			led_off(LED_GREEN);
////			led_off(LED_BLUE);
////			led_off(LED_RED);
//			break;
//		}
		
	}
}


int main()
{
	HwSetup();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	
	GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	
	IntEnable(INT_GPIOF);
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_FALLING_EDGE);
	
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	
	IntMasterEnable();
	
	while(true) {}

	return 0;
}