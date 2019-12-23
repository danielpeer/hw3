#include "ssd.h"
#include "common.h"
#include <assert.h>

#define RUN_TEST(SETUP, FUNC)   if(SETUP) popen("rm data/*.dat", "r"); \
						        SSD_INIT(); \
 								FUNC; \
								if(SETUP) SSD_TERM()

extern int32_t* mapping_table;

/**
 * simple test that writes all sectors in the device sequentially
 */
int test_access()
{
	int ret, i, lba;

	// write entire device 
	//SSD_WRITE(5*SECTORS_PER_PAGE, 0);
	for(i=0;i<(int)SECTOR_NB*0.05;i+=SECTORS_PER_PAGE){
		if ((i/SECTORS_PER_PAGE) % 1024*10==0){
			LOG("wrote %.3lf of device", (double)i  / (double)SECTOR_NB);
		}

		lba = i % SECTOR_NB;
		SSD_WRITE(SECTORS_PER_PAGE, lba);
	}

	printf("wrote seq\n");

	for(i=0;i<(int)SECTOR_NB*0.05;i+=SECTORS_PER_PAGE){
		if ((i/SECTORS_PER_PAGE) % 1024*10==0){
			LOG("read %.3lf of device", (double)i  / (double)SECTOR_NB);
		}

		lba = i % SECTOR_NB;
		SSD_READ(SECTORS_PER_PAGE, lba);
	}

	printf("read seq\n");

	return 0;
}

int main(int argc, char *argv[]){
	int setup = 1;

	RUN_TEST(setup, test_access());

	return 0;
}