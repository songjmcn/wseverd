#include "wsd_source_pool.h"
#include <malloc.h>
wsd_source_pool_t* wsd_pool_create()
{
	wsd_source_pool_t* pool_ptr=(wsd_source_pool_t*)malloc(sizeof(wsd_source_pool_t));
	if(pool_ptr==NULL)
	{
		return NULL;
	}
	pool_ptr->__head=(wsd_memory_controler_t*)calloc(WSD_MEMORY_BLOCK_SIZE,sizeof(wsd_memory_controler_t));
	pool_ptr->__end=pool_ptr->__head;
	pool_ptr->__rshead=(wsd_source_controler_t*)calloc(WSD_SOURCE_BLOCK_SIZE,sizeof(wsd_source_controler_t));
	pool_ptr->__rsend=pool_ptr->__rsend;
#ifdef WSD_MULTI_THREAD_OBJECT
	wsd_mutex_init(&pool->__lock);
#endif
	return pool_ptr;
}
/*
 *�ͷ��ڴ���е������ڴ�
 */
void wsd_memory_controler_free(wsd_memory_controler_t *head)
{
	int i=0;
	wsd_memory_controler_t* tmp=NULL;
	while(head!=NULL)
	{
		for (i=0;i<WSD_MEMORY_BLOCK_SIZE;i++)
		{
			if(head[i].__ptr!=NULL)
			{
				free(head[i].__ptr);
				head[i].__ptr=NULL;
			}
		}
		tmp=head;
		head=(wsd_memory_controler_t*)head[WSD_MEMORY_BLOCK_SIZE-1].__next;
		free(tmp);
	}
}
/*
 *�ͷ���Դ���е�������Դ
 */
void wsd_source_pool_free(wsd_source_controler_t* head)
{
	int i=0;
	wsd_source_controler_t* tmp=NULL;
	while(head!=NULL)
	{
		for (i=0;i<WSD_SOURCE_BLOCK_SIZE;i++)
		{
			if(head[i].__ptr!=NULL)
			{
				head[i].__release_fun(head[i].__ptr);
				head[i].__ptr=NULL;
			}
		}
		tmp=head;
		head=(wsd_source_controler_t*)head[WSD_SOURCE_BLOCK_SIZE-1].__next;
		free(tmp);
	}
}
/*
 *������Դ��
 */
void wsd_pool_destroy(wsd_source_pool_t* pool)
{
	if (pool!=NULL)
	{
		wsd_memory_controler_free(pool->__head);
		wsd_source_pool_free(pool->__rshead);
#ifdef WSD_MULTI_THREAD_OBJECT
		wsd_mutex_release(&pool->__lock);
#endif
		free(pool);
	}
}
void wsd_pool_add_source(wsd_source_pool_t* pool,void* source_ptr,release_fun fun)
{
	wsd_source_controler_t *tmp=NULL;
	int i=0;
	if(pool==NULL || source_ptr==NULL || fun==NULL)
	{
		return;
	}
	else
	{
#ifdef WSD_MULTI_THREAD_OBJECT
		wsd_mutex_lock(&pool->__lock);
#endif
		if(pool->__rsend[WSD_SOURCE_BLOCK_SIZE-1].__ptr!=NULL)
		{
			tmp=(wsd_source_controler_t*)calloc(WSD_SOURCE_BLOCK_SIZE,sizeof(wsd_source_controler_t));
			pool->__rsend[WSD_SOURCE_BLOCK_SIZE-1].__next=tmp;
			pool->__rsend=tmp;

		}
		tmp=(wsd_source_controler_t*)pool->__end;
		for (i=0;i<WSD_SOURCE_BLOCK_SIZE;i++)
		{
			if(tmp[i].__ptr==NULL)
			{
				tmp[i].__ptr=source_ptr;
				tmp[i].__release_fun=fun;
				if(i!=0)
				{
					tmp[i-1].__next=&tmp[i];
				}
			}
		}
	}
#ifdef WSD_MULTI_THREAD_OBJECT
	wsd_mutex_unlock(&pool->__lock);
#endif
}
void* wsd_pool_malloc(wsd_source_pool_t* pool,size_t len)
{
	size_t min_size=-1;
	wsd_memory_controler_t *min_block=NULL;
	int index=0;
	wsd_memory_controler_t *tmp=NULL;
	int i=0;
	if(pool==NULL || i==0)
	{
		return NULL;
	}
	i=len%4;
	len=len+(i==0?0:4-i);
	tmp=pool->__head;
#ifdef WSD_MULTI_THREAD_OBJECT
	wsd_mutex_lock(&pool->__lock);
#endif
	/*
	 *  �ȴ��ڴ����Ѱ���Ƿ�������Ҫ��Ŀ��п飬�еĻ��ͷ�����С�Ŀ��п�
	 */
	while (tmp!=NULL)
	{
		for (i=0;i<WSD_MEMORY_BLOCK_SIZE;i++)
		{
			if(tmp[i].__ptr!=NULL && tmp[i].__used)
			{
				if(len<=tmp[i].__size)
				{
					if(tmp[i].__size<min_size)
					{
						min_size=tmp[i].__size;
						min_block=tmp;
						index=i;
					}
				}
			}
		}
		tmp=(wsd_memory_controler_t*)tmp[WSD_MEMORY_BLOCK_SIZE-1].__next;
	}
	/*
	 *����ҵ�����Ҫ��Ŀ飬�ͷ�������飬����ǿ鱻ʹ��
	 */
	if(min_block!=NULL)
	{
		min_block[index].__used=1;
#ifdef WSD_MULTI_THREAD_OBJECT
		wsd_mutex_unlock(&pool->__lock);
#endif
		return min_block[index].__ptr;
	}
	/*
	 *��ʱû���ҵ����ʵĿ飬������һ���µĿ飬�������������ڱ�ʹ�ã���󷵻�
	 */
	else
	{
		if(pool->__end[WSD_MEMORY_BLOCK_SIZE-1].__ptr!=NULL)
		{
			tmp=(wsd_memory_controler_t*)calloc(WSD_MEMORY_BLOCK_SIZE,sizeof(wsd_memory_controler_t));
			pool->__end[WSD_MEMORY_BLOCK_SIZE-1].__next=tmp;
			pool->__end=tmp;
		}
		for (i=0;i<WSD_MEMORY_BLOCK_SIZE;i++)
		{
			if(pool->__end[i].__ptr==NULL)
			{
				pool->__end[i].__ptr=malloc(len);
				pool->__end[i].__used=1;
				if(i>0)
				{
					pool->__end[i-1].__next=&pool->__end[i];
				}
#ifdef WSD_MULTI_THREAD_OBJECT
				wsd_mutex_unlock(&pool->__lock);
#endif
				return pool->__end[i].__ptr;
			}
		}
	}
}