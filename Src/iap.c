/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-12
 * @brief       IAP ����
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
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

#include "iap.h"
#include "stmflash.h"
#include "ymodem.h"
#include "common.h"

/* USER CODE BEGIN PD */
#define FLASH_USER_START_ADDR  ADDR_FLASH_PAGE_16 // (FLASH_BASE + (26 * FLASH_PAGE_SIZE))   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR    ADDR_FLASH_PAGE_64 //(FLASH_USER_START_ADDR + FLASH_PAGE_SIZE)   /* End @ of user Flash area */


/*********************************************************************************************************
*	Function Name: void Flash_Serial_WriteData(uint32_t _ulFlashAddr, uint32_t _ucpSrc, uint32_t _ulSize)
*	Function:  Flash be earse of page
*   Input Ref:NO
*	Return Ref:NO
*		
*********************************************************************************************************/
void Flash_Serial_WriteData(uint32_t _ulFlashAddr, uint32_t _ucpSrc, uint32_t _ulSize)
{
//    /* 定义Flash写入结构体变量 */
//    FLASH_EraseInitTypeDef eraseInitStruct;

//    // 定义Flash写入变量
//    uint32_t data[512]; // 2KB数据缓冲区

//    //_ucpSrc = ;

//    // 写入Flash数据
//    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // 擦除类型为页
//    eraseInitStruct.Banks = FLASH_BANK_1; // 擦除的Flash Bank
//    eraseInitStruct.Page = 0; // 擦除的起始页号
//    eraseInitStruct.NbPages = 1; // 擦除的页数量

//    uint32_t pageError = 0; // 用于保存擦除错误的地址

//    // 写入连续的10个Flash页
//    HAL_FLASH_Unlock(); // 解锁Flash

//    for (uint32_t page = 8; page <  (8+ymodem_t.size+1); page++) {
//       // eraseInitStruct.Page = page; // 设置当前擦除的页号
//       // HAL_FLASHEx_Erase(&eraseInitStruct, &pageError); // 调用擦除函数
//        
//       // uint32_t address = FLASH_BASE + (page * FLASH_PAGE_SIZE); // 计算当前页的起始地址

//        //for (uint32_t i = 0; i < 2048; i++) {
//            HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _ulFlashAddr, _ucpSrc); // 写入数据
//            _ulFlashAddr += 4; // 增加4字节地址
//        //}
//    }

//    HAL_FLASH_Lock(); // 锁定Flash



}

/*********************************************************************************************************
*	Function Name: void Flash_Serial_ErasePage(void)
*	Function:  Flash be earse of page
*   Input Ref:NO
*	Return Ref:NO
*		
*********************************************************************************************************/
void Flash_Serial_ErasePage(void)
{
    uint8_t ret;
    uint32_t i;
    // 定义Flash擦除结构体变量
    FLASH_EraseInitTypeDef eraseInitStruct;

    // 擦除Flash页
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // 擦除类型为页
    eraseInitStruct.Banks = FLASH_BANK_1; // 擦除的Flash Bank
    eraseInitStruct.Page = 8;//ApplicationAddress;//0; // 擦除的起始页号
    eraseInitStruct.NbPages = 1; // (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)/FLASH_PAGE_SIZE;; // 擦除的页数量

    uint32_t pageError = 0; // 用于保存擦除错误的地址

    // 擦除连续的10个Flash页
    HAL_FLASH_Unlock(); // 解锁Flash

   for(i=8 ;i< 32 ;i++){
    
     eraseInitStruct.Page = i;

     HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) ;//!=HAL_OK){// 调用擦除函数

   
   }

   
  


}




