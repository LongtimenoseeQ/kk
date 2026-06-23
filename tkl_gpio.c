/**
 * @file tkl_gpio.c
 * @brief This is tuya tkl gpio src file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright 2021-2022 Tuya Inc. All Rights Reserved.
 *
 */
#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

#include "tkl_memory.h"
#include "tkl_gpio.h"
/******************************************************************************/
/**                  module description following                            **/
/******************************************************************************/
/* Main Features below:


*/
/******************************************************************************/
/**                  internal macro definition following                     **/
/******************************************************************************/
// when debug,disable it
#define tkl_gpio_release_en 0

#define INVALID_INT_NUM     0xFF
#define SUPPORT_MAX_INT     16
#define SUPPORT_MAX_PORT    4
#define SUPPORT_MAX_PIN     (GPIO_PA_COUNT+GPIO_PB_COUNT+GPIO_PC_COUNT+GPIO_PD_COUNT)

/******************************************************************************/
/**                  internal type definition following                      **/
/******************************************************************************/
typedef GPIO_Mode_TypeDef GPIO_MODE_T;
typedef GPIO_Port_TypeDef PORT_T;

typedef struct {
    PORT_T port;
    UINT32_T number;
} __PORT_MAP_T;

__PORT_MAP_T port_map[SUPPORT_MAX_PORT] = {
    {gpioPortA, GPIO_PA_COUNT},
    {gpioPortB, GPIO_PB_COUNT},
    {gpioPortC, GPIO_PC_COUNT},
    {gpioPortD, GPIO_PD_COUNT},
};

typedef enum {
    FALLING_EDGE = 0,
    RISING_EDGE,
    BOTH_EDGE,
} IRQ_MODE;

typedef struct {
    UINT8_T port;
    UINT8_T pin;
    GPIO_MODE_T mode;
    TUYA_GPIO_LEVEL_E level;
    IRQ_MODE irq_mode;
} __GPIO_INIT_T;

typedef struct {
    UINT8_T int_num;
    __GPIO_INIT_T gpio_init;
    VOID* irq_arg;
    TUYA_GPIO_IRQ_CB irq_cb;
} __TKL_GPIO_PRAVATE_IRQ_T;

BOOL_T disable_special_gpio = FALSE;
typedef VOID (*gpio_irq_cb_t)(UINT8_T int_num);
/******************************************************************************/
/**                  internal function declaration following                 **/
/******************************************************************************/

/******************************************************************************/
/**                  global veriables definition following                   **/
/******************************************************************************/
STATIC __TKL_GPIO_PRAVATE_IRQ_T sg_gpio_irq_tbl[SUPPORT_MAX_INT];

/******************************************************************************/
/**                  functions definition following                          **/
/******************************************************************************/
VOID __gpio_disable_tdio(VOID) 
{
    BUS_RegBitWrite(&(GPIO->DBGROUTEPEN), _GPIO_DBGROUTEPEN_TDIPEN_SHIFT,
                    0); /*disable TDI PA4*/
    BUS_RegBitWrite(&(GPIO->DBGROUTEPEN), _GPIO_DBGROUTEPEN_TDOPEN_SHIFT,
                    0); /*disable TDO PA3*/
    BUS_RegBitWrite(&(GPIO->TRACEROUTEPEN),
                    _GPIO_TRACEROUTEPEN_TRACECLKPEN_SHIFT,
                    0); /*disable TRACCLK PA4*/
    BUS_RegBitWrite(&(GPIO->TRACEROUTEPEN),
                    _GPIO_TRACEROUTEPEN_TRACEDATA0PEN_SHIFT,
                    0); /*disable TRACEDATA0 PA3*/
    BUS_RegBitWrite(&(GPIO->TRACEROUTEPEN), _GPIO_TRACEROUTEPEN_SWVPEN_SHIFT,
                    0); /*disable SWV PA3*/
}

VOID __gpio_disable_debug_AB(VOID) 
{
    BUS_RegBitWrite(&(GPIO->DBGROUTEPEN),
                    _GPIO_DBGROUTEPEN_SWCLKTCKPEN_SHIFT,
                    0); /*disable SWCLK PA1*/
    BUS_RegBitWrite(&(GPIO->DBGROUTEPEN),
                    _GPIO_DBGROUTEPEN_SWDIOTMSPEN_SHIFT,
                    0); /*disable SWD PA2*/
}

VOID __gpio_disable_debug(PORT_T port, UINT32_T pin) {
    if (port == gpioPortA && pin == 1 || port == gpioPortA && pin == 2) {
        BUS_RegBitWrite(&(GPIO->DBGROUTEPEN),
                        _GPIO_DBGROUTEPEN_SWCLKTCKPEN_SHIFT,
                        0); /*disable SWCLK PA1*/
        BUS_RegBitWrite(&(GPIO->DBGROUTEPEN),
                        _GPIO_DBGROUTEPEN_SWDIOTMSPEN_SHIFT,
                        0); /*disable SWD PA2*/
    }
}

TUYA_WEAK_ATTRIBUTE VOID_T tkl_gpio_disable_special_func(BOOL_T disable)
{
    disable_special_gpio = disable;
}

STATIC VOID_T __gpio_clear_all_special_func(PORT_T port, UINT32_T pin) {
    if(disable_special_gpio) {
        __gpio_disable_tdio();
        __gpio_disable_debug(port, pin);
    }
}

TUYA_WEAK_ATTRIBUTE UINT8_T __get_gpio_port_num(UINT32_T pin_id)
{
    UINT8_T i;
    UINT8_T port_num = 0;
    PORT_T port = (PORT_T)INVALID_INT_NUM;

    for(i = 0; i < SUPPORT_MAX_PORT; i++) {
        port_num += port_map[i].number;
        if(port_num > pin_id) {
            port = port_map[i].port;
            break;
        }
    }

    return (UINT8_T)port;
}

TUYA_WEAK_ATTRIBUTE UINT8_T __get_gpio_pin_num(UINT32_T pin_id)
{
    UINT8_T i;
    UINT8_T port_num = 0;
    UINT8_T pin = INVALID_INT_NUM;

    for(i = 0; i < SUPPORT_MAX_PORT; i++) {
        port_num += port_map[i].number;
        if(port_num > pin_id) {
            pin = port_map[i].number - (port_num - pin_id);
            break;
        }
    }

    return (UINT8_T)pin;
}

STATIC UINT32_T __get_gpio_id(UINT8_T port, UINT8_T pin)
{
    UINT8_T i;
    UINT8_T port_id = 0;
    
    for(i = 0; i < port; i++) {
        port_id += port_map[i].number;
    }
    port_id += pin;

    return (UINT8_T)port_id;
}

STATIC VOID_T __gpio_mode_convert(__GPIO_INIT_T* gpio_init,
                                TUYA_GPIO_MODE_E mode,
                                TUYA_GPIO_DRCT_E dir,
                                TUYA_GPIO_LEVEL_E level)
{
    gpio_init->level = level;
	
	if (dir == TUYA_GPIO_INPUT) {
        if (mode == TUYA_GPIO_PULLUP) {
            gpio_init->mode = gpioModeInputPull;
            gpio_init->level = TUYA_GPIO_LEVEL_HIGH;
        } else if (mode == TUYA_GPIO_PULLDOWN) {
            gpio_init->mode = gpioModeInputPull;
            gpio_init->level = TUYA_GPIO_LEVEL_LOW;
        } else if (mode == TUYA_GPIO_FLOATING ||
                   mode == TUYA_GPIO_HIGH_IMPEDANCE) {
            gpio_init->mode = gpioModeInput;
        }
    } else {
        if (mode == TUYA_GPIO_PUSH_PULL) {
            gpio_init->mode = gpioModePushPull;
        } else if (mode == TUYA_GPIO_OPENDRAIN) {
            gpio_init->mode = gpioModeWiredAnd;
        } else if (mode == TUYA_GPIO_OPENDRAIN_PULLUP) {
            gpio_init->mode = gpioModeWiredAndPullUp;
			// gpio_init->level = TUYA_GPIO_LEVEL_HIGH;
        } else if (mode == (TUYA_GPIO_OPENDRAIN_PULLUP + 1)) {
            gpio_init->mode = gpioModeWiredOrPullDown;
        }
    }
}

STATIC VOID_T __gpio_base_init(__GPIO_INIT_T* gpio_init)
{
#if !defined(_SILICON_LABS_32B_SERIES_2)
    CMU_ClockEnable(cmuClock_HFPER, TRUE);
#endif
    CMU_ClockEnable(cmuClock_GPIO, TRUE);

    GPIO_Unlock();

    __gpio_clear_all_special_func((PORT_T)gpio_init->port,
                                  (UINT32_T)gpio_init->pin);

    GPIO_PinModeSet((PORT_T)gpio_init->port, (UINT32_T)gpio_init->pin,
                    gpio_init->mode, (UINT32_T)gpio_init->level);
}

STATIC INT_T __gpio_int_num_init(VOID)
{
    UINT8_T i;

    for (i = 0; i < SUPPORT_MAX_INT; i++) {
        sg_gpio_irq_tbl[i].int_num = INVALID_INT_NUM;
    }

    return OPRT_OK;
}

/*
 *   On series 0 devices, the pin number parameter is not used. The
 *   pin number used on these devices is hardwired to the interrupt with the
 *   same number. @n
 *   On series 1 devices, the pin number can be selected freely within a group.
 *   Interrupt numbers are divided into 4 groups (intNo / 4) and valid pin
 *   number within the interrupt groups are:
 *       0: pins 0-3   (interrupt number 0-3)
 *       1: pins 4-7   (interrupt number 4-7)
 *       2: pins 8-11  (interrupt number 8-11)
 *       3: pins 12-15 (interrupt number 12-15)
 */
TUYA_WEAK_ATTRIBUTE UINT8_T __get_gpio_int_num(UINT8_T port, UINT8_T pin) 
{
    UINT8_T i;
    UINT8_T num = 0;

    if (pin < 4) {
        num = 0;
    } else if (pin < 8) {
        num = 4;
    } else if (pin < 12) {
        num = 8;
    } else if (pin < 16) {
        num = 12;
    } else {
        return INVALID_INT_NUM;
    }

    for (i = 0; i < SUPPORT_MAX_INT; i++) {
        if (sg_gpio_irq_tbl[i].int_num == INVALID_INT_NUM) {
            continue;
        }

        if (sg_gpio_irq_tbl[i].gpio_init.port == port &&
            sg_gpio_irq_tbl[i].gpio_init.pin == pin) {
            return sg_gpio_irq_tbl[i].int_num;
        }
        if (sg_gpio_irq_tbl[i].int_num == num) {
            num++;
        }
    }

    return num;
}

STATIC VOID_T __gpio_irq_type_convert(__GPIO_INIT_T* gpio_init, TUYA_GPIO_IRQ_E trig_type)
{
    if (trig_type == TUYA_GPIO_IRQ_FALL || trig_type == TUYA_GPIO_IRQ_LOW) {
        gpio_init->mode = gpioModeInputPull;
        gpio_init->level = TUYA_GPIO_LEVEL_HIGH;
        gpio_init->irq_mode = FALLING_EDGE;
    } else if (trig_type == TUYA_GPIO_IRQ_RISE || trig_type == TUYA_GPIO_IRQ_HIGH) {
        gpio_init->mode = gpioModeInputPull;
        gpio_init->level = TUYA_GPIO_LEVEL_LOW;
        gpio_init->irq_mode = RISING_EDGE;
    } else if (trig_type == TUYA_GPIO_IRQ_RISE_FALL) {
        gpio_init->mode = gpioModeInput;
        gpio_init->irq_mode = BOTH_EDGE;
    }
}

STATIC VOID_T __gpio_uniform_irq_cb(UINT8_T int_num)
{
    UINT8_T i;
    UINT8_T pin = 0;

    for (i = 0; i < SUPPORT_MAX_INT; i++) {
        if (sg_gpio_irq_tbl[i].int_num == int_num) {
            if (sg_gpio_irq_tbl[i].irq_cb) {
                if (sg_gpio_irq_tbl[i].irq_arg == NULL) {
                    pin = __get_gpio_id(sg_gpio_irq_tbl[i].gpio_init.port,
                                        sg_gpio_irq_tbl[i].gpio_init.pin);
                    
                    sg_gpio_irq_tbl[i].irq_cb((VOID_T*)&pin);
                } else {
                    sg_gpio_irq_tbl[i].irq_cb(sg_gpio_irq_tbl[i].irq_arg);
                }
            }
        }
    }
}

STATIC VOID_T __gpio_irq_base_init(__TKL_GPIO_PRAVATE_IRQ_T* irq_info, gpio_irq_cb_t irq_cb)
{
    UINT8_T rising_enable = 1;
    UINT8_T falling_enable = 1;

    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_DisableIRQ(GPIO_ODD_IRQn);
    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_DisableIRQ(GPIO_EVEN_IRQn);

    /* Enable GPIO in CMU */
    __gpio_base_init(&(irq_info->gpio_init));

    GPIOINT_CallbackRegister(irq_info->int_num, irq_cb);
    if (irq_info->gpio_init.irq_mode == FALLING_EDGE) {
        rising_enable = 0;
    } else if (irq_info->gpio_init.irq_mode == RISING_EDGE) {
        falling_enable = 0;
    }

    GPIO_ExtIntConfig((PORT_T)irq_info->gpio_init.port,
                      (UINT32_T)irq_info->gpio_init.pin,
                      (UINT32_T)(irq_info->int_num), 
					  (BOOL_T)rising_enable,
                      (BOOL_T)falling_enable, TRUE);

    GPIOINT_Init();
}

STATIC OPERATE_RET __gpio_irq_base_ctrl(UINT32_T pin_id, BOOL_T enable) 
{
    UINT8_T i;
    UINT8_T port_num, pin_num;

    port_num = __get_gpio_port_num(pin_id);
    pin_num = __get_gpio_pin_num(pin_id);

    if(port_num ==0xff ||pin_num ==0xff){
        return OPRT_INVALID_PARM;
    }

    for (i = 0; i < SUPPORT_MAX_INT; i++) {
        if (sg_gpio_irq_tbl[i].gpio_init.port == port_num &&
            sg_gpio_irq_tbl[i].gpio_init.pin == pin_num) {
            break;
        }
    }

    if (i == SUPPORT_MAX_INT) {
        return OPRT_COM_ERROR;
    }
    GPIO_IntClear(1UL << sg_gpio_irq_tbl[i].int_num);
    BUS_RegBitWrite(&(GPIO->IEN), sg_gpio_irq_tbl[i].int_num, enable);

    return OPRT_OK;
}


TUYA_WEAK_ATTRIBUTE OPERATE_RET tkl_gpio_deinit(TUYA_GPIO_NUM_E pin_id)
{
    __GPIO_INIT_T v_gpio_init;
    
    if(pin_id >= SUPPORT_MAX_PIN) {
        return OPRT_INVALID_PARM;
    }

    v_gpio_init.port = __get_gpio_port_num((UINT32_T)pin_id);
    v_gpio_init.pin = __get_gpio_pin_num((UINT32_T)pin_id);
    if(v_gpio_init.port ==0xff ||v_gpio_init.pin ==0xff){
        return OPRT_INVALID_PARM;
    }
    v_gpio_init.mode = gpioModeDisabled;
    v_gpio_init.level = TUYA_GPIO_LEVEL_LOW;

    __gpio_base_init(&v_gpio_init);

    return OPRT_OK;
}


TUYA_WEAK_ATTRIBUTE OPERATE_RET tkl_gpio_init(TUYA_GPIO_NUM_E pin_id, CONST TUYA_GPIO_BASE_CFG_T *cfg)
{
    __GPIO_INIT_T v_gpio_init;

    if(pin_id >= SUPPORT_MAX_PIN) {
        return OPRT_INVALID_PARM;
    }

    v_gpio_init.port = __get_gpio_port_num((UINT32_T)pin_id);
    v_gpio_init.pin = __get_gpio_pin_num((UINT32_T)pin_id);
    if(v_gpio_init.port ==0xff ||v_gpio_init.pin ==0xff){
        return OPRT_INVALID_PARM;
    }
    __gpio_mode_convert(&v_gpio_init, cfg->mode, cfg->direct, cfg->level);

    __gpio_base_init(&v_gpio_init);

    return OPRT_OK;
}


TUYA_WEAK_ATTRIBUTE OPERATE_RET tkl_gpio_read(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E *level)
{
    UINT8_T port_num, pin_num;
    GPIO_MODE_T gpio_mode;

    if (NULL == level || pin_id >= SUPPORT_MAX_PIN) {
        return OPRT_INVALID_PARM;
    }

    port_num = __get_gpio_port_num((UINT32_T)pin_id);
    pin_num = __get_gpio_pin_num((UINT32_T)pin_id);
    if(port_num ==0xff ||pin_num ==0xff){
        return OPRT_INVALID_PARM;
    }
    gpio_mode = GPIO_PinModeGet((PORT_T)port_num, (UINT32_T)pin_num);

    if (gpio_mode > gpioModeDisabled && gpio_mode <= gpioModeInputPullFilter) {
        *level = (TUYA_GPIO_LEVEL_E)GPIO_PinInGet((PORT_T)port_num, (UINT32_T)pin_num);
    } else if (gpio_mode >= gpioModePushPull) {
        *level = (TUYA_GPIO_LEVEL_E)GPIO_PinOutGet((PORT_T)port_num, (UINT32_T)pin_num);
    }

    return OPRT_OK;
}


TUYA_WEAK_ATTRIBUTE OPERATE_RET tkl_gpio_write(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E level)
{
    UINT8_T port_num, pin_num;
    
    if(pin_id >= SUPPORT_MAX_PIN) {
        return OPRT_INVALID_PARM;
    }

    port_num = __get_gpio_port_num((UINT32_T)pin_id);
    pin_num = __get_gpio_pin_num((UINT32_T)pin_id);
    if(port_num ==0xff ||pin_num ==0xff){
        return OPRT_INVALID_PARM;
    }
    if (level) {
        GPIO_PinOutSet((PORT_T)port_num, (UINT32_T)pin_num);
    } else {
        GPIO_PinOutClear((PORT_T)port_num, (UINT32_T)pin_num);
    }

    return OPRT_OK;
}


TUYA_WEAK_ATTRIBUTE OPERATE_RET tkl_gpio_irq_init(TUYA_GPIO_NUM_E pin_id, CONST TUYA_GPIO_IRQ_T *cfg)
{
    UINT8_T i;
    __GPIO_INIT_T gpio_init;
    STATIC UINT8_T sv_is_irq_tbl_init = 0;

    if(pin_id >= SUPPORT_MAX_PIN) {
        return OPRT_INVALID_PARM;
    }
    if(cfg == NULL || cfg->cb == NULL) {
        return OPRT_INVALID_PARM;
    }

    if (!sv_is_irq_tbl_init) {
        sv_is_irq_tbl_init = 1;
        __gpio_int_num_init();
    }

    gpio_init.port = __get_gpio_port_num((UINT32_T)pin_id);
    gpio_init.pin = __get_gpio_pin_num((UINT32_T)pin_id);
    if(gpio_init.port ==0xff ||gpio_init.pin ==0xff){
        return OPRT_INVALID_PARM;
    }
    __gpio_irq_type_convert(&gpio_init, cfg->mode);
#if 0
    for (i = 0; i < SUPPORT_MAX_INT; i++) {
        if (sg_gpio_irq_tbl[i].gpio_init.port == gpio_init.port &&
            sg_gpio_irq_tbl[i].gpio_init.pin == gpio_init.pin) {
            // aleady init,latest parametes is effectived
            sg_gpio_irq_tbl[i].irq_arg = cfg->arg;
            sg_gpio_irq_tbl[i].irq_cb = cfg->cb;
            sg_gpio_irq_tbl[i].int_num =
                __get_gpio_int_num(gpio_init.port, gpio_init.pin);
            tkl_system_memcpy(&sg_gpio_irq_tbl[i].gpio_init, &gpio_init,
                   sizeof(__GPIO_INIT_T));

            __gpio_irq_base_init(&sg_gpio_irq_tbl[i], __gpio_uniform_irq_cb);

            return OPRT_OK;
        }
    }
#endif
    // a new port
    for (i = 0; i < SUPPORT_MAX_INT; i++) {
        if (sg_gpio_irq_tbl[i].int_num == INVALID_INT_NUM) {
            sg_gpio_irq_tbl[i].irq_arg = cfg->arg;
            sg_gpio_irq_tbl[i].irq_cb = cfg->cb;
            sg_gpio_irq_tbl[i].int_num = __get_gpio_int_num(gpio_init.port, gpio_init.pin);
            tkl_system_memcpy(&sg_gpio_irq_tbl[i].gpio_init, (VOID_T *)&gpio_init, sizeof(__GPIO_INIT_T));

            __gpio_irq_base_init(&sg_gpio_irq_tbl[i], __gpio_uniform_irq_cb);

            return OPRT_OK;
        }
    }

    // irq port register over limit
    return OPRT_COM_ERROR;
}


TUYA_WEAK_ATTRIBUTE OPERATE_RET tkl_gpio_irq_enable(TUYA_GPIO_NUM_E pin_id)
{
    if(pin_id >= SUPPORT_MAX_PIN) {
        return OPRT_INVALID_PARM;
    }
    return __gpio_irq_base_ctrl((UINT32_T)pin_id, TRUE);
}


TUYA_WEAK_ATTRIBUTE OPERATE_RET tkl_gpio_irq_disable(TUYA_GPIO_NUM_E pin_id)
{
    if(pin_id >= SUPPORT_MAX_PIN) {
        return OPRT_INVALID_PARM;
    }
    return __gpio_irq_base_ctrl((UINT32_T)pin_id, FALSE);
}

/******************************************************************************/
/**                              EOF                                         **/
/******************************************************************************/
