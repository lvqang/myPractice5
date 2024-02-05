

#include "sem_shm.h"
#include<stdio.h>
#define __LIBRARY__
#include<unistd.h>
#include <fcntl.h>
/*#include <stdlib.h>*/
#include <string.h>
#include <stdarg.h>


typedef struct
{
    int ini;
    int fd;
}logMsg;
logMsg logi = {0,0};


_syscall3(sem_t *,sem_open,const char *,name,int,oflag,unsigned int,value)
_syscall1(int,sem_wait,sem_t *,sem)
_syscall1(int,sem_post,sem_t *,sem)
_syscall1(int,sem_unlink,const char *,name)


_syscall2(key_t,ftok,const char *,pathname,int,proj_id)
_syscall3(int,shmget,key_t,key,size_t,size, int,shmflg)
_syscall3(void*,shmat,int,shmid,const void *,shmaddr,int,shmflg)
_syscall1(int,shmdt,const void *,shmaddr)

_syscall1(int,unlink,const char *,name)


int crearShmAndSem(shmadrtype **shmad, sem_t **mutex, sem_t **full, sem_t **empty)
{
    key_t key;
    int shmid, proj_id = PROJID(0); 
    shmadrtype *shmaddr = NULL;
    sem_t *semtmp;
    const char _Mutex[] = "myMutex";
    const char _Full[]  = "myFull";
    const char _Empty[] = "myEmpty";


    /*key = ftok("./", proj_id);*/
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
    if(((int)shmaddr)<=0)
    {
        printf("shmat err\n");
        return -1;
    }
    *shmad = shmaddr;

    semtmp = sem_open(_Mutex, O_CREAT, 1);
    if(semtmp==SEM_FAILED)
    {
        printf("sem_open _Mutex err\n");
        return -1;
    }
    *mutex = semtmp;

    semtmp = sem_open(_Full, O_CREAT, 0);
    if(semtmp==SEM_FAILED)
    {
        printf("sem_open _Full err\n");
        return -1;
    }
    *full = semtmp;

    semtmp = sem_open(_Empty, O_CREAT, CONSUM_NUM);
    if(semtmp==SEM_FAILED)
    {
        printf("sem_open _Empty err\n");
        return -1;
    }
    *empty = semtmp;

    return 0;
}

int deleteShmAndSem(shmadrtype *shmad )
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
        print("\n","(%d):getQueneValue Empty\n", getpid());
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
        print("\n","(%d):putQueneValue Full\n", getpid());
        return;
    }
    else
    {
        quene->shmad[quene->head] = value;
        quene->head = (quene->head+1)%QUENE_NUM;
    }
}




/*log*/

void print(const char *name, const char *buff, ...)
{
    struct stat sb;
    va_list args;
    char logibuf[64]={0,};

    if(logi.ini==0)
    {
        logi.ini = 1;
        logi.fd = open(PATH_NAME, O_CREAT|O_WRONLY|O_APPEND, 0777);
        if(logi.fd<0)
        {
            printf("open fail\n");
            return;
        }
        else
        {
            printf("(%d)open log success\n", getpid());
        }
    }
    /*if(stat(PATH_NAME, &sb)!=0)*/
    /*{*/
    /*    printf("stat fail\n");*/
    /*    return;*/
    /*}*/
    if(write(logi.fd, name, strlen(name))<1)
    {
        printf("write name fail\n");
        return;
    }

    va_start(args, buff);
    vsprintf((char*)logibuf, buff, args);
    va_end(args);
    if(write(logi.fd, logibuf, 1+strlen(buff))<1)/*\0 he \n*/
    {
        printf("write buff fail\n");
        return;
    }
    
    sync();
}
void closelog()
{
    if(logi.ini==1)
    {
        if(close(logi.fd)!=0)
        {
            printf("close fail\n");
            return;
        }
        logi.ini = 0;
    }
}