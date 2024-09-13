#include "gd32e23x.h"
#include "systick.h"

#define SOFTWARE_OR_HARDWARE   0    // 1��ʹ�����IIC	0��ʹ��Ӳ��IIC

//SDA�˿���ֲ
#define         RCU_SDA      	RCU_GPIOA
#define         PORT_SDA        GPIOA
#define         GPIO_SDA        GPIO_PIN_1
//SCL�˿���ֲ
#define         RCU_SCL         RCU_GPIOA
#define         PORT_SCL        GPIOA
#define         GPIO_SCL        GPIO_PIN_0

// IIC
#define			RCU_IIC			RCU_I2C1
#define			BSP_IIC			I2C1
#define 		IIC_ADDR		0x78
#define 		IIC_AH10		0x70
#define			IIC_AF			GPIO_AF_4

 
//����SDA���ģʽ
#define SDA_OUT()   	gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
 
//����SDA����ģʽ
#define SDA_IN()        gpio_mode_set(PORT_SDA,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
 
//��ȡSDA���ŵĵ�ƽ�仯
#define SDA_GET()       gpio_input_bit_get(PORT_SDA,GPIO_SDA) 
 
//SDA��SCL���
#define SDA(x)    		gpio_bit_write(PORT_SDA,GPIO_SDA, (x?SET:RESET) )
#define SCL(x)    		gpio_bit_write(PORT_SCL,GPIO_SCL, (x?SET:RESET) )

uint8_t AHT10ReadData(float *temperature,uint8_t *humidity);
void AHT10Reset(void);
uint8_t AHT10Check(void);

void AHT10Init(void) ;
// ���IIC  ��ȡSHT20����ʪ������
float SHT20_receive_data( unsigned char num );

// Ӳ��IIC  ��ȡSHT20����ʪ������
float iic_hardware_get_data(unsigned int num);

// ����һ���ֽ�
unsigned char IIC_Read_Byte(void);

//IIC�ȴ�Ӧ��
//Ӧ�𷵻�0  ��Ӧ�𷵻�1
unsigned char IIC_Wait_Ack(void);

//IIC���ͷ�Ӧ��
void IIC_Send_Nack(void);

void JH_Init(void) ;
//IIC����Ӧ��
void IIC_Send_Ack(void);

//IICֹͣ�ź�
void IIC_Stop(void);

 //IIC��ʼ�ź�
void IIC_Start(void);

void iic_gpio_config(void);


