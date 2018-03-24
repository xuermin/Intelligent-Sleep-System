#include "stm32f10x.h"
#include "ADS1115.h"
#include "stm32f10x_adc.h"
#include "USER_delay.h"


void ADC_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  //
	ADC_InitTypeDef ADC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1,ENABLE);//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  //
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;//
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  
  
  ADC_DeInit(ADC1);
  ADC_InitStructure.ADC_Mode =ADC_Mode_Independent;//
  ADC_InitStructure.ADC_ScanConvMode =DISABLE;//
  ADC_InitStructure.ADC_ContinuousConvMode =ENABLE;//
  ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_None;//
  ADC_InitStructure.ADC_DataAlign =ADC_DataAlign_Right;//
  ADC_InitStructure.ADC_NbrOfChannel =1;//
  ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_239Cycles5);//
  ADC_Cmd(ADC1,ENABLE);//
  ADC_ResetCalibration(ADC1);//
	while(ADC_GetResetCalibrationStatus(ADC1));// 
	ADC_StartCalibration(ADC1);//
	while(ADC_GetCalibrationStatus(ADC1));//
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);//
  
}
/*************ADS1115��ʼ������*************/
/***********����ֵ ͨ�� 0,1,2,3*************/
/**********����ֵ ���յ�ѹ ��λV************/
/***********����ֵ�Ǵ���С����**************/
/*********�ڽ�����������ת��ʱ**************/
/*******һ��Ҫע���������͵Ķ���************/
//float ADS1115(u8 Channel)
//{
//	unsigned char  MSB,LSB;  //���ڴ�Ŷ������ļĴ�������
//	float Value;
//	///////////����Config �Ĵ���
//	
//	ADS1115_Start();          //������ʼ�ź�
//	ADS1115_Write_Byte(0x90); //������ַ+д
//	while(!get_ack());
//	ADS1115_Write_Byte(0x01); //Config registor�Ĵ�����ַ
//	while(!get_ack());
//	switch(Channel)
//	{ 
//		//��ʵ��,��16λ�Ĵ�����һλ����Ϊ1,�����޷����ת��,��֪��Ϊɶ
//		case 0:   //ʹ��ͨ��0,FSΪ4.096
//		ADS1115_Write_Byte(0xc2);	
//		break;
//		case 1:   //ʹ��ͨ��1
//		ADS1115_Write_Byte(0xd2);	
//		break;
//	  case 2:   //ʹ��ͨ��2
//		ADS1115_Write_Byte(0xe2);	
//		break;
//		case 3:   //ʹ��ͨ��3
//		ADS1115_Write_Byte(0xf2);	
//		break;
//	}
//	while(!get_ack());
//	ADS1115_Write_Byte(0xe2); //�������ص�һ���������������õ�
//	while(!get_ack());
//	ADS1115_Stop();   //������ֹ�ź�
//	
//	delay_ms(2);
//	//����ʱ��ʼ��ȡ�Ĺ���
//	////////////����pointer �Ĵ���,���������ǹ̶���
//	ADS1115_Start();          //������ʼ�ź�
//	ADS1115_Write_Byte(0x90); //������ַ+д
//	while(!get_ack());
//	ADS1115_Write_Byte(0x00);
//	while(!get_ack());
//	ADS1115_Stop();   //������ֹ�ź�
//	delay_ms(2);
//	
//	ADS1115_Start(); 
//	ADS1115_Write_Byte(0x91);  //������ַ+��
//	while(!get_ack());
//	MSB = ADS1115_Read_Byte(); //�߰�λ
//	ACK();
//	LSB = ADS1115_Read_Byte(); //�Ͱ�λ
//	NACK();    //�����������,Ϊʲô���һ�η��ͷ�Ӧ���ź�??
//	ADS1115_Stop();   //������ֹ�ź�
//	Value = MSB*256 + LSB;
//	
//	//Vin = +FS/32768 * Value
//	Value = Value*4.096/32768;  //����������,��λ:����V
//	return Value;
//}