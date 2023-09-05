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

uint16_t g_iapbuf[1024];       /* 2K�ֽڻ��� */
static void sys_msr_msp(uint32_t addr);
/**
 * @brief       IAPд��APP BIN
 * @param       appxaddr : Ӧ�ó������ʼ��ַ
 * @param       appbuf   : Ӧ�ó���CODE
 * @param       appsize  : Ӧ�ó����С(�ֽ�)
 * @retval      ��
 */
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint16_t t;
    uint16_t i = 0;
    uint16_t temp;
    uint32_t fwaddr = appxaddr; /* ��ǰд��ĵ�ַ */
    uint8_t *dfu = appbuf;

    for (t = 0; t < appsize; t += 2)
    {
        temp = (uint16_t)dfu[1] << 8;
        temp |= (uint16_t)dfu[0];
        dfu += 2;               /* ƫ��2���ֽ� */
        g_iapbuf[i++] = temp;

        if (i == 1024)
        {
            i = 0;
            stmflash_write(fwaddr, g_iapbuf, 1024);
         
            fwaddr += 2048;     /* ƫ��2048  16 = 2 * 8  ����Ҫ����2 */
        }
    }

    if (i)
    {
        stmflash_write(fwaddr, g_iapbuf, i);  /* ������һЩ�����ֽ�д��ȥ */
      
    }
}

/**
 * @brief       ��ת��Ӧ�ó����(ִ��APP)
 * @param       appxaddr : Ӧ�ó������ʼ��ַ

 * @retval      ��
 */
void iap_load_app(uint32_t appxaddr)
{
//  //  if (((*(volatile  uint32_t *)appxaddr) & 0x2FFE0000) == 0x20000000)     /* ���ջ����ַ�Ƿ�Ϸ�.���Է����ڲ�SRAM��64KB(0x20000000) */
//    {
//        /* �û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ) */
//        jump2app = (iapfun) * (volatile uint32_t *)(appxaddr + 4);
//        
//        /* ��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ) */
//        sys_msr_msp(*(volatile uint32_t *)appxaddr);
//        
//        /* ��ת��APP */
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
    __set_MSP(addr);    /* ����ջ����ַ */
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
*	�� �� ��: bsp_CmpCpuFlash
*	����˵��: �Ƚ�Flashָ����ַ������.
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpBuf : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ:
*			FLASH_IS_EQU		0   Flash���ݺʹ�д���������ȣ�����Ҫ������д����
*			FLASH_REQ_WRITE		1	Flash����Ҫ������ֱ��д
*			FLASH_REQ_ERASE		2	Flash��Ҫ�Ȳ���,��д
*			FLASH_PARAM_ERR		3	������������
*********************************************************************************************************
*/
#if 0
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucIsEqu;	/* ��ȱ�־ */
	uint8_t ucByte;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
	{
		return FLASH_PARAM_ERR;		/*��������������*/
	}

	/* ����Ϊ0ʱ������ȷ */
	if (_ulSize == 0)
	{
		return FLASH_IS_EQU;		/* Flash���ݺʹ�д���������� */
	}

	ucIsEqu = 1;			/* �ȼ��������ֽںʹ�д���������ȣ���������κ�һ������ȣ�������Ϊ 0 */
	for (i = 0; i < _ulSize; i++)
	{
		ucByte = *(uint8_t *)_ulFlashAddr;

		if (ucByte != *_ucpBuf)
		{
			if (ucByte != 0xFF)
			{
				return FLASH_REQ_ERASE;		/* ��Ҫ��������д */
			}
			else
			{
				ucIsEqu = 0;	/* ����ȣ���Ҫд */
			}
		}

		_ulFlashAddr++;
		_ucpBuf++;
	}

	if (ucIsEqu == 1)
	{
		return FLASH_IS_EQU;	/* Flash���ݺʹ�д���������ȣ�����Ҫ������д���� */
	}
	else
	{
		return FLASH_REQ_WRITE;	/* Flash����Ҫ������ֱ��д */
	}
}
#endif 
/*
*********************************************************************************************************
*	�� �� ��: bsp_EraseCpuFlash
*	����˵��: ����CPU FLASHһ������ ��128KB)
*	��    ��: _ulFlashAddr : Flash��ַ
*	�� �� ֵ: 0 �ɹ��� 1 ʧ��
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
	/* ���� */
	HAL_FLASH_Unlock();
	
	/* ��ȡ�˵�ַ���ڵ����� */
	//FirstSector = bsp_GetSector(_ulFlashAddr);
    FirstPage = GetPage(FLASH_USER_START_ADDR);
	
	/* �̶�1������ */
	NbOfPages = 1;	

	/* ������������ */
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;

	
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
	
	/* �������� */	
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
	
	/* ������Ϻ����� */
	HAL_FLASH_Lock();	
	
	return re;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_WriteCpuFlash
*	����˵��: д���ݵ�CPU �ڲ�Flash�� ���밴32�ֽ�������д����֧�ֿ�������������С128KB. \
*			  д֮ǰ��Ҫ��������. ���Ȳ���32�ֽ�������ʱ����󼸸��ֽ�ĩβ��0д��.
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpSrc : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽ�, ������32�ֽ���������
*	�� �� ֵ: 0-�ɹ���1-���ݳ��Ȼ��ַ�����2-дFlash����(����Flash������)
*********************************************************************************************************
*/
#if 0
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
	uint32_t i;
	uint8_t ucRet;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
	{
		return 1;
	}

	/* ����Ϊ0ʱ����������  */
	if (_ulSize == 0)
	{
		return 0;
	}

	ucRet = bsp_CmpCpuFlash(_ulFlashAddr, _ucpSrc, _ulSize);

	if (ucRet == FLASH_IS_EQU)
	{
		return 0;
	}

	__set_PRIMASK(1);  		/* ���ж� */

	/* FLASH ���� */
	HAL_FLASH_Unlock();

	for (i = 0; i < _ulSize / 32; i++)	
	{
		uint64_t FlashWord[4];
		
		memcpy((char *)FlashWord, _ucpSrc, 32);
		_ucpSrc += 32;
		
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, _ulFlashAddr, (uint64_t)((uint32_t)FlashWord)) == HAL_OK)
		{
			_ulFlashAddr = _ulFlashAddr + 32; /* ������������һ��256bit */				
		}		
		else
		{
			goto err;
		}
	}
	
	/* ���Ȳ���32�ֽ������� */
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
	
  	/* Flash ��������ֹдFlash���ƼĴ��� */
  	HAL_FLASH_Lock();

  	__set_PRIMASK(0);  		/* ���ж� */

	return 0;
	
err:
  	/* Flash ��������ֹдFlash���ƼĴ��� */
  	HAL_FLASH_Lock();

  	__set_PRIMASK(0);  		/* ���ж� */

	return 1;
}


#endif 



