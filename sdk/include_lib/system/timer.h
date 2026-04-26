#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include "generic/typedef.h"
#include "generic/list.h"

/*
 *  System Timer
 */

void sys_timer_init(void);

/*
 * @brief   获取系统运行时间
 * @param
 * @return  系统自timer初始化后运行的总时间
 * @note
 *  1、该函数在定时器中断调用,2ms周期
*/
u32 sys_timer_get_ms(void);

/**
 * @brief   sys_timer定时扫描接口
 * @param
 * @return
 * @note
   1、该函数在定时器中断调用,2ms周期
*/
void sys_timer_schedule(void);

/**
 * @brief 添加ms级系统非timeout类型定时任务。
 * @param [in] priv 定时任务处理函数的输入参数
 * @param [in] func 定时任务处理函数
 * @param [in] msec 定时时间
 * @return 定时器分配的ID
 * @note
 * 1、sys_timer由tick timer提供时基,属于同步接口,
 *    当定时时间到时,sys_timer模块,会发送call event,通知主函数进行回调函数处理
 *    主循环调用sys_event_get接口时,内部会拦截call event进行函数调用(callback)
 * 2、与sys_timer_del成对使用。
 */
u16 sys_timer_add(void *priv, void (*func)(void *priv), u32 msec);

/**
 * @brief sys_timer删除接口
 * @param [in] id:sys_timer_add时分配的id号
 * @note
 * 1、与sys_timer_add成对使用。
 */
void sys_timer_del(u16 id);

/**
 * @brief sys_timer修改定时扫描时间接口
 * @param [in] id sys_timer_add时分配的id号
 * @param [in] msec 定时时间，单位：毫秒
*/
int sys_timer_modify(u16 id, u32 msec);

/**
 * @brief 添加ms级系统timeout类型定时任务。
 * @param [in] priv 定时任务处理函数的输入参数
 * @param [in] func 定时任务处理函数
 * @param [in] msec 定时时间
 * @return 定时器分配的id号
 * @note
 * 1、sys_timer由tick timer提供时基,属于同步接口,
 *    当定时时间到时,sys_timer模块,会发送call event,通知主函数进行回调函数处理
 *    主循环调用sys_event_get接口时,内部会拦截call event进行函数调用(callback)
 * 2、timeout回调只会被执行一次；
 * 3、与sys_timerout_del成对使用。
 */
u16 sys_timeout_add(void *priv, void (*func)(void *priv), u32 msec);

/**
 * @brief 删除ms级timeout类型定时任务。
 * @param [in] id:sys_timerout_add时分配的id号
 * @note
 * 1、与sys_timerout_add成对使用。
 */
void sys_timeout_del(u16 id);

/**
 * @brief sys_timerout修改超时时间接口
 * @param [in] id sys_timeout_add时分配的id号
 * @param [in] msec 定时时间，单位：毫秒
*/
int sys_timeout_modify(u16 id, u32 msec);

/**
 * @brief sys_time输出调试信息
 * @param
 * @return
 * @note
 * 1.调试时可用
 * 2.将输出所有被add定时器的id及其时间(msec)
*/
void sys_timer_dump(void);

/**
 * @brief 重置ms级定时任务定时时间
 * @param [in] id sys_timer_add时分配的id号
 * @note
*/
/*-----------------------------------------------------------*/
void sys_timer_reset(u16 id);


/*
 *  Usr Timer
 */

void usr_timer_schedule();

/**
 * @brief usr_timer定时扫描增加接口
 * @param [in] priv 私有参数
 * @param [in] func 定时扫描回调函数
 * @param [in] msec 定时时间， 单位：毫秒
 * @param [in] priority 优先级，范围：0/1
 * @return 定时器分配的id号
 * @note
 * 1、usr_timer的参数priority（优先级）为1，使用该类定时器，系统无法进入低功耗；
 * 2、usr_timer的参数priority（优先级）为0，使用该类定时器，系统低功耗会忽略该节拍，节拍不会丢失，但是定时周期会变；
 * 3、usr_timer属于异步接口， add的时候注册的扫描函数将在硬件定时器中时基到时候被调用;
 * 4、对应释放接口usr_timer_del。
*/
u16 usr_timer_add(void *priv, void (*func)(void *priv), u32 msec, u8 priority);

/**
 * @brief usr_timer超时增加接口
 * @param [in] priv 私有参数
 * @param [in] func 超时回调函数
 * @param [in] msec 定时时间， 单位：毫秒
 * @param [in] priority 优先级,范围：0/1
 * @return 定时器分配的id号
 * @note
 * 1、usr_timerout的参数priority（优先级）为1，使用该类定时器，系统无法进入低功耗；
 * 2、usr_timerout的参数priority（优先级）为0，使用该类定时器，系统低功耗会忽略该节拍，节拍不会丢失，但是定时周期会变；
 * 3、usr_timerout属于异步接口， add的时候注册的扫描函数将在硬件定时器中时基到时候被调用；
 * 4、对应释放接口usr_timerout_del；
 * 5、timeout回调只会被执行一次。
*/
u16 usr_timeout_add(void *priv, void (*func)(void *priv), u32 msec, u8 priority);

/**
 * @brief usr_timer修改定时扫描时间接口
 * @param [in] id usr_timer_add时分配的id号
 * @param [in] msec 定时时间，单位：毫秒
*/
int usr_timer_modify(u16 id, u32 msec);

/**
 * @brief usr_timerout修改超时时间接口
 * @param [in] id usr_timerout_add时分配的id号
 * @param [in] msec 定时时间，单位：毫秒
*/
int usr_timeout_modify(u16 id, u32 msec);

/**
 * @brief usr_timer删除接口
 * @param [in] id usr_timer_add时分配的id号
 * @note
 * 1、注意与usr_timer_add成对使用。
*/
void usr_timer_del(u16 id);

/**
 * @brief usr_timeout删除接口
 * @param [in] id usr_timerout_add时分配的id号
 * @note
 * 1、注意与usr_timerout_add成对使用。
*/
void usr_timeout_del(u16 id);

/**
 * @brief usr_timer复位接口
 * @param [in] id usr_timer_add时分配的id号
 * @note
*/
void usr_timer_reset(u16 id);

/**
 * @brief usr_time输出调试信息
 * @note
 * 1.调试时可用；
 * 2.将输出所有被add定时器的id及其时间(msec)。
*/
void usr_timer_dump(void);

/**
 * @brief 打印当前系统运行时间
 * @note
 */
void sys_timer_dump_time(void);

/*-----------------------------------------------------------*/

#endif


