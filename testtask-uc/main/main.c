/*
*Code must starting on Raspberry Pi RP2040.
*Used compiler gcc-arm-none-eabi 10.3.1, build system mingw32, generator CMake. It's path's must be defined.
*PicoSDK MUST be on computer and it's path's MUST be defined.
*https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html
*/

/*Including neccessary libraries*/
#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include <ctype.h>
/*PWM(LED) definition*/
#define LED0 7
#define LED1 8
#define LED2 9
#define LED3 10
#define LED4 11
#define LED5 12
#define LED6 13
#define LED7 14
#define GPIO_MASK_MAX 0x7F80
/*UART definition*/
#define UART_ID uart0
#define BAUD_RATE_h 115200
#define UART_TX_PIN_h 0
#define UART_RX_PIN_h 1
/*PWM variables*/
uint slice_num[8];
uint8_t s_num = 0;
uint8_t s_num_prev = 0;
bool once_flag = false;
/*UART variables*/
uint8_t ch = 0;
/*Clearing func for correctly start PWM control task*/
void clear(uint8_t type)
{
    switch (type)
    {
    case 0:
        for (uint8_t i = 0; i < 8; i++)
        {
            pwm_set_chan_level(slice_num[i], PWM_CHAN_B, 500);
            pwm_set_chan_level(slice_num[i], PWM_CHAN_A, 500);
        }
        s_num = 0;
        s_num_prev = 0;
        break;
    case 1:
        for (uint8_t i = 0; i < 8; i++)
        {
            pwm_set_chan_level(slice_num[i], PWM_CHAN_B, 0);
            pwm_set_chan_level(slice_num[i], PWM_CHAN_A, 0);
        }
        s_num = 0;
        s_num_prev = 0;
        break;
    default:
        pwm_set_chan_level(slice_num[0], PWM_CHAN_A, 250);
        pwm_set_chan_level(slice_num[0], PWM_CHAN_B, 250);
        pwm_set_chan_level(slice_num[1], PWM_CHAN_A, 500);
        pwm_set_chan_level(slice_num[1], PWM_CHAN_B, 500);
        pwm_set_chan_level(slice_num[2], PWM_CHAN_A, 500);
        pwm_set_chan_level(slice_num[2], PWM_CHAN_B, 500);
        pwm_set_chan_level(slice_num[3], PWM_CHAN_A, 250);
        pwm_set_chan_level(slice_num[3], PWM_CHAN_B, 250);
        for (uint8_t i = 4; i < 8; i++)
        {
            pwm_set_chan_level(slice_num[i], PWM_CHAN_A, 0);
            pwm_set_chan_level(slice_num[i], PWM_CHAN_B, 0);
        }
        s_num = 2;
        s_num_prev = 1;
        break;
    }
}
/*PWM control task*/
void led_task(uint8_t state)
{
    switch (state)
    {
    case 1:
    /*Wave*/
        s_num = 2;
        s_num_prev = 1;
        while (s_num < 6)
        {
            for (uint16_t i = 0; i < 500; i++)
            {
                pwm_set_chan_level(slice_num[s_num_prev - 1], PWM_CHAN_A, 250 - i / 2);
                pwm_set_chan_level(slice_num[s_num_prev - 1], PWM_CHAN_B, 250 - i / 2);
                pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_A, 500 - i / 2);
                pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_B, 500 - i / 2);
                (s_num_prev + 3) > 7 ?: pwm_set_chan_level(slice_num[s_num_prev + 3], PWM_CHAN_A, i / 2);
                (s_num_prev + 3) > 7 ?: pwm_set_chan_level(slice_num[s_num_prev + 3], PWM_CHAN_B, i / 2);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_A, 500 - i / 2);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_B, 500 - i / 2);
                (s_num + 1) > 7 ?: pwm_set_chan_level(slice_num[s_num + 1], PWM_CHAN_A, 250 + i / 2);
                (s_num + 1) > 7 ?: pwm_set_chan_level(slice_num[s_num + 1], PWM_CHAN_B, 250 + i / 2);
                sleep_ms(2);
            }

            s_num_prev = s_num;
            s_num++;
        }
        s_num = 5;
        s_num_prev = 4;
        while (s_num_prev > 0)
        {
            for (uint16_t i = 0; i < 500; i++)
            {
                pwm_set_chan_level(slice_num[s_num_prev - 1], PWM_CHAN_A, i / 2);     //0 to mid
                pwm_set_chan_level(slice_num[s_num_prev - 1], PWM_CHAN_B, i / 2);     //0 to mid
                pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_A, 250 + i / 2);
                pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_B, 250 + i / 2);
                
                pwm_set_chan_level(slice_num[s_num + 2], PWM_CHAN_A, 250 - i / 2);
                pwm_set_chan_level(slice_num[s_num_prev + 2], PWM_CHAN_B, 250 - i / 2);
                (s_num + 1) > 7 ?: pwm_set_chan_level(slice_num[s_num + 1], PWM_CHAN_A, 500 - i / 2);
                (s_num + 1) > 7 ?: pwm_set_chan_level(slice_num[s_num + 1], PWM_CHAN_B, 500 - i / 2);
                sleep_ms(2);
            }
            s_num = s_num_prev;
            s_num_prev--;
        }
        break;
    case 2:
    /*Running light*/
        while (s_num < 8)
        {
            for (uint16_t i = 0; i < 500; i++)
            {
                s_num == s_num_prev ?: (slice_num[s_num_prev], PWM_CHAN_A, 500 - i);
                s_num == s_num_prev ?: pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_B, 500 - i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_A, i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_B, i);
                sleep_ms(2);
            }
            s_num_prev = s_num;
            s_num++;
        }
        s_num = 6;
        while (s_num != 0)
        {
            for (uint16_t i = 0; i < 500; i++)
            {
                s_num == s_num_prev ?: (slice_num[s_num_prev], PWM_CHAN_A, 500 - i);
                s_num == s_num_prev ?: pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_B, 500 - i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_A, i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_B, i);
                sleep_ms(2);
            }
            s_num_prev = s_num;
            s_num--;
        }
        break;
    case 3:
    /*Flashing LED0*/
        pwm_set_chan_level(slice_num[0], PWM_CHAN_A, 500);
        pwm_set_chan_level(slice_num[0], PWM_CHAN_B, 500);
        sleep_ms(500);
        pwm_set_chan_level(slice_num[0], PWM_CHAN_A, 0);
        pwm_set_chan_level(slice_num[0], PWM_CHAN_B, 0);
        sleep_ms(500);
        break;
    default:
    /*Running shadow*/
        while (s_num < 8)
        {
            for (uint16_t i = 500; i > 0; i--)
            {
                s_num == s_num_prev ?: (slice_num[s_num_prev], PWM_CHAN_A, 500 - i);
                s_num == s_num_prev ?: pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_B, 500 - i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_A, i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_B, i);
                sleep_ms(20);
            }
            s_num_prev = s_num;
            s_num++;
        }
        s_num = 6;
        while (s_num != 0)
        {
            for (uint16_t i = 500; i > 0; i--)
            {
                s_num == s_num_prev ?: pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_A, 500 - i);
                s_num == s_num_prev ?: pwm_set_chan_level(slice_num[s_num_prev], PWM_CHAN_B, 500 - i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_A, i);
                pwm_set_chan_level(slice_num[s_num], PWM_CHAN_B, i);
                sleep_ms(20);
            }
            s_num_prev = s_num;
            s_num--;
        }
        break;
    }
}
/*UART callback*/
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        ch = uart_getc(UART_ID);
    }
    if (!isdigit(ch))
    {
        ch = 0;
    }
}
/*UART initialisation*/
void UART_Init()
{
    uart_init(UART_ID, BAUD_RATE_h);
    gpio_set_function(UART_TX_PIN_h, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN_h, GPIO_FUNC_UART);
    uart_set_fifo_enabled(UART_ID, false);
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}
/*PWM initialisation*/
void PWM_Init()
{
    gpio_set_function(LED0, GPIO_FUNC_PWM);
    gpio_set_function(LED1, GPIO_FUNC_PWM);
    gpio_set_function(LED2, GPIO_FUNC_PWM);
    gpio_set_function(LED3, GPIO_FUNC_PWM);
    gpio_set_function(LED4, GPIO_FUNC_PWM);
    gpio_set_function(LED5, GPIO_FUNC_PWM);
    gpio_set_function(LED6, GPIO_FUNC_PWM);
    gpio_set_function(LED7, GPIO_FUNC_PWM);
    gpio_set_function(25, GPIO_FUNC_PWM);
    for (uint8_t i = 0; i < 8; i++)
    {
        slice_num[i] = pwm_gpio_to_slice_num(7 + i);
        pwm_set_enabled(slice_num[i], true);
        pwm_set_wrap(slice_num[i], 500);
        pwm_set_chan_level(slice_num[i], PWM_CHAN_A, 0);
        pwm_set_chan_level(slice_num[i], PWM_CHAN_B, 0);
    }
}
/*Program entry point*/
int main()
{
    stdio_init_all();
    UART_Init();
    PWM_Init();
    while (1)
    {
        led_task(ch);
    }
    return 0;
}
/*
*Note:
*Launch script maked for openocd and jlink, you can change it, if you want, in launch.json file.
*Note 2:
*If you want to compile this program (pre-compiled executable is build/main/testtask_uc.uf2)
*please, read information in settings.json and c_cpp_properties.json and change path's to your's.
*/