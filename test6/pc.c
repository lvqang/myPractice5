
#include<stdio.h>
#include<sys/ipc.h>
#include<unistd.h>
#include"sem_shm.h"




int main()
{
    int i;
    pid_t pid;
    char consum_name[15];
    shmadrtype *shmadd;
    sem_t *mutex;
    sem_t * full; 
    sem_t *empty;

    for(i=0; i<CONSUM_NUM; i++)
    {
        if((pid=fork())==0)
        {
            printf("creat process=%d\n", getpid());
            memset(consum_name, 0, sizeof(consum_name));
            sprintf(consum_name, "./consumer_%02d", i);
            execve(consum_name, NULL,NULL);
            break;
        }
        else if(pid>0)
        {
            //printf("creat process=%d\n", getpid());
        }
    }

//sem  shm
    if( 0 != crearShmAndSem(&(shmadd), &mutex, &full, &empty) )
    {
        printf("p(%d) crearShmAndSem fail\n", getpid());
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
    while (i<SHMDATA)//写入1-500
    {
        i ++;
        sem_wait(empty);
        sem_wait(mutex);
        putQueneValue(shmadd, i);
        sem_post(mutex);
        sem_post(full);
    }
    
    printf("p(%d) producer exit\n", getpid());
    return 0;
}


