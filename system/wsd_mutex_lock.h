#ifndef WSD_MUTEX_LOCK
#define WSD_MUTEX_LOCK
#include "os.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
/*
 *互斥量定义 windows下使用临界区
 */
#define wsd_mutex_t CRITICAL_SECTION
/*
  初始化互斥体
 */
#define wsd_mutex_init InitializeCriticalSection
/*
 * 对互斥体加锁
 */
#define wsd_mutex_lock EnterCriticalSection
/*
 * 对互斥体解锁
 */
#define wsd_mutex_unlock LeaveCriticalSection
/*
 * 释放互斥体
 */
#define wsd_mutex_release DeleteCriticalSection
#ifdef __cplusplus
};
#endif

#endif
