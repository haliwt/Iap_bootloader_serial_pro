/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-12
 * @brief       IAP ����
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200512
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __IAP_H
#define __IAP_H

#include "main.h"




typedef void (*iapfun)(void);                   /* ����һ���������͵Ĳ��� */

#define FLASH_APP1_ADDR         0x08004000      /* ��һ��Ӧ�ó�����ʼ��ַ(������ڲ�FLASH)
                                                 * ���� 0X08000000~0X08003FFF(16KB) �Ŀռ�Ϊ Bootloader ʹ�� */

#define CPU_FLASH_BASE_ADDR      (uint32_t)(FLASH_BASE)		/* 0x08000000 */
#define CPU_FLASH_END_ADDR       (uint32_t)(0x0800FFFF)

#define CPU_FLASH_SIZE       	(31 * 2048)	/* FLASH×ÜÈÝÁ¿ */
#define CPU_FLASH_SECTOR_SIZE	(128 * 1024)		/* ÉÈÇø´óÐ¡£¬×Ö½Ú */



void iap_load_app(uint32_t appxaddr);   /* ��ת��APP����ִ�� */
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);   /* ��ָ����ַ��ʼ,д��bin */
uint8_t bsp_EraseCpuFlash(uint32_t _ulFlashAddr);
void Flash_Serial_ErasePage(void);
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize);
void Flash_Serial_WriteData(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize);

#endif







































