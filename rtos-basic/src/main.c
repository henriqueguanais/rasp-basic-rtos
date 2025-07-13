#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/adc.h"

#include "FreeRTOS.h"
#include "task.h"

#include "control_joy.h"

#define LED_PIN_B 12
#define TEMPERATURE_SENSOR 4

void setup();
void vBlinkTask(void *pvParameters);
void vread_temp(void *pvParameters);
void vjoystick_direction(void *pvParameters);

void main()
{
    setup();
    setup_joystick();

    xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);
    xTaskCreate(vread_temp, "Read Temp Task", 512, NULL, 1, NULL);
    xTaskCreate(vjoystick_direction, "Joystick Direction Task", 256, NULL, 1, NULL);
    
    vTaskStartScheduler();
}

void setup() {
    stdio_init_all();

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

}

void vBlinkTask(void *pvParameters)
{

    for (;;)
    {
        gpio_put(LED_PIN_B, 1);
        vTaskDelay(pdMS_TO_TICKS(50));

        gpio_put(LED_PIN_B, 0);
        vTaskDelay(pdMS_TO_TICKS(950));

        printf("Piscando\n");
    }
}

void vread_temp(void *pvParameters) {
    const float conversion_factor = 3.3f / 4095;
    float adc = 0, tempC = 0;
    float voltage = 0;
    adc_set_temp_sensor_enabled(true);

    for (;;) {
        adc_select_input(TEMPERATURE_SENSOR);
        
        adc = adc_read();
        voltage = adc * conversion_factor;
        tempC = 27 - (voltage - 0.706f) / 0.001721f;

        printf("Temperatura: %.2f C\n", tempC);
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void vjoystick_direction(void *pvParameters)
{
    uint16_t x = 0, y = 0, sw = 0;
    uint8_t direction[4] = {0}; // Cima, Baixo, Esquerda, Direita
    int print_counter = 0;

    for (;;)
    {
        joystick_read_axis(&x, &y, &sw);
        
        if (x > 3000) {
            direction[0] = 1; direction[1] = 0; direction[2] = 0; direction[3] = 0;
        } else if (x < 1000) {
            direction[0] = 0; direction[1] = 1; direction[2] = 0; direction[3] = 0;
        } else if (y > 3000) {
            direction[0] = 0; direction[1] = 0; direction[2] = 0; direction[3] = 1;
        } else if (y < 1000) {
            direction[0] = 0; direction[1] = 0; direction[2] = 1; direction[3] = 0;
        }

        if (++print_counter >= 5) {
            printf("Joystick Direction - Cima: %d, Baixo: %d, Esquerda: %d, Direita: %d\n",
                direction[0], direction[1], direction[2], direction[3]);
            print_counter = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}