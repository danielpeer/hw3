// File: ssd.c
// Date: 2014. 12. 03.
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2014
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#include "ssd.h"
#include "common.h"

#ifdef GET_WORKLOAD
#include <time.h>
#include <sys/time.h>
#endif

//sector_entry* PARSE_SECTOR_LIST(trim_data, length);
void SSD_INIT(void)
{
	FTL_INIT();
}

void SSD_TERM(void)
{	
	FTL_TERM();
}

void SSD_WRITE(unsigned int length, int32_t sector_nb)
{
#if defined SSD_THREAD	

	pthread_mutex_lock(&cq_lock);
	DEQUEUE_COMPLETED_READ();
	pthread_mutex_unlock(&cq_lock);

	if(EVENT_QUEUE_IS_FULL(WRITE, length)){
		w_queue_full = 1;
		while(w_queue_full == 1){
			pthread_cond_signal(&eq_ready);
		}
	}
	
	pthread_mutex_lock(&eq_lock);
	ENQUEUE_IO(WRITE, sector_nb, length);
	pthread_mutex_unlock(&eq_lock);

    #ifdef SSD_THREAD_MODE_1
	pthread_cond_signal(&eq_ready);
    #endif

#elif defined FIRM_IO_BUFFER
	DEQUEUE_COMPLETED_READ();
	if(EVENT_QUEUE_IS_FULL(WRITE, length)){
		SECURE_WRITE_BUFFER();
	}
	ENQUEUE_IO(WRITE, sector_nb, length);
#else
	FTL_WRITE(sector_nb, length);
#endif
}

void SSD_READ(unsigned int length, int32_t sector_nb)
{
#if defined SSD_THREAD

	pthread_mutex_lock(&cq_lock);
	DEQUEUE_COMPLETED_READ();
	pthread_mutex_unlock(&cq_lock);

	if(EVENT_QUEUE_IS_FULL(READ, length)){
		r_queue_full = 1;
		while(r_queue_full == 1){
			pthread_cond_signal(&eq_ready);
		}
	}

	pthread_mutex_lock(&eq_lock);
	ENQUEUE_IO(READ, sector_nb, length);
	pthread_mutex_unlock(&eq_lock);

    #ifdef SSD_THREAD_MODE_1
	pthread_cond_signal(&eq_ready);
    #endif

#elif defined FIRM_IO_BUFFER
	DEQUEUE_COMPLETED_READ();
	if(EVENT_QUEUE_IS_FULL(READ, length)){
		SECURE_READ_BUFFER();
	}
	ENQUEUE_IO(READ, sector_nb, length);
#else
	FTL_READ(sector_nb, length);
#endif
}

void SSD_DSM_TRIM(unsigned int length, void* trim_data)
{
/*
	if(DSM_TRIM_ENABLE == 0)
		return;
		
	
	sector_entry* pSE_T = NULL;
	sector_entry* pSE = NULL;
	
	int i;
	int64_t sector_nb;
	unsigned int sector_length;
	int* pBuff;
	
	pBuff = (int*)trim_data;
	int k = 0;
	for(i=0;i<length;i++)
	{
		sector_nb = (int64_t)*pBuff;
		pBuff++;
		sector_length = (int)*pBuff;	
		sector_length = sector_length >> 16;
		if(sector_nb == 0 && sector_length == 0)
			break;
		
		pSE_T = new_sector_entry();
		pSE_T->sector_nb = sector_nb;
		pSE_T->length = sector_length;
		
		if(pSE == NULL)
			pSE = pSE_T;
		else
		{
			k++;
			add_sector_list(pSE, pSE_T);
		}
		
		pBuff++;
	}
	
	INSERT_TRIM_SECTORS(pSE);

	release_sector_list(pSE);
*/
}

int SSD_IS_SUPPORT_TRIM(void)
{
//	return DSM_TRIM_ENABLE;
	return 0;
}

