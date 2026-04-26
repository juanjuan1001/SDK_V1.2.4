#ifndef SEGMENT_CODE_LCD_DRV_H
#define SEGMENT_CODE_LCD_DRV_H

#include "generic/typedef.h"
#include "asm/power/p33_sfr.h"
#include "asm/power/p33_interface.h"

/*
 *  断码屏驱动,最大支持 4*27 或 6*25
 *
 *  使用流程：
 *  1、LCD IO不复用
 *      (1):配置使用到的LCD COM和SEG IO高阻输入,且DIE=0 DIEH=0
 *      (2):配置LCD显示像素点寄存器
 *      (3):配置LCD模式配置
 *      (4):LCD配置成连续推屏模式
 *      (5):开启LCD使能位
 *      (6):写入软件KIST START BIT(KST)
 *      (7):LCD模块开始工作,接管IO
 *
 *  2、LCD IO复用切换
 *      (1):LCD配置成断续推屏模式
 *      (2):等待LCD当前帧结束pending或者中断
 *      (3):关闭LCD模块使能位
 *      (4):LCD模块停止工作,COM IO保持高阻输入
 *      (5):SEG IO可释放,复用做其他功能
 *      (6):其他功能完成后,恢复SEG IO为高阻输入,继续推屏
 *      (7):开启LCD模块使能位
 *      (8):写入软件KIST START BIT(KST)
 *      (9):LCD模块开始工作,接管IO,下一步到(2)
 *
 *  3、LCD IO不复用切换
 *      (1):LCD配置成断续推屏模式
 *      (2):等待LCD当前帧结束pending或者中断
 *      (3):关闭LCD模块使能位
 *      (4):LCD模块停止工作,COM IO保持高阻输入
 *      (5):SEG IO可释放,复用做其他功能
 *      (6):其他功能完成后,恢复SEG IO为高阻输入
 *      (7):LCD配置成连续推屏模式
 *      (8):开启LCD模块使能位
 *      (9):写入软件KIST START BIT(KST)
 *      (10):LCD模块开始工作,接管IO
 *  注意,复用条件:
 *      (1):LCD IO复用其他功能时,LCD的COM口需要保持高阻状态
 *      (2):LCD IO驱动过程中,会翻转,因此其他功能复用需要考虑协议相互干扰
 */

/************************P3_CHG_PUMP*****************************/
#define LCDCP_WVBG_EN(en)           P33_CON_SET(P3_CHG_PUMP, 1, 1, en);  //LCDCP_WVBG_EN

/************************P3_LCD_CHG_CON0*****************************/
#define LCDCP_PUMP_EN(en)           P33_CON_SET(P3_LCD_CHG_CON0, 0, 1, en) //LCD charge pump enable bit
#define LCDCP_LDOB_EN(en)           P33_CON_SET(P3_LCD_CHG_CON0, 1, 1, en) //AVDDBLDO enable for LCDDRV
#define LCDCP_HFREQ_EN(en)          P33_CON_SET(P3_LCD_CHG_CON0, 2, 1, en) //increase ISC clk frequency
#define LCDCP_COMP_EN(en)           P33_CON_SET(P3_LCD_CHG_CON0, 3, 1, en) //LCD CHARGE PUMP voltage comparator enable
#define LCDCP_PUMPCLK_EN(en)        P33_CON_SET(P3_LCD_CHG_CON0, 4, 1, en) //LCD CHARGE PUMP clk enable
#define LCDCP_ADC_DET_EN(en)        P33_CON_SET(P3_LCD_CHG_CON0, 5, 1, en) //1/4 vlcd voltage detect enable
#define LCDCP_STANDBY_SET_BIT(en)   P33_CON_SET(P3_LCD_CHG_CON0, 6, 1, en) //Mode select 0 to be STANBBY MODE
#define LCDCP_X2MODE_SET_BIT(en)    P33_CON_SET(P3_LCD_CHG_CON0, 7, 1, en) //MODE select 0:2XMODE,VLCD=2XVDDIO

/************************P3_LCD_CHG_CON1*****************************/
//LCDCP_OSCCAP_S
enum {
    LCDCP_OSCCAP_SEL_1P9M = 0x0,
    LCDCP_OSCCAP_SEL_1P5M = 0x1,
    LCDCP_OSCCAP_SEL_1P2M = 0x3,//default
    LCDCP_OSCCAP_SEL_1P0M = 0x7,
    LCDCP_OSCCAP_SEL_0P9M = 0xf,
};
#define LCDCP_OSCCAP_SEL(sel)       P33_CON_SET(P3_LCD_CHG_CON1, 0, 4, sel)

//LCDCP_PUMPVOL_S
enum lcdcp_pmup_vol {
    LCDCP_PUMPVOL_SEL_3P0V,
    LCDCP_PUMPVOL_SEL_3P1V,
    LCDCP_PUMPVOL_SEL_3P2V,
    LCDCP_PUMPVOL_SEL_3P3V,
    LCDCP_PUMPVOL_SEL_3P4V,
    LCDCP_PUMPVOL_SEL_3P5V,
    LCDCP_PUMPVOL_SEL_3P6V,//default
    LCDCP_PUMPVOL_SEL_3P7V,
    LCDCP_PUMPVOL_SEL_3P8V,
    LCDCP_PUMPVOL_SEL_3P9V,
    LCDCP_PUMPVOL_SEL_4P0V,
    LCDCP_PUMPVOL_SEL_4P1V,
    LCDCP_PUMPVOL_SEL_4P2V,
    LCDCP_PUMPVOL_SEL_4P3V,
    LCDCP_PUMPVOL_SEL_4P4V,
    LCDCP_PUMPVOL_SEL_4P5V,
};
#define LCDCP_PUMPVOL_SEL(sel)      P33_CON_SET(P3_LCD_CHG_CON1, 4, 4, sel)

/************************P3_LCD_CHG_CON2*****************************/
//LCDCP_LDOB_S
enum lcdcp_ldob_vol {
    LCDCP_LDOB_SEL_2P7V,
    LCDCP_LDOB_SEL_3P0V,
    LCDCP_LDOB_SEL_3P3V, //default
    LCDCP_LDOB_SEL_3P6V,
};
#define LCDCP_LDOB_SEL(sel)         P33_CON_SET(P3_LCD_CHG_CON2, 0, 2, sel)

/************************P3_CLK_CON0*****************************/
//LCD时钟选择
enum lcd_clk {
    LCD_CLK_SEL_OSL,    //OSC 32K -- 有外挂晶振时选他
    LCD_CLK_SEL_WCLK,   //WCLK 32K
    LCD_CLK_SEL_LRC,    //LRC 200K, DIV6
    LCD_CLK_SEL_PAT,    //一般不选
};
#define LCD_CLK_SEL(sel)            P33_CON_SET(P3_CLK_CON0, 6, 2, sel)
#define LCD_CLK_EN(en)              P33_CON_SET(P3_CLK_CON0, 5, 1, en)

/************************P3_LCDC_CON0*****************************/
//VLCDS
enum lcd_vol {
    LCD_VOL_SEL_0P886XLCDCP,
    LCD_VOL_SEL_0P902XLCDCP,
    LCD_VOL_SEL_0P918XLCDCP,
    LCD_VOL_SEL_0P934XLCDCP,
    LCD_VOL_SEL_0P950XLCDCP,
    LCD_VOL_SEL_0P967XLCDCP,
    LCD_VOL_SEL_0P984XLCDCP,
    LCD_VOL_SEL_1P000XLCDCP,
};
#define LCD_VOL_SEL(sel)            P33_CON_SET(P3_LCDC_CON0, 4, 3, sel)

//BIAS
enum lcd_bias {
    LCD_BIAS_DISABLE,   //模拟模块禁止工作
    LCD_BIAS_1DIV2,     //1/2 bias
    LCD_BIAS_1DIV3,     //1/3 bias
    LCD_BIAS_1DIV4,     //1/4 bias
};
#define LCD_BIAS_SEL(sel)           P33_CON_SET(P3_LCDC_CON0, 2, 2, sel)
#define LCD_DOT_EN(en)              P33_CON_SET(P3_LCDC_CON0, 1, 1, en) //DOTEN
#define LCD_ENABLE(en)              P33_CON_SET(P3_LCDC_CON0, 0, 1, en) //LCDEN

/************************P3_LCDC_CON1*****************************/
//CHGDUTY
#define LCD_SET_CHGDUTY(duty)       P33_CON_SET(P3_LCDC_CON1, 4, 4, duty)
//CHG_CURR
#define LCD_CHG_CURR(sel)           P33_CON_SET(P3_LCDC_CON1, 2, 2, sel)
//CHGMODE
enum lcd_chgmode {
    LCD_CHGMODE_WAKENESS,   //一直使用弱充电模式
    LCD_CHGMODE_STRONG,     //一直使用强充电模式
    LCD_CHGMODE_HALF_CYCLE, //半cycle充电模式,在状态切换时开始转强驱,0.5个32KHz时钟后撤销强驱动
    LCD_CHGMODE_MANY_CYCLE, //多cycle充电模式,经过CHGDUTY+1个32KHz时钟后撤销强驱动
};
#define LCD_SET_CHGMODE(mode)       P33_CON_SET(P3_LCDC_CON1, 0, 2, mode)

/************************P3_LCDC_CON2*****************************/
#define LCD_DEADBAND_EN(en)         P33_CON_SET(P3_LCDC_CON2, 5, 1, en) //死区控制使能
#define LCD_TEST_EN(en)             P33_CON_SET(P3_LCDC_CON2, 4, 1, en) //开启测试模式,IO直接输出LCD静态电平
#define LCD_CLK_DIV(div)            P33_CON_SET(P3_LCDC_CON2, 0, 2, div)//模块时钟预分频选择;(CLK_DIV + 1)分频

/************************P3_LCDC_CON3*****************************/
#define LCD_GET_PENDING()          (p33_rx_1byte(P3_LCDC_CON3) & BIT(7))    //获取PND状态
#define LCD_CLR_PENDING()           p33_or_1byte(P3_LCDC_CON3, BIT(6))      //清除PND状态
#define LCD_SW_KST(en)              P33_CON_SET(P3_LCDC_CON3, 4, 1, en)     //开启测试模式,IO直接输出LCD静态电平
#define LCD_IE_EN(en)               P33_CON_SET(P3_LCDC_CON3, 1, 1, en)     //断续推屏模式下,完成一帧后,LCD帧中断使能位
#define LCD_CTU_EN(en)              P33_CON_SET(P3_LCDC_CON3, 0, 1, en)     //连续推屏使能位:
//0:断续推屏模式(完成一帧后起帧pending,同时停止推屏)
//1:连续推屏模式(完成一帧后不会起帧pending,硬件会自动继续进行下一帧的推屏)

/************************P3_LCDC_CON4*****************************/
#define LCD_COM_IO_EN(en)           P33_CON_SET(P3_LCDC_CON4, 0, 6, en)     //对应bit0~bit5对应com0~com5的使能

/************************P3_LCDC_CON5*****************************/
#define LCD_DOT_SEG_SEL(sel)        P33_CON_SET(P3_LCDC_CON5, 3, 5, sel)    //跳秒脚SEG口选择
#define LCD_DOT_COM_SEL(sel)        P33_CON_SET(P3_LCDC_CON5, 0, 3, sel)    //跳秒脚COM口选择

/************************P3_LCDC_CON6*****************************/
enum lcd_seg_test_output {
    LCD_SEG_TEST_OUTPUT_LCDVDD = BIT(0),
    LCD_SEG_TEST_OUTPUT_LCDVL2 = BIT(1),
    LCD_SEG_TEST_OUTPUT_LCDVL1 = BIT(2),
    LCD_SEG_TEST_OUTPUT_LCDVL0 = BIT(3),
    LCD_SEG_TEST_OUTPUT_PD     = BIT(4),
};
#define LCD_SEG_TEST_FLAG(flag)     P33_CON_SET(P3_LCDC_CON6, 0, 5, flag)   //测试模式通道使能,SEG口IO直接输出LCD对应通道

/************************P3_LCDC_CON7*****************************/
enum lcd_com_test_output {
    LCD_COM_TEST_OUTPUT_LCDVDD = BIT(0),
    LCD_COM_TEST_OUTPUT_LCDVL2 = BIT(1),
    LCD_COM_TEST_OUTPUT_LCDVL1 = BIT(2),
    LCD_COM_TEST_OUTPUT_LCDVL0 = BIT(3),
    LCD_COM_TEST_OUTPUT_PD     = BIT(4),
};
#define LCD_COM_TEST_FLAG(flag)     P33_CON_SET(P3_LCDC_CON7, 0, 5, flag)   //测试模式通道使能,COM口IO直接输出LCD对应通道

enum lcd_seg_index {
    LCD_SEG0 = 0,
    LCD_SEG1,
    LCD_SEG2,
    LCD_SEG3,
    LCD_SEG4,
    LCD_SEG5,
    LCD_SEG6,
    LCD_SEG7,
    LCD_SEG8,
    LCD_SEG9,
    LCD_SEG10,
    LCD_SEG11,
    LCD_SEG12,
    LCD_SEG13,
    LCD_SEG14,
    LCD_SEG15,
    LCD_SEG16,
    LCD_SEG17,
    LCD_SEG18,
    LCD_SEG19,
    LCD_SEG20,
    LCD_SEG21,
    LCD_SEG22,
    LCD_SEG23,
    LCD_SEG24,
    LCD_SEG25,
    LCD_SEG26,
    LCD_SEG_MAX,
};

enum lcd_com_index {
    LCD_COM0 = 0,
    LCD_COM1,
    LCD_COM2,
    LCD_COM3,
    LCD_COM4,
    LCD_COM5,
    LCD_COM_MAX,
};

//描述一个点
#define LCD_POINT(com, seg)     (seg << 3 | com)
//把某个点的com设置成INVALID_COM,则该点无效
#define INVALID_COM             0x07

//lcd初始化结构体
struct segment_code_lcd_platform_data {
    enum lcd_clk clk;               //时钟配置
    enum lcdcp_pmup_vol chgpump_vol;//charge pump电压配置
    enum lcdcp_ldob_vol ldob_vol;   //ldob电压配置
    enum lcd_vol vlcd;              //电压分压设置
    enum lcd_bias bias;             //偏压设置
    enum lcd_chgmode chgmode;       //充电模式设置
    u8   chgduty;                   //强充时间设置,当chgmode==LCD_CHGMODE_MANY_CYCLE时有效
    enum lcd_com_index dot_com;     //跳秒脚对应的com口(使用OSL_CLK时钟时才有效)
    enum lcd_seg_index dot_seg;     //跳秒脚对应的seg口(使用OSL_CLK时钟时才有效)
    u8   lcd_com_en;                //设置使用的com的IO, bit0~bit5 对应 com0~com5
    u32  lcd_seg_en;                //设置使用的seg的IO, bit0~bit26 对应 seg0~seg26
    u8   ctu_en;                    //1:连续模式(CPU控制) 0:断续模式(ie=1)
    void (*isr_cb)(void);           //断续模式时,中断回调函数
};

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_init lcd断码屏驱动初始化接口
   @param
			lcd_dat:初始化配置传参
   @return  void
   @note    先初始化之后才能调用其他api
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_init(const struct segment_code_lcd_platform_data *lcd_dat);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_deinit lcd断码屏驱动初始化取消
   @param   void
   @return  void
   @note    需要关闭LCD断码屏模块时调用
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_deinit(void);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_set_ctu_en 设置LCD驱动为连续模式还是断续模式
   @param
            ctu_en: 0:断续模式 1:连续模式(CPU控制)
   @return  void
   @note    中途改变LCD驱动方式时可调用
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_set_ctu_en(u8 ctu_en);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_set_dot_en 设置跳秒使能/禁能
   @param
            dot_en: 0:跳秒禁能 1:跳秒使能
   @return  void
   @note    跳秒显示切换时调用
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_set_dot_en(u8 dot_en);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_start 启动LCD模块
   @param   void
   @return  void
   @note    当使用断续推屏模式时,复用功能完成后,调用该接口重新启动推屏
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_start(void);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_fill_point 填充一个点或者清除一个点
   @param
            icon: 点的位置, 高5位表示seg口,低3bit表示com口
            isfill: 0:清除 1:填充
   @return  void
   @note    需要填充一个点或者清除一个点时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_fill_point(u8 icon, u8 isfill);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_fill_number 填充一个数字
   @param
            ptr[]:需要显示数字的7段LCD对应的点位置的数组
            num: 需要填充的数字,num > 11时清除显示
   @return  void
   @note    需要填充数字时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_fill_number(const u8 ptr[], u8 num);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_fill_char 填充一个英文字符
   @param
            ptr[]:需要显示英文字符的7段LCD对应的点位置的数组
            cc: 需要填充的英文字符, 'a'~'z' 或 'A'~'Z'
   @return  void
   @note    需要填充英文字符时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_fill_char(const u8 ptr[], u8 cc);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_disp_clear 清屏
   @param   void
   @return  void
   @note    需要清屏时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_disp_clear(void);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_disp_all 全显
   @param   void
   @return  void
   @note    需要全显时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_disp_all(void);

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_disp_update 更新显示缓存
   @param   void
   @return  void
   @note    需要把缓存更新到寄存器时调用
*/
/*----------------------------------------------------------------------------*/
extern void segment_code_lcd_disp_update(void);

#endif


