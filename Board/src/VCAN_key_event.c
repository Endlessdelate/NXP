/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       VCAN_key_event.h
 * @brief      KEY �¼�����ʵ��
 * @author     ɽ��Ƽ�
 * @version    v6.0
 * @date       2017-11-04
 */

#include "VCAN_key_event.h"
#include "VCAN_UI_VAR.h"
//#include "NRF24L0.h"
//#include "NRF24L0_MSG.h"


void key_event_init()
{
    //��ʼ�� ȫ�� ����
    key_init(KEY_MAX);

    //��ʼ����ʱ����
    var_init();

    //ͬ��ȫ�����ݲ���ʾ
    var_syn(VAR_MAX);       //ͬ��ȫ�� ,������ͬ������ʾȫ������Ϊ�п���ͬ��ʧ�ܡ�
    var_display(VAR_MAX);   //��ʾȫ��
}

void deal_key_event()
{
    KEY_MSG_t keymsg;

    while(get_key_msg(& keymsg))     //��ð����ͽ��д���
    {
        if(keymsg.status == KEY_DOWN)
        {
            switch(keymsg.key)
            {
            case KEY_1:
                var_value(VAR_ADD);
                break;

            case KEY_2:
                var_value(VAR_SUB);
                break;

            case KEY_3:
                var_select(VAR_PREV);
                break;

            case KEY_4:
                var_select(VAR_NEXT);
                break;

            case KEY_5:
                var_ok();
                break;

            case KEY_6:
                val_cancel();
                break;

            case KEY_7:
                save_var(CAR_CTRL,CAR_START);
                var_syn(CAR_CTRL);
                var_display(CAR_CTRL);
                break;
            case KEY_8:
                save_var(CAR_CTRL,CAR_STOP);
                var_syn(CAR_CTRL);
                var_display(CAR_CTRL);
                break;

            default:
                break;
            }
        }
        else if(keymsg.status == KEY_HOLD)
        {
            switch(keymsg.key)
            {
            case KEY_1:
                var_value(VAR_ADD_HOLD);
                break;

            case KEY_2:
                var_value(VAR_SUB_HOLD);
                break;

            case KEY_3:
                var_select(VAR_PREV_HOLD);
                break;

            case KEY_4:
                var_select(VAR_NEXT_HOLD);
                break;

            case KEY_6:                //���� OK �� ͬ��ȫ�����ݲ���ʾ
                var_syn(VAR_MAX);       //ͬ��ȫ�� ,������ͬ������ʾȫ������Ϊ�п���ͬ��ʧ�ܡ�
                var_display(VAR_MAX);   //��ʾȫ��
                break;

            case KEY_7:
                save_var(CAR_CTRL,CAR_START);
                while(var_syn(CAR_CTRL)==0);
                var_display(CAR_CTRL);
                break;

            case KEY_8:
                save_var(CAR_CTRL,CAR_STOP);
                while(var_syn(CAR_CTRL)==0);
                var_display(CAR_CTRL);

            default:            //����Ҫ���� cancel
                break;
            }
        }
        else
        {
            //KEY_UP ,�����д���
        }
    }
}





