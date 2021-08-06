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

struct receive_packet{
	uint8_t red: 1;
	uint8_t green: 1;
	uint8_t blue: 1;
};

struct transmit_packet {
	uint8_t red: 1;
	uint8_t green: 1;
	uint8_t blue: 1;
	uint8_t btn1: 1;
	uint8_t btn2: 1;
};

void board_init()
{
}

bool received = false;

void UART0_Handler(void)
{
	uint32_t status;
	status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, status); 

	received = true;
}

int main()
{
	HwSetup();

	bool btn1_was_pressed = false;
	bool btn2_was_pressed = false;
	
	while(true) {
	
		if(received) {
			
			while(UARTCharsAvail(UART0_BASE))
			{
				int32_t casted_character = UARTCharGetNonBlocking(UART0_BASE);
				uint8_t character = ((uint8_t*)&casted_character)[0];
		
				struct receive_packet packet = *((struct receive_packet*)&character);
		
				if(packet.red) led_toggle(LED_RED);
				if(packet.green) led_toggle(LED_GREEN);
				if(packet.blue) led_toggle(LED_BLUE);
			}
			
			received = false;
		}
		
		bool current_btn2 = isButtonPressed(BTN_1);
		bool current_btn1 = isButtonPressed(BTN_2);
		
		
		if((!btn1_was_pressed && current_btn1) || (!btn2_was_pressed && current_btn2)) {
		
			struct transmit_packet packet = {0, 0, 0, 0, 0};

			packet.red = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)& GPIO_PIN_1) >> 1;
			packet.blue = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)& GPIO_PIN_2) >> 2;
			packet.green = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) & GPIO_PIN_3) >> 3;

//			if(current_btn1) packet.btn1 = 1;
//			if(current_btn2) packet.btn2 = 1;
			packet.btn1 = current_btn1;
			packet.btn2 = current_btn2;
			
			UARTCharPutNonBlocking(UART0_BASE, *((uint8_t*)&packet));
			
		}
		
		btn1_was_pressed = current_btn1;
		btn2_was_pressed = current_btn2;
	}

	return 0;
}
