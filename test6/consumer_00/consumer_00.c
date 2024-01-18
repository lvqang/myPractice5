

#include<stdio.h>
#include<ipc.h>
#include<unistd.h>
#include"sem_shm.h"




int main()
{
    int i;
    pid_t pid;
    //char consum_name[15];
    shmadrtype *shmadd;
    sem_t *mutex;
    sem_t * full; 
    sem_t *empty;



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

    while (1)//读1-500
    {
        sem_wait(full);
        sem_wait(mutex);
        if(shmadd->endflag==0x55AA)
        {
            sem_post(mutex);
            sem_post(empty);
            break;
        }
        i = getQueneValue(shmadd);
        if(i>=0)
        {
            printf("%d: %d\n", getpid(), i);
        }
        else
        {
            printf("%d getvalue err: %d\n", getpid(), i);
        }
        if(i==SHMDATA)
        {
            shmadd->endflag = 0x55AA;
        }
        sem_post(mutex);
        sem_post(empty);
    }
    
    printf("p(%d) consumer exit\n", getpid());
    return 0;
}




