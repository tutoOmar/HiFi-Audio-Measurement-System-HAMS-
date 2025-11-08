/*******************************************************************************
 * Archivo: i2s_sine_wave.h
 * Descripción: Declaraciones de funciones para generador de sine wave
 ******************************************************************************/

#ifndef I2S_SINE_WAVE_H
#define I2S_SINE_WAVE_H

#include <stdint.h>

/*******************************************************************************
 * DECLARACIÓN DE FUNCIONES PÚBLICAS
 ******************************************************************************/

/*
 * Función: generate_sine_table
 * Descripción: Genera la tabla precalculada de sine wave (llamar una sola vez)
 * Parámetros: ninguno
 * Retorno: ninguno
 */
void generate_sine_table(void);

/*
 * Función: I2S_Send_Sample
 * Descripción: Envía una muestra estéreo al DAC vía I2S
 * Parámetros:
 *   sample_left  - Muestra canal izquierdo (24-bit con signo)
 *   sample_right - Muestra canal derecho (24-bit con signo)
 * Retorno: ninguno
 */
void I2S_Send_Sample(int32_t sample_left, int32_t sample_right);

/*
 * Función: I2S_Generate_Sine_Wave
 * Descripción: Loop infinito que genera y transmite sine wave continuamente
 * Parámetros: ninguno
 * Retorno: ninguno (nunca retorna)
 */
void I2S_Generate_Sine_Wave(void);

#endif /* I2S_SINE_WAVE_H */