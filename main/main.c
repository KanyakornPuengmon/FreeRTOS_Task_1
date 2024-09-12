#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Handle สำหรับ task ทั้งสอง
TaskHandle_t MyFirstTaskHandle = NULL;
TaskHandle_t MySeconeTaskHandle = NULL;

// Task แรก
void My_First_Task(void * arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My First Task %d\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        i++;

        // Suspend task ที่สองเมื่อ i เท่ากับ 5
        if(i == 5)
        {
            vTaskSuspend(MySeconeTaskHandle);
            printf("Second Task suspended\n");
        }

        // Resume task ที่สองเมื่อ i เท่ากับ 10
        if(i == 10)
        {
            vTaskResume(MySeconeTaskHandle);
            printf("Second Task resumed\n");
        }

        // ลบ task ที่สองเมื่อ i เท่ากับ 15
        if(i == 15)
        {
            vTaskDelete(MySeconeTaskHandle);
            printf("Second Task deleted\n");
        }

        // Suspend ตัวเองเมื่อ i เท่ากับ 20
        if(i == 20)
        {
            printf("MyFirstTaskHandle will suspend itself\n");
            vTaskSuspend(NULL);  // NULL จะหมายถึง task ปัจจุบัน
        }
    }
}

// Task ที่สอง
void My_Second_Task(void * arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My Second Task %d\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        i++;
    }
}

// ฟังก์ชันหลักของโปรแกรม
void app_main(void)
{
    // สร้างและรัน task แรก
    xTaskCreate(My_First_Task, "First_Task", 4096, NULL, 10, &MyFirstTaskHandle);

    // สร้างและรัน task ที่สอง
    xTaskCreate(My_Second_Task, "Second_Task", 4096, NULL, 10, &MySeconeTaskHandle);
}

