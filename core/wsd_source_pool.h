/*
 * ���ļ�ʵ������Դ��
 *ʹ����Դ�ؿ����������������Դ���ڴ棬�ļ��ȣ�
 *��Դ���ڲ�ʹ������ṹ�����������Դ������Ϊ�˼ӿ�����ٶȣ�ʹ���˾�̬����
 */

#ifndef WSD_SOURCE_POOL_H 
#define WSD_SOURCE_POOL_H
#include "wsd_type.h"
#include "wsd_mutex_lock.h"
#ifdef __cplusplus

#define  WSD_MEMORY_BLOCK_SIZE 8 //�ڴ�ؿ��С
//��Դ�ؿ��С
#define  WSD_SOURCE_BLOCK_SIZE 8 //������Դ�ش�С
extern "C"
{
#endif
typedef void (*release_fun)(void* pdata); /*��Դ�ͷź��� */
typedef struct source_controler
{
	void *__ptr;   /*��Դָ��*/
	release_fun __release_fun; 
	void *__next;  /*��һ����Դ����ṹ��ָ��*/      
}wsd_source_controler_t;
typedef struct memory_controler
{
	void *__ptr;  /*�ڴ�ָ��*/
	void *__next; /*��һ��memory_controler*/
	int __size;   /*������ڴ��С*/
	int __used;
}wsd_memory_controler_t;
typedef struct source_pool
{
	wsd_memory_controler_t *__head;
	wsd_memory_controler_t *__end;
	wsd_source_controler_t *__rshead;
	wsd_source_controler_t *__rsend;
#ifdef WSD_MULTI_THREAD_OBJECT
	wsd_mutex_t __lock;
#endif /*endif WSD_MUILTI_THREAD_OBJECT*/
}wsd_source_pool_t;
/*
 *������Դ�� ��������Դ�ص�ָ��
 */
wsd_source_pool_t* wsd_pool_create();
/*
 * ����Դ���������ڴ�
 */
void* wsd_pool_malloc(wsd_source_pool_t* pool,size_t len);
void wsd_pool_free(wsd_source_pool_t* pool,void* memort_ptr);
void wsd_pool_add_source(wsd_source_pool_t* pool,void* source_ptr,release_fun fun);
void wsd_pool_destroy(wsd_source_pool_t* pool);
#ifdef __cplusplus
};
#endif


#endif
