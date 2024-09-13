/************************************************************************************
*  Copyright (c), 2014, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: OLED_I2C.c
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
*
* Author : С��
* Version: 1.00
* Date   : 2014.4.8
* Email  : hello14blog@gmail.com
* Modification: none
*
* Description:128*64�����OLED��ʾ�������ļ����������ڻ����Զ���(heltec.taobao.com)��SD1306����IICͨ�ŷ�ʽ��ʾ��
*
* Others: none;
*
* Function List:
*	1. void I2C_Configuration(void) -- ����CPU��Ӳ��I2C
* 2. void I2C_WriteByte(uint8_t addr,uint8_t data) -- ��Ĵ�����ַдһ��byte������
* 3. void WriteCmd(unsigned char I2C_Command) -- д����
* 4. void WriteDat(unsigned char I2C_Data) -- д����
* 5. void OLED_Init(void) -- OLED����ʼ��
* 6. void OLED_SetPos(unsigned char x, unsigned char y) -- ������ʼ������
* 7. void OLED_Fill(unsigned char fill_Data) -- ȫ�����
* 8. void OLED_CLS(void) -- ����
* 9. void OLED_ON(void) -- ����
* 10. void OLED_OFF(void) -- ˯��
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- ��ʾ�ַ���(�����С��6*8��8*16����)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- ��ʾ����(������Ҫ��ȡģ��Ȼ��ŵ�codetab.h��)
* 13. void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]) -- BMPͼƬ
*
* History: none;
*
*************************************************************************************/

#include "OLED_I2C.h"
#include "delay.h"
#include "codetab.h"

void I2C_Configuration(void)
{


    /* '????? */
    rcu_periph_clock_enable(RCU_SCL);
    rcu_periph_clock_enable(RCU_SDA);

#if		SOFTWARE_OR_HARDWARE

    /* ????SCL????g? */
    gpio_mode_set(PORT_SCL,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SCL);
    /* ???????�??? 50MHZ */
    gpio_output_options_set(PORT_SCL,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SCL);

    /* ????SDA????g? */
    gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA);
    /* ???????�??? 50MHZ */
    gpio_output_options_set(PORT_SDA,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SDA);

#else

    /* ????SCL???�???IIC???? */
    gpio_af_set(PORT_SCL, IIC_AF, GPIO_SCL);
    /* ????SDA???�???IIC???? */
    gpio_af_set(PORT_SDA, IIC_AF, GPIO_SDA);
    /* ????SCL???? */
    gpio_mode_set(PORT_SCL, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SCL);
    gpio_output_options_set(PORT_SCL, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SCL);

    /* ????SDA???? */
    gpio_mode_set(PORT_SDA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SDA);
    gpio_output_options_set(PORT_SDA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SDA);

    /* '??IIC??? */
    rcu_periph_clock_enable(RCU_IIC);
    /* IIC??? */
    i2c_deinit(I2C0);
    /* ????IIC???? */
    i2c_clock_config(BSP_IIC, 400000, I2C_DTCY_2);
    /* ????I2C??? */
    i2c_mode_addr_config(BSP_IIC, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, IIC_ADDR);
    /* '??I2C */
    i2c_enable(BSP_IIC);
    /* '??????? */
    i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
#endif


}


void I2C_WriteByte(uint32_t addr, uint8_t data)
{

    // ������ʼ�ź�
    i2c_start_on_bus(BSP_IIC);

    // �ȴ���ʼ�źŷ������ (SBSEND ��־λ��λ)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_SBSEND));

    // �����豸��ַ��д����
    i2c_master_addressing(BSP_IIC, 0x78, I2C_TRANSMITTER);

    // �ȴ���ַ������� (ADDSEND ��־λ��λ)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_ADDSEND));

    // �����ַ���ͱ�־λ
    i2c_flag_clear(BSP_IIC, I2C_FLAG_ADDSEND);

    // �ȴ����ͻ�����Ϊ�� (TBE ��־λ��λ)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_TBE));

    // ���������ֽ�
    i2c_data_transmit(BSP_IIC, addr);

    // �ȴ��ֽڴ������ (BTC ��־λ��λ)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_BTC));
    // ���������ֽ�
    i2c_data_transmit(BSP_IIC, data);

    // �ȴ��ֽڴ������ (BTC ��־λ��λ)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_BTC));

    // ����ֹͣ�ź�
    i2c_stop_on_bus(BSP_IIC);

    //  while (I2C_CTL0(I2C1) & 0x0200);


}


void WriteCmd(uint8_t I2C_Command)//д����
{
    I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(uint8_t I2C_Data)//д����
{
    I2C_WriteByte(0x40, I2C_Data);
}

void OLED_Init(void)
{
    //�������ʱ����Ҫ
    DelayMs(100);
    WriteCmd(0xAE); //display off
    WriteCmd(0x02);	//Set Memory Addressing Mode
    WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0x40);	//Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xB0);	//Set COM Output Scan Direction
    WriteCmd(0x81); //---set low column address
    WriteCmd(0xCF); //---set high column address
    WriteCmd(0xA1); //--set start line address
    WriteCmd(0xC8); //--set contrast control register
    WriteCmd(0xAF); //���ȵ��� 0x00~0xff
    WriteCmd(0xA7); //--set segment re-map 0 to 127
    WriteCmd(0xA8); //--set normal display
    WriteCmd(0x3F); //--set multiplex ratio(1 to 64)
    WriteCmd(0xD3); //
    WriteCmd(0x00); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xD5); //-set display offset
    WriteCmd(0x80); //-not offset
    WriteCmd(0xD9); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xF1); //--set divide ratio
    WriteCmd(0xDA); //--set pre-charge period
    WriteCmd(0x12); //
    WriteCmd(0xDB); //--set com pins hardware configuration
    WriteCmd(0x40);
    WriteCmd(0xA4); //--set vcomh
    WriteCmd(0xA6); //0x20,0.77xVcc
    WriteCmd(0xAF); //--set DC-DC enable

}

void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{
    WriteCmd(0xb0+y);
    WriteCmd((((x+2&0xf0)>>4)|0x10));
    WriteCmd((x+2&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//ȫ�����
{
    unsigned char m,n;
    for(m=0; m<8; m++)
    {
        WriteCmd(0xb0+m);		//page0-page1
        WriteCmd(0x02);		//low column start address
        WriteCmd(0x10);		//high column start address
        for(n=0; n<128; n++)
        {
            WriteDat(fill_Data);
        }
    }
}

void OLED_CLS(void)//����
{
    OLED_Fill(0x02);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          :
// Parameters     : none
// Description    : ��OLED�������л���
//--------------------------------------------------------------
void OLED_ON(void)
{
    WriteCmd(0X8D);  //���õ�ɱ�
    WriteCmd(0X14);  //������ɱ�
    WriteCmd(0XAF);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          :
// Parameters     : none
// Description    : ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
    WriteCmd(0X8D);  //���õ�ɱ�
    WriteCmd(0X10);  //�رյ�ɱ�
    WriteCmd(0XAE);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          :
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); ch[] -- Ҫ��ʾ���ַ���; TextSize -- �ַ���С(1:6*8 ; 2:8*16)
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
    unsigned char c = 0,i = 0,j = 0;
    switch(TextSize)
    {
    case 1:
    {
        while(ch[j] != '\0')
        {
            c = ch[j] - 32;
            if(x > 126)
            {
                x = 0;
                y++;
            }
            OLED_SetPos(x,y);
            for(i=0; i<6; i++)
                WriteDat(F6x8[c][i]);
            x += 6;
            j++;
        }
    }
    break;
    case 2:
    {
        while(ch[j] != '\0')
        {
            c = ch[j] - 32;
            if(x > 120)
            {
                x = 0;
                y++;
            }
            OLED_SetPos(x,y);
            for(i=0; i<8; i++)
                WriteDat(F8X16[c*16+i]);
            OLED_SetPos(x,y+1);
            for(i=0; i<8; i++)
                WriteDat(F8X16[c*16+i+8]);
            x += 8;
            j++;
        }
    }
    break;
    }
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          :
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
// Description    : ��ʾcodetab.h�еĺ���,16*16����
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned char wm=0;
    unsigned int  adder=32*N;
    OLED_SetPos(x, y);
    for(wm = 0; wm < 16; wm++)
    {
        WriteDat(F16x16[adder]);
        adder += 1;
    }
    OLED_SetPos(x,y + 1);
    for(wm = 0; wm < 16; wm++)
    {
        WriteDat(F16x16[adder]);
        adder += 1;
    }
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          :
// Parameters     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
    unsigned int j=0;
    unsigned char x,y;

    if(y1%8==0)
        y = y1/8;
    else
        y = y1/8 + 1;
    for(y=y0; y<y1; y++)
    {
        OLED_SetPos(x0-1,y);
        for(x=x0+1; x<x1; x++)
        {
            WriteDat(BMP[j++]);
        }
    }
}
void AHT10Init(void)   //��ʼ��AHT10
{
    DelayMs(50);
    //������ʼ�ź�
    i2c_start_on_bus(BSP_IIC);
    //�ȴ�SBSEND��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //����������ַ��д����
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //�ȴ�ADDSEND��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
    //���ADDSEND��־λ
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //�ȴ�TBE��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
    //��������
    i2c_data_transmit (BSP_IIC, 0xe1);
    //�ȴ�BTC��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x08);
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x00);
    //�ȴ�BTC��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    //����ֹͣ�ź�
    i2c_stop_on_bus(BSP_IIC);
    DelayMs(50);
}

uint8_t AHT10Check(void)
{
    uint8_t ack=0;

    //������ʼ�ź�
    i2c_start_on_bus(BSP_IIC);
    //�ȴ�SBSEND��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //����������ַ��д����
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //�ȴ�ADDSEND��־λ��һ
    ack=i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND);
    //���ADDSEND��־λ
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
    //����ֹͣ�ź�
    i2c_stop_on_bus(BSP_IIC);
    return ack;
}

/**
brief AHT10��λ
param NONE
return NONE
*/
void AHT10Reset(void)
{

    //������ʼ�ź�
    i2c_start_on_bus(BSP_IIC);
    //�ȴ�SBSEND��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //����������ַ��д����
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //�ȴ�ADDSEND��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
    //���ADDSEND��־λ
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //�ȴ�TBE��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
    //��������
    i2c_data_transmit (BSP_IIC, 0xba);

    //�ȴ�BTC��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    //����ֹͣ�ź�
    i2c_stop_on_bus(BSP_IIC);


}
void AHT10ReadData(uint32_t *ct)
{
    volatile uint8_t  Byte_1th=0;
    volatile uint8_t  Byte_2th=0;
    volatile uint8_t  Byte_3th=0;
    volatile uint8_t  Byte_4th=0;
    volatile uint8_t  Byte_5th=0;
    volatile uint8_t  Byte_6th=0;
    uint32_t RetuData = 0;
    uint16_t cnt = 0;

    uint8_t ack;
    uint32_t SRH=0,ST=0;
    uint8_t databuff[6];

    unsigned int timeout = 0;
    i2c_start_on_bus(BSP_IIC);
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //����������ַ��д����
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //�ȴ�ADDSEND��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
    //���ADDSEND��־λ
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //�ȴ�TBE��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
    //��������
    i2c_data_transmit (BSP_IIC, 0xac);
    //�ȴ�BTC��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x33);
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x00);
    //�ȴ�BTC��־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    //����ֹͣ�ź�
    i2c_stop_on_bus(BSP_IIC);
again:
    DelayMs(80);
    //��ʱһ��ȴ����ݶ���
    //������ʼ�ź�
    i2c_start_on_bus(BSP_IIC);
    //�ȴ���ʼ�źŷ�����ɱ�־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //����������ַ�Ӷ�����
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_RECEIVER);
    //�ȴ�������ַ���ͳɹ���־λ��һ

    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) )
    {
        DelayMs(1);
        timeout++;
        if( timeout > 10 )//�ȴ�����10�ζ�ʧ��
        {
            timeout = 0;
            goto again;//���·�����ʼ�ź�
        }
    }
    ack=i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND);
    //�����ַ���ͳɹ���־λ
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
    i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
    //�ȴ����ݼĴ����ǿձ�־λ��һ
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
    //��������


    //����ֹͣ�ź�

    //���ADDSEND��־λ



    if((ack&0x40)==0)
    {
        Byte_1th  =i2c_data_receive (BSP_IIC);
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_2th = i2c_data_receive (BSP_IIC);
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        //i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        //while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_3th = i2c_data_receive (BSP_IIC);
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_4th = i2c_data_receive (BSP_IIC);
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_5th = i2c_data_receive (BSP_IIC);
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_6th = i2c_data_receive (BSP_IIC);
        i2c_stop_on_bus(BSP_IIC);
        RetuData = (RetuData|Byte_2th)<<8;
        RetuData = (RetuData|Byte_3th)<<8;
        RetuData = (RetuData|Byte_4th);
        RetuData =RetuData >>4;
        ct[0] = RetuData;
        RetuData = 0;
        RetuData = (RetuData|Byte_4th)<<8;
        RetuData = (RetuData|Byte_5th)<<8;
        RetuData = (RetuData|Byte_6th);
        RetuData = RetuData&0xfffff;
        ct[1] =RetuData;
    }
}
