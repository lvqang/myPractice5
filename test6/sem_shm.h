#ifndef _SEM_SHM_H
#define _SEM_SHM_H

#ifdef __cplusplus/*告诉C++代码使用C的方式链接这个库函数-即这段代码要被C++使用，但是用C的链接方式*/
extern "C" {
#endif

#include<sys/ipc.h>
#include<linux/kernel.h>
#define CONSUM_NUM     (3)
#define SHMDATA        (10)

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

#define PROJID(x)    (projID_##x)
#define QUENE_NUM     (20)
typedef struct
{
    int head;
    int tail;
    int shmad[QUENE_NUM];
    int iniflag;
    int endflag;
} shmadrtype;

extern sem_t *sem_open(const char *name, int oflag, unsigned int value);
extern int sem_wait(sem_t *sem);
extern int sem_post(sem_t *sem);
extern int sem_unlink(const char *name);

extern key_t ftok(const char *pathname, int proj_id);
extern int shmget(key_t key, size_t size, int shmflg);
extern void* shmat(int shmid, const void * shmaddr, int shmflg);
extern int shmdt(const void *shmaddr);


extern int crearShmAndSem(shmadrtype **shmad, sem_t **mutex, sem_t **full, sem_t **empty);
extern int deleteShmAndSem(shmadrtype *shmad );




int getQueneValue(shmadrtype *quene);
void putQueneValue(shmadrtype *quene, int value);

extern void print(const char *name, const char *buff, ...);
extern void closelog();

#ifdef __cplusplus 
} 
#endif 


#endif

