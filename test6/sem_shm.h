#ifndef _SEM_SHM_H
#define _SEM_SHM_H

#ifdef __cplusplus//告诉C++代码使用C的方式链接这个库函数-即这段代码要被C++使用，但是用C的链接方式
extern "C" {
#endif

#include<sys/ipc.h>
#define CONSUM_NUM     (10)
#define SHMDATA        (500)

typedef enum
{
    projID_0 = 10,
    projID_1,
    projID_2,
    projID_3,
    projID_4,
    projID_5,
    projID_6,
    projID_7,
    projID_8,
    projID_9,
    projID_10,
    projID_11,
}prjID;

#define PROJID(x)    (projID##x)



extern int crearShmAndSem(shmadrtype **shmad, sem_t **mutex, sem_t ** full, sem_t ** empty);
extern int deleteShmAndSem(void *shmad );





#define QUENE_NUM     (20)
typedef struct
{
    int head;
    int tail;
    int shmad[QUENE_NUM];
    int iniflag;
    int endflag;
} shmadrtype;

int getQueneValue(shmadrtype *quene);
void putQueneValue(shmadrtype *quene, int value);



#ifdef __cplusplus 
} 
#endif 


#endif

