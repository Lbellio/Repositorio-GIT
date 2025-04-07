/*1. Desarrolla un sistema de autenticación basado en un patrón táctil.
2. El usuario debe tocar la secuencia en un pin táctil:
(a) 3 toques largos.
(b) 3 toques cortos.
(c) 3 toques largos.
3. Luego de hacer esa secuencia, se debe tocar otro pin táctil para validar
la secuencia.
4. Imprimir por serial "APROBADO" o "NO APROBADO" si la secuencia ingresada es correcta o no.
5. Por grupo definir el tiempo a su criterio para determinar que es "toque
largo" y por "toque corto".
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include"esp_timer.h"

#define touch_counter TOUCH_PAD_NUM0 // GIOP-4 o pin 26
#define touch_validate TOUCH_PAD_NUM4 //GIOP-13 o pin 15

#define short_touch_min 200 // 200Ms
#define short_touch_max 500 //  500MS
#define long_touch_min 700 // 700Ms 

#define max_sequence 9 //Solo recibira 9 toques
#define touch_max_range 700

char input_sequence[max_sequence];
int sequence_i =0;
//Deteccion del toque 
void detect_touch_sequence(){
    uint16_t touch_value;
    bool touched = false;
    unsigned long start = 0, end = 0;

    touch_pad_read(touch_counter, &touch_value);

    if (touch_value<touch_max_range && !touched)
    {
     start = esp_timer_get_time()/1000;
     touched=true;
     while (1)
     {
        touch_pad_read(touch_counter, &touch_value);
        if(touch_value >= touch_max_range){
            end=esp_timer_get_time()/1000;
            break;
        } vTaskDelay(10/portTICK_PERIOD_MS);
     }
     unsigned long duration = end-start;
     if (duration >= short_touch_min && duration <= short_touch_max)
     {
        input_sequence[sequence_i++] = 'S'; // Short, o toque corto
        printf("Toque corto (%lu ms)\n", duration);
        fflush(stdout);
     } else if (duration >= long_touch_min){
        input_sequence[sequence_i++]='L'; // Long, o Toque largo.
        printf("Toque largo (%lu ms)\n", duration);
        fflush(stdout);
     }
     {
        //Espera entre toques
        vTaskDelay(300/ portTICK_PERIOD_MS);
     }
     
     
    }
}

//Funcion para validar la contraseña. Tambien si es necesario cambiarla.
// Corregimos la función con nombre y comparación correcta
bool validate_pattern(){
    const char password[max_sequence+1]= "LLLSSSLLL";
    bool valid = strncmp(input_sequence, password, max_sequence)==0;

    if (valid){
        printf("APROBADO\n"); 
    } else {
        printf("NO APROBADO\n");
    }
    sequence_i = 0; // Reiniciar secuencia
    memset(input_sequence, 0, sizeof(input_sequence)); // Limpiar buffer
    return valid;
}

void app_main(){
    int attemps = 0;    
        // Inicializar el subsistema de pines táctiles
        touch_pad_init();

        // Configurar el pin táctil que se va a utilizar (GPIO 4 por defecto para TOUCH0)
        touch_pad_config(touch_counter, 0);
        touch_pad_config(touch_validate, 0);

    printf("Bienvenido al sistema de autenticacion basado en un patron tactil\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("Para acceder es necesario de una contraseña basada en toques largos y toques cortos.\n");
    printf("El programa toma en cuenta un toque corto entre 1 y 2 segundos y un toque largo de 3 segundos o más.\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("Por favor, digite la constraseña: \n");
    

       while (1)
       {
        if (attemps>=3)
        {
            printf("Demasiados intentos fallidos. Se cerrara el sistema....\n");
            fflush(stdout);
            vTaskDelay(1000/portTICK_PERIOD_MS);
            exit(0);
            
            
        }
        
        if (sequence_i<max_sequence){detect_touch_sequence();
        }else
        {
            uint16_t validate_value;
            touch_pad_read(touch_validate, &validate_value);
            if (validate_value < touch_max_range)
            {
                printf("Validando secuencia.....\n");
                fflush(stdout);
                bool success = validate_pattern();  // <- ¡Solo se llama una vez!
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            
                if (!success) {
                    attemps++;
                    printf("Intento fallido. Intentos restantes: %d\n", 3 - attemps);
                    fflush(stdout);
                } else {
                    printf("Aprobado. Finalizando programa.\n");
                    fflush(stdout);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    exit(0); // Sal del programa si está aprobado
                }
                
            
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
       }
       
}
