/*
 * File      : eeprom.c
 * This file is part of Robot PDS
 * COPYRIGHT (C) 2012-2017, Rainbonic Inc
 */

#include "gy530.h"
#include "string.h"
#include "drivers\i2c.h"
/* RT_USING_COMPONENTS_INIT */
#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif

static rt_device_t dev = RT_NULL;

rt_err_t rt_write_parameters(void);
void rt_loaddefault_parameters(void);

/* Initial eeprom storage pins  */
int rt_hw_gy530_init(void)
{
    RT_ASSERT(shell != RT_NULL);
    dev = rt_device_find("i2c1");
    if (dev == RT_NULL)
    {
        rt_kprintf("finsh: can not find device: %s\n", GY530_I2C_NAME);
        return -RT_ERROR;
    }
    rt_kprintf("finsh: find %s\n", GY530_I2C_NAME);
    if (rt_device_open(dev, RT_NULL) != RT_EOK) {
        rt_kprintf("finish: open %s fails\n", GY530_I2C_NAME);
    }
    rt_kprintf("finish: open %s ok.\n", GY530_I2C_NAME);
    return RT_EOK;
}
/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_gy530_init);

rt_uint8_t rt_gy530_read(rt_uint8_t addr, rt_uint8_t* buf, rt_uint8_t len);
rt_uint8_t rt_gy530_write(rt_uint8_t addr, rt_uint8_t* buf, rt_uint8_t len);

rt_uint8_t rt_gy530_write(rt_uint8_t addr, rt_uint8_t* buf, rt_uint8_t len)
{
    rt_uint8_t ret;
    rt_uint8_t *wbuf = rt_malloc(len+1);
    if (wbuf == RT_NULL)
        return 0;
    wbuf[0] = addr;
    memcpy(wbuf+1, buf, len);
    ret = rt_device_write(dev, (RT_I2C_WR<<16) | GY530_I2C_ADDR, wbuf, len+1);
    rt_free(wbuf);
    return ret;
}

rt_uint8_t rt_gy530_read(rt_uint8_t addr, rt_uint8_t *buf, rt_uint8_t len) 
{

    rt_device_write(dev, (RT_I2C_WR<<16) | GY530_I2C_ADDR, &addr, 1);
    return rt_device_read(dev, (RT_I2C_RD<<16) | GY530_I2C_ADDR, buf, len);

}

