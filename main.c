#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "include/ssd1306.h"
#include "include/adc_read.h"
#include "include/ekf.h"

#define I2C_PORT i2c1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA_PIN  14  
#define OLED_SCL_PIN  15 
#define OLED_ADDR 0x3C  
#define MUX_A 8
#define MUX_B 9

#define ADC_CHANNEL 28


double Vt_Estimated = 0.0, Vt_Error = 0;
double KG[2][1];
double X[2][1];
double Nc=1.425;


double P[2][2] = {{0.950000073892419, 0}, {0, 9.999996034254936}};
double Q[2][2] = {{9.999944378671664e-08, 0}, {0, 9.999998929097178}}; 
double R = 7.999987139962964;

ssd1306_t disp;

typedef struct {
    double voltage;
    double temperature;
    double current;
    double soc;
} sensor_data_t;


sensor_data_t battery;

void adc_task(void *params) {
    double voltage;
    double temperature;
    double current;
    X[0][0]=1;
    X[1][0]=0;
    uint16_t value;
    while (1)
    {
        gpio_put(MUX_A, 0);
        gpio_put(MUX_B, 0);
        vTaskDelay(pdMS_TO_TICKS(1));
        value=adc_read();
        voltage = adc_to_voltage(value);

        gpio_put(MUX_A, 0);
        gpio_put(MUX_B, 1);
        vTaskDelay(pdMS_TO_TICKS(1));
        
        value=adc_read();
        current = adc_to_current(value);    
        
        gpio_put(MUX_A, 1);
        gpio_put(MUX_B, 0);
        vTaskDelay(pdMS_TO_TICKS(1));

        value=adc_read();
        temperature = adc_to_temperature(value);
        
        EKF_SOC_OPT(current, voltage, temperature, Nc, X, &Vt_Estimated, &Vt_Error, P, Q, R, KG);
        

        battery.voltage = voltage;
        battery.temperature = temperature;
        battery.current = current;
        battery.soc = X[0][0]*100;

        printf("Voltage: %lf, Temperature: %lf, Current: %lf, SoC:  %lf\n", voltage, temperature, current,X[0][0]);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void display_task(void *params) {
    while (1) {
        ssd1306_clear(&disp);
        char buf[32];

        sprintf(buf, "V: %.3f V", battery.voltage);
        ssd1306_draw_string(&disp, 0, 10, 1, buf);

        sprintf(buf, "T: %.2f C", battery.temperature);
        ssd1306_draw_string(&disp, 0, 20, 1, buf);

        sprintf(buf, "I: %.4f A", battery.current);
        ssd1306_draw_string(&disp, 0, 30, 1, buf);

        sprintf(buf, "SoC: %.1f%%", battery.soc);
        ssd1306_draw_string(&disp, 0, 40, 1, buf);


        ssd1306_show(&disp);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(ADC_CHANNEL);
    adc_select_input(2);  


    i2c_init(I2C_PORT, 400000);

    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);


    gpio_init(MUX_A);
    gpio_set_dir(MUX_A, GPIO_OUT);
    gpio_init(MUX_B);
    gpio_set_dir(MUX_B, GPIO_OUT);

    ssd1306_init(&disp, SCREEN_WIDTH, SCREEN_HEIGHT, OLED_ADDR, I2C_PORT);


    xTaskCreate(display_task, "Button Task", 256, NULL, 1, NULL);
    xTaskCreate(adc_task, "ADC Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (true) {}
}
