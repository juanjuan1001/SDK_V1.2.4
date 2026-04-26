#include "system/includes.h"
#include "syscfg_id.h"

void vm_demo_test(void)
{
    u8 rbuf[32];
    int ret, rlen;

    // 实际应用中请在枚举vm_id中添加vm id
    u32 test_id = CFG_GROUP_VM + 10;

    u8 wbuf[32];
    memset(wbuf, 0xaa, 32);

    // 测试写入16bytes后再读出
    memset(rbuf, 0, 32);
    printf("test write 16bytes 0xaa\n");
    ret = syscfg_write(test_id, wbuf, 16);
    if (ret <= 0) {
        printf("vm write err %d\n", ret);
        return;
    }
    rlen = syscfg_read(test_id, rbuf, 32);
    if (rlen != 16) {
        printf("rlen err %d\n", rlen);
        return;
    }
    printf_buf(rbuf, rlen);
    if (memcmp(wbuf, rbuf, rlen)) {
        printf("16bytes vm_test err\n");
        return;
    } else {
        printf("16bytes vm_test succ\n");
    }

    // 测试写入32bytes后再读出
    memset(rbuf, 0, 32);
    printf("test write 32bytes 0xaa\n");
    ret = syscfg_write(test_id, wbuf, 32);
    if (ret <= 0) {
        printf("vm write err %d\n", ret);
        return;
    }
    rlen = syscfg_read(test_id, rbuf, 32);
    if (rlen != 32) {
        printf("rlen err %d\n", rlen);
        return;
    }
    printf_buf(rbuf, rlen);
    if (memcmp(wbuf, rbuf, rlen)) {
        printf("32bytes vm_test err\n");
        return;
    } else {
        printf("32bytes vm_test succ\n");
    }
}





