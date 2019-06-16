#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/*
 * Include 用户自定义的头文件
 */
#include  "MK66_wdog.h"
#include  "MK66_gpio.h"     //IO口操作
#include  "MK66_uart.h"     //串口
#include  "MK66_i2c.h"      //I2C
#include  "MK66_spi.h"      //SPI
#include  "MK66_rtc.h"      //RTC
#include  "MK66_adc.h"      //ADC
#include  "MK66_dma.h"      //DMA
#include  "MK66_sdhc.h"     //SDHC


#include  "VCAN_LED.H"          //LED
#include  "VCAN_KEY.H"          //KEY
#include  "VCAN_MMA7455.h"      //三轴加速度MMA7455
#include  "VCAN_NRF24L0.h"      //无线模块NRF24L01+
#include  "VCAN_RTC_count.h"    //RTC 时间转换
#include  "VCAN_camera.h"       //摄像头总头文件
#include  "ff.h"                //FatFs
#include  "VCAN_TSL1401.h"      //线性CCD
#include  "VCAN_key_event.h"    //按键消息处理
#include  "VCAN_NRF24L0_MSG.h"  //无线模块消息处理

#include  "VCAN_BMP.h"          //BMP
#include  "vcan_img2sd.h"       //存储图像到sd卡一个文件
#include  "vcan_sd_app.h"       //SD卡应用（显示sd看上图片固件）

#include  "Vcan_touch.h"        //触摸驱动

#include  "VCAN_computer.h"     //多功能调试助手

#include  "enter.h"
#include  "define.h"

//用户函数头文件定义
#include "ADC.h"
#include "key.h"
#include "OLED.h"
#include "menu.h"
#include "Beep.h"
#include "Superware.h"
#include "Stop.h"

#endif  //__INCLUDE_H__



/**********************************************
*                   _ooOoo_                   *
*                  o8888888o                  * 
*                  88" . "88                  *
*                  (| -_- |)                  *
*                  O\  =  /O                  *
*               ____/`---'\____               * 
*             .'  \\|     |//  `.             *
*            /  \\|||  :  |||//  \            *
*           /  _||||| -:- |||||-  \           *
*           |   | \\\  -  /// |   |           *
*           | \_|  ''\---/''  |   |           *
*           \  .-\__  `-`  ___/-. /           *
*         ___`. .'  /--.--\  `. . __          *
*      ."" '<  `.___\_<|>_/___.'  >'"".       *
*     | | :  `- \`.;`\ _ /`;.`/ - ` : | |     *
*     \  \ `-.   \_ __\ /__ _/   .-` /  /     * 
*======`-.____`-.___\_____/___.-`____.-'======*
*                   `=---='                   *
*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*
*          佛祖保佑          永无BUG          *
\*********************************************/
/*佛曰:   写字楼里写字间，写字间里程序员； 
//        程序人员写程序，又拿程序换酒钱。 
//        酒醒只在网上坐，酒醉还来网下眠； 
//        酒醉酒醒日复日，网上网下年复年。 
//        但愿老死电脑间，不愿鞠躬老板前； 
//        奔驰宝马贵者趣，公交自行程序员。 
//        别人笑我忒疯癫，我笑自己命太贱； 
//        不见满街漂亮妹，哪个归得程序员？
*********************************************************************************************************/ 
