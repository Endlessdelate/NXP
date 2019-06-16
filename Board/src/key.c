#include "key.h"
#include "IIC.h"

KEY key = {0, 0};                       //存储按键值，和按键标志位
extern uint8 mode;

void PORTE_IRQHandler(void);           //按键中断函数
void Key_Handler(void);                //按键服务函数

void Key_Init(void)                    //按键初始化
{
	I2C1_SCL_H
	DELAY_MS(20);
	I2C1_SDA_H
	DELAY_MS(500);
	Key_Write(0x48,0x4d);
//板载拨码开关初始化
    key_init(KEY_MAX);

    port_init(Int_GPIO , ALT1 | IRQ_FALLING | PULLUP );           //中断引脚初始化
    set_vector_handler(Int_VECT ,PORTE_IRQHandler);          //配置中断
    enable_irq (Int_PORT);                                        //使能中断
}

void Key_Write(uint8 addr,uint8 dat)    //对ch455写指令
{
	IIC_start(I2C_1);
	IIC_send_byte(I2C_1,addr);
	IIC_send_byte(I2C_1,dat);
	IIC_stop(I2C_1);
}

uint8 Get_Keynum(void)                  //读取键值
{
    const uint8 table[12] = {87, 86, 85, 95, 94, 93, 103, 102, 101, 111, 110, 109};     //按键值转化表
	uint8 count = 0;

	key.key_value = Key_Read(0x4f);

    for(count = 0; count < 12; count ++)
    {
        if(table[count] == key.key_value)
        {
            key.key_flag = 1;
            break;
        }
    }
	return (count + 1);
}

uint8 Key_Read(uint8 addr)                  //读取ch455寄存器
{
	uint8 dat;

	IIC_start(I2C_1);
	IIC_send_byte(I2C_1,addr);
	dat=IIC_read_byte(I2C_1);
    DELAY_MS(1);
    IIC_stop(I2C_1);

    return dat;
}

void PORTE_IRQHandler(void)                 //按键中断函数
{
    PORT_FUNC(E, 26, Key_Handler);
}

void Key_Handler(void)                      //按键服务函数
{
    key.key_value = Get_Keynum();
    beep_on = 1;
}

void KEY_Mode()
{
  if(gpio_get(PTD1))
    mode =1;
  else if(gpio_get(PTD2))
    mode = 2;
  else if(gpio_get(PTD3))
    mode = 3;
  else if(gpio_get(PTD4))
    mode = 4;
  else{}
}