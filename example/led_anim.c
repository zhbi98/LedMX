/**
 * @file led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "led_anim.h"
#include "ledmx.h"
#include "gpio.h"
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _driver_refer(uint8_t state);
static void example();

/**********************
 *  STATIC VARIABLES
 **********************/

static volatile uint32_t _led_tick = 0;

static led_dev_t led_device[1] = {
    {.refer = _driver_refer},
};

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Direct GPIO control for LED hardware, uses PB0 pin for LED output.
 * @param state Boolean value 0 LED OFF 1 LED ON.
 */
static void _driver_refer(uint8_t state)
{
    if (state) {
        HAL_GPIO_WritePin(GPIOB, 
            GPIO_PIN_12, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOB, 
            GPIO_PIN_12, GPIO_PIN_RESET);
    }
}

/**
 * Each task callback function is timed 
 * and used to execute the corresponding 
 * callback function at the end of time.
 */
void led_anim_tick_inc(uint32_t tick_period)
{
    static uint32_t _last_tick = 0;
    static uint32_t _tick = 0;

    _led_tick += tick_period;
    _tick = _led_tick;

    if (_tick - _last_tick > 20) {
        _last_tick = _led_tick;
        led_dev_tick_inc(1); 
    }
}

/**
 * Initializes LED device in control system, 
 * registers LED control instance with device manager.
 */
void led_anim_start()
{
    led_dev_register(led_device, 1);
}

#define TICK_TIMES 1U

/**
 * example indication pattern (300ms period, 2 blinks)
 * error state visualization through timed toggling.
 */
static void example()
{
    uint16_t time = 300 * TICK_TIMES;
    uint8_t led_id = 0;

    led_dev_twinkle_by_cnt(led_id, 
        time, 3, LED_OFF);
}

/**
 * example indication pattern (300ms period, 2 blinks)
 * error state visualization through timed toggling.
 */
int32_t main()
{
    led_anim_start();
    example();

    for (;;) {
        led_dev_task_handler();
        led_anim_tick_inc(1);
    }

    return 0;
}
