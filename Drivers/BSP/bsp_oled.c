
/***************************************************************************************
  * �������ɽ�Э�Ƽ���������ѿ�Դ����
  * ���������鿴��ʹ�ú��޸ģ���Ӧ�õ��Լ�����Ŀ֮��
  * �����Ȩ�齭Э�Ƽ����У��κ��˻���֯���ý����Ϊ����
  * 
  * �������ƣ�				0.96��OLED��ʾ����������4���I2C�ӿڣ�
  * ���򴴽�ʱ�䣺			2023.10.24
  * ��ǰ����汾��			V2.0
  * ��ǰ�汾����ʱ�䣺		2024.10.20
  * 
  * ��Э�Ƽ��ٷ���վ��		jiangxiekeji.com
  * ��Э�Ƽ��ٷ��Ա��꣺	jiangxiekeji.taobao.com
  * ������ܼ����¶�̬��	jiangxiekeji.com/tutorial/oled.html
  * 
  * ����㷢�ֳ����е�©�����߱��󣬿�ͨ���ʼ������Ƿ�����feedback@jiangxiekeji.com
  * �����ʼ�֮ǰ��������ȵ����¶�̬ҳ��鿴���³�������������Ѿ��޸ģ��������ٷ��ʼ�
  ***************************************************************************************
  */

#include "stm32f4xx.h"
#include "bsp_oled.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

/**
  * ���ݴ洢��ʽ��
  * ����8�㣬��λ���£��ȴ����ң��ٴ��ϵ���
  * ÿһ��Bit��Ӧһ�����ص�
  * 
  *      B0 B0                  B0 B0
  *      B1 B1                  B1 B1
  *      B2 B2                  B2 B2
  *      B3 B3  ------------->  B3 B3 --
  *      B4 B4                  B4 B4  |
  *      B5 B5                  B5 B5  |
  *      B6 B6                  B6 B6  |
  *      B7 B7                  B7 B7  |
  *                                    |
  *  -----------------------------------
  *  |   
  *  |   B0 B0                  B0 B0
  *  |   B1 B1                  B1 B1
  *  |   B2 B2                  B2 B2
  *  --> B3 B3  ------------->  B3 B3
  *      B4 B4                  B4 B4
  *      B5 B5                  B5 B5
  *      B6 B6                  B6 B6
  *      B7 B7                  B7 B7
  * 
  * �����ᶨ�壺
  * ���Ͻ�Ϊ(0, 0)��
  * ��������ΪX�ᣬȡֵ��Χ��0~127
  * ��������ΪY�ᣬȡֵ��Χ��0~63
  * 
  *       0             X��           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y�� |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */


/*ȫ�ֱ���*********************/

/**
  * OLED�Դ�����
  * ���е���ʾ��������ֻ�ǶԴ��Դ�������ж�д
  * ������OLED_Update������OLED_UpdateArea����
  * �ŻὫ�Դ���������ݷ��͵�OLEDӲ����������ʾ
  */
uint8_t OLED_DisplayBuf[8][128];

/*********************ȫ�ֱ���*/


/*��������*********************/

/**
  * ��    ����OLEDдSCL�ߵ͵�ƽ
  * ��    ����Ҫд��SCL�ĵ�ƽֵ����Χ��0/1
  * �� �� ֵ����
  * ˵    �������ϲ㺯����ҪдSCLʱ���˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ����SCL��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ����SCLΪ�͵�ƽ������������1ʱ����SCLΪ�ߵ�ƽ
  */
void OLED_W_SCL(uint8_t BitValue)
{
	/*����BitValue��ֵ����SCL�øߵ�ƽ���ߵ͵�ƽ*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)BitValue);
	
	/*�����Ƭ���ٶȹ��죬���ڴ�����������ʱ���Ա��ⳬ��I2Cͨ�ŵ�����ٶ�*/
	//...
}

/**
  * ��    ����OLEDдSDA�ߵ͵�ƽ
  * ��    ����Ҫд��SDA�ĵ�ƽֵ����Χ��0/1
  * �� �� ֵ����
  * ˵    �������ϲ㺯����ҪдSDAʱ���˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ����SDA��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ����SDAΪ�͵�ƽ������������1ʱ����SDAΪ�ߵ�ƽ
  */
void OLED_W_SDA(uint8_t BitValue)
{
	/*����BitValue��ֵ����SDA�øߵ�ƽ���ߵ͵�ƽ*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)BitValue);
	
	/*�����Ƭ���ٶȹ��죬���ڴ�����������ʱ���Ա��ⳬ��I2Cͨ�ŵ�����ٶ�*/
	//...
}

/**
  * ��    ����OLED���ų�ʼ��
  * ��    ������
  * �� �� ֵ����
  * ˵    �������ϲ㺯����Ҫ��ʼ��ʱ���˺����ᱻ����
  *           �û���Ҫ��SCL��SDA���ų�ʼ��Ϊ��©ģʽ�����ͷ�����
  */
void OLED_GPIO_Init(void)
{
	uint32_t i, j;
	
	/*�ڳ�ʼ��ǰ������������ʱ����OLED�����ȶ�*/
	for (i = 0; i < 1000; i ++)
	{
		for (j = 0; j < 1000; j ++);
	}
	
	/*F407: SCL=PB14, SDA=PB15, 开漏输出*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
 	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
 	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*�ͷ�SCL��SDA*/
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/*********************��������*/


/*ͨ��Э��*********************/

/**
  * ��    ����I2C��ʼ
  * ��    ������
  * �� �� ֵ����
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);		//�ͷ�SDA��ȷ��SDAΪ�ߵ�ƽ
	OLED_W_SCL(1);		//�ͷ�SCL��ȷ��SCLΪ�ߵ�ƽ
	OLED_W_SDA(0);		//��SCL�ߵ�ƽ�ڼ䣬����SDA��������ʼ�ź�
	OLED_W_SCL(0);		//��ʼ���SCLҲ���ͣ���Ϊ��ռ�����ߣ�ҲΪ�˷�������ʱ���ƴ��
}

/**
  * ��    ����I2C��ֹ
  * ��    ������
  * �� �� ֵ����
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);		//����SDA��ȷ��SDAΪ�͵�ƽ
	OLED_W_SCL(1);		//�ͷ�SCL��ʹSCL���ָߵ�ƽ
	OLED_W_SDA(1);		//��SCL�ߵ�ƽ�ڼ䣬�ͷ�SDA��������ֹ�ź�
}

/**
  * ��    ����I2C����һ���ֽ�
  * ��    ����Byte Ҫ���͵�һ���ֽ����ݣ���Χ��0x00~0xFF
  * �� �� ֵ����
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	
	/*ѭ��8�Σ��������η������ݵ�ÿһλ*/
	for (i = 0; i < 8; i++)
	{
		/*ʹ������ķ�ʽȡ��Byte��ָ��һλ���ݲ�д�뵽SDA��*/
		/*����!�������ǣ������з����ֵ��Ϊ1*/
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);	//�ͷ�SCL���ӻ���SCL�ߵ�ƽ�ڼ��ȡSDA
		OLED_W_SCL(0);	//����SCL��������ʼ������һλ����
	}
	
	OLED_W_SCL(1);		//�����һ��ʱ�ӣ�������Ӧ���ź�
	OLED_W_SCL(0);
}

/**
  * ��    ����OLEDд����
  * ��    ����Command Ҫд�������ֵ����Χ��0x00~0xFF
  * �� �� ֵ����
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();				//I2C��ʼ
	OLED_I2C_SendByte(0x78);		//����OLED��I2C�ӻ���ַ
	OLED_I2C_SendByte(0x00);		//�����ֽڣ���0x00����ʾ����д����
	OLED_I2C_SendByte(Command);		//д��ָ��������
	OLED_I2C_Stop();				//I2C��ֹ
}

/**
  * ��    ����OLEDд����
  * ��    ����Data Ҫд�����ݵ���ʼ��ַ
  * ��    ����Count Ҫд�����ݵ�����
  * �� �� ֵ����
  */
void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
	uint8_t i;
	
	OLED_I2C_Start();				//I2C��ʼ
	OLED_I2C_SendByte(0x78);		//����OLED��I2C�ӻ���ַ
	OLED_I2C_SendByte(0x40);		//�����ֽڣ���0x40����ʾ����д����
	/*ѭ��Count�Σ���������������д��*/
	for (i = 0; i < Count; i ++)
	{
		OLED_I2C_SendByte(Data[i]);	//���η���Data��ÿһ������
	}
	OLED_I2C_Stop();				//I2C��ֹ
}

/*********************ͨ��Э��*/


/*Ӳ������*********************/

/**
  * ��    ����OLED��ʼ��
  * ��    ������
  * �� �� ֵ����
  * ˵    ����ʹ��ǰ����Ҫ���ô˳�ʼ������
  */
void OLED_Init(void)
{
	OLED_GPIO_Init();			//�ȵ��õײ�Ķ˿ڳ�ʼ��
	
	/*д��һϵ�е������OLED���г�ʼ������*/
	OLED_WriteCommand(0xAE);	//������ʾ����/�رգ�0xAE�رգ�0xAF����
	
	OLED_WriteCommand(0xD5);	//������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WriteCommand(0x80);	//0x00~0xFF
	
	OLED_WriteCommand(0xA8);	//���ö�·������
	OLED_WriteCommand(0x3F);	//0x0E~0x3F
	
	OLED_WriteCommand(0xD3);	//������ʾƫ��
	OLED_WriteCommand(0x00);	//0x00~0x7F
	
	OLED_WriteCommand(0x40);	//������ʾ��ʼ�У�0x40~0x7F
	
	OLED_WriteCommand(0xA1);	//�������ҷ���0xA1������0xA0���ҷ���
	
	OLED_WriteCommand(0xC8);	//�������·���0xC8������0xC0���·���

	OLED_WriteCommand(0xDA);	//����COM����Ӳ������
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//���öԱȶ�
	OLED_WriteCommand(0xCF);	//0x00~0xFF

	OLED_WriteCommand(0xD9);	//����Ԥ�������
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//����VCOMHȡ��ѡ�񼶱�
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//����������ʾ��/�ر�

	OLED_WriteCommand(0xA6);	//��������/��ɫ��ʾ��0xA6������0xA7��ɫ

	OLED_WriteCommand(0x8D);	//���ó���
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//������ʾ
	
	OLED_Clear();				//����Դ�����
	OLED_Update();				//������ʾ����������ֹ��ʼ����δ��ʾ����ʱ����
}

/**
  * ��    ����OLED������ʾ���λ��
  * ��    ����Page ָ��������ڵ�ҳ����Χ��0~7
  * ��    ����X ָ��������ڵ�X�����꣬��Χ��0~127
  * �� �� ֵ����
  * ˵    ����OLEDĬ�ϵ�Y�ᣬֻ��8��BitΪһ��д�룬��1ҳ����8��Y������
  */
void OLED_SetCursor(uint8_t Page, uint8_t X)
{
	/*���ʹ�ô˳�������1.3���OLED��ʾ��������Ҫ�����ע��*/
	/*��Ϊ1.3���OLED����оƬ��SH1106����132��*/
	/*��Ļ����ʼ�н����˵�2�У������ǵ�0��*/
	/*������Ҫ��X��2������������ʾ*/
	X=X+2;
	/*ͨ��ָ������ҳ��ַ���е�ַ*/
	OLED_WriteCommand(0xB0 | Page);					//����ҳλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//����Xλ�ø�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));			//����Xλ�õ�4λ
}

/*********************Ӳ������*/


/*���ߺ���*********************/

/*���ߺ��������ڲ����ֺ���ʹ��*/

/**
  * ��    �����η�����
  * ��    ����X ����
  * ��    ����Y ָ��
  * �� �� ֵ������X��Y�η�
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//���Ĭ��Ϊ1
	while (Y --)			//�۳�Y��
	{
		Result *= X;		//ÿ�ΰ�X�۳˵������
	}
	return Result;
}

/**
  * ��    �����ж�ָ�����Ƿ���ָ��������ڲ�
  * ��    ����nvert ����εĶ�����
  * ��    ����vertx verty ��������ζ����x��y���������
  * ��    ����testx testy ���Ե��X��y����
  * �� �� ֵ��ָ�����Ƿ���ָ��������ڲ���1�����ڲ���0�������ڲ�
  */
uint8_t OLED_pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx, int16_t testy)
{
	int16_t i, j, c = 0;
	
	/*���㷨��W. Randolph Franklin���*/
	/*�ο����ӣ�https://wrfranklin.org/Research/Short_Notes/pnpoly.html*/
	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

/**
  * ��    �����ж�ָ�����Ƿ���ָ���Ƕ��ڲ�
  * ��    ����X Y ָ���������
  * ��    ����StartAngle EndAngle ��ʼ�ǶȺ���ֹ�Ƕȣ���Χ��-180~180
  *           ˮƽ����Ϊ0�ȣ�ˮƽ����Ϊ180�Ȼ�-180�ȣ��·�Ϊ�������Ϸ�Ϊ������˳ʱ����ת
  * �� �� ֵ��ָ�����Ƿ���ָ���Ƕ��ڲ���1�����ڲ���0�������ڲ�
  */
uint8_t OLED_IsInAngle(int16_t X, int16_t Y, int16_t StartAngle, int16_t EndAngle)
{
	int16_t PointAngle;
	PointAngle = atan2(Y, X) / 3.14 * 180;	//����ָ����Ļ��ȣ���ת��Ϊ�Ƕȱ�ʾ
	if (StartAngle < EndAngle)	//��ʼ�Ƕ�С����ֹ�Ƕȵ����
	{
		/*���ָ���Ƕ�����ʼ��ֹ�Ƕ�֮�䣬���ж�ָ������ָ���Ƕ�*/
		if (PointAngle >= StartAngle && PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	else			//��ʼ�Ƕȴ�������ֹ�Ƕȵ����
	{
		/*���ָ���Ƕȴ�����ʼ�ǶȻ���С����ֹ�Ƕȣ����ж�ָ������ָ���Ƕ�*/
		if (PointAngle >= StartAngle || PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	return 0;		//�������������������ж��ж�ָ���㲻��ָ���Ƕ�
}

/*********************���ߺ���*/


/*���ܺ���*********************/

/**
  * ��    ������OLED�Դ�������µ�OLED��Ļ
  * ��    ������
  * �� �� ֵ����
  * ˵    �������е���ʾ��������ֻ�Ƕ�OLED�Դ�������ж�д
  *           ������OLED_Update������OLED_UpdateArea����
  *           �ŻὫ�Դ���������ݷ��͵�OLEDӲ����������ʾ
  *           �ʵ�����ʾ������Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_Update(void)
{
	uint8_t j;
	/*����ÿһҳ*/
	for (j = 0; j < 8; j ++)
	{
		/*���ù��λ��Ϊÿһҳ�ĵ�һ��*/
		OLED_SetCursor(j, 0);
		/*����д��128�����ݣ����Դ����������д�뵽OLEDӲ��*/
		OLED_WriteData(OLED_DisplayBuf[j], 128);
	}
}

/**
  * ��    ������OLED�Դ����鲿�ָ��µ�OLED��Ļ
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Width ָ������Ŀ��ȣ���Χ��0~128
  * ��    ����Height ָ������ĸ߶ȣ���Χ��0~64
  * �� �� ֵ����
  * ˵    �����˺��������ٸ��²���ָ��������
  *           �����������Y��ֻ��������ҳ����ͬһҳ��ʣ�ಿ�ֻ����һ�����
  * ˵    �������е���ʾ��������ֻ�Ƕ�OLED�Դ�������ж�д
  *           ������OLED_Update������OLED_UpdateArea����
  *           �ŻὫ�Դ���������ݷ��͵�OLEDӲ����������ʾ
  *           �ʵ�����ʾ������Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t j;
	int16_t Page, Page1;
	
	/*���������ڼ���ҳ��ַʱ��Ҫ��һ��ƫ��*/
	/*(Y + Height - 1) / 8 + 1��Ŀ����(Y + Height) / 8������ȡ��*/
	Page = Y / 8;
	Page1 = (Y + Height - 1) / 8 + 1;
	if (Y < 0)
	{
		Page -= 1;
		Page1 -= 1;
	}
	
	/*����ָ�������漰�����ҳ*/
	for (j = Page; j < Page1; j ++)
	{
		if (X >= 0 && X <= 127 && j >= 0 && j <= 7)		//������Ļ�����ݲ���ʾ
		{
			/*���ù��λ��Ϊ���ҳ��ָ����*/
			OLED_SetCursor(j, X);
			/*����д��Width�����ݣ����Դ����������д�뵽OLEDӲ��*/
			OLED_WriteData(&OLED_DisplayBuf[j][X], Width);
		}
	}
}

/**
  * ��    ������OLED�Դ�����ȫ������
  * ��    ������
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j ++)				//����8ҳ
	{
		for (i = 0; i < 128; i ++)			//����128��
		{
			OLED_DisplayBuf[j][i] = 0x00;	//���Դ���������ȫ������
		}
	}
}

/**
  * ��    ������OLED�Դ����鲿������
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Width ָ������Ŀ��ȣ���Χ��0~128
  * ��    ����Height ָ������ĸ߶ȣ���Χ��0~64
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t i, j;
	
	for (j = Y; j < Y + Height; j ++)		//����ָ��ҳ
	{
		for (i = X; i < X + Width; i ++)	//����ָ����
		{
			if (i >= 0 && i <= 127 && j >=0 && j <= 63)				//������Ļ�����ݲ���ʾ
			{
				OLED_DisplayBuf[j / 8][i] &= ~(0x01 << (j % 8));	//���Դ�����ָ����������
			}
		}
	}
}

/**
  * ��    ������OLED�Դ�����ȫ��ȡ��
  * ��    ������
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_Reverse(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j ++)				//����8ҳ
	{
		for (i = 0; i < 128; i ++)			//����128��
		{
			OLED_DisplayBuf[j][i] ^= 0xFF;	//���Դ���������ȫ��ȡ��
		}
	}
}
	
/**
  * ��    ������OLED�Դ����鲿��ȡ��
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Width ָ������Ŀ��ȣ���Χ��0~128
  * ��    ����Height ָ������ĸ߶ȣ���Χ��0~64
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	int16_t i, j;
	
	for (j = Y; j < Y + Height; j ++)		//����ָ��ҳ
	{
		for (i = X; i < X + Width; i ++)	//����ָ����
		{
			if (i >= 0 && i <= 127 && j >=0 && j <= 63)			//������Ļ�����ݲ���ʾ
			{
				OLED_DisplayBuf[j / 8][i] ^= 0x01 << (j % 8);	//���Դ�����ָ������ȡ��
			}
		}
	}
}

/**
  * ��    ����OLED��ʾһ���ַ�
  * ��    ����X ָ���ַ����Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���ַ����Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Char ָ��Ҫ��ʾ���ַ�����Χ��ASCII��ɼ��ַ�
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
					OLED_12X24		��12���أ���24����
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize)
{
	if (FontSize == OLED_8X16)		//����Ϊ��8���أ���16����
	{
		/*��ASCII��ģ��OLED_F8x16��ָ��������8*16��ͼ���ʽ��ʾ*/
		OLED_ShowImage(X, Y, 8, 16, OLED_F8x16[Char - ' ']);
	}
	else if(FontSize == OLED_6X8)	//����Ϊ��6���أ���8����
	{
		/*��ASCII��ģ��OLED_F6x8��ָ��������6*8��ͼ���ʽ��ʾ*/
		OLED_ShowImage(X, Y, 6, 8, OLED_F6x8[Char - ' ']);
	}
	else if(FontSize == OLED_12X24)	//����Ϊ��12���أ���24����
	{
		/*��ASCII��ģ��OLED_F12x24��ָ��������12*24��ͼ���ʽ��ʾ*/
		OLED_ShowImage(X, Y, 12, 24, OLED_F12x24[Char - ' ']);
	}
}

/**
  * ��    ����OLED��ʾ�ַ�����֧��ASCII������Ļ��д�룩
  * ��    ����X ָ���ַ������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���ַ������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����String ָ��Ҫ��ʾ���ַ�������Χ��ASCII��ɼ��ַ��������ַ���ɵ��ַ���
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
  * �� �� ֵ����
  * ˵    ������ʾ�������ַ���Ҫ��OLED_Data.c���OLED_CF16x16���鶨��
  *           δ�ҵ�ָ�������ַ�ʱ������ʾĬ��ͼ�Σ�һ�������ڲ�һ���ʺţ�
  *           �������СΪOLED_8X16ʱ�������ַ���16*16����������ʾ
  *           �������СΪOLED_6X8ʱ�������ַ���6*8������ʾ'?'
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize)
{
	uint16_t i = 0;
	char SingleChar[5];
	uint8_t CharLength = 0;
	uint16_t XOffset = 0;
	uint16_t pIndex;
	
	while (String[i] != '\0')	//�����ַ���
	{
		
#ifdef OLED_CHARSET_UTF8						//�����ַ���ΪUTF8
		/*�˶δ����Ŀ���ǣ���ȡUTF8�ַ����е�һ���ַ���ת�浽SingleChar���ַ�����*/
		/*�ж�UTF8�����һ���ֽڵı�־λ*/
		if ((String[i] & 0x80) == 0x00)			//��һ���ֽ�Ϊ0xxxxxxx
		{
			CharLength = 1;						//�ַ�Ϊ1�ֽ�
			SingleChar[0] = String[i ++];		//����һ���ֽ�д��SingleChar��0��λ�ã����iָ����һ���ֽ�
			SingleChar[1] = '\0';				//ΪSingleChar�����ַ���������־λ
		}
		else if ((String[i] & 0xE0) == 0xC0)	//��һ���ֽ�Ϊ110xxxxx
		{
			CharLength = 2;						//�ַ�Ϊ2�ֽ�
			SingleChar[0] = String[i ++];		//����һ���ֽ�д��SingleChar��0��λ�ã����iָ����һ���ֽ�
			if (String[i] == '\0') {break;}		//�������������ѭ����������ʾ
			SingleChar[1] = String[i ++];		//���ڶ����ֽ�д��SingleChar��1��λ�ã����iָ����һ���ֽ�
			SingleChar[2] = '\0';				//ΪSingleChar�����ַ���������־λ
		}
		else if ((String[i] & 0xF0) == 0xE0)	//��һ���ֽ�Ϊ1110xxxx
		{
			CharLength = 3;						//�ַ�Ϊ3�ֽ�
			SingleChar[0] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[1] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[2] = String[i ++];
			SingleChar[3] = '\0';
		}
		else if ((String[i] & 0xF8) == 0xF0)	//��һ���ֽ�Ϊ11110xxx
		{
			CharLength = 4;						//�ַ�Ϊ4�ֽ�
			SingleChar[0] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[1] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[2] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[3] = String[i ++];
			SingleChar[4] = '\0';
		}
		else
		{
			i ++;			//���������iָ����һ���ֽڣ����Դ��ֽڣ������ж���һ���ֽ�
			continue;
		}
#endif
		
#ifdef OLED_CHARSET_GB2312						//�����ַ���ΪGB2312
		/*�˶δ����Ŀ���ǣ���ȡGB2312�ַ����е�һ���ַ���ת�浽SingleChar���ַ�����*/
		/*�ж�GB2312�ֽڵ����λ��־λ*/
		if ((String[i] & 0x80) == 0x00)			//���λΪ0
		{
			CharLength = 1;						//�ַ�Ϊ1�ֽ�
			SingleChar[0] = String[i ++];		//����һ���ֽ�д��SingleChar��0��λ�ã����iָ����һ���ֽ�
			SingleChar[1] = '\0';				//ΪSingleChar�����ַ���������־λ
		}
		else									//���λΪ1
		{
			CharLength = 2;						//�ַ�Ϊ2�ֽ�
			SingleChar[0] = String[i ++];		//����һ���ֽ�д��SingleChar��0��λ�ã����iָ����һ���ֽ�
			if (String[i] == '\0') {break;}		//�������������ѭ����������ʾ
			SingleChar[1] = String[i ++];		//���ڶ����ֽ�д��SingleChar��1��λ�ã����iָ����һ���ֽ�
			SingleChar[2] = '\0';				//ΪSingleChar�����ַ���������־λ
		}
#endif
		
		/*��ʾ����������ȡ����SingleChar*/
		if (CharLength == 1)	//����ǵ��ֽ��ַ�
		{
			/*ʹ��OLED_ShowChar��ʾ���ַ�*/
			OLED_ShowChar(X + XOffset, Y, SingleChar[0], FontSize);
			XOffset += FontSize;
		}
		else					//���򣬼����ֽ��ַ�
		{
			/*����������ģ�⣬����ģ����Ѱ�Ҵ��ַ�������*/
			/*����ҵ����һ���ַ�������Ϊ���ַ����������ʾ�ַ�δ����ģ�ⶨ�壬ֹͣѰ��*/
			for (pIndex = 0; strcmp(OLED_CF16x16[pIndex].Index, "") != 0; pIndex ++)
			{
				/*�ҵ�ƥ����ַ�*/
				if (strcmp(OLED_CF16x16[pIndex].Index, SingleChar) == 0)
				{
					break;		//����ѭ������ʱpIndex��ֵΪָ���ַ�������
				}
			}
			if (FontSize == OLED_8X16)		//��������Ϊ8*16����
			{
				/*����ģ��OLED_CF16x16��ָ��������16*16��ͼ���ʽ��ʾ*/
				OLED_ShowImage(X + XOffset, Y, 16, 16, OLED_CF16x16[pIndex].Data);
				XOffset += 16;
			}
			else if (FontSize == OLED_6X8)	//��������Ϊ6*8����
			{
				/*�ռ䲻�㣬��λ����ʾ'?'*/
				OLED_ShowChar(X + XOffset, Y, '?', OLED_6X8);
				XOffset += OLED_6X8;
			}
		}
	}
}

/**
  * ��    ����OLED��ʾ���֣�ʮ���ƣ���������
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Number ָ��Ҫ��ʾ�����֣���Χ��0~4294967295
  * ��    ����Length ָ�����ֵĳ��ȣ���Χ��0~10
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//�������ֵ�ÿһλ							
	{
		/*����OLED_ShowChar������������ʾÿ������*/
		/*Number / OLED_Pow(10, Length - i - 1) % 10 ����ʮ������ȡ���ֵ�ÿһλ*/
		/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * ��    ����OLED��ʾ�з������֣�ʮ���ƣ�������
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Number ָ��Ҫ��ʾ�����֣���Χ��-2147483648~2147483647
  * ��    ����Length ָ�����ֵĳ��ȣ���Χ��0~10
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	uint32_t Number1;
	
	if (Number >= 0)						//���ִ��ڵ���0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//��ʾ+��
		Number1 = Number;					//Number1ֱ�ӵ���Number
	}
	else									//����С��0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//��ʾ-��
		Number1 = -Number;					//Number1����Numberȡ��
	}
	
	for (i = 0; i < Length; i++)			//�������ֵ�ÿһλ								
	{
		/*����OLED_ShowChar������������ʾÿ������*/
		/*Number1 / OLED_Pow(10, Length - i - 1) % 10 ����ʮ������ȡ���ֵ�ÿһλ*/
		/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
		OLED_ShowChar(X + (i + 1) * FontSize, Y, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * ��    ����OLED��ʾʮ���������֣�ʮ�����ƣ���������
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Number ָ��Ҫ��ʾ�����֣���Χ��0x00000000~0xFFFFFFFF
  * ��    ����Length ָ�����ֵĳ��ȣ���Χ��0~8
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)		//�������ֵ�ÿһλ
	{
		/*��ʮ��������ȡ���ֵ�ÿһλ*/
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		
		if (SingleNumber < 10)			//��������С��10
		{
			/*����OLED_ShowChar��������ʾ������*/
			/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber + '0', FontSize);
		}
		else							//�������ִ���10
		{
			/*����OLED_ShowChar��������ʾ������*/
			/*+ 'A' �ɽ�����ת��Ϊ��A��ʼ��ʮ�������ַ�*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber - 10 + 'A', FontSize);
		}
	}
}

/**
  * ��    ����OLED��ʾ���������֣������ƣ���������
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Number ָ��Ҫ��ʾ�����֣���Χ��0x00000000~0xFFFFFFFF
  * ��    ����Length ָ�����ֵĳ��ȣ���Χ��0~16
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//�������ֵ�ÿһλ	
	{
		/*����OLED_ShowChar������������ʾÿ������*/
		/*Number / OLED_Pow(2, Length - i - 1) % 2 ���Զ�������ȡ���ֵ�ÿһλ*/
		/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(2, Length - i - 1) % 2 + '0', FontSize);
	}
}

/**
  * ��    ����OLED��ʾ�������֣�ʮ���ƣ�С����
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Number ָ��Ҫ��ʾ�����֣���Χ��-4294967295.0~4294967295.0
  * ��    ����IntLength ָ�����ֵ�����λ���ȣ���Χ��0~10
  * ��    ����FraLength ָ�����ֵ�С��λ���ȣ���Χ��0~9��С����������������ʾ
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
	uint32_t PowNum, IntNum, FraNum;
	
	if (Number >= 0)						//���ִ��ڵ���0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//��ʾ+��
	}
	else									//����С��0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//��ʾ-��
		Number = -Number;					//Numberȡ��
	}
	
	/*��ȡ�������ֺ�С������*/
	IntNum = Number;						//ֱ�Ӹ�ֵ�����ͱ�������ȡ����
	Number -= IntNum;						//��Number��������������ֹ֮��С���˵�����ʱ����������ɴ���
	PowNum = OLED_Pow(10, FraLength);		//����ָ��С����λ����ȷ������
	FraNum = round(Number * PowNum);		//��С���˵�������ͬʱ�������룬������ʾ���
	IntNum += FraNum / PowNum;				//��������������˽�λ������Ҫ�ټӸ�����
	
	/*��ʾ��������*/
	OLED_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);
	
	/*��ʾС����*/
	OLED_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);
	
	/*��ʾС������*/
	OLED_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

/**
  * ��    ����OLED��ʾͼ��
  * ��    ����X ָ��ͼ�����Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ��ͼ�����Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Width ָ��ͼ��Ŀ��ȣ���Χ��0~128
  * ��    ����Height ָ��ͼ��ĸ߶ȣ���Χ��0~64
  * ��    ����Image ָ��Ҫ��ʾ��ͼ��
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	uint8_t i = 0, j = 0;
	int16_t Page, Shift;
	
	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);
	
	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j ++)
	{
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i ++)
		{
			if (X + i >= 0 && X + i <= 127)		//������Ļ�����ݲ���ʾ
			{
				/*���������ڼ���ҳ��ַ����λʱ��Ҫ��һ��ƫ��*/
				Page = Y / 8;
				Shift = Y % 8;
				if (Y < 0)
				{
					Page -= 1;
					Shift += 8;
				}
				
				if (Page + j >= 0 && Page + j <= 7)		//������Ļ�����ݲ���ʾ
				{
					/*��ʾͼ���ڵ�ǰҳ������*/
					OLED_DisplayBuf[Page + j][X + i] |= Image[j * Width + i] << (Shift);
				}
				
				if (Page + j + 1 >= 0 && Page + j + 1 <= 7)		//������Ļ�����ݲ���ʾ
				{					
					/*��ʾͼ������һҳ������*/
					OLED_DisplayBuf[Page + j + 1][X + i] |= Image[j * Width + i] >> (8 - Shift);
				}
			}
		}
	}
}

/**
  * ��    ����OLEDʹ��printf������ӡ��ʽ���ַ�����֧��ASCII������Ļ��д�룩
  * ��    ����X ָ����ʽ���ַ������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ����ʽ���ַ������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����FontSize ָ�������С
  *           ��Χ��OLED_8X16		��8���أ���16����
  *                 OLED_6X8		��6���أ���8����
  * ��    ����format ָ��Ҫ��ʾ�ĸ�ʽ���ַ�������Χ��ASCII��ɼ��ַ��������ַ���ɵ��ַ���
  * ��    ����... ��ʽ���ַ��������б�
  * �� �� ֵ����
  * ˵    ������ʾ�������ַ���Ҫ��OLED_Data.c���OLED_CF16x16���鶨��
  *           δ�ҵ�ָ�������ַ�ʱ������ʾĬ��ͼ�Σ�һ�������ڲ�һ���ʺţ�
  *           �������СΪOLED_8X16ʱ�������ַ���16*16����������ʾ
  *           �������СΪOLED_6X8ʱ�������ַ���6*8������ʾ'?'
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...)
{
	char String[256];						//�����ַ�����
	va_list arg;							//����ɱ�����б��������͵ı���arg
	va_start(arg, format);					//��format��ʼ�����ղ����б���arg����
	vsprintf(String, format, arg);			//ʹ��vsprintf��ӡ��ʽ���ַ����Ͳ����б����ַ�������
	va_end(arg);							//��������arg
	OLED_ShowString(X, Y, String, FontSize);//OLED��ʾ�ַ����飨�ַ�����
}

/**
  * ��    ����OLED��ָ��λ�û�һ����
  * ��    ����X ָ����ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ����������꣬��Χ��-32768~32767����Ļ����0~63
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_DrawPoint(int16_t X, int16_t Y)
{
	if (X >= 0 && X <= 127 && Y >=0 && Y <= 63)		//������Ļ�����ݲ���ʾ
	{
		/*���Դ�����ָ��λ�õ�һ��Bit������1*/
		OLED_DisplayBuf[Y / 8][X] |= 0x01 << (Y % 8);
	}
}

/**
  * ��    ����OLED��ȡָ��λ�õ��ֵ
  * ��    ����X ָ����ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ����������꣬��Χ��-32768~32767����Ļ����0~63
  * �� �� ֵ��ָ��λ�õ��Ƿ��ڵ���״̬��1��������0��Ϩ��
  */
uint8_t OLED_GetPoint(int16_t X, int16_t Y)
{
	if (X >= 0 && X <= 127 && Y >=0 && Y <= 63)		//������Ļ�����ݲ���ȡ
	{
		/*�ж�ָ��λ�õ�����*/
		if (OLED_DisplayBuf[Y / 8][X] & 0x01 << (Y % 8))
		{
			return 1;	//Ϊ1������1
		}
	}
	
	return 0;		//���򣬷���0
}

/**
  * ��    ����OLED����
  * ��    ����X0 ָ��һ���˵�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y0 ָ��һ���˵�������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����X1 ָ����һ���˵�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y1 ָ����һ���˵�������꣬��Χ��-32768~32767����Ļ����0~63
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1)
{
	int16_t x, y, dx, dy, d, incrE, incrNE, temp;
	int16_t x0 = X0, y0 = Y0, x1 = X1, y1 = Y1;
	uint8_t yflag = 0, xyflag = 0;
	
	if (y0 == y1)		//���ߵ�������
	{
		/*0�ŵ�X�������1�ŵ�X���꣬�򽻻�����X����*/
		if (x0 > x1) {temp = x0; x0 = x1; x1 = temp;}
		
		/*����X����*/
		for (x = x0; x <= x1; x ++)
		{
			OLED_DrawPoint(x, y0);	//���λ���
		}
	}
	else if (x0 == x1)	//���ߵ�������
	{
		/*0�ŵ�Y�������1�ŵ�Y���꣬�򽻻�����Y����*/
		if (y0 > y1) {temp = y0; y0 = y1; y1 = temp;}
		
		/*����Y����*/
		for (y = y0; y <= y1; y ++)
		{
			OLED_DrawPoint(x0, y);	//���λ���
		}
	}
	else				//б��
	{
		/*ʹ��Bresenham�㷨��ֱ�ߣ����Ա����ʱ�ĸ������㣬Ч�ʸ���*/
		/*�ο��ĵ���https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
		/*�ο��̳̣�https://www.bilibili.com/video/BV1364y1d7Lo*/
		
		if (x0 > x1)	//0�ŵ�X�������1�ŵ�X����
		{
			/*������������*/
			/*������Ӱ�컭�ߣ����ǻ��߷����ɵ�һ���������������ޱ�Ϊ��һ��������*/
			temp = x0; x0 = x1; x1 = temp;
			temp = y0; y0 = y1; y1 = temp;
		}
		
		if (y0 > y1)	//0�ŵ�Y�������1�ŵ�Y����
		{
			/*��Y����ȡ��*/
			/*ȡ����Ӱ�컭�ߣ����ǻ��߷����ɵ�һ�������ޱ�Ϊ��һ����*/
			y0 = -y0;
			y1 = -y1;
			
			/*�ñ�־λyflag����ס��ǰ�任���ں���ʵ�ʻ���ʱ���ٽ����껻����*/
			yflag = 1;
		}
		
		if (y1 - y0 > x1 - x0)	//����б�ʴ���1
		{
			/*��X������Y���껥��*/
			/*������Ӱ�컭�ߣ����ǻ��߷����ɵ�һ����0~90�ȷ�Χ��Ϊ��һ����0~45�ȷ�Χ*/
			temp = x0; x0 = y0; y0 = temp;
			temp = x1; x1 = y1; y1 = temp;
			
			/*�ñ�־λxyflag����ס��ǰ�任���ں���ʵ�ʻ���ʱ���ٽ����껻����*/
			xyflag = 1;
		}
		
		/*����ΪBresenham�㷨��ֱ��*/
		/*�㷨Ҫ�󣬻��߷������Ϊ��һ����0~45�ȷ�Χ*/
		dx = x1 - x0;
		dy = y1 - y0;
		incrE = 2 * dy;
		incrNE = 2 * (dy - dx);
		d = 2 * dy - dx;
		x = x0;
		y = y0;
		
		/*����ʼ�㣬ͬʱ�жϱ�־λ�������껻����*/
		if (yflag && xyflag){OLED_DrawPoint(y, -x);}
		else if (yflag)		{OLED_DrawPoint(x, -y);}
		else if (xyflag)	{OLED_DrawPoint(y, x);}
		else				{OLED_DrawPoint(x, y);}
		
		while (x < x1)		//����X���ÿ����
		{
			x ++;
			if (d < 0)		//��һ�����ڵ�ǰ�㶫��
			{
				d += incrE;
			}
			else			//��һ�����ڵ�ǰ�㶫����
			{
				y ++;
				d += incrNE;
			}
			
			/*��ÿһ���㣬ͬʱ�жϱ�־λ�������껻����*/
			if (yflag && xyflag){OLED_DrawPoint(y, -x);}
			else if (yflag)		{OLED_DrawPoint(x, -y);}
			else if (xyflag)	{OLED_DrawPoint(y, x);}
			else				{OLED_DrawPoint(x, y);}
		}	
	}
}

/**
  * ��    ����OLED����
  * ��    ����X ָ���������Ͻǵĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ���������Ͻǵ������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Width ָ�����εĿ��ȣ���Χ��0~128
  * ��    ����Height ָ�����εĸ߶ȣ���Χ��0~64
  * ��    ����IsFilled ָ�������Ƿ����
  *           ��Χ��OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled)
{
	int16_t i, j;
	if (!IsFilled)		//ָ�����β����
	{
		/*��������X���꣬����������������*/
		for (i = X; i < X + Width; i ++)
		{
			OLED_DrawPoint(i, Y);
			OLED_DrawPoint(i, Y + Height - 1);
		}
		/*��������Y���꣬����������������*/
		for (i = Y; i < Y + Height; i ++)
		{
			OLED_DrawPoint(X, i);
			OLED_DrawPoint(X + Width - 1, i);
		}
	}
	else				//ָ���������
	{
		/*����X����*/
		for (i = X; i < X + Width; i ++)
		{
			/*����Y����*/
			for (j = Y; j < Y + Height; j ++)
			{
				/*��ָ�����򻭵㣬���������*/
				OLED_DrawPoint(i, j);
			}
		}
	}
}

/**
  * ��    ����OLED������
  * ��    ����X0 ָ����һ���˵�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y0 ָ����һ���˵�������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����X1 ָ���ڶ����˵�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y1 ָ���ڶ����˵�������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����X2 ָ���������˵�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y2 ָ���������˵�������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����IsFilled ָ���������Ƿ����
  *           ��Χ��OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled)
{
	int16_t minx = X0, miny = Y0, maxx = X0, maxy = Y0;
	int16_t i, j;
	int16_t vx[] = {X0, X1, X2};
	int16_t vy[] = {Y0, Y1, Y2};
	
	if (!IsFilled)			//ָ�������β����
	{
		/*���û��ߺ���������������ֱ������*/
		OLED_DrawLine(X0, Y0, X1, Y1);
		OLED_DrawLine(X0, Y0, X2, Y2);
		OLED_DrawLine(X1, Y1, X2, Y2);
	}
	else					//ָ�����������
	{
		/*�ҵ���������С��X��Y����*/
		if (X1 < minx) {minx = X1;}
		if (X2 < minx) {minx = X2;}
		if (Y1 < miny) {miny = Y1;}
		if (Y2 < miny) {miny = Y2;}
		
		/*�ҵ�����������X��Y����*/
		if (X1 > maxx) {maxx = X1;}
		if (X2 > maxx) {maxx = X2;}
		if (Y1 > maxy) {maxy = Y1;}
		if (Y2 > maxy) {maxy = Y2;}
		
		/*��С�������֮��ľ���Ϊ������Ҫ��������*/
		/*���������������еĵ�*/
		/*����X����*/		
		for (i = minx; i <= maxx; i ++)
		{
			/*����Y����*/	
			for (j = miny; j <= maxy; j ++)
			{
				/*����OLED_pnpoly���ж�ָ�����Ƿ���ָ��������֮��*/
				/*����ڣ��򻭵㣬������ڣ���������*/
				if (OLED_pnpoly(3, vx, vy, i, j)) {OLED_DrawPoint(i, j);}
			}
		}
	}
}

/**
  * ��    ����OLED��Բ
  * ��    ����X ָ��Բ��Բ�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ��Բ��Բ�������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Radius ָ��Բ�İ뾶����Χ��0~255
  * ��    ����IsFilled ָ��Բ�Ƿ����
  *           ��Χ��OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled)
{
	int16_t x, y, d, j;
	
	/*ʹ��Bresenham�㷨��Բ�����Ա����ʱ�ĸ������㣬Ч�ʸ���*/
	/*�ο��ĵ���https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
	/*�ο��̳̣�https://www.bilibili.com/video/BV1VM4y1u7wJ*/
	
	d = 1 - Radius;
	x = 0;
	y = Radius;
	
	/*��ÿ���˷�֮һԲ������ʼ��*/
	OLED_DrawPoint(X + x, Y + y);
	OLED_DrawPoint(X - x, Y - y);
	OLED_DrawPoint(X + y, Y + x);
	OLED_DrawPoint(X - y, Y - x);
	
	if (IsFilled)		//ָ��Բ���
	{
		/*������ʼ��Y����*/
		for (j = -y; j < y; j ++)
		{
			/*��ָ�����򻭵㣬��䲿��Բ*/
			OLED_DrawPoint(X, Y + j);
		}
	}
	
	while (x < y)		//����X���ÿ����
	{
		x ++;
		if (d < 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d += 2 * x + 1;
		}
		else			//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			y --;
			d += 2 * (x - y) + 1;
		}
		
		/*��ÿ���˷�֮һԲ���ĵ�*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X + y, Y + x);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - y, Y - x);
		OLED_DrawPoint(X + x, Y - y);
		OLED_DrawPoint(X + y, Y - x);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X - y, Y + x);
		
		if (IsFilled)	//ָ��Բ���
		{
			/*�����м䲿��*/
			for (j = -y; j < y; j ++)
			{
				/*��ָ�����򻭵㣬��䲿��Բ*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
			
			/*�������ಿ��*/
			for (j = -x; j < x; j ++)
			{
				/*��ָ�����򻭵㣬��䲿��Բ*/
				OLED_DrawPoint(X - y, Y + j);
				OLED_DrawPoint(X + y, Y + j);
			}
		}
	}
}

/**
  * ��    ����OLED����Բ
  * ��    ����X ָ����Բ��Բ�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ����Բ��Բ�������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����A ָ����Բ�ĺ�����᳤�ȣ���Χ��0~255
  * ��    ����B ָ����Բ��������᳤�ȣ���Χ��0~255
  * ��    ����IsFilled ָ����Բ�Ƿ����
  *           ��Χ��OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled)
{
	int16_t x, y, j;
	int16_t a = A, b = B;
	float d1, d2;
	
	/*ʹ��Bresenham�㷨����Բ�����Ա��ⲿ�ֺ�ʱ�ĸ������㣬Ч�ʸ���*/
	/*�ο����ӣ�https://blog.csdn.net/myf_666/article/details/128167392*/
	
	x = 0;
	y = b;
	d1 = b * b + a * a * (-b + 0.5);
	
	if (IsFilled)	//ָ����Բ���
	{
		/*������ʼ��Y����*/
		for (j = -y; j < y; j ++)
		{
			/*��ָ�����򻭵㣬��䲿����Բ*/
			OLED_DrawPoint(X, Y + j);
			OLED_DrawPoint(X, Y + j);
		}
	}
	
	/*����Բ������ʼ��*/
	OLED_DrawPoint(X + x, Y + y);
	OLED_DrawPoint(X - x, Y - y);
	OLED_DrawPoint(X - x, Y + y);
	OLED_DrawPoint(X + x, Y - y);
	
	/*����Բ�м䲿��*/
	while (b * b * (x + 1) < a * a * (y - 0.5))
	{
		if (d1 <= 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d1 += b * b * (2 * x + 3);
		}
		else				//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
			y --;
		}
		x ++;
		
		if (IsFilled)	//ָ����Բ���
		{
			/*�����м䲿��*/
			for (j = -y; j < y; j ++)
			{
				/*��ָ�����򻭵㣬��䲿����Բ*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
		}
		
		/*����Բ�м䲿��Բ��*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X + x, Y - y);
	}
	
	/*����Բ���ಿ��*/
	d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;
	
	while (y > 0)
	{
		if (d2 <= 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d2 += b * b * (2 * x + 2) + a * a * (-2 * y + 3);
			x ++;
			
		}
		else				//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			d2 += a * a * (-2 * y + 3);
		}
		y --;
		
		if (IsFilled)	//ָ����Բ���
		{
			/*�������ಿ��*/
			for (j = -y; j < y; j ++)
			{
				/*��ָ�����򻭵㣬��䲿����Բ*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
		}
		
		/*����Բ���ಿ��Բ��*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X + x, Y - y);
	}
}

/**
  * ��    ����OLED��Բ��
  * ��    ����X ָ��Բ����Բ�ĺ����꣬��Χ��-32768~32767����Ļ����0~127
  * ��    ����Y ָ��Բ����Բ�������꣬��Χ��-32768~32767����Ļ����0~63
  * ��    ����Radius ָ��Բ���İ뾶����Χ��0~255
  * ��    ����StartAngle ָ��Բ������ʼ�Ƕȣ���Χ��-180~180
  *           ˮƽ����Ϊ0�ȣ�ˮƽ����Ϊ180�Ȼ�-180�ȣ��·�Ϊ�������Ϸ�Ϊ������˳ʱ����ת
  * ��    ����EndAngle ָ��Բ������ֹ�Ƕȣ���Χ��-180~180
  *           ˮƽ����Ϊ0�ȣ�ˮƽ����Ϊ180�Ȼ�-180�ȣ��·�Ϊ�������Ϸ�Ϊ������˳ʱ����ת
  * ��    ����IsFilled ָ��Բ���Ƿ���䣬����Ϊ����
  *           ��Χ��OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ����
  * ˵    �������ô˺�����Ҫ�������س�������Ļ�ϣ�������ø��º���
  */
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled)
{
	int16_t x, y, d, j;
	
	/*�˺�������Bresenham�㷨��Բ�ķ���*/
	
	d = 1 - Radius;
	x = 0;
	y = Radius;
	
	/*�ڻ�Բ��ÿ����ʱ���ж�ָ�����Ƿ���ָ���Ƕ��ڣ��ڣ��򻭵㣬���ڣ���������*/
	if (OLED_IsInAngle(x, y, StartAngle, EndAngle))	{OLED_DrawPoint(X + x, Y + y);}
	if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y - y);}
	if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y + x);}
	if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y - x);}
	
	if (IsFilled)	//ָ��Բ�����
	{
		/*������ʼ��Y����*/
		for (j = -y; j < y; j ++)
		{
			/*�����Բ��ÿ����ʱ���ж�ָ�����Ƿ���ָ���Ƕ��ڣ��ڣ��򻭵㣬���ڣ���������*/
			if (OLED_IsInAngle(0, j, StartAngle, EndAngle)) {OLED_DrawPoint(X, Y + j);}
		}
	}
	
	while (x < y)		//����X���ÿ����
	{
		x ++;
		if (d < 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d += 2 * x + 1;
		}
		else			//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			y --;
			d += 2 * (x - y) + 1;
		}
		
		/*�ڻ�Բ��ÿ����ʱ���ж�ָ�����Ƿ���ָ���Ƕ��ڣ��ڣ��򻭵㣬���ڣ���������*/
		if (OLED_IsInAngle(x, y, StartAngle, EndAngle)) {OLED_DrawPoint(X + x, Y + y);}
		if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y + x);}
		if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y - y);}
		if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y - x);}
		if (OLED_IsInAngle(x, -y, StartAngle, EndAngle)) {OLED_DrawPoint(X + x, Y - y);}
		if (OLED_IsInAngle(y, -x, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y - x);}
		if (OLED_IsInAngle(-x, y, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y + y);}
		if (OLED_IsInAngle(-y, x, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y + x);}
		
		if (IsFilled)	//ָ��Բ�����
		{
			/*�����м䲿��*/
			for (j = -y; j < y; j ++)
			{
				/*�����Բ��ÿ����ʱ���ж�ָ�����Ƿ���ָ���Ƕ��ڣ��ڣ��򻭵㣬���ڣ���������*/
				if (OLED_IsInAngle(x, j, StartAngle, EndAngle)) {OLED_DrawPoint(X + x, Y + j);}
				if (OLED_IsInAngle(-x, j, StartAngle, EndAngle)) {OLED_DrawPoint(X - x, Y + j);}
			}
			
			/*�������ಿ��*/
			for (j = -x; j < x; j ++)
			{
				/*�����Բ��ÿ����ʱ���ж�ָ�����Ƿ���ָ���Ƕ��ڣ��ڣ��򻭵㣬���ڣ���������*/
				if (OLED_IsInAngle(-y, j, StartAngle, EndAngle)) {OLED_DrawPoint(X - y, Y + j);}
				if (OLED_IsInAngle(y, j, StartAngle, EndAngle)) {OLED_DrawPoint(X + y, Y + j);}
			}
		}
	}
}

/*********************���ܺ���*/


/*****************��Э�Ƽ�|��Ȩ����****************/
/*****************jiangxiekeji.com*****************/
