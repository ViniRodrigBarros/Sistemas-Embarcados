#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define DELAY_MS 40   


#define LED1 4
#define LED2 5
#define LED3 6
#define LED4 7
#define BUZZER 8


int led_pins[] = {LED1, LED2, LED3, LED4};
ledc_channel_t led_channels[] = {
    LEDC_CHANNEL_0,
    LEDC_CHANNEL_1,
    LEDC_CHANNEL_2,
    LEDC_CHANNEL_3
};
#define BUZZER_CHANNEL LEDC_CHANNEL_4


void pwm_init() {
    ledc_timer_config_t led_timer = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 1000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&led_timer);

    ledc_timer_config_t buzzer_timer = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 1000, // inicial em 1kHz
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&buzzer_timer);


    for (int i = 0; i < 4; i++) {
        ledc_channel_config_t ledc_channel = {
            .channel    = led_channels[i],
            .duty       = 0,
            .gpio_num   = led_pins[i],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        };
        ledc_channel_config(&ledc_channel);
    }


    ledc_channel_config_t buzzer_channel = {
        .channel    = BUZZER_CHANNEL,
        .duty       = 0,
        .gpio_num   = BUZZER,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_1
    };
    ledc_channel_config(&buzzer_channel);
}


void set_led_duty(int led, int duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, led_channels[led], duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, led_channels[led]);
}


void set_buzzer_freq(int freq) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, freq);
}


void buzzer_on() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL, 512);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL);
}


void buzzer_off() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL);
}




void fading_sincronizado() {
    for (int duty = 0; duty <= 1023; duty += 10) {
        for (int i = 0; i < 4; i++) set_led_duty(i, duty);
        vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
    }
    for (int duty = 1023; duty >= 0; duty -= 10) {
        for (int i = 0; i < 4; i++) set_led_duty(i, duty);
        vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
    }
}

void sequencia_leds() {

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) set_led_duty(j, 0); 
        set_led_duty(i, 1023); 
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }


    for (int i = 2; i >= 0; i--) {
        for (int j = 0; j < 4; j++) set_led_duty(j, 0);
        set_led_duty(i, 1023);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }


    for (int j = 0; j < 4; j++) set_led_duty(j, 0);
}


void teste_buzzer() {
    buzzer_on();
    for (int freq = 500; freq <= 2000; freq += 50) {
        set_buzzer_freq(freq);
        vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
    }
    for (int freq = 2000; freq >= 500; freq -= 50) {
        set_buzzer_freq(freq);
        vTaskDelay(DELAY_MS / portTICK_PERIOD_MS);
    }
    buzzer_off();
}


void app_main(void) {
    pwm_init();

    while (1) {
        fading_sincronizado();   // Fase 1
        sequencia_leds();        // Fase 2 
        teste_buzzer();          // Fase 3
    }
}
