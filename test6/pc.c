
#include<stdio.h>
#include<unistd.h>
#include "sem_shm.h"




int main()
{
    int i;
    pid_t pid;
    char consum_name[30];
    shmadrtype *shmadd;
    sem_t *mutex;
    sem_t * full; 
    sem_t *empty;

    for(i=0; i<CONSUM_NUM; i++)
    {
        if((pid=fork())==0)
        {
            printf("creat process=%d\n", getpid());
            print("pc:", "creat process=%d\n", getpid());
            memset(consum_name, 0, sizeof(consum_name));
            sprintf(consum_name, "./consumer_00/consumer_%02d", i);
            execve(consum_name, NULL,NULL);
            break;
        }
        else if(pid>0)
        {
            /*printf("creat process=%d\n", getpid());*/
        }
    }
    while (1)
    {
        sleep(20);
    }
    
    printf("pc begain write\n");    
/*sem  shm*/
    if( 0 != crearShmAndSem(&(shmadd), &mutex, &full, &empty) )
    {
        printf("p(%d) crearShmAndSem fail\n", getpid());
        print("pc:", "crearShmAndSem fail\n");
        return 0;
    }
    sem_wait(mutex);
    if(shmadd->head!=0x55AA)
    {
        shmadd->head = 0;
        shmadd->tail = 0;
        memset(shmadd->shmad, 0, sizeof(shmadd->shmad));
        shmadd->iniflag = 0x55AA;
        shmadd->endflag = 0;
    } 
    sem_post(mutex);

    i = 0;
    while (i<SHMDATA)/*写入1-500*/
    {
        i ++;
        sem_wait(empty);
        sem_wait(mutex);
        putQueneValue(shmadd, i);
        printf("pc: write=%d\n", i);
        /*print("pc:", "write=%d\n", i);*/
        sem_post(mutex);
        sem_post(full);
    }
    
    if(deleteShmAndSem(shmadd )!=0)
    {
        printf("p(%d) deleteShmAndSem fail\n", getpid());
        /*print("pc:", "deleteShmAndSem fail\n");*/
    }

    sleep(20);
    printf("p(%d) producer exit\n", getpid());
    closelog();
    /*print("pc:", "producer exit\n");*/
    return 0;
}


