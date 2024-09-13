#include "IIC.h"
#include "bsp_usart.h"
#include "stdio.h"

void iic_gpio_config(void)
{
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_SCL);
	rcu_periph_clock_enable(RCU_SDA);
	
#if		SOFTWARE_OR_HARDWARE	
	
	/* 配置SCL为输出模式 */
	gpio_mode_set(PORT_SCL,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SCL);
	/* 配置为开漏输出 50MHZ */
	gpio_output_options_set(PORT_SCL,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SCL);
					
	/* 配置SDA为输出模式 */
	gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA);
	/* 配置为开漏输出 50MHZ */
	gpio_output_options_set(PORT_SDA,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SDA);

#else
	
	/* 配置SCL引脚复用IIC功能 */
	gpio_af_set(PORT_SCL, IIC_AF, GPIO_SCL);
	/* 配置SDA引脚复用IIC功能 */
	gpio_af_set(PORT_SDA, IIC_AF, GPIO_SDA);
	/* 配置SCL引脚 */
	gpio_mode_set(PORT_SCL, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SCL);
	gpio_output_options_set(PORT_SCL, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SCL);
	   
	/* 配置SDA引脚 */
	gpio_mode_set(PORT_SDA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SDA);
	gpio_output_options_set(PORT_SDA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SDA);
	
	/* 使能IIC时钟 */
	rcu_periph_clock_enable(RCU_IIC);
	/* IIC复位 */
	i2c_deinit(I2C0);
	/* 配置IIC速率 */
	i2c_clock_config(BSP_IIC, 400000, I2C_DTCY_2);
	/* 配置I2C地址 */
	i2c_mode_addr_config(BSP_IIC, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, IIC_ADDR);
	/* 使能I2C */
	i2c_enable(BSP_IIC);
	/* 使能应答功能 */
	i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
	
#endif
	
}

 //IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();//设置SDA为输出模式     
    SDA(1);
    SCL(1);         
    delay_us(5);   
    SDA(0);
    delay_us(5);
    SCL(0);
    delay_us(5);         
}
 
//IIC停止信号
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
 
//IIC发送应答
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
 
//IIC发送非应答
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
 
//IIC等待应答
//应答返回0  非应答返回1
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
    
    //非应答
    if( ack_flag <= 0 )
    {
            IIC_Stop();
            return 1;
    }
    else//应答
    {
            SCL(0);
            SDA_OUT();
    }
    return ack;
}


//发送一个字节
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

//接收一个字节
unsigned char IIC_Read_Byte(void)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
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
 * 函 数 名 称：SHT20_receive_data
 * 函 数 功 能：读取SHT20的温湿度数据
 * 传 入 参 数：num=0xf3读取温度数据                num=0xf5读取湿度数据
 * 函 数 返 回：读取到的环境数据
 * 作       者：LC
 * 备       注：无
**********************************************************/
float SHT20_receive_data( unsigned char num )
{
        uint8_t data_msb = 0;//数据高8位
        uint8_t data_lsb = 0;//数据低8位
        uint8_t check = 0;   //校验位
        uint16_t dat = 0;    //高低位整合后数据
        float temp = 0;      //换算的温湿度结果
 
        IIC_Start();//发送起始信号
        IIC_Send_Byte(IIC_ADDR);//发送器件地址加写
        if( IIC_Wait_Ack() == 1 ) //如果返回非应答，则串口提示失败-1
        {
        printf("receive fail -1\r\n");
        }
        IIC_Send_Byte( num );//发送非主机模式读取命令
        if( IIC_Wait_Ack() == 1 ) //如果返回非应答，则串口提示失败-2
        {
        printf("receive fail -2\r\n");
        }
           do   //重新发送起始信号与器件地址+读
        {
                delay_us(10);//等待
                IIC_Start(); //起始信号
                IIC_Send_Byte(0X81);//器件地址+读
        }
        while( IIC_Wait_Ack() == 1 );//等待应答
        data_msb = IIC_Read_Byte();//读取第一个字节
        IIC_Send_Ack();            //发送应答
        data_lsb = IIC_Read_Byte();//读取第二个字节
        IIC_Send_Ack();            //发送应答
        check = IIC_Read_Byte();   //读取第三个字节
        IIC_Send_Nack();           //发送非应答
        IIC_Stop();                //发送停止信号
        
        //数据整合
        dat = data_msb<<8;
        dat = dat | data_lsb;
        dat &= ~(1<<1);
       //数据换算
        if( num == 0xf3 )//测量温度
        {
                temp = ( dat / 65536.0 ) * 175.72 - 46.85;
        }
        if( num == 0xf5)//测量湿度
        {
                temp = ( dat / 65536.0 ) * 125 - 6;
        }
        return temp;
}

 /**********************************************************
 * 函 数 名 称：iic_hardware_get_data
 * 函 数 功 能：读取SHT20的温湿度数据
 * 传 入 参 数：num=0xf3读取温度数据                num=0xf5读取湿度数据
 * 函 数 返 回：读取到的环境数据
 * 作       者：LC
 * 备       注：无
**********************************************************/
float iic_hardware_get_data(unsigned int num)
{
        unsigned int timeout = 0;
        unsigned char data_H = 0;
        unsigned char data_L = 0;
        unsigned int dat = 0;
        //发送起始信号
        i2c_start_on_bus(BSP_IIC);
       //等待SBSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //发送器件地址加写操作
        i2c_master_addressing(BSP_IIC, IIC_ADDR, I2C_TRANSMITTER);
       //等待ADDSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
       //清除ADDSEND标志位
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
        
        //等待TBE标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
        //发送数据
        i2c_data_transmit (BSP_IIC, num);
       //等待BTC标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
        //发送停止信号
        i2c_stop_on_bus(BSP_IIC);
        
        again:
        //发送起始信号
        i2c_start_on_bus(BSP_IIC);
       //等待起始信号发送完成标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //发送器件地址加读操作
        i2c_master_addressing(BSP_IIC, IIC_ADDR, I2C_RECEIVER);
       //等待器件地址发送成功标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) )
        {
                delay_1ms(1);
                timeout++;
                if( timeout > 10 )//等待超过10次都失败
                {
                        timeout = 0;
                        goto again;//重新发送起始信号
                }
        }
       //清除地址发送成功标志位
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
 
        //使能应答功能，即接收时自动发送应答信号
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //等待数据寄存器非空标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
       //接收数据
        data_H = i2c_data_receive (BSP_IIC);
        
        //关闭应答信号，即接收时自动发送非应答信号
        i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //等待数据寄存器非空标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
        //接收数据
        data_L = i2c_data_receive (BSP_IIC);
        
        //发送停止信号
        i2c_stop_on_bus(BSP_IIC);
        //数据整合
        dat = data_H<<8;
        dat = dat | data_L;
        dat &= ~(0x03);
       //数据换算
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

void AHT10Init(void)   //初始化AHT10
{
delay_ms(50);
	  //发送起始信号
        i2c_start_on_bus(BSP_IIC);
       //等待SBSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //发送器件地址加写操作
        i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
       //等待ADDSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
       //清除ADDSEND标志位
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
        
        //等待TBE标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
        //发送数据
        i2c_data_transmit (BSP_IIC, 0xe1);
       //等待BTC标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
	       i2c_data_transmit (BSP_IIC, 0x08);
	        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
	       i2c_data_transmit (BSP_IIC, 0x00);
       //等待BTC标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
        //发送停止信号
        i2c_stop_on_bus(BSP_IIC);
delay_ms(50);
}

uint8_t AHT10Check(void)
{
	uint8_t ack=0;

	  //发送起始信号
        i2c_start_on_bus(BSP_IIC);
       //等待SBSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //发送器件地址加写操作
        i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
       //等待ADDSEND标志位置一
        ack=i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND);
       //清除ADDSEND标志位
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
        //发送停止信号
        i2c_stop_on_bus(BSP_IIC);
	return ack;
}
 
/**
brief AHT10软复位
param NONE
return NONE
*/
void AHT10Reset(void)
{
	
	  //发送起始信号
        i2c_start_on_bus(BSP_IIC);
       //等待SBSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //发送器件地址加写操作
        i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
       //等待ADDSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
       //清除ADDSEND标志位
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
        
        //等待TBE标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
        //发送数据
        i2c_data_transmit (BSP_IIC, 0xba);

       //等待BTC标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
        //发送停止信号
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
        //发送器件地址加写操作
        i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_TRANSMITTER);
       //等待ADDSEND标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) );
       //清除ADDSEND标志位
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
        
        //等待TBE标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_TBE) );
        //发送数据
        i2c_data_transmit (BSP_IIC, 0xac);
       //等待BTC标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
	       i2c_data_transmit (BSP_IIC, 0x33);
	        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
	       i2c_data_transmit (BSP_IIC, 0x00);
       //等待BTC标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_BTC) );
        //发送停止信号
        i2c_stop_on_bus(BSP_IIC);
  again:
	delay_ms(80);//延时一会等待数据读出
  //发送起始信号
        i2c_start_on_bus(BSP_IIC);
       //等待起始信号发送完成标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_SBSEND) );
        //发送器件地址加读操作
        i2c_master_addressing(BSP_IIC, IIC_AH10, I2C_RECEIVER);
       //等待器件地址发送成功标志位置一
			 
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND) )
        {
                delay_1ms(1);
                timeout++;
                if( timeout > 10 )//等待超过10次都失败
                {
                        timeout = 0;
                        goto again;//重新发送起始信号
                }
        }
				ack=i2c_flag_get(BSP_IIC,I2C_FLAG_ADDSEND);
       //清除地址发送成功标志位
        i2c_flag_clear(BSP_IIC,I2C_FLAG_ADDSEND);
 
        //使能应答功能，即接收时自动发送应答信号
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //等待数据寄存器非空标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
       //接收数据
   
        
        //发送停止信号
      
       //清除ADDSEND标志位
			 	
       

	if((ack&0x40)==0)
	{
		databuff[0]=i2c_data_receive (BSP_IIC);
		        //关闭应答信号，即接收时自动发送非应答信号
       // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //等待数据寄存器非空标志位置一
       // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		        //使能应答功能，即接收时自动发送应答信号
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //等待数据寄存器非空标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		databuff[1]=i2c_data_receive (BSP_IIC);
		        //关闭应答信号，即接收时自动发送非应答信号
        //i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //等待数据寄存器非空标志位置一
        //while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		        //使能应答功能，即接收时自动发送应答信号
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //等待数据寄存器非空标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		databuff[2]=i2c_data_receive (BSP_IIC);
		        //关闭应答信号，即接收时自动发送非应答信号
       // i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //等待数据寄存器非空标志位置一
       // while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
			         //使能应答功能，即接收时自动发送应答信号
        i2c_ack_config(BSP_IIC, I2C_ACK_ENABLE);
       //等待数据寄存器非空标志位置一
        while( !i2c_flag_get(BSP_IIC,I2C_FLAG_RBNE) );
		databuff[3]=i2c_data_receive (BSP_IIC);
		        //关闭应答信号，即接收时自动发送非应答信号
        //关闭应答信号，即接收时自动发送非应答信号
        i2c_ack_config(BSP_IIC, I2C_ACK_DISABLE);
        //等待数据寄存器非空标志位置一
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

