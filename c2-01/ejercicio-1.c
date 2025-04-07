/* En un proyecto de telemetría se tiene un caudalímetro que envía por puerto
 serial cada periodo de tiempo la cantidad de caudal detecta, el cual es un
 número entero no mayor de 2 dígitos (es decir, se enviará un número entre
 00 y 99)
 Se recibirá el número por puerto serial, se debe imprimir por serial la
 siguiente información:
 1. El número mínimo recibido.
 2. El número mayor recibido.
 3. El último número recibido.
 4. El promedio de todos los números recibidos.*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Definiciones para la UART
#define TAMAÑO_BUFFER 1024
#define VELOCIDAD_BAUDIOS 115200
#define PUERTO_UART UART_NUM_0

// Variables para estadísticas
static int valor_minimo = 99;
static int valor_maximo = 0;
static int ultimo_valor = 0;
static int suma_valores = 0;
static int cantidad_valores = 0;

// Inicializa la UART con parámetros específicos
void inicializar_uart() {
    uart_config_t configuracion_uart = {
        .baud_rate = VELOCIDAD_BAUDIOS,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(PUERTO_UART, &configuracion_uart);
    uart_driver_install(PUERTO_UART, TAMAÑO_BUFFER * 2, 0, 0, NULL, 0);
}

// Procesa un número válido recibido
void procesar_valor(int valor) {
    ultimo_valor = valor;

    if (valor < valor_minimo) valor_minimo = valor;
    if (valor > valor_maximo) valor_maximo = valor;

    suma_valores += valor;
    cantidad_valores++;

    float promedio = (float)suma_valores / cantidad_valores;

    printf("Último: %d. Mínimo: %d. Máximo: %d. Promedio: %.2f\n",
           ultimo_valor, valor_minimo, valor_maximo, promedio);
}

// Procesa el buffer recibido por UART, aceptando solo números válidos
void procesar_datos(const char* datos, int longitud) {
    int i = 0;

    while (i < longitud - 1) {
        // Verifica si hay dos dígitos consecutivos
        if (isdigit((unsigned char)datos[i]) && isdigit((unsigned char)datos[i + 1])) {
            // Verifica que no hay más dígitos antes o después, ni letras incrustadas
            bool antes_es_digito = (i > 0) && isdigit((unsigned char)datos[i - 1]);
            bool despues_es_digito = (i + 2 < longitud) && isdigit((unsigned char)datos[i + 2]);
            bool antes_es_letra = (i > 0) && isalpha((unsigned char)datos[i - 1]);
            bool despues_es_letra = (i + 2 < longitud) && isalpha((unsigned char)datos[i + 2]);

            if (!antes_es_digito && !despues_es_digito && !antes_es_letra && !despues_es_letra) {
                int valor = (datos[i] - '0') * 10 + (datos[i + 1] - '0');
                procesar_valor(valor);
                i += 2; // Salta el segundo dígito ya procesado
                continue;
            }
        }

        i++; // Avanza al siguiente carácter
    }
}

// Función principal del programa (bucle principal)
void app_main() {
    inicializar_uart();

    printf("Sistema de telemetría de caudalímetro iniciado.\n");
    printf("Esperando datos en formato de 2 dígitos (00-99)...\n\n");

    uint8_t datos[TAMAÑO_BUFFER];

    while (1) {
        int longitud = uart_read_bytes(PUERTO_UART, datos, TAMAÑO_BUFFER, 20 / portTICK_PERIOD_MS);

        if (longitud > 0) {
            procesar_datos((char*)datos, longitud);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

