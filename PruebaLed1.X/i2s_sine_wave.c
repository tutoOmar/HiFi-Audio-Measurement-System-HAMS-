/*******************************************************************************
 * Archivo: i2s_sine_wave.c
 * Descripción: Generador de sine wave a 1kHz @ 48kHz para PCM5102 (DAC I2S)
 * Nota: Este código solo genera y transmite la sine wave. La configuración
 *       de I2S, clocks y pines ya está en i2s_init.c
 ******************************************************************************/

#include <math.h>
#include "definitions.h"

/*******************************************************************************
 * CONFIGURACIÓN DE SINE WAVE
 ******************************************************************************/
#define SAMPLE_RATE         48000      // Frecuencia de muestreo: 48 kHz
#define SINE_FREQ           1000       // Frecuencia de la señal: 1 kHz
#define SINE_AMPLITUDE      0.8f       // Amplitud: 80% del rango (evita clipping)
#define SAMPLES_PER_CYCLE   (SAMPLE_RATE / SINE_FREQ)  // 48 samples por ciclo
#define BUFFER_SIZE         96         // 2 ciclos completos (2 * 48 samples)

/*******************************************************************************
 * VARIABLES GLOBALES
 ******************************************************************************/
// Tabla de sine wave precalculada (valores de 24-bit con signo)
static int32_t sine_table[BUFFER_SIZE];

// Índice actual en la tabla (para recorrer circularmente)
static volatile uint32_t buffer_index = 0;

/*******************************************************************************
 * FUNCIÓN: generate_sine_table
 * 
 * Descripción:
 *   Genera una tabla precalculada de valores de sine wave para optimizar
 *   el rendimiento en tiempo de ejecución. Los valores se calculan una sola
 *   vez al inicio y luego se leen repetidamente desde la tabla.
 * 
 * Proceso:
 *   1. Para cada muestra, calcula el ángulo: theta = 2*PI * i / samples_per_cycle
 *   2. Calcula sin(theta) y lo escala a rango de 24-bit con signo
 *   3. Convierte a entero de 32-bit y verifica que esté en rango válido
 * 
 * Rango 24-bit con signo:
 *   - Máximo positivo: +8388607  (0x7FFFFF)
 *   - Máximo negativo: -8388608  (0x800000)
 ******************************************************************************/
void generate_sine_table(void) {
    // Amplitud en valores de 24-bit: 2^23 - 1 = 8388607
    const float amplitude_24bit = SINE_AMPLITUDE * 8388607.0f;
    
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        // Calcular ángulo para esta muestra
        // theta = 2*PI * (sample_index / samples_per_cycle)
        float angle = 2.0f * M_PI * (float)i / (float)SAMPLES_PER_CYCLE;
        
        // Calcular valor de sine wave: y = A * sin(theta)
        float sample_float = amplitude_24bit * sinf(angle);
        
        // Convertir a entero de 24-bit con signo
        sine_table[i] = (int32_t)sample_float;
        
        // Clamp para asegurar que está en el rango válido de 24-bit
        if (sine_table[i] > 8388607) {
            sine_table[i] = 8388607;
        }
        if (sine_table[i] < -8388608) {
            sine_table[i] = -8388608;
        }
    }
}

/*******************************************************************************
 * FUNCIÓN: I2S_Send_Sample
 * 
 * Descripción:
 *   Envía una muestra estéreo (left + right) al DAC a través de I2S.
 *   Utiliza polling para verificar que el buffer esté listo antes de escribir.
 * 
 * Parámetros:
 *   sample_left  - Muestra para canal izquierdo (24-bit con signo)
 *   sample_right - Muestra para canal derecho (24-bit con signo)
 * 
 * Formato I2S:
 *   - Slot size: 32-bit
 *   - Data size: 24-bit
 *   - Los 24 bits se alinean en los MSBs del slot de 32-bit
 *   - Por lo tanto, se hace shift left de 8 bits antes de escribir
 * 
 * Ejemplo:
 *   sample = 0x7FFFFF (24-bit) -> 0x7FFFFF00 (32-bit slot)
 ******************************************************************************/
void I2S_Send_Sample(int32_t sample_left, int32_t sample_right) {
    // Esperar a que el buffer de transmisión del Serializer 1 esté listo
    // TXRDY1 = 1 indica que I2S_DATA[1] está listo para recibir datos
    while (!(I2S_REGS->I2S_INTFLAG & I2S_INTFLAG_TXRDY1_Msk));
    
    // Enviar muestra del canal izquierdo
    // Shift left de 8 bits para alinear 24-bit data en slot de 32-bit
    // Formato: [23:0] data + [7:0] padding = [31:0] slot
    I2S_REGS->I2S_DATA[1] = (uint32_t)(sample_left << 8);
    
    // Esperar siguiente slot (canal derecho)
    while (!(I2S_REGS->I2S_INTFLAG & I2S_INTFLAG_TXRDY1_Msk));
    
    // Enviar muestra del canal derecho
    I2S_REGS->I2S_DATA[1] = (uint32_t)(sample_right << 8);
}

/*******************************************************************************
 * FUNCIÓN: I2S_Generate_Sine_Wave
 * 
 * Descripción:
 *   Loop infinito que genera continuamente la sine wave precalculada y la
 *   envía al DAC. Lee muestras secuencialmente de sine_table y hace wrap-around
 *   al llegar al final para generar una señal continua.
 * 
 * Comportamiento:
 *   - Lee muestra actual de sine_table[buffer_index]
 *   - Envía la misma muestra a ambos canales (estéreo mono)
 *   - Incrementa buffer_index con wrap-around circular
 *   - Repite indefinidamente
 * 
 * Nota:
 *   Esta función nunca retorna. Se ejecuta continuamente después de la
 *   inicialización del sistema.
 ******************************************************************************/
void I2S_Generate_Sine_Wave(void) {
    while (1) {
        // Obtener muestra actual de la tabla precalculada
        int32_t sample = sine_table[buffer_index];
        // Enviar la misma muestra a ambos canales L y R (estéreo mono)
        I2S_Send_Sample(sample, sample);
        // Avanzar al siguiente índice con wrap-around circular
        buffer_index++;
        if (buffer_index >= BUFFER_SIZE) {
            LED_INT_Toggle();
            buffer_index = 0;  // Volver al inicio de la tabla
        }
    }
}

// Función que genera un voltaje fijo en cierto tiempo
void test_i2s_manual(void) {
 uint32_t counter = 0;
    
    // LED para indicar que está corriendo
    LED_INT_Set();
    
    while(1) {
        // Patrón 1: Enviar máximo positivo (10 muestras)
        for(int i = 0; i < 10; i++) {
            while (!(I2S_REGS->I2S_INTFLAG & I2S_INTFLAG_TXRDY1_Msk));
            I2S_REGS->I2S_DATA[1] = 0x7FFFFF00;  // Canal L
            
            while (!(I2S_REGS->I2S_INTFLAG & I2S_INTFLAG_TXRDY1_Msk));
            I2S_REGS->I2S_DATA[1] = 0x7FFFFF00;  // Canal R
        }
        
        // Patrón 2: Enviar máximo negativo (10 muestras)
        for(int i = 0; i < 10; i++) {
            while (!(I2S_REGS->I2S_INTFLAG & I2S_INTFLAG_TXRDY1_Msk));
            I2S_REGS->I2S_DATA[1] = 0x80000000;  // Canal L
            
            while (!(I2S_REGS->I2S_INTFLAG & I2S_INTFLAG_TXRDY1_Msk));
            I2S_REGS->I2S_DATA[1] = 0x80000000;  // Canal R
        }
        
        // Toggle LED cada ciclo
        counter++;
        if(counter % 100 == 0) {
            LED_INT_Toggle();
        }
    }
}