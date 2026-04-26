
# make 编译并下载
# make VERBOSE=1 显示编译详细过程
# make clean 清除编译临时文件
#
# 注意： Linux 下编译方式：
#     1. 从 http://pkgman.jieliapp.com/doc/all 处找到下载链接
#     2. 下载后，解压到 /opt/jieli 目录下，保证
#       /opt/jieli/common/bin/clang 存在（注意目录层次）
#     3. 确认 ulimit -n 的结果足够大（建议大于8096），否则链接可能会因为打开文件太多而失败
#       可以通过 ulimit -n 8096 来设置一个较大的值
#

# 工具路径设置
ifeq ($(OS), Windows_NT)
# Windows 下工具链位置
TOOL_DIR := C:/JL/pi32/bin
CC    := clang.exe
CXX   := clang.exe
LD    := lto-wrapper.exe
AR    := llvm-ar.exe
MKDIR := mkdir_win -p
RM    := rm -rf

SYS_LIB_DIR := C:/JL/pi32/libc
SYS_INC_DIR := C:/JL/pi32/include/libc
EXT_CFLAGS  := # Windows 下不需要 -D__SHELL__
export PATH:=$(TOOL_DIR);$(PATH)

## 后处理脚本
FIXBAT          := tools/utils/fixbat.exe # 用于处理 utf8->gbk 编码问题
POST_SCRIPT     := cpu/cd09/tools/download.bat
RUN_POST_SCRIPT := cpu\cd09\tools\download.bat
else
# Linux 下工具链位置
TOOL_DIR := /opt/jieli/pi32/bin
CC    := clang
CXX   := clang
LD    := lto-wrapper
AR    := lto-ar
MKDIR := mkdir -p
RM    := rm -rf
export OBJDUMP := $(TOOL_DIR)/objdump
export OBJCOPY := $(TOOL_DIR)/objcopy
export OBJSIZEDUMP := $(TOOL_DIR)/objsizedump

SYS_LIB_DIR := $(TOOL_DIR)/../lib
SYS_INC_DIR := $(TOOL_DIR)/../include
EXT_CFLAGS  := -D__SHELL__ # Linux 下需要这个保证正确处理 download.c
export PATH:=$(TOOL_DIR):$(PATH)

## 后处理脚本
FIXBAT          := touch # Linux下不需要处理 bat 编码问题
POST_SCRIPT     := cpu/cd09/tools/download.sh
RUN_POST_SCRIPT := bash $(POST_SCRIPT)
endif

CC  := $(TOOL_DIR)/$(CC)
CXX := $(TOOL_DIR)/$(CXX)
LD  := $(TOOL_DIR)/$(LD)
AR  := $(TOOL_DIR)/$(AR)
# 输出文件设置
OUT_ELF   := cpu/cd09/tools/sdk.elf
OBJ_FILE  := $(OUT_ELF).objs.txt
# 编译路径设置
BUILD_DIR := objs

# 编译参数设置
CFLAGS := \
	-target pi32 \
	-integrated-as \
	-fno-builtin \
	-mllvm -pi32-memreg-opt \
	-mllvm -pi32-mem-offset-adj-opt \
	-mllvm -pi32-const-spill \
	-mllvm -pi32-enable-jz \
	-mllvm -pi32-tailcall-opt \
	-mllvm -inline-threshold=5 \
	-mllvm -pi32-enable-itblock=1 \
	-Oz \
	-flto \
	-integrated-as \
	-g \
	-O0 \
	-flto \
	-Os \
	-fallow-pointer-null \
	-Wincompatible-pointer-types \
	-Wundef \
	-Wformat \
	-nostrictpi32 \
	-fprefer-gnu-section \
	-Wframe-larger-than=256 \
	-Wuninitialized \
	-Wno-invalid-noreturn \
	-Wno-shift-negative-value \
	-fms-extensions \
	-fdiscrete-bitfield-abi \


# C++额外的编译参数
CXXFLAGS :=


# 宏定义
DEFINES := \
	-DCONFIG_RELEASE_ENABLE \
	-DCONFIG_CPU_CD09 \
	-DCONFIG_CBUF_IN_MASKROM \
	-D__GCC_PI32_LTO__ \


DEFINES += $(EXT_CFLAGS) # 额外的一些定义

# 头文件搜索路径
INCLUDES := \
	-Iinclude_lib \
	-Iinclude_lib/driver \
	-Iinclude_lib/driver/device \
	-Iinclude_lib/driver/cpu \
	-Iinclude_lib/driver/cpu/cd09 \
	-Iinclude_lib/driver/cpu/periph \
	-Iinclude_lib/system \
	-Iinclude_lib/system/generic \
	-Iinclude_lib/system/device \
	-Iinclude_lib/system/fs \
	-Iinclude_lib/system/ui \
	-Iinclude_lib/system/math/cpu/cd09 \
	-Iapps/common \
	-Iapps/common/device/key \
	-Iapps/common/eeprom \
	-Iapps/common/iic_soft \
	-Iapps/gp_mcu/include \
	-Icpu/cd09 \
	-Iapps/common/cJSON \
	-Iapps/common/lcd \
	-Iapps/common/font \
	-Iapps/common/ui/resource \
	-Iapps/common/ui_platform \
	-Iapps/common/ui_platform/core \
	-Iinclude_lib/ui_platform \
	-Iinclude_lib/update \
	-Iinclude_lib/update/code_v2 \
	-Iapps/common/device \
	-Iapps/common/device/usb \
	-Iapps/common/device/usb/device \
	-Iapps/common/device/usb/host \
	-I$(SYS_INC_DIR) \


# 需要编译的 .c 文件
c_SRC_FILES := \
	apps/common/at_char/at_char_app.c \
	apps/common/at_char/at_char_drv.c \
	apps/common/battery/battery.c \
	apps/common/cJSON/cJSON.c \
	apps/common/debug/debug.c \
	apps/common/decode/decode.c \
	apps/common/decode/voice.c \
	apps/common/device/key/adkey.c \
	apps/common/device/key/iokey.c \
	apps/common/device/key/irkey.c \
	apps/common/device/key/key_driver.c \
	apps/common/device/key/touch_key.c \
	apps/common/device/usb/device/cdc.c \
	apps/common/device/usb/device/custom_hid.c \
	apps/common/device/usb/device/descriptor.c \
	apps/common/device/usb/device/hid.c \
	apps/common/device/usb/device/msd.c \
	apps/common/device/usb/device/msd_upgrade.c \
	apps/common/device/usb/device/rcsp_hid_inter.c \
	apps/common/device/usb/device/task_pc.c \
	apps/common/device/usb/device/uac1.c \
	apps/common/device/usb/device/uac_stream.c \
	apps/common/device/usb/device/usb_device.c \
	apps/common/device/usb/device/user_setup.c \
	apps/common/device/usb/host/usb_bulk_transfer.c \
	apps/common/device/usb/host/usb_ctrl_transfer.c \
	apps/common/device/usb/host/usb_host.c \
	apps/common/device/usb/host/usb_storage.c \
	apps/common/device/usb/usb_config.c \
	apps/common/device/usb/usb_host_config.c \
	apps/common/eeprom/iic_eeprom.c \
	apps/common/font/font_textout.c \
	apps/common/font/font_unicode.c \
	apps/common/lcd/lcd_drive.c \
	apps/common/lcd/lcd_pap_st7789_240x240.c \
	apps/common/lcd/lcd_pap_st7789_240x320.c \
	apps/common/lcd/lcd_spi_st7735_128x128.c \
	apps/common/lcd/lcd_spi_st7735_80x160.c \
	apps/common/lcd/lcd_spi_st7789_BOE1.54_update_240x240.c \
	apps/common/sin_generator/sin_generator.c \
	apps/common/ui/resource/rle.c \
	apps/common/ui/resource/ui_resfile.c \
	apps/common/ui/ui_draw/ui_image.c \
	apps/common/ui_platform/core/ui_core.c \
	apps/common/ui_platform/core/ui_draw.c \
	apps/common/ui_platform/core/ui_ll.c \
	apps/common/ui_platform/ui_api.c \
	apps/common/ui_platform/ui_demo.c \
	apps/common/update/dual_bank_demo.c \
	apps/gp_mcu/app_config.c \
	apps/gp_mcu/app_main.c \
	apps/gp_mcu/device_config.c \
	cpu/cd09/gpadc.c \
	cpu/cd09/gpio_func.c \
	cpu/cd09/gpio_irq.c \
	cpu/cd09/hadc/hadc_analog.c \
	cpu/cd09/hadc/hadc_digital.c \
	cpu/cd09/mcpwm.c \
	cpu/cd09/norflash.c \
	cpu/cd09/pap.c \
	cpu/cd09/plcnt.c \
	cpu/cd09/power/power_app.c \
	cpu/cd09/power/power_config.c \
	cpu/cd09/power/power_trim.c \
	cpu/cd09/rdec.c \
	cpu/cd09/segment_code_lcd/segment_code_lcd_drv.c \
	cpu/cd09/setup.c \
	cpu/cd09/spi_hw_v2.c \
	cpu/cd09/tick_timer.c \
	cpu/cd09/uart_log.c \
	cpu/components/ir_decoder.c \
	cpu/components/ir_encoder.c \
	cpu/components/rdec_soft.c \
	cpu/demo/at_char_demo.c \
	cpu/demo/gpadc_demo.c \
	cpu/demo/gptimer_demo.c \
	cpu/demo/hadc_demo.c \
	cpu/demo/ich_irq_demo.c \
	cpu/demo/iic_demo.c \
	cpu/demo/mcpwm_demo.c \
	cpu/demo/norflash_demo.c \
	cpu/demo/rtc_demo.c \
	cpu/demo/segment_code_lcd_demo.c \
	cpu/demo/sin_generator_demo.c \
	cpu/demo/spi_demo.c \
	cpu/demo/uart_demo.c \
	cpu/demo/vm_demo.c \
	cpu/demo/voice_demo.c \
	cpu/gpio.c \
	cpu/iic_api.c \
	cpu/iic_soft.c \


# 需要编译的 .S 文件
S_SRC_FILES := \
	apps/gp_mcu/sdk_version.z.S \


# 需要编译的 .s 文件
s_SRC_FILES :=


# 需要编译的 .cpp 文件
cpp_SRC_FILES :=


# 需要编译的 .cc 文件
cc_SRC_FILES :=


# 需要编译的 .cxx 文件
cxx_SRC_FILES :=


# 链接参数
LFLAGS := \
	--plugin-opt=-inline-threshold=5 \
	--plugin-opt=save-temps \
	--plugin-opt=-inline-normal-into-special-section=true \
	--plugin-opt=-dont-used-symbol-list=malloc,free,sprintf,printf,puts,putchar \
	--plugin-opt=-mattr=+no-constpool \
	--plugin-opt=-warn-stack-size=256 \
	--sort-common \
	--start-group \
	cpu/cd09/liba/system.a \
	cpu/cd09/liba/cpu.a \
	cpu/cd09/liba/ump3_dec_lib.a \
	cpu/cd09/liba/ui_platform.a \
	cpu/cd09/liba/lib_speed_pitch.a \
	cpu/cd09/liba/update.a \
	--end-group \
	-Tcpu/cd09/sdk.ld \
	-M=cpu/cd09/tools/sdk.map \
	--plugin-opt=-pi32-memreg-opt \
	--plugin-opt=-pi32-mem-offset-adj-opt \
	--plugin-opt=-pi32-const-spill \
	--plugin-opt=-pi32-enable-jz \
	--plugin-opt=-pi32-tailcall-opt \
	--plugin-opt=-inline-threshold=5 \
	--plugin-opt=-pi32-enable-itblock=1 \


LIBPATHS := \
	-L$(SYS_LIB_DIR) \


LIBS := \
	C:/JL/pi32/lib/libc.a \
	C:/JL/pi32/lib/libm.a \
	C:/JL/pi32/lib/libcompiler-rt.a \



c_OBJS    := $(c_SRC_FILES:%.c=%.c.o)
S_OBJS    := $(S_SRC_FILES:%.S=%.S.o)
s_OBJS    := $(s_SRC_FILES:%.s=%.s.o)
cpp_OBJS  := $(cpp_SRC_FILES:%.cpp=%.cpp.o)
cxx_OBJS  := $(cxx_SRC_FILES:%.cxx=%.cxx.o)
cc_OBJS   := $(cc_SRC_FILES:%.cc=%.cc.o)

OBJS      := $(c_OBJS) $(S_OBJS) $(s_OBJS) $(cpp_OBJS) $(cxx_OBJS) $(cc_OBJS)
DEP_FILES := $(OBJS:%.o=%.d)


OBJS      := $(addprefix $(BUILD_DIR)/, $(OBJS))
DEP_FILES := $(addprefix $(BUILD_DIR)/, $(DEP_FILES))


VERBOSE ?= 0
ifeq ($(VERBOSE), 1)
QUITE :=
else
QUITE := @
endif

# 一些旧的 make 不支持 file 函数，需要 make 的时候指定 LINK_AT=0 make
LINK_AT ?= 1

# 表示下面的不是一个文件的名字，无论是否存在 all, clean, pre_build 这样的文件
# 还是要执行命令
# see: https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean pre_build

# 不要使用 make 预设置的规则
# see: https://www.gnu.org/software/make/manual/html_node/Suffix-Rules.html
.SUFFIXES:

all: pre_build $(OUT_ELF)
	$(info +POST-BUILD)
	$(QUITE) $(RUN_POST_SCRIPT) sdk

pre_build:
	$(info +PRE-BUILD)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/cd09/sdk_ld.c -o cpu/cd09/sdk.ld
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/cd09/tools/download_bat.c -o $(POST_SCRIPT)
	$(QUITE) $(FIXBAT) $(POST_SCRIPT)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/cd09/tools/download_sh.c -o cpu/cd09/tools/download.sh
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/cd09/tools/isd_config_rule.c -o cpu/cd09/tools/isd_config.ini

clean:
	$(QUITE) $(RM) $(OUT_ELF)
	$(QUITE) $(RM) $(BUILD_DIR)



ifeq ($(LINK_AT), 1)
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(file >$(OBJ_FILE), $(OBJS))
	$(QUITE) $(LD) -o $(OUT_ELF) @$(OBJ_FILE) $(LFLAGS) $(LIBPATHS) $(LIBS)
else
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(QUITE) $(LD) -o $(OUT_ELF) $(OBJS) $(LFLAGS) $(LIBPATHS) $(LIBS)
endif


$(BUILD_DIR)/%.c.o : %.c
	$(info +CC $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MMD -MF $(@:.o=.d) -c $< -o $@

$(BUILD_DIR)/%.S.o : %.S
	$(info +AS $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MMD -MF $(@:.o=.d) -c $< -o $@

$(BUILD_DIR)/%.s.o : %.s
	$(info +AS $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MMD -MF $(@:.o=.d) -c $< -o $@

$(BUILD_DIR)/%.cpp.o : %.cpp
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MMD -MF $(@:.o=.d) -c $< -o $@

$(BUILD_DIR)/%.cxx.o : %.cxx
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MMD -MF $(@:.o=.d) -c $< -o $@

$(BUILD_DIR)/%.cc.o : %.cc
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MMD -MF $(@:.o=.d) -c $< -o $@

-include $(DEP_FILES)
