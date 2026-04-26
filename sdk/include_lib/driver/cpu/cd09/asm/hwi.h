//*********************************************************************************//
// Module name : hwi.h                                                             //
// Description : j32CPU interrupt head file                                        //
// By Designer : zequan_liu                                                        //
// Dat changed :                                                                   //
//*********************************************************************************//
#ifndef __J32CPU_HWI__
#define __J32CPU_HWI__

#define IRQ_SDTAP_IDX       0
#define IRQ_EXCEPTION_IDX   1
#define IRQ_SYSCALL_IDX     2
#define IRQ_TICK_TMR_IDX    3
#define IRQ_TIME0_IDX       4
#define IRQ_TIME1_IDX       5
#define IRQ_TIME2_IDX       6
#define IRQ_UART0_IDX       7
#define IRQ_UART1_IDX       8
#define IRQ_UART2_IDX       9
#define IRQ_SPI0_IDX        10
#define IRQ_SPI1_IDX        11
#define IRQ_SPI2_IDX        28
#define IRQ_IIC0_IDX        12
#define IRQ_IIC1_IDX        13
#define IRQ_PORT_IDX        14
#define IRQ_SARADC_IDX      15
#define IRQ_GPCNT_IDX	    16
#define IRQ_USB_SOF_IDX     17
#define IRQ_USB_CTRL_IDX    18
#define IRQ_TIME3_IDX       19
#define IRQ_MCPWM_TMR_IDX   20
#define IRQ_PMU_SOFT_IDX    21
#define IRQ_PMU_TIMER0_IDX  22
#define IRQ_PMU_TIMER1_IDX  23
#define IRQ_QDEC_IDX        24
#define IRQ_PAP_IDX         25
#define IRQ_LCD_IDX         26
#define IRQ_MCPWM_CHX_IDX   27
#define IRQ_DMA_GEN0_IDX    32
#define IRQ_DMA_GEN1_IDX    33
#define IRQ_DMA0_IDX        34
#define IRQ_DMA1_IDX        35
#define IRQ_AUDIO_IDX       36
#define IRQ_APA_IDX         37
#define IRQ_HADC_IDX        38
#define IRQ_SOFT0_IDX       60
#define IRQ_SOFT1_IDX       61
#define IRQ_SOFT2_IDX       62
#define IRQ_SOFT3_IDX       63

#define MAX_IRQ_ENTRY_NUM   64

#define j32CPU_used
#define j32CPU_num          1
#define ISR_ENTRY           0x3f09f00
#define SSP_STACK_OFFSET    0x001000
#define SSP_STACK           (0x3f09f00-(j32CPU_num-1)*SSP_STACK_OFFSET)
#define USP_STACK           (0x3f08f00-(j32CPU_num-1)*SSP_STACK_OFFSET)

extern u32 _IRQ_MEM_ADDR[];

struct irq_info {
    u8 index;   //中断号
    u8 priority;//优先级
};

//---------------------------------------------//
// interrupt install
//---------------------------------------------//
void reg_set_ip(unsigned char index, unsigned char priority);
void bit_set_ie(unsigned char index);
void bit_clr_ie(unsigned char index);
void bit_set_swi(unsigned char index);
void bit_clr_swi(unsigned char index);
bool irq_read(u32 index);
void irq_info_table_set(const struct irq_info *table, u8 table_size);

void irq_unmask_set(u8 index);
void irq_unmask_disable(u8 index);

void interrupt_init(void);
void request_irq(u8 index, u8 priority, void (*handler)(void), u8 cpu_id);
void unrequest_irq(u8 index);

void exception_irq_handler(void);
void request_irq_rom(u8 index, u8 priority, void (*handler)(void), u8 cpu_id);

#define irq_disable(x)  bit_clr_ie(x)
#define irq_enable(x)   bit_set_ie(x)

//---------------------------------------------//
// core_num
//---------------------------------------------//

static inline int core_num(void)
{
#if (j32CPU_num>1)
    u32 num;
    asm volatile("%0 = sr12" : "=r"(num) :);   // special -> sr12 = ie0(unuse) = cnum
    return num;
#else
    return 0;
#endif
}



//---------------------------------------------//
// low power waiting
//---------------------------------------------//
static inline void lp_waiting(int *ptr, int pnd, int cpd, char inum)
{
    j32CPU(core_num())->IWKUP_NUM = inum;
    while (!(*ptr & pnd)) {
        asm volatile("idle");
    }
    *ptr |= cpd;
}

//*********************************************************************************//
//                                                                                 //
//                               end of this module                                //
//                                                                                 //
//*********************************************************************************//
#endif

