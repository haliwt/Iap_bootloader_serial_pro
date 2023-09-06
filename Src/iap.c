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

uint16_t g_iapbuf[1024];       /* 2K字节缓存 */
static void sys_msr_msp(uint32_t addr);
/**
 * @brief       IAP写入APP BIN
 * @param       appxaddr : 应用程序的起始地址
 * @param       appbuf   : 应用程序CODE
 * @param       appsize  : 应用程序大小(字节)
 * @retval      无
 */
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint16_t t;
    uint16_t i = 0;
    uint16_t temp;
    uint32_t fwaddr = appxaddr; /* 当前写入的地址 */
    uint8_t *dfu = appbuf;

    for (t = 0; t < appsize; t += 2)
    {
        temp = (uint16_t)dfu[1] << 8;
        temp |= (uint16_t)dfu[0];
        dfu += 2;               /* 偏移2个字节 */
        g_iapbuf[i++] = temp;

        if (i == 1024)
        {
            i = 0;
            stmflash_write(fwaddr, g_iapbuf, 1024);
         
            fwaddr += 2048;     /* 偏移2048  16 = 2 * 8  所以要乘以2 */
        }
    }

    if (i)
    {
        stmflash_write(fwaddr, g_iapbuf, i);  /* 将最后的一些内容字节写进去 */
      
    }
}

/**
 * @brief       跳转到应用程序段(执行APP)
 * @param       appxaddr : 应用程序的起始地址

 * @retval      无
 */
void iap_load_app(uint32_t appxaddr)
{
//  //  if (((*(volatile  uint32_t *)appxaddr) & 0x2FFE0000) == 0x20000000)     /* 检查栈顶地址是否合法.可以放在内部SRAM共64KB(0x20000000) */
//    {
//        /* 用户代码区第二个字为程序开始地址(复位地址) */
//        jump2app = (iapfun) * (volatile uint32_t *)(appxaddr + 4);
//        
//        /* 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址) */
//        sys_msr_msp(*(volatile uint32_t *)appxaddr);
//        
//        /* 跳转到APP */
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
    __set_MSP(addr);    /* 设置栈顶地址 */
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
*	函 数 名: bsp_CmpCpuFlash
*	功能说明: 比较Flash指定地址的数据.
*	形    参: _ulFlashAddr : Flash地址
*			 _ucpBuf : 数据缓冲区
*			 _ulSize : 数据大小（单位是字节）
*	返 回 值:
*			FLASH_IS_EQU		0   Flash内容和待写入的数据相等，不需要擦除和写操作
*			FLASH_REQ_WRITE		1	Flash不需要擦除，直接写
*			FLASH_REQ_ERASE		2	Flash需要先擦除,再写
*			FLASH_PARAM_ERR		3	函数参数错误
*********************************************************************************************************
*/
#if 0
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucIsEqu;	/* 相等标志 */
	uint8_t ucByte;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
	{
		return FLASH_PARAM_ERR;		/*　函数参数错误　*/
	}

	/* 长度为0时返回正确 */
	if (_ulSize == 0)
	{
		return FLASH_IS_EQU;		/* Flash内容和待写入的数据相等 */
	}

	ucIsEqu = 1;			/* 先假设所有字节和待写入的数据相等，如果遇到任何一个不相等，则设置为 0 */
	for (i = 0; i < _ulSize; i++)
	{
		ucByte = *(uint8_t *)_ulFlashAddr;

		if (ucByte != *_ucpBuf)
		{
			if (ucByte != 0xFF)
			{
				return FLASH_REQ_ERASE;		/* 需要擦除后再写 */
			}
			else
			{
				ucIsEqu = 0;	/* 不相等，需要写 */
			}
		}

		_ulFlashAddr++;
		_ucpBuf++;
	}

	if (ucIsEqu == 1)
	{
		return FLASH_IS_EQU;	/* Flash内容和待写入的数据相等，不需要擦除和写操作 */
	}
	else
	{
		return FLASH_REQ_WRITE;	/* Flash不需要擦除，直接写 */
	}
}
#endif 
/*
*********************************************************************************************************
*	函 数 名: bsp_EraseCpuFlash
*	功能说明: 擦除CPU FLASH一个扇区 （128KB)
*	形    参: _ulFlashAddr : Flash地址
*	返 回 值: 0 成功， 1 失败
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
	/* 解锁 */
	HAL_FLASH_Unlock();
	
	/* 获取此地址所在的扇区 */
	//FirstSector = bsp_GetSector(_ulFlashAddr);
     FirstPage = GetPage(_ulFlashAddr);//flashdestination + i*128*16;//0x08004000;//GetPage();
	 //FirstPage    =  bsp_GetSector(_ulFlashAddr);
	/* 固定1个扇区 */
	NbOfPages = 1;

	/* 擦除扇区配置 */
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
	
	/* 扇区擦除 */	
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
	
	/* 擦除完毕后，上锁 */
	HAL_FLASH_Lock();	
	
	return re;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_WriteCpuFlash
*	功能说明: 写数据到CPU 内部Flash。 必须按32字节整数倍写。不支持跨扇区。扇区大小128KB. \
*			  写之前需要擦除扇区. 长度不是32字节整数倍时，最后几个字节末尾补0写入.
*	形    参: _ulFlashAddr : Flash地址
*			 _ucpSrc : 数据缓冲区
*			 _ulSize : 数据大小（单位是字节, 必须是32字节整数倍）
*	返 回 值: 0-成功，1-数据长度或地址溢出，2-写Flash出错(估计Flash寿命到)
*********************************************************************************************************
*/
#if 0
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucRet;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
	{
		return 1;
	}

	/* 长度为0时不继续操作  */
	if (_ulSize == 0)
	{
		return 0;
	}

	ucRet = bsp_CmpCpuFlash(_ulFlashAddr, _ucpSrc, _ulSize);

	if (ucRet == FLASH_IS_EQU)
	{
		return 0;
	}

	__set_PRIMASK(1);  		/* 关中断 */

	/* FLASH 解锁 */
	HAL_FLASH_Unlock();

	for (i = 0; i < _ulSize / 32; i++)	
	{
		uint64_t FlashWord[4];
		
		memcpy((char *)FlashWord, _ucpSrc, 32);
		_ucpSrc += 32;
		
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, _ulFlashAddr, (uint64_t)((uint32_t)FlashWord)) == HAL_OK)
		{
			_ulFlashAddr = _ulFlashAddr + 32; /* 递增，操作下一个256bit */				
		}		
		else
		{
			goto err;
		}
	}
	
	/* 长度不是32字节整数倍 */
	if (_ulSize % 32)
	{
		uint64_t FlashWord[4];
		
		FlashWord[0] = 0;
		FlashWord[1] = 0;
		FlashWord[2] = 0;
		FlashWord[3] = 0;
		memcpy((char *)FlashWord, _ucpSrc, _ulSize % 32);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, _ulFlashAddr, (uint64_t)((uint32_t)FlashWord)) == HAL_OK)
		{
			; // _ulFlashAddr = _ulFlashAddr + 32;		
		}		
		else
		{
			goto err;
		}
	}
	
  	/* Flash 加锁，禁止写Flash控制寄存器 */
  	HAL_FLASH_Lock();

  	__set_PRIMASK(0);  		/* 开中断 */

	return 0;
	
err:
  	/* Flash 加锁，禁止写Flash控制寄存器 */
  	HAL_FLASH_Lock();

  	__set_PRIMASK(0);  		/* 开中断 */

	return 1;
}


#endif 



