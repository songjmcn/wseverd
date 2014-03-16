/*
 * 此文件实现了资源池
 *使用资源池可以用来管理各种资源（内存，文件等）
 *资源池内部使用链表结构来管理各种资源，但是为了加快访问速度，使用了静态链表
 */

#ifndef WSD_SOURCE_POOL_H 
#define WSD_SOURCE_POOL_H
#include "wsd_type.h"
#include "wsd_mutex_lock.h"
#ifdef __cplusplus

#define  WSD_MEMORY_BLOCK_SIZE 8 //内存池块大小
//资源池块大小
#define  WSD_SOURCE_BLOCK_SIZE 8 //其他资源池大小
extern "C"
{
#endif
typedef void (*release_fun)(void* pdata); /*资源释放函数 */
typedef struct source_controler
{
	void *__ptr;   /*资源指针*/
	release_fun __release_fun; 
	void *__next;  /*下一个资源管理结构体指针*/      
}wsd_source_controler_t;
typedef struct memory_controler
{
	void *__ptr;  /*内存指针*/
	void *__next; /*下一个memory_controler*/
	int __size;   /*分配的内存大小*/
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
 *创建资源池 并返回资源池的指针
 */
wsd_source_pool_t* wsd_pool_create();
/*
 * 从资源池中申请内存
 */
void* wsd_pool_malloc(wsd_source_pool_t* pool,size_t len);
void wsd_pool_free(wsd_source_pool_t* pool,void* memort_ptr);
void wsd_pool_add_source(wsd_source_pool_t* pool,void* source_ptr,release_fun fun);
void wsd_pool_destroy(wsd_source_pool_t* pool);
#ifdef __cplusplus
};
#endif


#endif
