/*��
 * @file       ADC.c
 * @brief      ADC�ɼ�
 * @author     MAZY
 * @version    v1.0
 * @date       2019-04-03
 */
#include "include.h"
#include "ADC.h"

#define MAXQSIZE 6 //�ܴ�MAXQSIZE-1 = 5��Ԫ��
//#define OK 1
#define ERROR 0
#define OVERFLOW -1

float weight[MAXQSIZE-1] = {0.2 ,0.3 ,0.4 ,0.5 ,0.6};//Ȩ�ر�

int16 L_AD = 0,
       R_AD = 0,
       LS_AD = 0,
       RS_AD = 0,
       LM_AD = 0,
       RM_AD = 0,
       M_AD = 0,
       MS_AD = 0;
       


typedef uint32 Item;
//ѭ�����ж���
typedef struct 
{
    Item *base;
    int front;
    int rear;
}Queue;

Queue Value[8];

//��ʼ������
void InitQueue(Queue *q) 
{
    q->base = (uint32*)malloc(MAXQSIZE * sizeof(uint32));
    if (q->base == NULL)
        exit(OVERFLOW);
    q->front = 0;
    q->rear = MAXQSIZE-1;
}
//�ж϶����Ƿ���Ա
uint8 IsFull(Queue q) 
{
    return (q.rear + 1) % MAXQSIZE == q.front;
}
//�ж��Ƿ�Ϊ��
uint8 IsEmpty(Queue q) 
{
    return q.rear == q.front;
}
//���Ԫ�ص�β��
uint8 EnQueue(Queue *q, Item e) 
{
    if (IsFull(*q))
        return ERROR;
    q->base[q->rear] = e;
    q->rear = (q->rear + 1) % MAXQSIZE;

    return 1;
}
//ɾ���ײ�Ԫ��
uint8 DeQueue(Queue *q) 
{
    if(IsEmpty(*q))
        return ERROR;
    q->front = (q->front + 1) % MAXQSIZE;
    return 1;
}
//�����Ԫ�أ�ɾȥ��Ԫ��
void Add_Value(uint32 value,Queue *q)
{
    DeQueue(q);/* &q? */
    EnQueue(q, value);
}
uint8 QueueLength(Queue q) 
{
    return (q.rear - q.front + MAXQSIZE) % MAXQSIZE;
}
//��Ȩ���ȡƽ��
uint32 Get_Value_Sum(Queue *q)
{
    int i, j;
    uint32 sum = 0;
    for (i = 0, j = q->front; i < QueueLength(*q); i++, j = (j + 1) % MAXQSIZE) 
    {
        sum += q->base[j] * weight[i];
    }
    return sum/2;
}
/*!
 *  @brief      ADC��ʼ��
 *  @since      V1.0
 *  @note       
 */
void ADC_Init()
{
    uint8 i;
    adc_init(ADC1);
    adc_init(ADC2);
    adc_init(ADC3);
    adc_init(ADC4);
    adc_init(ADC5);
    adc_init(ADC6);
   //���2·
    adc_init(ADC7);
    adc_init(ADC8);
    //��ʼ����ж���
    for(i = 0;i<8;i++)
    {
        InitQueue(&Value[i]);
    }
}


uint32 Get_Ntimes_average(ADCn_Ch_e adcn_ch, ADC_nbit bit,uint8 N)
{
    uint32 value;
    uint8 i;
    for(i=0;i<N;i++)
    {
        value += adc_once(adcn_ch,bit);//get adc
    }
    return value/N;
}
/*!
 *  @brief      ��β���ADC���˲�
 *  @param      ADCn_Ch_e adcn_ch      ADCͨ��
 *  @param      ADC_nbit bit           ADC�ɼ�����
 *  @param      uint8 N                �ɼ�����
 *  @since      V1.0
 *  @note       
 */
void Get_Smoothed_ADC()
{
    Add_Value(Get_Ntimes_average(ADC1,ADC_10bit,3),&Value[0]);
    Add_Value(Get_Ntimes_average(ADC2,ADC_10bit,3),&Value[1]);
    Add_Value(Get_Ntimes_average(ADC3,ADC_10bit,3),&Value[2]);
    Add_Value(Get_Ntimes_average(ADC4,ADC_10bit,3),&Value[3]);
    Add_Value(Get_Ntimes_average(ADC5,ADC_10bit,3),&Value[4]);
    Add_Value(Get_Ntimes_average(ADC6,ADC_10bit,3),&Value[5]);
    Add_Value(Get_Ntimes_average(ADC7,ADC_10bit,3),&Value[6]);
    Add_Value(Get_Ntimes_average(ADC8,ADC_10bit,3),&Value[7]);
}
/*!
 *  @brief      �õ�ȫ�����ֵ
 *  @since      V1.0
 *  @note       
 */
void Read_ADC()
{
    Get_Smoothed_ADC();
    L_AD  = Get_Value_Sum(&Value[0]);
    R_AD  = Get_Value_Sum(&Value[1]);
    LS_AD = Get_Value_Sum(&Value[2]);
    RS_AD = Get_Value_Sum(&Value[3]);
    LM_AD = Get_Value_Sum(&Value[4]);
    RM_AD = Get_Value_Sum(&Value[5]);
    #if 0
    L_AD = 200,
    R_AD = 0,
    LS_AD = 100,
    RS_AD = 0;
    #endif
    #ifdef ADC_UP_DATA
    UP_Value[0] = L_AD;
    UP_Value[1] = R_AD;
    UP_Value[2] = LS_AD;
    UP_Value[3] = RS_AD;
    UP_Value[4] = LM_AD;
    UP_Value[5] = RM_AD;
    //vcan_sendware(UP_Value, sizeof(UP_Value));
    #endif
    
}