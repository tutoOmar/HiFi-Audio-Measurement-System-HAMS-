#include "i2s_init.h"
#include "definitions.h"

/**
 * Configuración inicial 
 * I2S.
 */
void i2s_init(void){

    GCLK_I2S_Initialize();
    PM_I2S_Initialize();
    i2s_config_clk();

}
/**
 * Configuracio del reloj de I2S
 */
void i2s_config_clk(void )
{
    // 1. RESET
    I2S_REGS->I2S_CTRLA = (1 << 0);  // SWRST
    while (I2S_REGS->I2S_CTRLA & (1 << 0));
    while (I2S_REGS->I2S_SYNCBUSY);
    // 2. CONFIGURAR CLKCTRL[0]
    I2S_REGS->I2S_CLKCTRL[0] = 
        (0UL << 24) |  // MCKOUTDIV = 0
        (3UL << 19) |  // MCKDIV = 3
        (1UL << 18) |  // MCKEN = 1
        (0UL << 16) |  // MCKSEL = 0
        (0UL << 11) |  // FSINV = 0
        (0UL << 8)  |  // FSSEL = 0
        (1UL << 7)  |  // ? BITDELAY = 1 (BIT 7, no bit 1)
        (0UL << 5)  |  // FSWIDTH = 0 (SLOT)
        (1UL << 2)  |  // ? NBSLOTS = 1 (2 slots) - BITS 2-4
        (3UL << 0);    // ? SLOTSIZE = 3 (32 bits) - BITS 0-1
     // 3. HABILITAR PERIFÉRICO PRIMERO
    I2S_REGS->I2S_CTRLA = (1 << 2) |  // CKEN0
                          (1 << 1);    // ENABLE
    while (I2S_REGS->I2S_SYNCBUSY);  // Esperar TODA la sincronización
    // Delay para estabilización de clocks
    for(volatile uint32_t i = 0; i < 10000; i++);
    // ADC 
    // 4. AHORA SÍ configurar serializers (con clocks activos) 
    I2S_REGS->I2S_SERCTRL[0] = 
        (0UL << 13) |  // EXTEND = 0 (ZERO - extend with zeros)
        (1UL << 12) |  // WORDADJ = 1 (LEFT - ajuste izquierdo)
        (1UL << 9)  |  // DATASIZE = 1 (24 bits)
        (1UL << 7)  |  // SLOTADJ = 1 (LEFT)
        (0UL << 5)  |  // CLKSEL = 0 (usar Clock Unit 0)
        (0UL << 4)  |  // TXSAME = 0 (transmit data in case of underrun)
        (0UL << 2)  |  // TXDEFAULT = 0 (output default value 0)
        (0UL << 0);    // SERMODE = 0 (RX - transmit) 
    // DAC 
    I2S_REGS->I2S_SERCTRL[1] = 
        (0UL << 15) |  // BITREV = 0 (MSB first - NO reverse)
        (0UL << 13) |  // EXTEND = 0 (ZERO - extend with zeros)
        (0UL << 12) |  // WORDADJ = 1 (LEFT - ajuste izquierdo)
        (0UL << 9)  |  // DATASIZE = 1 (24 bits)
        (0UL << 7)  |  // SLOTADJ = 1 (LEFT)
        (0UL << 5)  |  // CLKSEL = 0 (usar Clock Unit 0)
        (1UL << 4)  |  // TXSAME = 0 (transmit data in case of underrun)
        (0UL << 2)  |  // TXDEFAULT = 0 (output default value 0)
        (1UL << 0);    // SERMODE = 1 (TX - transmit) 
    // 5. HABILITAR SERIALIZERS  
    I2S_REGS->I2S_CTRLA |= (1 << 4);  // SEREN0
    while (I2S_REGS->I2S_SYNCBUSY & (1 << 4));  // SYNCBUSY.SEREN0
   // Delay corto
    for(volatile uint32_t i = 0; i < 5000; i++);
    I2S_REGS->I2S_CTRLA |= (1 << 5);  // SEREN1
    while (I2S_REGS->I2S_SYNCBUSY & (1 << 5));

}
/**
 * Configuración de los relojes para el I2S
 */
void GCLK_I2S_Initialize(void)
{  
    // Conectar GCLK0 al Clock I2S_0
    GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID(0x23U) |      // 0x23 = I2S_0
                              GCLK_CLKCTRL_GEN_GCLK0 |       // Fuente: GCLK0
                              GCLK_CLKCTRL_CLKEN_Msk;        // Habilitar
    while((GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk) == GCLK_STATUS_SYNCBUSY_Msk){}
}
/**
 * Se necesita habilitar el bus APB para I2S
 */
void PM_I2S_Initialize(void)
{
    PM_REGS->PM_APBCMASK |= PM_APBCMASK_I2S_Msk;
}