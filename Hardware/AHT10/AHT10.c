#include "IIC.h"
#include "bsp_usart.h"
#include "stdio.h"

void iic_gpio_config(void)
{
	/* ʹ��ʱ�� */
	rcu_periph_clock_enable(RCU_SCL);
	rcu_periph_clock_enable(RCU_SDA);
	
#if		SOFTWARE_OR_HARDWARE	
	
	/* ����SCLΪ���ģʽ */
	gpio_mode_set(PORT_SCL,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SCL);
	/* ����Ϊ��©��� 50MHZ */
	gpio_output_options_set(PORT_SCL,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SCL);
					
	/* ����SDAΪ���ģʽ */
	gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA);
	/* ����Ϊ��©��� 50MHZ */
	gpio_output_options_set(PORT_SDA,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SDA);

#else
	
	/* ����SCL���Ÿ���IIC���� */
	gpio_af_set(PORT_SCL, IIC_AF, GPIO_SCL);
	/* ����SDA���Ÿ���IIC���� */
	gpio_af_set(PORT_SDA, IIC_AF, GPIO_SDA);
	/* ����SCL���� */
	gpio_mode_set(PORT_SCL, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SCL);
	gpio_output_options_set(PORT_SCL, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SCL);
	   
	/* ����SDA���� */
	gpio_mode_set(PORT_SDA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SDA);
	gpio_output_options_set(PORT_SDA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SDA);
	
	/* ʹ��IICʱ�� */
	rcu_periph_clock_enable(RCU_IIC);
	/* IIC��λ */
	i2c_deinit(I2C0);
	/* ����IIC���� */
	i2c_clock_config(BSP_IIC, 400000, I2C_DTCY_2);
	/* ����I2C��ַ */
	i2c_mode_addr_config(BSP_IIC, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, IIC_ADDR);
	/* ʹ��I2C */
	i2c_enable(BSP_IIC);
	/* ʹ��Ӧ���� */
	i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
	
#endif
	
}

 //IIC��ʼ�ź�
void IIC_Start(void)
{
    SDA_OUT();//����SDAΪ���ģʽ     
    SDA(1);
    SCL(1);         
    delay_us(5);   
    SDA(0);
    delay_us(5);
    SCL(0);
    delay_us(5);         
}
 
//IICֹͣ�ź�
void IIC_Stop(void)
{
    SDA_OUT();
    SCL(0);
    SDA(0); 
    SCL(1);
    delay_us(5);
    SDA(1);
    delay_us(5);
}
 
//IIC����Ӧ��
void IIC_Send_Ack(void)
{
    SDA_OUT();
    SCL(0);
    SDA(1);
    SDA(0);
    SCL(1);
    delay_us(5);
    SCL(0);
    SDA(1);
}
 
//IIC���ͷ�Ӧ��
void IIC_Send_Nack(void)
{
    SDA_OUT();
    SCL(0);
    SDA(0);
    SDA(1);
    SCL(1);
    delay_us(5);
    SCL(0);
    SDA(0);
}
 
//IIC�ȴ�Ӧ��
//Ӧ�𷵻�0  ��Ӧ�𷵻�1
unsigned char IIC_Wait_Ack(void)
{
    char ack = 0;
    unsigned char ack_flag = 10;
    SCL(0);
    SDA(1);
    SDA_IN();
    delay_us(5);
    SCL(1);
    delay_us(5);
 
    while( (SDA_GET()==1)  &&  ( ack_flag ) )
    {
            ack_flag--;
            delay_us(5);
    }
    
    //��Ӧ��
    if( ack_flag <= 0 )
    {
            IIC_Stop();
            return 1;
    }
    else//Ӧ��
    {
            SCL(0);
            SDA_OUT();
    }
    return ack;
}


//����һ���ֽ�
void IIC_Send_Byte(uint8_t dat)
{
    int i = 0;
    SDA_OUT();
    SCL(0);        
    for( i = 0; i < 8; i++ )
    {
        SDA( (dat & 0x80) >> 7 );
        delay_us(1);
        SCL(1);
        delay_us(5);
        SCL(0);
        delay_us(5);
        dat<<=1;
    }        
}

//����һ���ֽ�
unsigned char IIC_Read_Byte(void)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
    {
        SCL(0);
        delay_us(5);
        SCL(1);
        delay_us(5);
        receive<<=1;
        if( SDA_GET() )
        {        
            receive |= 1;   
        } 
    }
    SCL(0); 
    return receive;
}

/**********************************************************
 * �� �� �� �ƣ�SHT20_receive_data
 * �� �� �� �ܣ���ȡSHT20����ʪ������
 * �� �� �� ����num=0xf3��ȡ�¶�����                num=0xf5��ȡʪ������
 * �� �� �� �أ���ȡ���Ļ�������
 * ��       �ߣ�LC
 * ��       ע����
**********************************************************/
float SHT20_receive_data( unsigned char num )
{
        uint8_t data_msb = 0;//���ݸ�8λ
        uint8_t data_lsb = 0;//���ݵ�8λ
        uint8_t check = 0;   //У��λ
        uint16_t dat = 0;    //�ߵ�λ���Ϻ�����
        float temp = 0;      //�������ʪ�Ƚ��
 
        IIC_Start();//������ʼ�ź�
        IIC_Send_Byte(IIC_ADDR);//����������ַ��д
        if( IIC_Wait_Ack() == 1 ) //������ط�Ӧ���򴮿���ʾʧ��-1
        {
        printf("receive fail -1\r\n");
        }
        IIC_Send_Byte( num );//���ͷ�����ģʽ��ȡ����
        if( IIC_Wait_Ack() == 1 ) //������ط�Ӧ���򴮿���ʾʧ��-2
        {
        printf("receive fail -2\r\n");
        }
           do   //���·�����ʼ�ź���������ַ+��
        {
                delay_us(10);//�ȴ�
                IIC_Start(); //��ʼ�ź�
                IIC_Send_Byte(0X81);//������ַ+��
        }
        while( IIC_Wait_Ack() == 1 );//�ȴ�Ӧ��
        data_msb = IIC_Read_Byte();//��ȡ��һ���ֽ�
        IIC_Send_Ack();            //����Ӧ��
        data_lsb = IIC_Read_Byte();//��ȡ�ڶ����ֽ�
        IIC_Send_Ack();            //����Ӧ��
        check = IIC_Read_Byte();   //��ȡ�������ֽ�
        IIC_Send_Nack();           //���ͷ�Ӧ��
        IIC_Stop();                //����ֹͣ�ź�
        
        //��������
        dat = data_msb<<8;
        dat = dat | data_lsb;
        dat &= ~(1<<1);
       //���ݻ���
        if( num == 0xf3 )//�����¶�
        {
                temp = ( dat / 65536.0 ) * 175.72 - 46.85;
        }
        if( num == 0xf5)//����ʪ��
        {
                temp = ( dat / 65536.0 ) * 125 - 6;
        }
        return temp;
}

 /**********************************************************
 * �� �� �� �ƣ�iic_hardware_get_data
 * �� �� �� �ܣ���ȡSHT20����ʪ������
 * �� �� �� ����num=0xf3��ȡ�¶�����                num=0xf5��ȡʪ������
 * �� �� �� �أ���ȡ���Ļ�������
 * ��       �ߣ�LC
 * ��       ע����
**********************************************************/
float iic_hardware_get_data(unsigned int num)
{
        unsigned int timeout = 0;
        unsigned char data_H = 0;
        unsigned char data_L = 0;
        unsigned int dat = 0;
        //������ʼ�ź�
        i2c_start_on_bus(BSP_IIC);
       //�ȴ�SBSEND��־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //����������ַ��д����
        i2c_master_addressing(BSP_IIC, IIC_ADDR, I2C_TRANSMITTER);
       //�ȴ�ADDSEND��־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
       //���ADDSEND��־λ
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
        
        //�ȴ�TBE��־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
        //��������
        i2c_data_transmit (BSP_IIC, num);
       //�ȴ�BTC��־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
        //����ֹͣ�ź�
        i2c_stop_on_bus(BSP_IIC);
        
        again:
        //������ʼ�ź�
        i2c_start_on_bus(BSP_IIC);
       //�ȴ���ʼ�źŷ�����ɱ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //����������ַ�Ӷ�����
        i2c_master_addressing(BSP_IIC, IIC_ADDR, I2C_RECEIVER);
       //�ȴ�������ַ���ͳɹ���־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) )
        {
                delay_1ms(1);
                timeout++;
                if( timeout > 10 )//�ȴ�����10�ζ�ʧ��
                {
                        timeout = 0;
                        goto again;//���·�����ʼ�ź�
                }
        }
       //�����ַ���ͳɹ���־λ
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
 
        //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
       //��������
        data_H = i2c_data_receive (BSP_IIC);
        
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //��������
        data_L = i2c_data_receive (BSP_IIC);
        
        //����ֹͣ�ź�
        i2c_stop_on_bus(BSP_IIC);
        //��������
        dat = data_H<<8;
        dat = dat | data_L;
        dat &= ~(0x03);
       //���ݻ���
        if( num == 0xf3 )
        {
                return ( ( dat / 65536.0 ) * 175.72 - 46.85);
        }
        if( num == 0xf5 )
        {
                return ( ( dat / 65536.0 ) *125 - 6);
        }
        return 0;
}

void AHT10Init(void)   //��ʼ��AHT10
{
delay_ms(50);
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
delay_ms(50);
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
uint8_t AHT10ReadData(float *temperature,uint8_t *humidity)
{
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
	delay_ms(80);//��ʱһ��ȴ����ݶ���
  //������ʼ�ź�
        i2c_start_on_bus(BSP_IIC);
       //�ȴ���ʼ�źŷ�����ɱ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //����������ַ�Ӷ�����
        i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_RECEIVER);
       //�ȴ�������ַ���ͳɹ���־λ��һ
			 
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) )
        {
                delay_1ms(1);
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
		databuff[0]=i2c_data_receive (BSP_IIC);
		        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
       // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
       // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		        //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		databuff[1]=i2c_data_receive (BSP_IIC);
		        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        //i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        //while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		        //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		databuff[2]=i2c_data_receive (BSP_IIC);
		        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
       // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
       // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
			         //ʹ��Ӧ���ܣ�������ʱ�Զ�����Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		databuff[3]=i2c_data_receive (BSP_IIC);
		        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        //�ر�Ӧ���źţ�������ʱ�Զ����ͷ�Ӧ���ź�
        i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //�ȴ����ݼĴ����ǿձ�־λ��һ
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		databuff[4]=i2c_data_receive (BSP_IIC);
	   i2c_stop_on_bus(BSP_IIC);
		SRH=(databuff[0]<<12)+(databuff[1]<<4)+(databuff[2]>>4);
		ST=((databuff[2]&0X0f)<<16)+(databuff[3]<<8)+(databuff[4]);
		*humidity=(int)(SRH*100.0/1024/1024+0.5);
		*temperature=((int)(ST*2000.0/1024/1024+0.5))/10.0-50;
		return 0;
	}
    i2c_stop_on_bus(BSP_IIC);
	return 1;
}

