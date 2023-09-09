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
//    /* 瀹氫箟Flash鍐欏叆缁撴瀯浣撳彉閲� */
//    FLASH_EraseInitTypeDef eraseInitStruct;

//    // 瀹氫箟Flash鍐欏叆鍙橀噺
//    uint32_t data[512]; // 2KB鏁版嵁缂撳啿鍖�

//    //_ucpSrc = ;

//    // 鍐欏叆Flash鏁版嵁
//    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // 鎿﹂櫎绫诲瀷涓洪〉
//    eraseInitStruct.Banks = FLASH_BANK_1; // 鎿﹂櫎鐨凢lash Bank
//    eraseInitStruct.Page = 0; // 鎿﹂櫎鐨勮捣濮嬮〉鍙�
//    eraseInitStruct.NbPages = 1; // 鎿﹂櫎鐨勯〉鏁伴噺

//    uint32_t pageError = 0; // 鐢ㄤ簬淇濆瓨鎿﹂櫎閿欒鐨勫湴鍧�

//    // 鍐欏叆杩炵画鐨�10涓狥lash椤�
//    HAL_FLASH_Unlock(); // 瑙ｉ攣Flash

//    for (uint32_t page = 8; page <  (8+ymodem_t.size+1); page++) {
//       // eraseInitStruct.Page = page; // 璁剧疆褰撳墠鎿﹂櫎鐨勯〉鍙�
//       // HAL_FLASHEx_Erase(&eraseInitStruct, &pageError); // 璋冪敤鎿﹂櫎鍑芥暟
//        
//       // uint32_t address = FLASH_BASE + (page * FLASH_PAGE_SIZE); // 璁＄畻褰撳墠椤电殑璧峰鍦板潃

//        //for (uint32_t i = 0; i < 2048; i++) {
//            HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _ulFlashAddr, _ucpSrc); // 鍐欏叆鏁版嵁
//            _ulFlashAddr += 4; // 澧炲姞4瀛楄妭鍦板潃
//        //}
//    }

//    HAL_FLASH_Lock(); // 閿佸畾Flash



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
    // 瀹氫箟Flash鎿﹂櫎缁撴瀯浣撳彉閲�
    FLASH_EraseInitTypeDef eraseInitStruct;

    // 鎿﹂櫎Flash椤�
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // 鎿﹂櫎绫诲瀷涓洪〉
    eraseInitStruct.Banks = FLASH_BANK_1; // 鎿﹂櫎鐨凢lash Bank
    eraseInitStruct.Page = 8;//ApplicationAddress;//0; // 鎿﹂櫎鐨勮捣濮嬮〉鍙�
    eraseInitStruct.NbPages = 1; // (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)/FLASH_PAGE_SIZE;; // 鎿﹂櫎鐨勯〉鏁伴噺

    uint32_t pageError = 0; // 鐢ㄤ簬淇濆瓨鎿﹂櫎閿欒鐨勫湴鍧�

    // 鎿﹂櫎杩炵画鐨�10涓狥lash椤�
    HAL_FLASH_Unlock(); // 瑙ｉ攣Flash

   for(i=8 ;i< 32 ;i++){
    
     eraseInitStruct.Page = i;

     HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) ;//!=HAL_OK){// 璋冪敤鎿﹂櫎鍑芥暟

   
   }

   
  


}




