#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// กำหนดขา GPIO สำหรับ LED และปุ่มกด
#define LED_PIN 27
#define PUSH_BUTTON_PIN 33

// Handle สำหรับ task ทั้งหมด
TaskHandle_t ISR = NULL;
TaskHandle_t MyFirstTaskHandle = NULL;
TaskHandle_t MySeconeTaskHandle = NULL;
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

// Task สำหรับจัดการ interrupt
void interrupt_task(void *arg)
{
  bool led_status = false;
  while(1)
  {
    vTaskSuspend(NULL);  // รอจนกว่าจะถูก resume
    led_status = !led_status;
    gpio_set_level(LED_PIN, led_status);  // เปิด/ปิด LED
    printf("Button pressed!\n");
  }
}
void IRAM_ATTR button_isr_handler(void *arg)
{
  xTaskResumeFromISR(ISR);  // Resume task ที่ handle interrupt
}

// ฟังก์ชันหลักของโปรแกรม
void app_main(void)
{
    // กำหนดทิศทางของขา GPIO
    esp_rom_gpio_pad_select_gpio(PUSH_BUTTON_PIN);
    esp_rom_gpio_pad_select_gpio(LED_PIN);

    gpio_set_direction(PUSH_BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(LED_PIN ,GPIO_MODE_OUTPUT);

    // กำหนดประเภท interrupt สำหรับปุ่มกด
    gpio_set_intr_type(PUSH_BUTTON_PIN, GPIO_INTR_POSEDGE);

    // ติดตั้ง ISR service
    gpio_install_isr_service(0);

    // กำหนด handler สำหรับปุ่มกด
    gpio_isr_handler_add(PUSH_BUTTON_PIN, button_isr_handler, NULL);

    // สร้างและรัน task สำหรับ interrupt
    xTaskCreate(interrupt_task, "interrupt_task", 4096, NULL, 10, &ISR);

    // สร้างและรัน task แรก
    xTaskCreate(My_First_Task, "First_Task", 4096, NULL, 10, &MyFirstTaskHandle);

    // สร้างและรัน task ที่สอง
    xTaskCreate(My_Second_Task, "Second_Task", 4096, NULL, 10, &MySeconeTaskHandle);
}
