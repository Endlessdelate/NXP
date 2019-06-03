#include "key.h"
#include "IIC.h"

KEY key = {0, 0};                       //�洢����ֵ���Ͱ�����־λ
extern uint8 mode;

void PORTE_IRQHandler(void);           //�����жϺ���
void Key_Handler(void);                //����������

void Key_Init(void)                    //������ʼ��
{
	I2C1_SCL_H
	DELAY_MS(20);
	I2C1_SDA_H
	DELAY_MS(500);
	Key_Write(0x48,0x4d);
//���ز��뿪�س�ʼ��
    key_init(KEY_MAX);

    port_init(Int_GPIO , ALT1 | IRQ_FALLING | PULLUP );           //�ж����ų�ʼ��
    set_vector_handler(Int_VECT ,PORTE_IRQHandler);          //�����ж�
    enable_irq (Int_PORT);                                        //ʹ���ж�
}

void Key_Write(uint8 addr,uint8 dat)    //��ch455дָ��
{
	IIC_start(I2C_1);
	IIC_send_byte(I2C_1,addr);
	IIC_send_byte(I2C_1,dat);
	IIC_stop(I2C_1);
}

uint8 Get_Keynum(void)                  //��ȡ��ֵ
{
    const uint8 table[12] = {109, 110, 111, 101, 102, 103, 93, 94, 95, 85, 86, 87};     //����ֵת����
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

uint8 Key_Read(uint8 addr)                  //��ȡch455�Ĵ���
{
	uint8 dat;

	IIC_start(I2C_1);
	IIC_send_byte(I2C_1,addr);
	dat=IIC_read_byte(I2C_1);
    DELAY_MS(1);
    IIC_stop(I2C_1);

    return dat;
}

void PORTE_IRQHandler(void)                 //�����жϺ���
{
    PORT_FUNC(E, 26, Key_Handler);
}

void Key_Handler(void)                      //����������
{
    key.key_value = Get_Keynum();
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