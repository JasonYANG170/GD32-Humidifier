#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "gd32e23x.h"

#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
#define SOFTWARE_OR_HARDWARE   0    // 1??'??????IIC	0??'?????IIC

//SDA??????
#define         RCU_SDA      	RCU_GPIOA
#define         PORT_SDA        GPIOA
#define         GPIO_SDA        GPIO_PIN_1
//SCL? ?????
#define         RCU_SCL         RCU_GPIOA
#define         PORT_SCL        GPIOA
#define         GPIO_SCL        GPIO_PIN_0

// IIC
#define			RCU_IIC			RCU_I2C1
#define			BSP_IIC			I2C1
#define 		IIC_ADDR		0x78
#define 		IIC_AH10		0x70
#define			IIC_AF			GPIO_AF_4

void AHT10ReadData(uint32_t *ct);
void AHT10Reset(void);
uint8_t AHT10Check(void);

void AHT10Init(void) ;

void I2C_Configuration(void);
void I2C_WriteByte(uint32_t addr,uint8_t data);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);

#endif
