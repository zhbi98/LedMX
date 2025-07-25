# LED 驱动框架

LED控制软件框架，可以十分方便地管理设备各个 LED 的状态，甚至多个 LED 组合控制，如跑马灯等，移植方便，无需修改，只需要初始化提供读取相关 IO 状态写入函数即可。

## 功能

(1) 可以实现单个LED亮灭、翻转、闪烁、呼吸灯、自定义（如多少秒快闪几次等）等多种要求的功能。

(1) 可以实现多个LED组合跑马灯、流水灯等功能。

(1) 同时支持设置上述模式的重复执行次数。

## 要点

如果设置某个LED为呼吸灯模式，则需要保证 `led_dev_task_handler` 调用周期为 1ms（优先级需要最高，或者定时器调度效果最好）
非阻塞执行方式，因此裸机和操作系统都适用，操作系统下非线程安全，最好可以使用读写锁，如果没有读写锁则至少使用互斥锁。

## 使用

使用前初始化函数 `led_dev_register`，把 LED 灯控制 IO 注册到 ledMX。
需要实现写入IO状态的函数，然后初始化结构体的 `pfnLedCtrl`即可。

周期调用函数 `led_dev_task_handler` 和 `led_dev_tick_inc` 入参为毫秒级的系统时长，用来具体控制 LED 状态时间
调用相关函数设置灯光效果，不支持重复调用，重复调用设置函数相关信息会复位

## 注册

```c
enum {
    LED_0 = 0,
    LED_1,
    LED_2,
    _LED_LAST
};

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

static led_dev_t led_device[1] = {
    {.refer = _driver_refer},
};

/**
 * Initializes LED device in control system, 
 * registers LED control instance with device manager.
 */
void led_anim_start()
{
    led_dev_register(led_device, 1);
}

```

## 执行

在 RTOS 中为 ledMX 创建 Tick 处理任务。

```c
void LedTask(void)
{
    uint32_t time;
    led_anim_start();
  
    while (1)
    {
        led_dev_task_handler();
        led_dev_tick_inc(1);
    }
}
```

在单片机主循环中处理 ledMX 的 Tick 任务。

```c
int main(void)
{
    int time1ms_cnt = 0;

    led_anim_start();

    while(1)
    {
        if (flag_1ms)
        {
            flag_1ms = 0;

            led_dev_task_handler();
            led_dev_tick_inc(1);
            time1ms_cnt++;
        }
    }
}
```

## 设置

执行下方 example 函数就可以让 led 闪烁 3 次后熄灭。

```c
#define TICK_TIMES 1U

/**
 * Stall indication pattern (300ms period, 2 blinks)
 * error state visualization through timed toggling.
 */
static void _example()
{
    uint16_t time = 300 * TICK_TIMES;
    uint8_t led_id = 0;

    led_dev_twinkle_by_cnt(led_id, 
        time, 3, LED_OFF);
}

```
