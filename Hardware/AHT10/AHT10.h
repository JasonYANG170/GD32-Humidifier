#include "gd32e23x.h"
#include "systick.h"

#define SOFTWARE_OR_HARDWARE   0    // 1：使用软件IIC	0：使用硬件IIC

//SDA端口移植
#define         RCU_SDA      	RCU_GPIOA
#define         PORT_SDA        GPIOA
#define         GPIO_SDA        GPIO_PIN_1
//SCL端口移植
#define         RCU_SCL         RCU_GPIOA
#define         PORT_SCL        GPIOA
#define         GPIO_SCL        GPIO_PIN_0

// IIC
#define			RCU_IIC			RCU_I2C1
#define			BSP_IIC			I2C1
#define 		IIC_ADDR		0x78
#define 		IIC_AH10		0x70
#define			IIC_AF			GPIO_AF_4

 
//设置SDA输出模式
#define SDA_OUT()   	gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
 
//设置SDA输入模式
#define SDA_IN()        gpio_mode_set(PORT_SDA,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
 
//获取SDA引脚的电平变化
#define SDA_GET()       gpio_input_bit_get(PORT_SDA,GPIO_SDA) 
 
//SDA与SCL输出
#define SDA(x)    		gpio_bit_write(PORT_SDA,GPIO_SDA, (x?SET:RESET) )
#define SCL(x)    		gpio_bit_write(PORT_SCL,GPIO_SCL, (x?SET:RESET) )

uint8_t AHT10ReadData(float *temperature,uint8_t *humidity);
void AHT10Reset(void);
uint8_t AHT10Check(void);

void AHT10Init(void) ;
// 软件IIC  读取SHT20的温湿度数据
float SHT20_receive_data( unsigned char num );

// 硬件IIC  读取SHT20的温湿度数据
float iic_hardware_get_data(unsigned int num);

// 接收一个字节
unsigned char IIC_Read_Byte(void);

//IIC等待应答
//应答返回0  非应答返回1
unsigned char IIC_Wait_Ack(void);

//IIC发送非应答
void IIC_Send_Nack(void);

void JH_Init(void) ;
//IIC发送应答
void IIC_Send_Ack(void);

//IIC停止信号
void IIC_Stop(void);

 //IIC起始信号
void IIC_Start(void);

void iic_gpio_config(void);


