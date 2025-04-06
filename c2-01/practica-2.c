/*
Crear un programa que imprime por serial un número aleatorio entre
1 y 6. El número solo se actualiza cuando el usuario toca un pin táctil.
Mientras no se toca el pin táctil no se imprime nada.
*/

#include <stdio.h>
#include<stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include<time.h>
#include "driver/touch_pad.h"

uint16_t touch_value;

void app_main()
{
    srand(time(NULL));
    // Inicializar el subsistema de pines táctiles
    touch_pad_init();

    // Configurar los pines táctiles que se van a utilizar
    touch_pad_config(TOUCH_PAD_NUM0, 0);

    while (1)
    {
        touch_pad_read(TOUCH_PAD_NUM0, &touch_value);
        if (touch_value<700)
        {
            int numeroramdom=(rand()%6)+1;
            printf("Se ha tocado el pin: %d\n", touch_value);
            vTaskDelay( 500 / portTICK_PERIOD_MS);
            printf("\nNumero aleatorio: %d\n",numeroramdom );
            vTaskDelay( 500 / portTICK_PERIOD_MS);
        }
    }
    
}

