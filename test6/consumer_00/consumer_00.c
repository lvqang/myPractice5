
#include<stdio.h>
#include<unistd.h>
#include "../sem_shm.h"



int main()
{
    int i;
    int flag;
    /*char consum_name[15];*/
    shmadrtype *shmadd;
    sem_t *mutex;
    sem_t * full; 
    sem_t *empty;


    print("consumer", "(%d):start\n", getpid());
/*sem  shm*/
    if( 0 != crearShmAndSem(&(shmadd), &mutex, &full, &empty) )
    {
        printf("p(%d) crearShmAndSem fail\n", getpid());
        print("consumer", "(%d):crearShmAndSem fail\n",getpid());
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

    flag = 1;
    while (flag)/*读1-500*/
    {
        /*print("co", "(%d)1:->\n",getpid());*/
        sem_wait(full);
        /*print("co", "(%d)2:->\n",getpid());*/
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
            /*printf("%d: %d\n", getpid(), i);*/
            print("co", "(%d):r=%d\n",getpid(), i);
        }
        else
        {
            print("co", "(%d):r err=%d\n",getpid(), i);
        }
        if(i==SHMDATA)
        {
            shmadd->endflag = 0x55AA;
            flag = 0;
        }
        sem_post(mutex);
        /*print("co", "(%d):<-\n",getpid());*/
        sem_post(empty);
    }
    
    sem_post(full);/*for exit other process*/
    printf("p(%d) consumer exit\n", getpid());
    print("consumer", "(%d):consumer exit\n",getpid());
    if(deleteShmAndSem(shmadd )!=0)
    {
        printf("p(%d) deleteShmAndSem fail\n", getpid());
        /*print("pc:", "deleteShmAndSem fail\n");*/
    }
    closelog();
    return 0;
}




