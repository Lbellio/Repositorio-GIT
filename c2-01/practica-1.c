/*
De los pines táctiles disponibles en su placa, escoger uno e imprimir
cuantas veces se ha tocado el pin táctil. Tener en cuenta que se debe
considerar un "toque" como presionar y soltar el pin táctil.
*/

#include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
#include "driver/touch_pad.h"

uint16_t touch_value;

void app_main()
{
    // Inicializar el subsistema de pines táctiles
    touch_pad_init();

    // Configurar los pines táctiles que se van a utilizar
    touch_pad_config(TOUCH_PAD_NUM0, 0);

    while(1)
    {
        touch_pad_read(TOUCH_PAD_NUM0, &touch_value);
        if (touch_value<700)
        {
            printf("Se ha tocado el pin \n");
            printf("%d\n", touch_value);
            vTaskDelay( 500 / portTICK_PERIOD_MS);
        }
        else
        {
            printf("No se ha tocado el pin \n");
            printf("%d\n", touch_value);
            vTaskDelay( 500 / portTICK_PERIOD_MS);
        }
        
    }
}