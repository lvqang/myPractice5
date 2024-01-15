#ifndef _LAMPORT_H
#define _LAMPORT_H

#ifdef __cplusplus//告诉C++代码使用C的方式链接这个库函数-即这段代码要被C++使用，但是用C的链接方式
extern "C" {
#endif

#include"sched.h"
#include"kernel.h"
#define TRUE   1
#define FALSE  0


typedef struct
{
    int pid;         //pid从1开始递增  不止64个  0号进程永远不会
    int choosing;   //标记
    int num;        //进程的号码
}_lamport;
_lamport lamparray[NR_TASKS] = {{0,0,0},};

static inline void lamportLock(int pid)
{                              
    int i,j, nulltask = 0;              
    for(i=1;i<NR_TASKS;i++)
    {
        if(lamparray[i].pid==pid)//进程重启后失效了 考虑用进程名
        {
            break;
        }
        if(lamparray[i].pid==0)
        {
            nulltask = i;
        }
    }
    if(i>=NR_TASKS)
    {
        lamparray[nulltask].pid = pid;
    }
    else
    {
        nulltask = i;
        //lamparray[i].pid = pid;
    }
    lamparray[nulltask].choosing = TRUE;
    lamparray[nulltask].num = 0;
    for(j=1;j<NR_TASKS;j++)
    {
        if(j!=nulltask)
        {
            lamparray[nulltask].num = (lamparray[j].num>lamparray[nulltask].num? lamparray[j].num:lamparray[nulltask].num);
        } 
    }
    lamparray[nulltask].num = lamparray[nulltask].num + 1;
    lamparray[nulltask].choosing = FALSE;
    for(j=0;j<NR_TASKS;j++)
    {
        while(lamparray[j].choosing);   //保护lamparray[j].num互斥
        while(lamparray[j].num!=0 && ((lamparray[j].num<lamparray[nulltask].num) || (lamparray[j].num==lamparray[nulltask].num && j<nulltask) ));
    }
}

static inline void lamportUnlock(int pid)
{
    int i;

    for(i=1;i<NR_TASKS;i++)
    {
        if(lamparray[i].pid==pid)//进程重启后失效了 考虑用进程名
        {
            break;
        }
    }
    if(i>=NR_TASKS)
    {
        printk("lamportUnlock:unlock null\n");
    }
    else
    {
        lamparray[i].num = 0;
    }
}


#ifdef __cplusplus 
} 
#endif 

#endif
