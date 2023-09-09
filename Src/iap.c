/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      ÕıµãÔ­×ÓÍÅ¶Ó(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-12
 * @brief       IAP ´úÂë
 * @license     Copyright (c) 2020-2032, ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾
 ****************************************************************************************************
 * @attention
 *
 * ÊµÑéÆ½Ì¨:ÕıµãÔ­×Ó STM32F103¿ª·¢°å
 * ÔÚÏßÊÓÆµ:www.yuanzige.com
 * ¼¼ÊõÂÛÌ³:www.openedv.com
 * ¹«Ë¾ÍøÖ·:www.alientek.com
 * ¹ºÂòµØÖ·:openedv.taobao.com
 *
 * ĞŞ¸ÄËµÃ÷
 * V1.0 20200512
 * µÚÒ»´Î·¢²¼
 *
 ****************************************************************************************************
 */

#include "iap.h"
#include "stmflash.h"
#include "ymodem.h"
#include "common.h"

/* USER CODE BEGIN PD */
#define FLASH_USER_START_ADDR   (FLASH_BASE + (26 * FLASH_PAGE_SIZE))   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     (FLASH_USER_START_ADDR + FLASH_PAGE_SIZE)   /* End @ of user Flash area */
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;



uint32_t FirstPage = 0, NbOfPages = 0;
uint32_t Address = 0, PageError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;




iapfun jump2app;

typedef  void (*pFunction)(void);
#define APPLICATION_ADDRESS     (uint32_t)0x08004000      /* Start user code address: ADDR_FLASH_PAGE_8 */
pFunction JumpToApplication;
uint32_t JumpAddress_local;

uint16_t g_iapbuf[1024];       /* 2K×Ö½Ú»º´æ */
static void sys_msr_msp(uint32_t addr);
/**
 * @brief       IAPĞ´ÈëAPP BIN
 * @param       appxaddr : Ó¦ÓÃ³ÌĞòµÄÆğÊ¼µØÖ·
 * @param       appbuf   : Ó¦ÓÃ³ÌĞòCODE
 * @param       appsize  : Ó¦ÓÃ³ÌĞò´óĞ¡(×Ö½Ú)
 * @retval      ÎŞ
 */
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint16_t t;
    uint16_t i = 0;
    uint16_t temp;
    uint32_t fwaddr = appxaddr; /* µ±Ç°Ğ´ÈëµÄµØÖ· */
    uint8_t *dfu = appbuf;

    for (t = 0; t < appsize; t += 2)
    {
        temp = (uint16_t)dfu[1] << 8;
        temp |= (uint16_t)dfu[0];
        dfu += 2;               /* Æ«ÒÆ2¸ö×Ö½Ú */
        g_iapbuf[i++] = temp;

        if (i == 1024)
        {
            i = 0;
            stmflash_write(fwaddr, g_iapbuf, 1024);
         
            fwaddr += 2048;     /* Æ«ÒÆ2048  16 = 2 * 8  ËùÒÔÒª³ËÒÔ2 */
        }
    }

    if (i)
    {
        stmflash_write(fwaddr, g_iapbuf, i);  /* ½«×îºóµÄÒ»Ğ©ÄÚÈİ×Ö½ÚĞ´½øÈ¥ */
      
    }
}

/**
 * @brief       Ìø×ªµ½Ó¦ÓÃ³ÌĞò¶Î(Ö´ĞĞAPP)
 * @param       appxaddr : Ó¦ÓÃ³ÌĞòµÄÆğÊ¼µØÖ·

 * @retval      ÎŞ
 */
void iap_load_app(uint32_t appxaddr)
{
//  //  if (((*(volatile  uint32_t *)appxaddr) & 0x2FFE0000) == 0x20000000)     /* ¼ì²éÕ»¶¥µØÖ·ÊÇ·ñºÏ·¨.¿ÉÒÔ·ÅÔÚÄÚ²¿SRAM¹²64KB(0x20000000) */
//    {
//        /* ÓÃ»§´úÂëÇøµÚ¶ş¸ö×ÖÎª³ÌĞò¿ªÊ¼µØÖ·(¸´Î»µØÖ·) */
//        jump2app = (iapfun) * (volatile uint32_t *)(appxaddr + 4);
//        
//        /* ³õÊ¼»¯APP¶ÑÕ»Ö¸Õë(ÓÃ»§´úÂëÇøµÄµÚÒ»¸ö×ÖÓÃÓÚ´æ·ÅÕ»¶¥µØÖ·) */
//        sys_msr_msp(*(volatile uint32_t *)appxaddr);
//        
//        /* Ìø×ªµ½APP */
//        jump2app();
//    }
     JumpAddress_local= *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
      JumpToApplication = (pFunction) JumpAddress_local;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
      JumpToApplication();
}

static void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* ÉèÖÃÕ»¶¥µØÖ· */
}
/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;;
}

/*
*********************************************************************************************************
*	º¯ Êı Ãû: bsp_CmpCpuFlash
*	¹¦ÄÜËµÃ÷: ±È½ÏFlashÖ¸¶¨µØÖ·µÄÊı¾İ.
*	ĞÎ    ²Î: _ulFlashAddr : FlashµØÖ·
*			 _ucpBuf : Êı¾İ»º³åÇø
*			 _ulSize : Êı¾İ´óĞ¡£¨µ¥Î»ÊÇ×Ö½Ú£©
*	·µ »Ø Öµ:
*			FLASH_IS_EQU		0   FlashÄÚÈİºÍ´ıĞ´ÈëµÄÊı¾İÏàµÈ£¬²»ĞèÒª²Á³ıºÍĞ´²Ù×÷
*			FLASH_REQ_WRITE		1	Flash²»ĞèÒª²Á³ı£¬Ö±½ÓĞ´
*			FLASH_REQ_ERASE		2	FlashĞèÒªÏÈ²Á³ı,ÔÙĞ´
*			FLASH_PARAM_ERR		3	º¯Êı²ÎÊı´íÎó
*********************************************************************************************************
*/
#if 0
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucIsEqu;	/* ÏàµÈ±êÖ¾ */
	uint8_t ucByte;

	/* Èç¹ûÆ«ÒÆµØÖ·³¬¹ıĞ¾Æ¬ÈİÁ¿£¬Ôò²»¸ÄĞ´Êä³ö»º³åÇø */
	if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
	{
		return FLASH_PARAM_ERR;		/*¡¡º¯Êı²ÎÊı´íÎó¡¡*/
	}

	/* ³¤¶ÈÎª0Ê±·µ»ØÕıÈ· */
	if (_ulSize == 0)
	{
		return FLASH_IS_EQU;		/* FlashÄÚÈİºÍ´ıĞ´ÈëµÄÊı¾İÏàµÈ */
	}

	ucIsEqu = 1;			/* ÏÈ¼ÙÉèËùÓĞ×Ö½ÚºÍ´ıĞ´ÈëµÄÊı¾İÏàµÈ£¬Èç¹ûÓöµ½ÈÎºÎÒ»¸ö²»ÏàµÈ£¬ÔòÉèÖÃÎª 0 */
	for (i = 0; i < _ulSize; i++)
	{
		ucByte = *(uint8_t *)_ulFlashAddr;

		if (ucByte != *_ucpBuf)
		{
			if (ucByte != 0xFF)
			{
				return FLASH_REQ_ERASE;		/* ĞèÒª²Á³ıºóÔÙĞ´ */
			}
			else
			{
				ucIsEqu = 0;	/* ²»ÏàµÈ£¬ĞèÒªĞ´ */
			}
		}

		_ulFlashAddr++;
		_ucpBuf++;
	}

	if (ucIsEqu == 1)
	{
		return FLASH_IS_EQU;	/* FlashÄÚÈİºÍ´ıĞ´ÈëµÄÊı¾İÏàµÈ£¬²»ĞèÒª²Á³ıºÍĞ´²Ù×÷ */
	}
	else
	{
		return FLASH_REQ_WRITE;	/* Flash²»ĞèÒª²Á³ı£¬Ö±½ÓĞ´ */
	}
}
#endif 
/*
*********************************************************************************************************
*	º¯ Êı Ãû: bsp_EraseCpuFlash
*	¹¦ÄÜËµÃ÷: ²Á³ıCPU FLASHÒ»¸öÉÈÇø £¨128KB)
*	ĞÎ    ²Î: _ulFlashAddr : FlashµØÖ·
*	·µ »Ø Öµ: 0 ³É¹¦£¬ 1 Ê§°Ü
*			  HAL_OK       = 0x00,
*			  HAL_ERROR    = 0x01,
*			  HAL_BUSY     = 0x02,
*			  HAL_TIMEOUT  = 0x03
*
*********************************************************************************************************
*/
uint8_t bsp_EraseCpuFlash(uint32_t _ulFlashAddr)
{
    uint8_t re;
    NbOfPages = 1;//GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
	/* ½âËø */
	HAL_FLASH_Unlock();
	
	/* »ñÈ¡´ËµØÖ·ËùÔÚµÄÉÈÇø */
	//FirstSector = bsp_GetSector(_ulFlashAddr);
     FirstPage = GetPage(_ulFlashAddr);//flashdestination + i*128*16;//0x08004000;//GetPage();
	 //FirstPage    =  bsp_GetSector(_ulFlashAddr);
	/* ¹Ì¶¨1¸öÉÈÇø */
	NbOfPages = 1;

	/* ²Á³ıÉÈÇøÅäÖÃ */
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks  = FLASH_BANK_1;
	EraseInitStruct.Page      = FirstPage;
    EraseInitStruct.NbPages   = NbOfPages;

	
//	if (_ulFlashAddr >= ADDR_FLASH_SECTOR_0_BANK2)
//	{
//		EraseInitStruct.Banks         = FLASH_BANK_2;
//	}
//	else
//	{
//		EraseInitStruct.Banks         = FLASH_BANK_1;
//	}
//	
//	EraseInitStruct.Sector        = FirstSector;
//	EraseInitStruct.NbSectors     = NbOfSectors;
	
	/* ÉÈÇø²Á³ı */	
	//re = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PageError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    /* Infinite loop */
      re =0;
    }
    else 
        re =1;
	
	/* ²Á³ıÍê±Ïºó£¬ÉÏËø */
	HAL_FLASH_Lock();	
	
	return re;
}

/*
*********************************************************************************************************
*	º¯ Êı Ãû: bsp_WriteCpuFlash
*	¹¦ÄÜËµÃ÷: Ğ´Êı¾İµ½CPU ÄÚ²¿Flash¡£ ±ØĞë°´32×Ö½ÚÕûÊı±¶Ğ´¡£²»Ö§³Ö¿çÉÈÇø¡£ÉÈÇø´óĞ¡128KB. \
*			  Ğ´Ö®Ç°ĞèÒª²Á³ıÉÈÇø. ³¤¶È²»ÊÇ32×Ö½ÚÕûÊı±¶Ê±£¬×îºó¼¸¸ö×Ö½ÚÄ©Î²²¹0Ğ´Èë.
*	ĞÎ    ²Î: _ulFlashAddr : FlashµØÖ·
*			 _ucpSrc : Êı¾İ»º³åÇø
*			 _ulSize : Êı¾İ´óĞ¡£¨µ¥Î»ÊÇ×Ö½Ú, ±ØĞëÊÇ32×Ö½ÚÕûÊı±¶£©
*	·µ »Ø Öµ: 0-³É¹¦£¬1-Êı¾İ³¤¶È»òµØÖ·Òç³ö£¬2-Ğ´Flash³ö´í(¹À¼ÆFlashÊÙÃüµ½)
*********************************************************************************************************
*/
#if 0
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{

    uint32_t i;
	//uint8_t ucRet;
     // å®šä¹‰Flashå†™å…¥ç»“æ„ä½“å˜é‡
    FLASH_EraseInitTypeDef eraseInitStruct;

    // å®šä¹‰Flashå†™å…¥å˜é‡
   // uint32_t data[512]; // 2KBæ•°æ®ç¼“å†²åŒº

  //  data =_ucpSrc;

    /* ÃˆÃ§Â¹Ã»Ã†Â«Ã’Ã†ÂµÃ˜Ã–Â·Â³Â¬Â¹Ã½ÃÂ¾Ã†Â¬ÃˆÃÃÂ¿Â£Â¬Ã”Ã²Â²Â»Â¸Ã„ÃÂ´ÃŠÃ¤Â³Ã¶Â»ÂºÂ³Ã¥Ã‡Ã¸ */
	if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
	{
		return 1;
	}

	/* Â³Â¤Â¶ÃˆÃÂª0ÃŠÂ±Â²Â»Â¼ÃŒÃÃ¸Â²Ã™Ã—Ã·  */
	if (_ulSize == 0)
	{
		return 0;
	}

    // å†™å…¥Flashæ•°æ®
//    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // æ“¦é™¤ç±»å‹ä¸ºé¡µ
//    eraseInitStruct.Banks = FLASH_BANK_1; // æ“¦é™¤çš„Flash Bank
//    eraseInitStruct.Page = 8; // æ“¦é™¤çš„èµ·å§‹é¡µå·
//    eraseInitStruct.NbPages = 1; // æ“¦é™¤çš„é¡µæ•°é‡
//
//    uint32_t pageError = 0; // ç”¨äºä¿å­˜æ“¦é™¤é”™è¯¯çš„åœ°å€

    __set_PRIMASK(1);  		/* turn off interrupt  */
    // å†™å…¥è¿ç»­çš„10ä¸ªFlashé¡µ
    HAL_FLASH_Unlock(); // è§£é”Flash

    for (i = 0; i< _ulSize /32; i++) {
       // eraseInitStruct.Page = page; // è®¾ç½®å½“å‰æ“¦é™¤çš„é¡µå·
       // HAL_FLASHEx_Erase(&eraseInitStruct, &pageError); // è°ƒç”¨æ“¦é™¤å‡½æ•°
        
       // uint32_t address = _ulFlashAddr;//FLASH_BASE + (page * FLASH_PAGE_SIZE); // è®¡ç®—å½“å‰é¡µçš„èµ·å§‹åœ°å€
       uint64_t FlashWord[4];
		
	   memcpy((char *)FlashWord, _ucpSrc, 32);
		_ucpSrc += 32;

      
         if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD , _ulFlashAddr, (uint64_t)((uint32_t)FlashWord))==HAL_OK){ // å†™å…¥æ•°æ®
            _ulFlashAddr = _ulFlashAddr + 32;//32 /* ÂµÃÃ”Ã¶Â£Â¬Â²Ã™Ã—Ã·ÃÃ‚Ã’Â»Â¸Ã¶256bit */	

         }
         else{

           printf("flash write error\r\n");
           goto err;
         }
        
    }

    /* Â³Â¤Â¶ÃˆÂ²Â»ÃŠÃ‡32Ã—Ã–Â½ÃšÃ•Ã»ÃŠÃ½Â±Â¶ */
	if (_ulSize % 16)
	{
		uint64_t FlashWord[4];
		
		FlashWord[0] = 0;
		FlashWord[1] = 0;
		FlashWord[2] = 0;
		FlashWord[3] = 0;
		memcpy((char *)FlashWord, _ucpSrc, _ulSize % 32);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD , _ulFlashAddr, (uint64_t)((uint32_t)FlashWord)) == HAL_OK)
		{
			; // _ulFlashAddr = _ulFlashAddr + 32;		
		}		
		else
		{
			printf("flash write error\r\n");//
			goto err;
		}
	}

    HAL_FLASH_Lock(); // é”å®šFlash
    __set_PRIMASK(0);  		/* turn on interrupt */

	return 0;

err:
   /* Flash Â¼Ã“Ã‹Ã¸Â£Â¬Â½Ã»Ã–Â¹ÃÂ´FlashÂ¿Ã˜Ã–Ã†Â¼Ã„Â´Ã¦Ã†Ã· */
  	HAL_FLASH_Lock();

  	__set_PRIMASK(0);  		/* Â¿ÂªÃ–ÃÂ¶Ã */

	return 1;

}


#endif 


void Flash_Serial_WriteData(uint32_t _ulFlashAddr, uint32_t _ucpSrc, uint32_t _ulSize)
{
    /* å®šä¹‰Flashå†™å…¥ç»“æ„ä½“å˜é‡ */
    FLASH_EraseInitTypeDef eraseInitStruct;

    // å®šä¹‰Flashå†™å…¥å˜é‡
    uint32_t data[512]; // 2KBæ•°æ®ç¼“å†²åŒº

    //_ucpSrc = ;

    // å†™å…¥Flashæ•°æ®
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // æ“¦é™¤ç±»å‹ä¸ºé¡µ
    eraseInitStruct.Banks = FLASH_BANK_1; // æ“¦é™¤çš„Flash Bank
    eraseInitStruct.Page = 0; // æ“¦é™¤çš„èµ·å§‹é¡µå·
    eraseInitStruct.NbPages = 1; // æ“¦é™¤çš„é¡µæ•°é‡

    uint32_t pageError = 0; // ç”¨äºä¿å­˜æ“¦é™¤é”™è¯¯çš„åœ°å€

    // å†™å…¥è¿ç»­çš„10ä¸ªFlashé¡µ
    HAL_FLASH_Unlock(); // è§£é”Flash

    for (uint32_t page = 8; page <  (8+ymodem_t.size+1); page++) {
       // eraseInitStruct.Page = page; // è®¾ç½®å½“å‰æ“¦é™¤çš„é¡µå·
       // HAL_FLASHEx_Erase(&eraseInitStruct, &pageError); // è°ƒç”¨æ“¦é™¤å‡½æ•°
        
       // uint32_t address = FLASH_BASE + (page * FLASH_PAGE_SIZE); // è®¡ç®—å½“å‰é¡µçš„èµ·å§‹åœ°å€

        //for (uint32_t i = 0; i < 2048; i++) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _ulFlashAddr, _ucpSrc); // å†™å…¥æ•°æ®
            _ulFlashAddr += 4; // å¢åŠ 4å­—èŠ‚åœ°å€
        //}
    }

    HAL_FLASH_Lock(); // é”å®šFlash



}

void Flash_Serial_ErasePage(uint32_t page_max)
{
    // å®šä¹‰Flashæ“¦é™¤ç»“æ„ä½“å˜é‡
    FLASH_EraseInitTypeDef eraseInitStruct;

    // æ“¦é™¤Flashé¡µ
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; // æ“¦é™¤ç±»å‹ä¸ºé¡µ
    eraseInitStruct.Banks = FLASH_BANK_1; // æ“¦é™¤çš„Flash Bank
    eraseInitStruct.Page = 8;//ApplicationAddress;//0; // æ“¦é™¤çš„èµ·å§‹é¡µå·
    eraseInitStruct.NbPages = 1; // æ“¦é™¤çš„é¡µæ•°é‡

    uint32_t pageError = 0; // ç”¨äºä¿å­˜æ“¦é™¤é”™è¯¯çš„åœ°å€

    // æ“¦é™¤è¿ç»­çš„10ä¸ªFlashé¡µ
    HAL_FLASH_Unlock(); // è§£é”Flash

    for (uint32_t page = 8; page < (8+page_max); page++) {
        eraseInitStruct.Page = page; // è®¾ç½®å½“å‰æ“¦é™¤çš„é¡µå·
        HAL_FLASHEx_Erase(&eraseInitStruct, &pageError); // è°ƒç”¨æ“¦é™¤å‡½æ•°
    }

    HAL_FLASH_Lock(); // é”å®šFlash



}

