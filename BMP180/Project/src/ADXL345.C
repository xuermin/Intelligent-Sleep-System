#include "stm32f10x.h"
#include "ADXL345.h"
#include "USER_delay.h"
#include "SCI.H"
#include "stm32f10x_gpio.h"

/*PB12--SCL,PB13--SDA*/
void ADXL345_GPIO_CLOCK(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/* Enable the GPIO Clock */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIO , ENABLE); 						 
				
		GPIO_InitStructure.GPIO_Pin = SCL ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIO, &GPIO_InitStructure);
}

void GPIO_DATAIN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable the GPIO Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIO , ENABLE);
	GPIO_InitStructure.GPIO_Pin = SDA ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIO, &GPIO_InitStructure);
}

void GPIO_DATAOUT(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/* Enable the GPIO Clock */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIO , ENABLE); 						 
				
		GPIO_InitStructure.GPIO_Pin = SDA ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIO, &GPIO_InitStructure);
}

/**************************************
��ʼ�ź�  SDA 1--0
**************************************/
void ADXL345_Start(void)
{
	GPIO_DATAOUT();
	SDAH;
	delay_us(10);
	SCLH;
	delay_us(10);
	SDAL;
	delay_us(10);
	SCLL;
}

/**************************************
ֹͣ�ź�   SDA 0--1
**************************************/
void ADXL345_Stop(void)
{
	GPIO_DATAOUT();
	SDAL;
	delay_us(10);
	SCLH;
	delay_us(10);
	SDAH;
	delay_us(10);
	SCLL;
	//delay_us(10);
}	
/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(unsigned char ack_flag)
{
	SCLL;
	GPIO_DATAOUT();
	if(ack_flag)
	{
		SDAH;
		delay_us(5);
		SCLH;
		delay_us(5);
		SCLL;
		SDAL;
	}
		
	else  
	{
		SDAL;
		delay_us(5);
		SCLH;
		delay_us(5);
		SCLL;
		SDAH;
	}
}
/**************************************
����Ӧ���ź�
**************************************/
unsigned char ADXL345_RecvACK(void)
{
	unsigned char CY;
	GPIO_DATAIN();
	SDAH;
	delay_us(5);
	SCLH;
	while(GPIO_ReadInputDataBit(GPIO,SDA))
	{
		CY++;
		if(CY>250)
		{
			ADXL345_Stop();
			return 1;
		}
	}
	SCLL;
	return 0;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void ADXL345_SendByte(unsigned char dat)
{
	uchar i;
	GPIO_DATAOUT();
	SCLL;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
			SDAH;
		else SDAL;
		delay_us(5);
		SCLH;
		delay_us(5);
		SCLL;
		delay_us(5);
		dat<<=1;
	}
	ADXL345_RecvACK();
}
/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
unsigned char ADXL345_RecvByte(void)
{
	uchar i,data=0;
	GPIO_DATAIN();

	for(i=0;i<8;i++)
	{
		SCLL;
		delay_us(5);
		SCLH;
		data<<=1;
		delay_us(1);
		SCLH;
		if(SDA_DATA)
			data|=0x01;
		else data&=0xfe;
	}
	if(ADXL345_RecvACK())
		ADXL345_SendACK(0);
	else
		ADXL345_SendACK(1);
	return data;
}

/**************************************
�����ֽ�д��
**************************************/
void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    ADXL345_Start();                  //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    ADXL345_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
    ADXL345_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    ADXL345_Stop();                   //����ֹͣ�ź�
}
/**************************************
�����ֽڶ�ȡ
**************************************/
uchar Single_Read_ADXL345(uchar REG_Address)
{  	
		uchar REG_data;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    ADXL345_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ	
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    REG_data=ADXL345_RecvByte();              //�����Ĵ�������
		ADXL345_SendACK(1);   
		ADXL345_Stop();                           //ֹͣ�ź�
    return REG_data; 
}

/**************************************
��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
**************************************/
void Multiple_read_ADXL345(unsigned char *BUF)
{  
		uchar i;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    ADXL345_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
		for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)
        {
           ADXL345_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //��ӦACK
        }
   }
    ADXL345_Stop();                          //ֹͣ�ź�
    delay_ms(5);
}

//��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�************************
void Init_ADXL345(void)
{
   Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ
   Single_Write_ADXL345(0x2C,0x08);   //�����趨Ϊ12.5 �ο�pdf13ҳ
   Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
   Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж�
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x20,0x7f);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
	
//	Iic_Init();
//	I2C_WriteByte(0xA6,BW_RATE,0x08);   //�����趨Ϊ12.5 �ο�pdf13ҳ
//	I2C_WriteByte(0xA6,POWER_CTL,0x08);   //��Դ������ʽ����
//	I2C_WriteByte(0xA6,DATA_FORMAT,0x0B);	
//	I2C_WriteByte(0xA6,INT_ENABLE,0X80);	
}


void get_x(unsigned char *buff1,unsigned char *x_buff)
{
	int data;
	float temp;
	data=(buff1[1]<<8)+buff1[0];

	if(data<0)
	{
		data=-data;
		x_buff[0]='-';
	}
	else x_buff[0]='+';
	temp= (float)data*3.9;
	data=temp;
	
	x_buff[1]=data/1000 +0x30;
	//x_buff[1]=0+0x30;
	data=data%1000;
	x_buff[2]='.';
	x_buff[3]=data/100+0x30;
	data%=100;
	x_buff[4]=data/10+0x30;
	data%=10;
	x_buff[5]=data+0x30;
	x_buff[6]='\0';
	
}
void get_z(unsigned char *buff1,unsigned char *z_buff)
{
	int data;
	float temp;
	data=(buff1[5]<<8)+buff1[4];
//	data+=0x30;
//	USART_printf(USART2,data);
	if(data<0)
	{
		data=-data;
		z_buff[0]='-';
	}
	else z_buff[0]='+';
		temp= (float)data*3.9;
	data=temp;
	z_buff[1]=data/1000 +0x30;
	data%=1000;
	z_buff[2]='.';
	z_buff[3]=data/100+0x30;
	data%=100;
	z_buff[4]=data/10+0x30;
	data%=10;
	z_buff[5]=data+0x30;
	z_buff[6]='\0';
}
void get_y(unsigned char *buff1,unsigned char *y_buff)
{
	int data;
	float temp;
	data=(buff1[3]<<8)+buff1[2];

	if(data<0)
	{
		data=-data;
		y_buff[0]='-';
	}
	else y_buff[0]='+';
		temp= (float)data*3.9;
	data=temp;
//	test[0]=data/1000;
//	RS232_Send_Data(test,1);
	y_buff[1]=data/1000 +0x30;
	data%=1000;
	y_buff[2]='.';
	y_buff[3]=data/100+0x30;
	data%=100;
	y_buff[4]=data/10+0x30;
	data%=10;
	y_buff[5]=data+0x30;
	y_buff[6]='\0';
}

