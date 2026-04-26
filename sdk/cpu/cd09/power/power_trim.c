#include "asm/power_interface.h"
#include "syscfg_id.h"

typedef struct {
    u8 except_cnt;
} pmu_voltage_type;

pmu_voltage_type pmu_voltage;

void update_vdd_table(u8 val);

void volatage_trim_init()
{

    memset((u8 *)&pmu_voltage, 0xff, sizeof(pmu_voltage));

    if (is_reset_source(P33_POWER_RETURN)) {
        pmu_voltage.except_cnt = (p33_rx_1byte(P3_SFLAG7) & 0x3);

    } else {
        int vm_len = syscfg_read(VM_PMU_VOLTAGE, (u8 *)&pmu_voltage, sizeof(pmu_voltage_type));

        /*上电第一次trim*/
        if (vm_len != sizeof(pmu_voltage_type)) {
            pmu_voltage.except_cnt = 0;
        }

        if (is_reset_source(P33_WDT_RST) || is_reset_source(P33_EXCEPTION_SOFT_RST)) {
            if ((pmu_voltage.except_cnt + 1) < 0xff) {
                pmu_voltage.except_cnt++;
                syscfg_write(VM_PMU_VOLTAGE, (u8 *)&pmu_voltage, sizeof(pmu_voltage_type));
            }
        }

    }

    update_vdd_table(((pmu_voltage.except_cnt <= 2) ? pmu_voltage.except_cnt : 2));
}

u8 get_pmu_voltage_cnt(void)
{
    return ((pmu_voltage.except_cnt <= 2) ? pmu_voltage.except_cnt : 2);
}

