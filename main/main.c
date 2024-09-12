#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t MyFirstTaskHandle = NULL;

void My_First_Task(void* arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My First Task %d\n", i);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        i++;
    }
}

void app_main(void)
{
    xTaskCreate(My_First_Task, "First_Task", 4096, NULL, 10, &MyFirstTaskHandle);
}
