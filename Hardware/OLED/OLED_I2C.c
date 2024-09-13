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
* Author : Ð¡ÁÖ
* Version: 1.00
* Date   : 2014.4.8
* Email  : hello14blog@gmail.com
* Modification: none
*
* Description:128*64µãÕóµÄOLEDÏÔÊ¾ÆÁÇý¶¯ÎÄ¼þ£¬½öÊÊÓÃÓÚ»ÝÌØ×Ô¶¯»¯(heltec.taobao.com)µÄSD1306Çý¶¯IICÍ¨ÐÅ·½Ê½ÏÔÊ¾ÆÁ
*
* Others: none;
*
* Function List:
*	1. void I2C_Configuration(void) -- ÅäÖÃCPUµÄÓ²¼þI2C
* 2. void I2C_WriteByte(uint8_t addr,uint8_t data) -- Ïò¼Ä´æÆ÷µØÖ·Ð´Ò»¸öbyteµÄÊý¾Ý
* 3. void WriteCmd(unsigned char I2C_Command) -- Ð´ÃüÁî
* 4. void WriteDat(unsigned char I2C_Data) -- Ð´Êý¾Ý
* 5. void OLED_Init(void) -- OLEDÆÁ³õÊ¼»¯
* 6. void OLED_SetPos(unsigned char x, unsigned char y) -- ÉèÖÃÆðÊ¼µã×ø±ê
* 7. void OLED_Fill(unsigned char fill_Data) -- È«ÆÁÌî³ä
* 8. void OLED_CLS(void) -- ÇåÆÁ
* 9. void OLED_ON(void) -- »½ÐÑ
* 10. void OLED_OFF(void) -- Ë¯Ãß
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- ÏÔÊ¾×Ö·û´®(×ÖÌå´óÐ¡ÓÐ6*8ºÍ8*16Á½ÖÖ)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- ÏÔÊ¾ÖÐÎÄ(ÖÐÎÄÐèÒªÏÈÈ¡Ä££¬È»ºó·Åµ½codetab.hÖÐ)
* 13. void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]) -- BMPÍ¼Æ¬
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
    /* ???????©??? 50MHZ */
    gpio_output_options_set(PORT_SCL,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SCL);

    /* ????SDA????g? */
    gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA);
    /* ???????©??? 50MHZ */
    gpio_output_options_set(PORT_SDA,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SDA);

#else

    /* ????SCL???Ÿ???IIC???? */
    gpio_af_set(PORT_SCL, IIC_AF, GPIO_SCL);
    /* ????SDA???Ÿ???IIC???? */
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

    // ·¢ËÍÆðÊ¼ÐÅºÅ
    i2c_start_on_bus(BSP_IIC);

    // µÈ´ýÆðÊ¼ÐÅºÅ·¢ËÍÍê³É (SBSEND ±êÖ¾Î»ÖÃÎ»)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_SBSEND));

    // ·¢ËÍÉè±¸µØÖ·¼ÓÐ´²Ù×÷
    i2c_master_addressing(BSP_IIC, 0x78, I2C_TRANSMITTER);

    // µÈ´ýµØÖ··¢ËÍÍê³É (ADDSEND ±êÖ¾Î»ÖÃÎ»)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_ADDSEND));

    // Çå³ýµØÖ··¢ËÍ±êÖ¾Î»
    i2c_flag_clear(BSP_IIC, I2C_FLAG_ADDSEND);

    // µÈ´ý·¢ËÍ»º³åÇøÎª¿Õ (TBE ±êÖ¾Î»ÖÃÎ»)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_TBE));

    // ·¢ËÍÊý¾Ý×Ö½Ú
    i2c_data_transmit(BSP_IIC, addr);

    // µÈ´ý×Ö½Ú´«ÊäÍê³É (BTC ±êÖ¾Î»ÖÃÎ»)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_BTC));
    // ·¢ËÍÊý¾Ý×Ö½Ú
    i2c_data_transmit(BSP_IIC, data);

    // µÈ´ý×Ö½Ú´«ÊäÍê³É (BTC ±êÖ¾Î»ÖÃÎ»)
    while (!i2c_flag_get(BSP_IIC, I2C_FLAG_BTC));

    // ·¢ËÍÍ£Ö¹ÐÅºÅ
    i2c_stop_on_bus(BSP_IIC);

    //  while (I2C_CTL0(I2C1) & 0x0200);


}


void WriteCmd(uint8_t I2C_Command)//Ð´ÃüÁî
{
    I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(uint8_t I2C_Data)//Ð´Êý¾Ý
{
    I2C_WriteByte(0x40, I2C_Data);
}

void OLED_Init(void)
{
    //ÕâÀïµÄÑÓÊ±ºÜÖØÒª
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
    WriteCmd(0xAF); //ÁÁ¶Èµ÷½Ú 0x00~0xff
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

void OLED_SetPos(unsigned char x, unsigned char y) //ÉèÖÃÆðÊ¼µã×ø±ê
{
    WriteCmd(0xb0+y);
    WriteCmd((((x+2&0xf0)>>4)|0x10));
    WriteCmd((x+2&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//È«ÆÁÌî³ä
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

void OLED_CLS(void)//ÇåÆÁ
{
    OLED_Fill(0x02);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          :
// Parameters     : none
// Description    : ½«OLED´ÓÐÝÃßÖÐ»½ÐÑ
//--------------------------------------------------------------
void OLED_ON(void)
{
    WriteCmd(0X8D);  //ÉèÖÃµçºÉ±Ã
    WriteCmd(0X14);  //¿ªÆôµçºÉ±Ã
    WriteCmd(0XAF);  //OLED»½ÐÑ
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          :
// Parameters     : none
// Description    : ÈÃOLEDÐÝÃß -- ÐÝÃßÄ£Ê½ÏÂ,OLED¹¦ºÄ²»µ½10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
    WriteCmd(0X8D);  //ÉèÖÃµçºÉ±Ã
    WriteCmd(0X10);  //¹Ø±ÕµçºÉ±Ã
    WriteCmd(0XAE);  //OLEDÐÝÃß
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          :
// Parameters     : x,y -- ÆðÊ¼µã×ø±ê(x:0~127, y:0~7); ch[] -- ÒªÏÔÊ¾µÄ×Ö·û´®; TextSize -- ×Ö·û´óÐ¡(1:6*8 ; 2:8*16)
// Description    : ÏÔÊ¾codetab.hÖÐµÄASCII×Ö·û,ÓÐ6*8ºÍ8*16¿ÉÑ¡Ôñ
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
// Parameters     : x,y -- ÆðÊ¼µã×ø±ê(x:0~127, y:0~7); N:ºº×ÖÔÚcodetab.hÖÐµÄË÷Òý
// Description    : ÏÔÊ¾codetab.hÖÐµÄºº×Ö,16*16µãÕó
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
// Parameters     : x0,y0 -- ÆðÊ¼µã×ø±ê(x0:0~127, y0:0~7); x1,y1 -- Æðµã¶Ô½ÇÏß(½áÊøµã)µÄ×ø±ê(x1:1~128,y1:1~8)
// Description    : ÏÔÊ¾BMPÎ»Í¼
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
void AHT10Init(void)   //³õÊ¼»¯AHT10
{
    DelayMs(50);
    //·¢ËÍÆðÊ¼ÐÅºÅ
    i2c_start_on_bus(BSP_IIC);
    //µÈ´ýSBSEND±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //·¢ËÍÆ÷¼þµØÖ·¼ÓÐ´²Ù×÷
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //µÈ´ýADDSEND±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
    //Çå³ýADDSEND±êÖ¾Î»
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //µÈ´ýTBE±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
    //·¢ËÍÊý¾Ý
    i2c_data_transmit (BSP_IIC, 0xe1);
    //µÈ´ýBTC±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x08);
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x00);
    //µÈ´ýBTC±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    //·¢ËÍÍ£Ö¹ÐÅºÅ
    i2c_stop_on_bus(BSP_IIC);
    DelayMs(50);
}

uint8_t AHT10Check(void)
{
    uint8_t ack=0;

    //·¢ËÍÆðÊ¼ÐÅºÅ
    i2c_start_on_bus(BSP_IIC);
    //µÈ´ýSBSEND±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //·¢ËÍÆ÷¼þµØÖ·¼ÓÐ´²Ù×÷
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //µÈ´ýADDSEND±êÖ¾Î»ÖÃÒ»
    ack=i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND);
    //Çå³ýADDSEND±êÖ¾Î»
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
    //·¢ËÍÍ£Ö¹ÐÅºÅ
    i2c_stop_on_bus(BSP_IIC);
    return ack;
}

/**
brief AHT10Èí¸´Î»
param NONE
return NONE
*/
void AHT10Reset(void)
{

    //·¢ËÍÆðÊ¼ÐÅºÅ
    i2c_start_on_bus(BSP_IIC);
    //µÈ´ýSBSEND±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //·¢ËÍÆ÷¼þµØÖ·¼ÓÐ´²Ù×÷
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //µÈ´ýADDSEND±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
    //Çå³ýADDSEND±êÖ¾Î»
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //µÈ´ýTBE±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
    //·¢ËÍÊý¾Ý
    i2c_data_transmit (BSP_IIC, 0xba);

    //µÈ´ýBTC±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    //·¢ËÍÍ£Ö¹ÐÅºÅ
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
    //·¢ËÍÆ÷¼þµØÖ·¼ÓÐ´²Ù×÷
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
    //µÈ´ýADDSEND±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
    //Çå³ýADDSEND±êÖ¾Î»
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //µÈ´ýTBE±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
    //·¢ËÍÊý¾Ý
    i2c_data_transmit (BSP_IIC, 0xac);
    //µÈ´ýBTC±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x33);
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    i2c_data_transmit (BSP_IIC, 0x00);
    //µÈ´ýBTC±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
    //·¢ËÍÍ£Ö¹ÐÅºÅ
    i2c_stop_on_bus(BSP_IIC);
again:
    DelayMs(80);
    //ÑÓÊ±Ò»»áµÈ´ýÊý¾Ý¶Á³ö
    //·¢ËÍÆðÊ¼ÐÅºÅ
    i2c_start_on_bus(BSP_IIC);
    //µÈ´ýÆðÊ¼ÐÅºÅ·¢ËÍÍê³É±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
    //·¢ËÍÆ÷¼þµØÖ·¼Ó¶Á²Ù×÷
    i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_RECEIVER);
    //µÈ´ýÆ÷¼þµØÖ··¢ËÍ³É¹¦±êÖ¾Î»ÖÃÒ»

    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) )
    {
        DelayMs(1);
        timeout++;
        if( timeout > 10 )//µÈ´ý³¬¹ý10´Î¶¼Ê§°Ü
        {
            timeout = 0;
            goto again;//ÖØÐÂ·¢ËÍÆðÊ¼ÐÅºÅ
        }
    }
    ack=i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND);
    //Çå³ýµØÖ··¢ËÍ³É¹¦±êÖ¾Î»
    i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);

    //Ê¹ÄÜÓ¦´ð¹¦ÄÜ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍÓ¦´ðÐÅºÅ
    i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
    //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
    while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
    //½ÓÊÕÊý¾Ý


    //·¢ËÍÍ£Ö¹ÐÅºÅ

    //Çå³ýADDSEND±êÖ¾Î»



    if((ack&0x40)==0)
    {
        Byte_1th  =i2c_data_receive (BSP_IIC);
        //¹Ø±ÕÓ¦´ðÐÅºÅ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍ·ÇÓ¦´ðÐÅºÅ
        // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //Ê¹ÄÜÓ¦´ð¹¦ÄÜ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍÓ¦´ðÐÅºÅ
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_2th = i2c_data_receive (BSP_IIC);
        //¹Ø±ÕÓ¦´ðÐÅºÅ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍ·ÇÓ¦´ðÐÅºÅ
        //i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        //while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //Ê¹ÄÜÓ¦´ð¹¦ÄÜ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍÓ¦´ðÐÅºÅ
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_3th = i2c_data_receive (BSP_IIC);
        //¹Ø±ÕÓ¦´ðÐÅºÅ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍ·ÇÓ¦´ðÐÅºÅ
        // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //Ê¹ÄÜÓ¦´ð¹¦ÄÜ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍÓ¦´ðÐÅºÅ
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_4th = i2c_data_receive (BSP_IIC);
        //¹Ø±ÕÓ¦´ðÐÅºÅ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍ·ÇÓ¦´ðÐÅºÅ
        // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //Ê¹ÄÜÓ¦´ð¹¦ÄÜ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍÓ¦´ðÐÅºÅ
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        Byte_5th = i2c_data_receive (BSP_IIC);
        //¹Ø±ÕÓ¦´ðÐÅºÅ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍ·ÇÓ¦´ðÐÅºÅ
        //¹Ø±ÕÓ¦´ðÐÅºÅ£¬¼´½ÓÊÕÊ±×Ô¶¯·¢ËÍ·ÇÓ¦´ðÐÅºÅ
        i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //µÈ´ýÊý¾Ý¼Ä´æÆ÷·Ç¿Õ±êÖ¾Î»ÖÃÒ»
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
