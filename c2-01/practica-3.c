/*Desarrollar un programa donde se toque un pin táctil LUEGO de
mostrar un mensaje por serial. La idea es que se toque lo más rápido
posible, por lo cual se debe imprimir el tiempo transcurrido.*/
        
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include <esp_timer.h>

uint16_t touch_value;
unsigned long touch_start_time;
unsigned long touch_end_time;

void app_main() {
    // Inicializar el subsistema de pines táctiles
    touch_pad_init();

    // Configurar el pin táctil que se va a utilizar (GPIO 4 por defecto para TOUCH0)
    touch_pad_config(TOUCH_PAD_NUM0, 0);

    printf("Prueba de velocidad\n");
    printf("-------------------------------------\n");
    printf("Toca cuando se le indique:\n");

    for (int i = 5; i > 0; i--) {
        printf("%d\n", i);
        fflush(stdout);  // Asegura que se imprima inmediatamente
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("¡YA!\n");
    fflush(stdout);

    // Tiempo de inicio justo después del "YA"
    touch_start_time = esp_timer_get_time() / 1000; // en milisegundos

    while (1) {
        touch_pad_read(TOUCH_PAD_NUM0, &touch_value);
        if (touch_value < 700) { // Umbral de toque, puedes ajustarlo
            touch_end_time = esp_timer_get_time() / 1000;

            unsigned long reaction_time = touch_end_time - touch_start_time;

            printf("Se ha tocado el pin (valor leído: %d)\n", touch_value);
            printf("Tiempo de reacción: %lu ms\n", reaction_time);

            break; // Finaliza el loop tras detectar el primer toque
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Pequeña pausa para no saturar la CPU
    }
}

