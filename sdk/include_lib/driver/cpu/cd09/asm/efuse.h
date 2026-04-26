#ifndef  __EFUSE_H__
#define  __EFUSE_H__

#include "typedef.h"

void efuse_init();

u32 efuse_get_vbg_trim();

u32 efuse_get_pinr_reset_en(void);

u32 efuse_get_hadc_vbg_trim(void);

u32 efuse_get_VBIM_ref_res_trim(void);

u32 efuse_get_wvdd_trim(void);

u32 efuse_get_sfc_fast_boot(void);

u32 get_chip_version();
#endif  /*EFUSE_H*/
