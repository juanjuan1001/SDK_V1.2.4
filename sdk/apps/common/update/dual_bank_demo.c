#include "fs/fs.h"
#include "system/includes.h"
#include "app_config.h"
#include "app_main.h"
#include "asm/sfc_norflash_api.h"
#include "dual_bank_updata_api.h"
#include "asm/crc16.h"
#include "boot.h"

#if (TCFG_DUAL_BANK_ENABLE && TCFG_DB_UPDATE_PASSIVE_DEMO_EN)
extern void wdt_clear(void);
static u8 tmp_buf[256];

static int dual_write_complete_cb(void *priv)
{
    u32 ret = (u32)priv;
    if (ret) {
        printf("err:update_write fail, %x\n", ret);
        return ret;
    }
    return 0;
}

static int dual_burn_boot_info_result_hdl(int err)
{
    if (0 == err) {
        //升级成功,复位系统
        printf("cpu_reset!!!\n");
        cpu_reset();
    } else {
        printf("err:boot_info_rusult fail, %x\n", err);
    }
    return 0;
}

static int dual_verify_result_hdl(int calc_crc)
{
    int ret = 0;
    if (1 == calc_crc) {
        ret = dual_bank_update_burn_boot_info(dual_burn_boot_info_result_hdl);
        if (ret) {
            printf("err:burn_boot_info fail, %x\n", ret);
        }
    } else {
        printf("err:update_verify fail, %x\n", calc_crc);
    }
    return ret;
}

// 使用加密数据进行crc校验
void dual_bank_test(void)
{
    u32 ret = 0;
    u16 data_crc = 0;
    struct vfs_attr attr = {0};
    SDFILE *file = sdfile_open("mnt/sdfile/app/updata", "r");
    if (!file) {
        return;
    }
    sdfile_get_attrs(file, &attr);
    u32 area_len = attr.fsize; // 注意：例子只是简单的使用整个预留区域的长度作为升级数据长度，实际使用需要填写与db_update_data.bin文件大小一致的值!!
    sdfile_seek(file, 0, SEEK_SET);
    for (u32 offset = 0, data_len = 0; offset < area_len; offset += data_len) {
        wdt_clear();
        data_len = area_len - offset;
        data_len = data_len > sizeof(tmp_buf) ? sizeof(tmp_buf) : data_len;
        sdfile_read(file, tmp_buf, data_len);
        data_crc = CRC16_with_initval(tmp_buf, data_len, data_crc);
    }

    //升级前先解除flash写保护
    norflash_set_write_protect(0, boot_info.vm.vm_saddr);
    ret = dual_bank_passive_update_init(data_crc, area_len, sizeof(tmp_buf), NULL);
    if (ret) {
        printf("err:update_init fail, %x\n", ret);
        goto _ERR_RET;
    }

    ret = dual_bank_update_allow_check(area_len);
    if (ret) {
        printf("err:update_allow_check fail, %x\n", ret);
        goto _ERR_RET;
    }

    sdfile_seek(file, 0, SEEK_SET);
    for (u32 offset = 0, data_len = 0; offset < area_len; offset += data_len) {
        wdt_clear();
        data_len = area_len - offset;
        data_len = data_len > sizeof(tmp_buf) ? sizeof(tmp_buf) : data_len;
        sdfile_read(file, tmp_buf, data_len);
        ret = dual_bank_update_write(tmp_buf, data_len, dual_write_complete_cb);
        if (ret) {
            printf("err:update_write fail, %x, %x\n", offset, ret);
            goto _ERR_RET;
        }
    }

    ret = dual_bank_update_verify(NULL, NULL, dual_verify_result_hdl);
    if (ret) {
        printf("err:update_verify fail, %x\n", ret);
        goto _ERR_RET;
    }

_ERR_RET:
    if (file) {
        sdfile_close(file);
    }
    //升级失败,恢复写保护
    norflash_set_write_protect(1, boot_info.vm.vm_saddr);
}

// 使用自校验，速度会比加密数据校验要慢
void dual_bank_test_2(void)
{
    u32 ret = 0;
    u16 data_crc = 0;
    struct vfs_attr attr = {0};
    SDFILE *file = sdfile_open("mnt/sdfile/app/updata", "r");
    if (!file) {
        return;
    }
    sdfile_get_attrs(file, &attr);
    u32 area_len = attr.fsize; // 注意：例子只是简单的使用整个预留区域的长度作为升级数据长度，实际使用需要填写与db_update_data.bin文件大小一致的值!!
    sdfile_seek(file, 0, SEEK_SET);
    /* for (u32 offset = 0, data_len = 0; offset < area_len; offset += data_len) { */
    /*     wdt_clear(); */
    /*     data_len = area_len - offset; */
    /*     data_len = data_len > sizeof(tmp_buf) ? sizeof(tmp_buf) : data_len; */
    /*     sdfile_read(file, tmp_buf, data_len); */
    /*     data_crc = CRC16_with_initval(tmp_buf, data_len, data_crc); */
    /* } */

    //升级前先解除flash写保护
    norflash_set_write_protect(0, boot_info.vm.vm_saddr);
    ret = dual_bank_passive_update_init(data_crc, area_len, sizeof(tmp_buf), NULL);
    if (ret) {
        printf("err:update_init fail, %x\n", ret);
        goto _ERR_RET;
    }

    ret = dual_bank_update_allow_check(area_len);
    if (ret) {
        printf("err:update_allow_check fail, %x\n", ret);
        goto _ERR_RET;
    }

    sdfile_seek(file, 0, SEEK_SET);
    for (u32 offset = 0, data_len = 0; offset < area_len; offset += data_len) {
        wdt_clear();
        data_len = area_len - offset;
        data_len = data_len > sizeof(tmp_buf) ? sizeof(tmp_buf) : data_len;
        sdfile_read(file, tmp_buf, data_len);
        ret = dual_bank_update_write(tmp_buf, data_len, dual_write_complete_cb);
        if (ret) {
            printf("err:update_write fail, %x, %x\n", offset, ret);
            goto _ERR_RET;
        }
    }

    ret = dual_bank_update_verify_without_crc();
    if (ret) {
        printf("err:update_verify_without_crc fail, %x\n", ret);
        goto _ERR_RET;
    }

    ret = dual_bank_update_burn_boot_info(dual_burn_boot_info_result_hdl);
    if (ret) {
        printf("err:burn_boot_info fail, %x\n", ret);
        goto _ERR_RET;
    }

_ERR_RET:
    if (file) {
        sdfile_close(file);
    }
    //升级失败,恢复写保护
    norflash_set_write_protect(1, boot_info.vm.vm_saddr);
}

#endif

