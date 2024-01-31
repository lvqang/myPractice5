#include"sys/ipc.h"
#include"unistd.h"
#include<linux/fs.h>
#include"fcntl.h"
#include<linux/kernel.h>
#include<linux/sched.h>
#include<asm/system.h>
#include <asm/segment.h>
#include <string.h>
#include<linux/lamport.h>


#define SEM_NUM  64
#define LAMPORT  1

typedef struct
{
    sem_t sem;
    char name[20];
    unsigned int value;
    //unsigned int curvalue;
    struct task_struct *wait;
}structSem;
structSem semarray[SEM_NUM] ={{0,"\0",0,0,NULL},};

int sys_inisem()
{
    int i;

    for(i=0;i<SEM_NUM;i++)
    {
        semarray[i].name[0] = '\0';
        semarray[i].sem = 0;
        semarray[i].value = 0;
        semarray[i].wait = NULL;
    }
    return 0;
}

/*
oflag   
O_CREAT不存在的时候创建
O_CREAT|O_EXCL 存在返回错误
*/
sem_t *sys_sem_open(const char *name, int oflag, unsigned int value)
{
    int i, inull=-1;
    char kername[20] = {0,};

    if(name ==NULL)
    {
        printk("sem_open:name is null\n");
        return SEM_FAILED;
    }
    else
    {
        for(i=0;i<20;i++)
        {
            if(!(kername[i] = get_fs_byte(&name[i])))
            {
                break;
            }
            if(i==19)
            {
                kername[i] = '\0';
                break;
            }
        }
    }
    for(i=0;i<SEM_NUM;i++)
    {
        if(0==strncmp(kername, semarray[i].name, sizeof(semarray[i].name)))//name 要从用户空间获取
        {
            break;
        }
        if(semarray[i].sem==0 && inull<0)
        {
            inull = i;
        }
    }
    if((oflag&O_CREAT) && i>=SEM_NUM)
    {
        if(inull>=0)
        {
            strncpy(semarray[inull].name, kername, sizeof(semarray[i].name));
            semarray[inull].sem = inull+1;//从1开始
            semarray[inull].value = value;
            //semarray[inull].curvalue = 0;
            semarray[inull].wait = NULL;
            return &(semarray[inull].sem);
        }
        else
        {
            printk("sem_open:have no null sem\n");
            return SEM_FAILED;
        }
    }
    else if(i<SEM_NUM && (oflag&O_CREAT) && (oflag&O_EXCL))
    {
        printk("sem_open:O_EXCL\n");
    }
    else if(i<SEM_NUM)
    {
        return &(semarray[i].sem);
    }
    else
    {
        printk("sem_open:unknow\n");
    }
    //O_RDONLY  O_WRONLY  O_RDWR
    return SEM_FAILED;
}

int sys_sem_wait(sem_t *sem)
{
    int i;
    structSem *cursem = NULL;
    //struct task_struct *p = NULL;

    if(sem == NULL)
    {
        printk("sem_wait:para err\n");
        return -1;
    }
    for(i=0;i<SEM_NUM;i++)
    {
        if((*sem)==semarray[i].sem)
        {
            cursem = &semarray[i];
            break;
        }
    }
    if(i>=SEM_NUM)
    {
        printk("sem_wait:have no exist\n");
        return -1;
    }
    //临界区保护
#if LAMPORT
    lamportLock(current->pid);
#else    
    cli();
#endif    
    while(0>=cursem->value)
    {
    #if LAMPORT
        lamportUnlock(current->pid);
    #else     
        sti();
    #endif 

        sleep_on(&(cursem->wait));

    #if LAMPORT
        lamportLock(current->pid);
    #else    
        cli();
    #endif     
    }   
    cursem->value++;
    //
#if LAMPORT
    lamportUnlock(current->pid);
#else     
    sti();
#endif  
  
    return 0;
}


int sys_sem_post(sem_t *sem)
{
    int i;
    structSem *cursem = NULL;
    //struct task_struct *p = NULL;

    if(sem == NULL)
    {
        printk("sem_post:para err\n");
        return -1;
    }

    for(i=0;i<SEM_NUM;i++)
    {
        if((*sem)==semarray[i].sem)
        {
            cursem = &semarray[i];
            break;
        }
    }
    if(i>=SEM_NUM)
    {
        printk("sem_wait:have no exist\n");
        return -1;
    }

        //临界区保护
#if LAMPORT
    lamportLock(current->pid);
#else    
    cli();
#endif  

    cursem->value++;

#if LAMPORT
    lamportUnlock(current->pid);
#else     
    sti();
#endif    
    wake_up(&(cursem->wait));
    return 0;
}

int sys_sem_unlink(const char *name)
{
    int i;
    char kername[20] = {0,};

    if(name ==NULL)
    {
        printk("sem_unlink:name is null\n");
        return -1;
    }
    else
    {
        for(i=0;i<sizeof(semarray[i].name);i++)
        {
            if(!(kername[i] = get_fs_byte(&name[i])))
            {
                break;
            }
            if(i==19)
            {
                kername[i] = '\0';
                break;
            }
        }
    }
    for(i=0;i<SEM_NUM;i++)
    {
        if(0==strncmp(kername, semarray[i].name, sizeof(semarray[i].name)))//name 要从用户空间获取
        {
            break;
        }
    }
    if(i>=SEM_NUM)
    {
        printk("sem_unlink:have no exist sem\n");
        return -1;
    }
    else
    {
        semarray[i].name[0] = '\0';
        semarray[i].sem = 0;//从1开始
        semarray[i].value = 0;
        //semarray[i].curvalue = 0;
        semarray[i].wait = NULL;
    }
    return 0;
}
