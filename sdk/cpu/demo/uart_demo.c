#include "system/includes.h"
#include "uart_v2.h"
#include "malloc.h"

static void uart_irq_func(int uart_num, enum uart_event event)
{
    if (event & UART_EVENT_TX_DONE) {
        printf("uart[%d] tx done\n", uart_num);
    }

    if (event & UART_EVENT_RX_DATA) {
        printf("uart[%d] rx data\n", uart_num);
    }

    if (event & UART_EVENT_RX_TIMEOUT) {
        printf("uart[%d] rx timerout data", uart_num);
    }

    if (event & UART_EVENT_RX_FIFO_OVF) {
        printf("uart[%d] rx fifo ovf\n", uart_num);
    }
}

struct uart_frame {
    u16 crc;
    u16 length;
    u8 data[0];
};
#define TEST_UART_IDX 1
struct uart_frame *frame_ptr;
void *uart_rx_ptr;
void uart_sync_demo_start(void)
{
    const struct uart_config config = {
        .baud_rate = 1000000,
        .tx_pin = IO_PORTC_08,
        .rx_pin = IO_PORTC_09,
        .parity = UART_PARITY_DISABLE,
        .tx_wait_mutex = 0,//1:不支持中断调用,互斥,0:支持中断,不互斥
    };

    uart_rx_ptr = malloc(128);
    frame_ptr = (struct uart_frame *)malloc(64);
    printf("uart_rx_ptr:%d\n", (u32)uart_rx_ptr);
    printf("frame_ptr:%d\n", (u32)frame_ptr);

    const struct uart_dma_config dma = {
        .rx_timeout_thresh = 3 * 10000000 / 1000000, //单位:us,公式：3*10000000/baud(ot:3个byte时间)
        .event_mask = UART_EVENT_TX_DONE | UART_EVENT_RX_TIMEOUT | UART_EVENT_RX_FIFO_OVF,
        .irq_callback = uart_irq_func,
        .irq_priority = 3,
        .rx_cbuffer = uart_rx_ptr,
        .rx_cbuffer_size = 128,
        .frame_size = 128,//=rx_cbuffer_size
    };

    printf("************uart demo***********\n");
    uart_dev uart_id = TEST_UART_IDX;
    int r;
    int ut = uart_init(uart_id, &config);
    if (ut < 0) {
        printf("uart(%d) init error\n", ut);
    } else {
        printf("uart(%d) init ok\n", ut);
    }
    r = uart_dma_init(uart_id, &dma);
    if (r < 0) {
        printf("uart(%d) dma init error\n", ut);
    } else {
        printf("uart(%d) dma init ok\n", ut);
    }
    uart_dump();
}

void uart_sync_demo_run(void)
{
    int r;
    if (frame_ptr) {
        r = uart_recv_blocking(TEST_UART_IDX, frame_ptr, 64, 10);
        if (r > 0) { //ok
            printf("r:%d\n", r);
            printf_buf((u8 *)frame_ptr, r);
        }
        r = uart_send_blocking(TEST_UART_IDX, frame_ptr, r, 20);
    }
}

void uart_sync_demo_stop(void)
{
    free(frame_ptr);
    free(uart_rx_ptr);
    uart_deinit(1);
    frame_ptr = NULL;
    uart_rx_ptr = NULL;
}



