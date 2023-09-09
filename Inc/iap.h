/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-12
 * @brief       IAP 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200512
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __IAP_H
#define __IAP_H

#include "main.h"




typedef void (*iapfun)(void);                   /* 定义一个函数类型的参数 */

#define FLASH_APP1_ADDR         0x08004000      /* 第一个应用程序起始地址(存放在内部FLASH)
                                                 * 保留 0X08000000~0X08003FFF(16KB) 的空间为 Bootloader 使用 */

#define CPU_FLASH_BASE_ADDR      (uint32_t)(FLASH_BASE)		/* 0x08000000 */
#define CPU_FLASH_END_ADDR       (uint32_t)(0x0800FFFF)

#define CPU_FLASH_SIZE       	(31 * 2048)	/* FLASH脳脺脠脻脕驴 */
#define CPU_FLASH_SECTOR_SIZE	(128 * 1024)		/* 脡脠脟酶麓贸脨隆拢卢脳脰陆脷 */



void iap_load_app(uint32_t appxaddr);   /* 跳转到APP程序执行 */
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);   /* 在指定地址开始,写入bin */
uint8_t bsp_EraseCpuFlash(uint32_t _ulFlashAddr);
void Flash_Serial_ErasePage(uint32_t page_max);

uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize);
void Flash_Serial_WriteData(uint32_t _ulFlashAddr, uint32_t _ucpSrc, uint32_t _ulSize);

#endif







































