#include "gpio.h"
#include "asm/clock.h"
#include "asm/wdt.h"
#include "event.h"
#include "asm/segment_code_lcd_drv.h"

#define LOG_TAG_CONST   LCD
#define LOG_TAG         "[LCD_SEG]"
#include "debug.h"

struct segment_code_lcd_var {
    u8 init;
    u32 disp_buffer[6];//显示缓存,最大6com
    u32 save_buffer[6];//记忆当前显示的内容
    const struct segment_code_lcd_platform_data *usr_data;
};

static struct segment_code_lcd_var _lcd_var = {0};
#define __this      (&_lcd_var)

#define LCD_CHG_CON0_INIT \
        /* EN             1bit  */ (0<<0) | \
        /* LDOB_EN        1bit  */ (0<<1) | \
        /* HFREQ_EN       1bit  */ (1<<2) | \
        /* COMP_EN        1bit  */ (1<<3) | \
        /* PUMPCLK_EN     1bit  */ (1<<4) | \
        /* ADC_DET_EN     1bit  */ (0<<5) | \
        /* STANDBY_SETB   1bit  */ (1<<6) | \
        /* X2MODE_SETB    1bit  */ (1<<7)

//seg index 到 IO的映射表
static const u8 hw_seg2pin_mapping[27] = {
    IO_PORTC_04, IO_PORTC_05, IO_PORTC_06, IO_PORTC_07,
    IO_PORTC_08, IO_PORTC_09, IO_PORTC_10, IO_PORTC_11,
    IO_PORTC_12, IO_PORTC_13, IO_PORTC_14, IO_PORTC_15,
    IO_PORTD_00, IO_PORTD_01, IO_PORTD_02, IO_PORTD_03,
    IO_PORTD_04, IO_PORTD_05, IO_PORTD_06, IO_PORTD_07,
    IO_PORTD_08, IO_PORTD_09, IO_PORTD_10, IO_PORTD_11,
    IO_PORTD_12, IO_PORTD_13, IO_PORTD_14,
};

//com index 到 IO的映射表
static const u8 hw_com2pin_mapping[6] = {
    IO_PORTC_00, IO_PORTC_01, IO_PORTC_02, IO_PORTC_03, IO_PORTC_04, IO_PORTC_05,
};

#define LED_A   BIT(0)
#define LED_B   BIT(1)
#define LED_C   BIT(2)
#define LED_D   BIT(3)
#define LED_E   BIT(4)
#define LED_F   BIT(5)
#define LED_G   BIT(6)
//数字'0' ~ '9' '-'显示段码表
static const u8 lcd_seg_number_2_seg[12] = {
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F),        //'0'
    (u8)(LED_B | LED_C),                                        //'1'
    (u8)(LED_A | LED_B | LED_D | LED_E | LED_G),                //'2'
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_G),                //'3'
    (u8)(LED_B | LED_C | LED_F | LED_G),                        //'4'
    (u8)(LED_A | LED_C | LED_D | LED_F | LED_G),                //'5'
    (u8)(LED_A | LED_C | LED_D | LED_E | LED_F | LED_G),        //'6'
    (u8)(LED_A | LED_B | LED_C),                                //'7'
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F | LED_G),//'8'
    (u8)(LED_A | LED_B | LED_C | LED_D | LED_F | LED_G),        //'9'
    (u8)(LED_G),                                                //'-'
    (u8)(0),                                                    //清除
};

//字母'A' ~ 'Z'显示段码表
static const  u8 lcd_seg_large_letter_2_seg[26] = {
    0x77, 0x40, 0x39, 0x3f, 0x79, ///<ABCDE
    0x71, 0x40, 0x76, 0x06, 0x40, ///<FGHIJ
    0x40, 0x38, 0x40, 0x37, 0x3f, ///<KLMNO
    0x73, 0x40, 0x50, 0x6d, 0x78, ///<PQRST
    0x3e, 0x3e, 0x40, 0x76, 0x40, ///<UVWXY
    0x40 ///<Z
};

//字母'a' ~ 'z'显示段码表
static const  u8 lcd_seg_small_letter_2_seg[26] = {
    0x77, 0x7c, 0x58, 0x5e, 0x79, ///<abcde
    0x71, 0x40, 0x40, 0x40, 0x40, ///<fghij
    0x40, 0x38, 0x40, 0x54, 0x5c, ///<klmno
    0x73, 0x67, 0x50, 0x40, 0x78, ///<pqrst
    0x3e, 0x3e, 0x40, 0x40, 0x40, ///<uvwxy
    0x40 ///<z
};

static void segment_code_lcd_pump_init(void)
{
    gpio_set_mode(IO_PORT_SPILT(IO_PORTD_15), PORT_OUTPUT_HIGH);
    delay_us(500);

    LCDCP_WVBG_EN(1);//LCDCP_WVBG_EN
    delay_us(500);

    p33_tx_1byte(P3_LCD_CHG_CON0, LCD_CHG_CON0_INIT);
    LCDCP_OSCCAP_SEL(LCDCP_OSCCAP_SEL_1P2M);
    LCDCP_PUMPVOL_SEL(__this->usr_data->chgpump_vol);
    LCDCP_LDOB_SEL(__this->usr_data->ldob_vol);
    delay_us(500);

    gpio_set_mode(IO_PORT_SPILT(IO_PORTD_15), PORT_HIGHZ);
    LCDCP_PUMP_EN(1);
    delay_us(1500);

    LCDCP_LDOB_EN(1);
}

___interrupt
static void segment_code_lcd_isr(void)
{
    //帧中断
    if (LCD_GET_PENDING()) {
        LCD_CLR_PENDING();
        LCD_ENABLE(0);//en=0
        if (__this->usr_data->isr_cb) {
            __this->usr_data->isr_cb();
        } else {
            LCD_ENABLE(1);
            LCD_SW_KST(1);
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_set_ctu_en 设置LCD驱动为连续模式还是断续模式
   @param
            ctu_en: 0:断续模式 1:连续模式(CPU控制)
   @return  void
   @note    中途改变LCD驱动方式时可调用
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_set_ctu_en(u8 ctu_en)
{
    if (__this->init == 0) {
        return;
    }
    if (ctu_en) {
        LCD_IE_EN(0);//IE=0
        LCD_CTU_EN(1);//连续推屏
    } else {
        LCD_IE_EN(1);//IE=1
        LCD_CTU_EN(0);//断续推屏
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_set_dot_en 设置跳秒使能/禁能
   @param
            dot_en: 0:跳秒禁能 1:跳秒使能
   @return  void
   @note    跳秒显示切换时调用
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_set_dot_en(u8 dot_en)
{
    if (__this->init == 0) {
        return;
    }
    if (__this->usr_data->clk != LCD_CLK_SEL_OSL) {
        return;
    }
    LCD_DOT_EN(dot_en);
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_start 启动LCD模块
   @param   void
   @return  void
   @note    当使用断续推屏模式时,复用功能完成后,调用该接口重新启动推屏
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_start(void)
{
    if (__this->init == 0) {
        return;
    }
    LCD_ENABLE(1);
    LCD_SW_KST(1);
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_set_all_io_highz 把所有LCD配置的IO设置成高阻态
   @param   void
   @return  void
   @note    断续推屏恢复时可调用
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_set_all_io_highz(void)
{
    u8 i;
    if (__this->usr_data == NULL) {
        return;
    }
    for (i = 0; i < LCD_COM_MAX; i++) {
        if (__this->usr_data->lcd_com_en & BIT(i)) {
            gpio_set_mode(IO_PORT_SPILT(hw_com2pin_mapping[i]), PORT_HIGHZ);
        }
    }
    for (i = 0; i < LCD_SEG_MAX; i++) {
        if (__this->usr_data->lcd_seg_en & BIT(i)) {
            gpio_set_mode(IO_PORT_SPILT(hw_seg2pin_mapping[i]), PORT_HIGHZ);
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_segment_code_lcd_fill_point 填充一个点或者清除一个点
   @param
            icon: 点的位置, 高5位表示seg口,低3bit表示com口
            isfill: 0:清除 1:填充
   @return  void
   @note    需要填充一个点或者清除一个点时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_fill_point(u8 icon, u8 isfill)
{
    u8 com;
    u8 seg;
    com = icon & 0x07;
    if (com >= LCD_COM_MAX) {
        return;
    }
    seg = icon >> 3;
    if (seg >= LCD_SEG_MAX) {
        return;
    }
    if (isfill) {
        __this->disp_buffer[com] |= BIT(seg);
    } else {
        __this->disp_buffer[com] &= ~BIT(seg);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_fill_number 填充一个数字
   @param
            ptr:需要显示数字的7段LCD对应的点位置的数组
            num: 需要填充的数字,num >= [数组大小] 时清除显示
   @return  void
   @note    需要填充数字时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_fill_number(const u8 *ptr, u8 num)
{
    if (ptr == NULL) {
        return;
    }
    if (num >= sizeof(lcd_seg_number_2_seg)) {
        num = sizeof(lcd_seg_number_2_seg) - 1;
    }
    segment_code_lcd_fill_point(ptr[0], lcd_seg_number_2_seg[num]&LED_A);
    segment_code_lcd_fill_point(ptr[1], lcd_seg_number_2_seg[num]&LED_B);
    segment_code_lcd_fill_point(ptr[2], lcd_seg_number_2_seg[num]&LED_C);
    segment_code_lcd_fill_point(ptr[3], lcd_seg_number_2_seg[num]&LED_D);
    segment_code_lcd_fill_point(ptr[4], lcd_seg_number_2_seg[num]&LED_E);
    segment_code_lcd_fill_point(ptr[5], lcd_seg_number_2_seg[num]&LED_F);
    segment_code_lcd_fill_point(ptr[6], lcd_seg_number_2_seg[num]&LED_G);
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_fill_char 填充一个英文字符
   @param
            ptr:需要显示英文字符的7段LCD对应的点位置的数组
            cc: 需要填充的英文字符, 'a'~'z' 或 'A'~'Z'
   @return  void
   @note    需要填充英文字符时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_fill_char(const u8 *ptr, u8 cc)
{
    u8 num;
    if (ptr == NULL) {
        return;
    }
    if (('a' <= cc) && (cc <= 'z')) {
        num = cc - 'a';
        segment_code_lcd_fill_point(ptr[0], lcd_seg_small_letter_2_seg[num]&LED_A);
        segment_code_lcd_fill_point(ptr[1], lcd_seg_small_letter_2_seg[num]&LED_B);
        segment_code_lcd_fill_point(ptr[2], lcd_seg_small_letter_2_seg[num]&LED_C);
        segment_code_lcd_fill_point(ptr[3], lcd_seg_small_letter_2_seg[num]&LED_D);
        segment_code_lcd_fill_point(ptr[4], lcd_seg_small_letter_2_seg[num]&LED_E);
        segment_code_lcd_fill_point(ptr[5], lcd_seg_small_letter_2_seg[num]&LED_F);
        segment_code_lcd_fill_point(ptr[6], lcd_seg_small_letter_2_seg[num]&LED_G);
    } else if (('A' <= cc) && (cc <= 'Z')) {
        num = cc - 'A';
        segment_code_lcd_fill_point(ptr[0], lcd_seg_large_letter_2_seg[num]&LED_A);
        segment_code_lcd_fill_point(ptr[1], lcd_seg_large_letter_2_seg[num]&LED_B);
        segment_code_lcd_fill_point(ptr[2], lcd_seg_large_letter_2_seg[num]&LED_C);
        segment_code_lcd_fill_point(ptr[3], lcd_seg_large_letter_2_seg[num]&LED_D);
        segment_code_lcd_fill_point(ptr[4], lcd_seg_large_letter_2_seg[num]&LED_E);
        segment_code_lcd_fill_point(ptr[5], lcd_seg_large_letter_2_seg[num]&LED_F);
        segment_code_lcd_fill_point(ptr[6], lcd_seg_large_letter_2_seg[num]&LED_G);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_disp_clear 清屏
   @param   void
   @return  void
   @note    需要清屏时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_disp_clear(void)
{
    memset(__this->disp_buffer, 0, sizeof(__this->disp_buffer));
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_disp_all 全显
   @param   void
   @return  void
   @note    需要全显时调用,该接口不会更新到硬件寄存器
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_disp_all(void)
{
    u8 i, j;
    if (__this->usr_data  == NULL) {
        return;
    }
    for (i = 0; i < LCD_COM_MAX; i++) {
        if (__this->usr_data->lcd_com_en & BIT(i)) {
            for (j = 0; j < LCD_SEG_MAX; j++) {
                if (__this->usr_data->lcd_seg_en & BIT(j)) {
                    __this->disp_buffer[i] |= BIT(j);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_disp_update 更新显示缓存
   @param   void
   @return  void
   @note    需要把缓存更新到寄存器时调用
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_disp_update(void)
{
    u8 i;
    if (__this->usr_data  == NULL) {
        return;
    }
    for (i = 0; i < LCD_COM_MAX; i++) {
        if (__this->usr_data->lcd_com_en & BIT(i)) {
            //缓存和显示不一致时才刷新
            if (__this->save_buffer[i] == __this->disp_buffer[i]) {
                continue;
            }
            __this->save_buffer[i] = __this->disp_buffer[i];
            if (i < 4) {
                p33_tx_nbyte(P3_SEG0_DAT0 + (i - 0) * 4, (u8 *)&__this->disp_buffer[i], 4);
            } else {
                p33_tx_nbyte(P3_SEG4_DAT0 + (i - 4) * 4, (u8 *)&__this->disp_buffer[i], 4);
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_init lcd断码屏驱动初始化接口
   @param
			lcd_dat:初始化配置传参
   @return  void
   @note    先初始化之后才能调用其他api
            如果使用外部晶振,需要再rtc_dev_init后初始化断码屏驱动
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_init(const struct segment_code_lcd_platform_data *lcd_dat)
{
    u8 temp = 0;
    if ((__this->init) || (lcd_dat == NULL)) {
        return;
    }

    __this->usr_data = lcd_dat;

    for (temp = P3_LCDC_CON0; temp <= P3_LCDC_CON7; temp++) {
        p33_tx_1byte(temp, 0);
    }
    for (temp = P3_SEG_IO_EN0; temp <= P3_SEG_IO_EN3; temp++) {
        p33_tx_1byte(temp, 0);
    }

    //LCDCP初始化
    segment_code_lcd_pump_init();

    //配置LCD时钟
    LCD_CLK_SEL(__this->usr_data->clk);
    if (__this->usr_data->clk == LCD_CLK_SEL_LRC) {
        LCD_CLK_DIV(5);// 200k/(5+1) = 33K
    } else if (__this->usr_data->clk == LCD_CLK_SEL_OSL) {
        //晶振统一由RTC模块进行初始化
        /* gpio_set_mode(PORTA, PORT_PIN_7 | PORT_PIN_8, PORT_HIGHZ); */
        /* p33_or_1byte(P3_OSL_CON, BIT(0)); */
        /* p33_or_1byte(P3_OSL_CON, BIT(1)); */
    }
    LCD_CLK_EN(1);

    //配置LCD IO设置成高阻态
    segment_code_lcd_set_all_io_highz();

    //配置VLCD、偏压、充电模式等
    LCD_VOL_SEL(__this->usr_data->vlcd);
    LCD_BIAS_SEL(__this->usr_data->bias);
    LCD_SET_CHGDUTY(__this->usr_data->chgduty);
    LCD_SET_CHGMODE(__this->usr_data->chgmode);
    LCD_CHG_CURR(1);
    LCD_DEADBAND_EN(1);

    //seg使能
    p33_tx_nbyte(P3_SEG_IO_EN0, (u8 *)&__this->usr_data->lcd_seg_en, 4);

    //com使能
    LCD_COM_IO_EN(__this->usr_data->lcd_com_en);

    //跳秒配置 -- 使用OSL_CLK时跳秒才有效
    if (__this->usr_data->clk == LCD_CLK_SEL_OSL) {
        LCD_DOT_COM_SEL(__this->usr_data->dot_com);
        LCD_DOT_SEG_SEL(__this->usr_data->dot_seg);
    }

    //中断配置
    LCD_CLR_PENDING();
    if (__this->usr_data->isr_cb) {
        request_irq(IRQ_LCD_IDX, 0, segment_code_lcd_isr, 0);
    }

    //连续推屏 or 断续推屏
    if (__this->usr_data->ctu_en) {
        LCD_IE_EN(0);//IE=0
        LCD_CTU_EN(1);//连续推屏
    } else {
        LCD_IE_EN(1);//IE=1
        LCD_CTU_EN(0);//断续推屏
    }

    memset(__this->save_buffer, 0xff, sizeof(__this->save_buffer));

    //清屏
    segment_code_lcd_disp_clear();
    segment_code_lcd_disp_update();

    //启动LCD控制器
    LCD_ENABLE(1);
    LCD_SW_KST(1);

    __this->init = 1;
}

/*----------------------------------------------------------------------------*/
/**@brief   segment_code_lcd_deinit lcd断码屏驱动初始化取消
   @param   void
   @return  void
   @note    需要关闭LCD断码屏模块时调用
*/
/*----------------------------------------------------------------------------*/
void segment_code_lcd_deinit(void)
{
    u8 temp = 0;
    if (!__this->init) {
        return;
    }
    for (temp = P3_LCDC_CON0; temp <= P3_LCDC_CON7; temp++) {
        p33_tx_1byte(temp, 0);
    }
    for (temp = P3_SEG_IO_EN0; temp <= P3_SEG_IO_EN3; temp++) {
        p33_tx_1byte(temp, 0);
    }
    LCDCP_WVBG_EN(0);
    LCD_CLK_EN(0);
    p33_tx_1byte(P3_LCD_CHG_CON0, 0);
    p33_tx_1byte(P3_LCD_CHG_CON1, 0);
    p33_tx_1byte(P3_LCD_CHG_CON2, 0);
    segment_code_lcd_set_all_io_highz();
    __this->init = 0;
}

