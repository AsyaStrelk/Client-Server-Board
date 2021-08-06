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

/*! \brief Структура, отражающая содержание получаемого байта 
 */
struct receive_packet{
	uint8_t red: 1;
	uint8_t green: 1;
	uint8_t blue: 1;
};

/*! \brief Структура, отражающая содержание байта, отправляемого на сервер при нажатии какой-либо кнопки 
 */
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

//флаг получения пакета
bool received = false;

/*! \brief Обработчик прерывания при появлении пакета на приеме
 */
void UART0_Handler(void)
{
	//читается и сбрасывается статус прерывания
	uint32_t status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, status); 

	//получен пакет
	received = true;
}

int main()
{
	HwSetup();

	//состояние светодиодов
	struct receive_packet led_state = {0, 0, 0};
	
	while(true) {
	
		if(received) {
			
			//обработка полученного пакета
			while(UARTCharsAvail(UART0_BASE))
			{
				//функция получения символа возвражает его приведя к int32_t
				int32_t casted_character = UARTCharGetNonBlocking(UART0_BASE);
				uint8_t character = ((uint8_t*)&casted_character)[0];
				struct receive_packet recv_packet = *((struct receive_packet*)&character);
		
				//изменение и сохранение состояния диодов
				if(recv_packet.red) 
				{
					led_toggle(LED_RED);
					led_state.red = led_state.red ? 0 : 1;
				}
				if(recv_packet.green)
				{
					led_toggle(LED_GREEN);
					led_state.green = led_state.green ? 0 : 1;
				}
				if(recv_packet.blue)
				{
					led_toggle(LED_BLUE);
					led_state.blue = led_state.blue ? 0 : 1;
				}
			}
			
			//сброс флага
			received = false;
		}
		
		//обработка нажатий кнопок
		uint8_t button_pressed = bspBtnPolling();
		if(button_pressed)
		{
			//если какая-либо кнопка была нажата необходимо подготовить пакет
			struct transmit_packet packet = {0, 0, 0, 0, 0};
			packet.red = led_state.red;
			packet.green = led_state.green;
			packet.blue = led_state.blue;
			
			switch(button_pressed)
			{
			case 1:
				packet.btn2 = 1;
				break;
			case 2:
				packet.btn1 = 1;
				break;
			}
			
			UARTCharPut(UART0_BASE, *((uint8_t*)&packet));
		}
	}

	return 0;
}
