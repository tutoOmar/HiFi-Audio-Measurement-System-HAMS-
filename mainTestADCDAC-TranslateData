/**
 * Main de prueba para probar que sirva de forma prueba para el conversor de ADC a DAC
 */
// Variables globales para inspeccionar en debugger
uint32_t g_sample_count = 0;
float g_sine_value = 0.0f;
uint32_t g_adc_unsigned = 0;
int32_t g_raw_left_justified = 0;
int32_t g_dac_output = 0;

int main(void)
{
    SYS_Initialize(NULL);
    delay_simple(1000000);
    
    DMA_I2S_Stop();
    LED_INT_Clear();
    
    // Parámetros del generador de seno
    float sample_rate = 48000.0f;
    float sine_freq = 1000.0f;  // 1 kHz
    
    while (1)
    {
        // ===== GENERADOR DE SEÑAL SENO =====
        // Calcular ángulo
        float t = (float)g_sample_count / sample_rate;
        float angle = 2.0f * 3.14159265f * sine_freq * t;
        
        // Calcular seno (-1.0 a +1.0)
        g_sine_value = sinf(angle);
        
        // Mapear a ADC: -1→0x000000, 0→0x800000, +1→0xFFFFFF
        g_adc_unsigned = (uint32_t)((g_sine_value + 1.0f) * 0x7FFFFF);
        if (g_adc_unsigned > 0xFFFFFF) g_adc_unsigned = 0xFFFFFF;
        
        // Formato left-justified (como viene del ADC)
        g_raw_left_justified = (int32_t)(g_adc_unsigned << 8);
        
        // ===== TRADUCCIÓN ADC → DAC =====
        g_dac_output = translate_adc_sample(g_raw_left_justified, 0x000000);
        
        // ===== BREAKPOINT AQUÍ ===== 
        // Poner breakpoint en esta línea para inspeccionar variables
        LED_INT_Toggle();
        
        // Enviar al DAC (cuando quieras probarlo de verdad)
        I2S_DAC_SendSample(g_dac_output, g_dac_output);
        
        g_sample_count++;
        
        // Delay para que no corra tan rápido (opcional)
        delay_simple(1000);
    }
    
    return 0;
}

static inline int32_t translate_adc_sample(int32_t raw24_left_justified, int32_t dc_offset)
{
    // 1) Quitar el left-justify: bajar 8 bits
    uint32_t adc_unsigned = (uint32_t)(raw24_left_justified >> 8) & 0xFFFFFF;
    // 2) Multiplicar por 0.49999997 usando enteros de 64 bits
    int64_t temp = ((int64_t)adc_unsigned * 8388607LL) / 16777215LL;
    int32_t dac_signed = (int32_t)temp;
    // 3) Reempaquetar a Left-Justified 32-bit
    return dac_signed;
}
