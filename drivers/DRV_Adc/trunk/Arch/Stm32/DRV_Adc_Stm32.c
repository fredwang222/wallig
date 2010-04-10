#include <string.h>
#include "stm32f10x.h"
#include "DRV_Adc.h"


#define ADC_DEVICE_COUNT ( sizeof(ADC_DeviceCfgList)/sizeof(Adc_Device_Cfg_t))

typedef struct
{
	char *pcName;
	unsigned char Channel;
	unsigned char SampleTime;
	GPIO_TypeDef* PORT;
	u16 Pin;
} Adc_Device_Cfg_t;

typedef struct
{
	Adc_Device_Cfg_t *pCfg;
	unsigned short *pusMeasure;
	DRV_Adc_Device_State eState;
} Adc_Device_Data_t;

struct
{
	unsigned char ucOpenedDeviceIndex;
} sADC_Driver_Data;


static const Adc_Device_Cfg_t ADC_DeviceCfgList[]=ADC_INIT_CFG;
static unsigned short ADC_tusMesures[ADC_DEVICE_COUNT];
static Adc_Device_Data_t ADC_DeviceDataList[ADC_DEVICE_COUNT];

void DRV_Adc_DmaInit( unsigned char ucBufferSize );
void DRV_Adc_AdcInit( unsigned char ucNBofChannel );
void DRV_Adc_Calibration( void );
void DRV_Adc_RemapAdcMeasurePointer(Adc_Device_Data_t *pDeviceData , unsigned short *pusMeasure);

void DRV_Adc_Init(void )
{
	int iDevicecount;

	for( iDevicecount = 0 ; iDevicecount < ADC_DEVICE_COUNT ; iDevicecount++)
	{
		ADC_DeviceDataList[iDevicecount].pCfg = (Adc_Device_Cfg_t*)&ADC_DeviceCfgList[iDevicecount];
		ADC_DeviceDataList[iDevicecount].pusMeasure =(unsigned short *) &ADC_tusMesures[iDevicecount];
		ADC_DeviceDataList[iDevicecount].eState = Adc_Device_Close;
	}
	sADC_Driver_Data.ucOpenedDeviceIndex = 0;

	RCC_AHBPeriphClockCmd(ADC_DMA_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(ADC_CLOCK , ENABLE);

}

DRV_Adc_Error DRV_Adc_Open( const char *pcName , DRV_Adc_Handle *pHandle)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  DRV_Adc_Error eError = Adc_Device_Not_Found;
	  Adc_Device_Data_t *pAdcData;
	  int iAdcIndex;
	  int iRank=1;

	  for ( iAdcIndex = 0 ; iAdcIndex < ADC_DEVICE_COUNT ; iAdcIndex++ )
	  	{
	  		if( !strcmp( pcName , ADC_DeviceDataList[iAdcIndex].pCfg->pcName) )
	  		{
	  			pAdcData = &ADC_DeviceDataList[iAdcIndex];
	  			eError=Adc_No_Error;
	  			break;
	  		}
	  	}
	  	if( eError==Adc_Device_Not_Found )
	  		return eError;
	  	if( pAdcData->eState == Adc_Device_Open)
	  		return Adc_AlreadyOpened;
	  	*pHandle =(DRV_Adc_Handle)pAdcData;
	  /* Stop ADC Software Conversion */
  	  ADC_SoftwareStartConvCmd(ADC_DEVICE, DISABLE );
	  sADC_Driver_Data.ucOpenedDeviceIndex++;
	  pAdcData->eState = Adc_Device_Open;
	  /* Remap the measure pointer */
	 // DRV_Adc_RemapAdcMeasurePointer(ADC_DeviceDataList , ( unsigned short *) ADC_tusMesures );
	  pAdcData->pusMeasure = (unsigned short*)&ADC_tusMesures[sADC_Driver_Data.ucOpenedDeviceIndex-1];
	  DRV_Adc_AdcInit(sADC_Driver_Data.ucOpenedDeviceIndex);
	  /* Reconfigure the DMA */
	  DRV_Adc_DmaInit( sADC_Driver_Data.ucOpenedDeviceIndex );
	  /* Configure the input: external or internal */
	  if( pAdcData->pCfg->Channel != ADC_Channel_17 && pAdcData->pCfg->Channel != ADC_Channel_16 )
	  {
		  /* Configure pio as analog input -----*/
		  GPIO_InitStructure.GPIO_Pin = pAdcData->pCfg->Pin;
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		  GPIO_Init(pAdcData->pCfg->PORT, &GPIO_InitStructure);
	  }

	  /* Enable All Opened Channel */
	  for ( iAdcIndex = 0 ; iAdcIndex < ADC_DEVICE_COUNT ; iAdcIndex++ )
	  {

		  if( ADC_DeviceDataList[iAdcIndex].eState == Adc_Device_Open )
		  {
			  ADC_RegularChannelConfig(ADC_DEVICE, ADC_DeviceDataList[iAdcIndex].pCfg->Channel , iRank++ , ADC_DeviceDataList[iAdcIndex].pCfg->SampleTime);
			  if( ADC_DeviceDataList[iAdcIndex].pCfg->Channel == ADC_Channel_17 || ADC_DeviceDataList[iAdcIndex].pCfg->Channel == ADC_Channel_16 )
				  ADC_TempSensorVrefintCmd(ENABLE);
		  }
	  }
	  ADC_DMACmd(ADC_DEVICE, ENABLE);
	  ADC_Cmd(ADC_DEVICE, ENABLE);
	  DRV_Adc_Calibration();
	  /* Start ADC Software Conversion */
   	  ADC_SoftwareStartConvCmd(ADC_DEVICE, ENABLE );
   	  /* Test on Channel 1 DMA_FLAG_TC flag */
   	  while(!DMA_GetFlagStatus(ADC_DMA_TC_FLAG));
   	  /* Clear Channel 1 DMA1_FLAG_TC flag */
   	  DMA_ClearFlag(ADC_DMA_TC_FLAG);

	  return eError;
}

void DRV_Adc_Close( DRV_Adc_Handle Handle)
{

}
unsigned short DRV_Adc_Read( DRV_Adc_Handle Handle )
{
	Adc_Device_Data_t *pAdcData = (Adc_Device_Data_t *)Handle;

	if( pAdcData != NULL )
		return *pAdcData->pusMeasure;
	return 0;
}

void DRV_Adc_DmaInit( unsigned char ucBufferSize )
{
	DMA_InitTypeDef   DMA_InitStructure;

	DMA_Cmd(ADC_DMA_CHANNEL, DISABLE);
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_ADR_SRC;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_tusMesures;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ucBufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);

	  /* Enable DMA1 channel1 */
	  DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
}

void DRV_Adc_AdcInit( unsigned char ucNBofChannel )
{
	ADC_InitTypeDef   ADC_InitStructure;
	/* ADC configuration ------------------------------------------------------*/
	ADC_Cmd(ADC_DEVICE, DISABLE);
	ADC_DeInit(ADC_DEVICE);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = ucNBofChannel;
	ADC_Init(ADC_DEVICE, &ADC_InitStructure);
}
void DRV_Adc_Calibration( void )
{
	ADC_ResetCalibration(ADC_DEVICE);
	/* Check the end of ADC reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC_DEVICE));
	/* Start ADC calibration */
	ADC_StartCalibration(ADC_DEVICE);
	/* Check the end of ADC calibration */
	while(ADC_GetCalibrationStatus(ADC_DEVICE));

}
void DRV_Adc_RemapAdcMeasurePointer(Adc_Device_Data_t *pDeviceData , unsigned short *pusMeasure )
{
	unsigned char ucAdcIndex,ucChannelIndex;

	for ( ucChannelIndex = 0 ; ucChannelIndex < ADC_CHANNEL_MAXCOUNT ; ucChannelIndex++ )
	{
		for ( ucAdcIndex = 0 ; ucAdcIndex < ADC_DEVICE_COUNT ; ucAdcIndex++)
		{
			if( pDeviceData[ucAdcIndex].eState == Adc_Device_Open)
			{
				if(pDeviceData[ucAdcIndex].pCfg->Channel == ucChannelIndex)
				{
					pDeviceData[ucAdcIndex].pusMeasure = pusMeasure;
					pusMeasure++;
				}
			}
		}
	}

}
