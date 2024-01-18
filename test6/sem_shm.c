

#include"sem_shm.h"
#include<stdio.h>


int crearShmAndSem(shmadrtype **shmad, sem_t **mutex, sem_t ** full, sem_t ** empty)
{
    key_t key;
    int shmid, proj_id = PROJID(0); 
    int *shmaddr = NULL;
    sem_t *semtmp;
    const char _Mutex[] = "myMutex";
    const char _Full[]  = "myFull";
    const char _Empty[] = "myEmpty";


    //key = ftok("./", proj_id);
    key = proj_id;
    if(key<=0)
    {
        printf("ftok err\n");
        key = 50;
    }

    shmid = shmget(key, 4096, IPC_CREAT);
    if(shmid<0)
    {
        printf("shmget err\n");
        return -1;
    }

    shmaddr = (shmadrtype*)shmat(shmid, NULL, 0);
    if(shmaddr<=((int*)0))
    {
        printf("shmat err\n");
        return -1;
    }
    *shmad = shmaddr;

    semtmp = sem_open(_Mutex, O_CREAT, 0, 1);
    if(semtmp==SEM_FAILED)
    {
        printf("sem_open _Mutex err\n");
        return -1;
    }
    *mutex = semtmp;

    semtmp = sem_open(_Full, O_CREAT, 0, 0);
    if(semtmp==SEM_FAILED)
    {
        printf("sem_open _Full err\n");
        return -1;
    }
    *full = semtmp;

    semtmp = sem_open(_Empty, O_CREAT, 0, CONSUM_NUM+1);
    if(semtmp==SEM_FAILED)
    {
        printf("sem_open _Empty err\n");
        return -1;
    }
    *empty = semtmp;

    return 0;
}

int deleteShmAndSem(void *shmad )
{
    const char _Mutex[] = "myMutex";
    const char _Full[]  = "myFull";
    const char _Empty[] = "myEmpty";

    if(shmad==NULL)
    {
        printf("deleteShmAndSem para err\n");
        return -1;
    }

    if(0 != shmdt(shmad))
    {
        printf("shmdt err\n");
        return -1;
    }

    if(0 != sem_unlink(_Mutex))
    {
        printf("sem_unlink _Mutex err\n");
        return -1;
    }
    if(0 != sem_unlink(_Full))
    {
        printf("sem_unlink _Full err\n");
        return -1;
    }
    if(0 != sem_unlink(_Empty))
    {
        printf("sem_unlink _Empty err\n");
        return -1;
    }

    return 0;
}


static int isQueneEmpty(shmadrtype *quene)
{
    if(quene->head==quene->tail  && quene->shmad[quene->head]==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int isQueneFull(shmadrtype *quene)
{
    if(((quene->head+1)%QUENE_NUM) == quene->tail)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int getQueneValue(shmadrtype *quene)
{
    int tmp;

    if(isQueneEmpty(quene) || quene==NULL)
    {
        printf("getQueneValue Empty\n");
        return -1;
    }
    else
    {
        tmp = quene->shmad[quene->tail];
        quene->shmad[quene->tail] = 0;
        quene->tail = (quene->tail+1)%QUENE_NUM;
        return tmp;
    }
}

void putQueneValue(shmadrtype *quene, int value)
{
    if(isQueneFull(quene) || quene==NULL)
    {
        printf("putQueneValue Full\n");
        return;
    }
    else
    {
        quene->shmad[quene->head] = value;
        quene->head = (quene->head+1)%QUENE_NUM;
    }
}




