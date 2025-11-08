#ifndef I2S_INIT_H
#define I2S_INIT_H

#include "definitions.h"
#include <stdbool.h>
#include <stdint.h>

void i2s_init(void);
void i2s_config_clk(void);
void i2s_enable_system_clocks(void);
void GCLK_I2S_Initialize(void);
void PM_I2S_Initialize(void);

#endif /* I2S_INIT_H */