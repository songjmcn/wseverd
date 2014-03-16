#ifndef WSD_MUTEX_LOCK
#define WSD_MUTEX_LOCK
#include "os.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
/*
 *���������� windows��ʹ���ٽ���
 */
#define wsd_mutex_t CRITICAL_SECTION
/*
  ��ʼ��������
 */
#define wsd_mutex_init InitializeCriticalSection
/*
 * �Ի��������
 */
#define wsd_mutex_lock EnterCriticalSection
/*
 * �Ի��������
 */
#define wsd_mutex_unlock LeaveCriticalSection
/*
 * �ͷŻ�����
 */
#define wsd_mutex_release DeleteCriticalSection
#ifdef __cplusplus
};
#endif

#endif
