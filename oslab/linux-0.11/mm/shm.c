#include <sys/ipc.h>
#include "unistd.h"
#include <linux/fs.h>
#include <fcntl.h>
#include <linux/kernel.h>
#include <linux/sched.h>    //attemp to export path 


#define PAGE_SIZE  4096
#define SHM_NUM  64
#define PAGE_NUM  64
#define TASK_NUM  (NR_TASKS/8+1)
typedef struct 
{
    key_t key;                      //索引值
    unsigned int pageNum;           //页数
    unsigned long shmaddr[PAGE_NUM];//物理内存最多64页
    unsigned char pid[TASK_NUM];    //pid位

    int shm_nattch;                 //关联的虚拟地址数量
    int shm_atime;
    int shm_dtime;
}shmstruct;
shmstruct shmnum[SHM_NUM];

/*
pathname：目录或者文件
proj_id：自定义的id
返回值失败-1，成功返回键值
*/
key_t sys_ftok(const char *pathname, int proj_id)
{
    struct m_inode * inode;
    int flag = O_RDONLY, mode = 0770;
    //int i;
    key_t result = 0;

    if(pathname==NULL || proj_id==0)
    {
        printk("ftok para err\n\r");
        return -1;
    }
    if (open_namei(pathname,flag,mode,&inode) < 0) 
    {
		return -1;
	}
    result = ((inode->i_num<<8)&0x00FFFF00) | (proj_id&0x000000FF);
    return result;
}

//find a address which has pageNum pages
static unsigned long findEmptyAdress(unsigned int pageNum)
{
    unsigned long data_limit,code_base,data_base;
    int i;
    unsigned long tmp, *page_table;

    code_base  = get_base(current->ldt[1]);
    data_base  = code_base;//database is same as withc codebahe at linux 0.11
    data_limit = 0x4000000;//data limit is 64M

    data_base += data_limit;
    for(i=0;i<pageNum && data_base>PAGE_SIZE;)
    {
        data_base -= PAGE_SIZE;
        page_table = (unsigned long *) ((data_base>>20) & 0xffc);
        if((*page_table)&1)
        {
            page_table = (unsigned long *)((*page_table)&0xFFFFF000);
        }
        else
        {
            if(!(tmp = get_free_page()))
            {
                printk("findEmptyAdress get free page fail\n");
                return 0;
            }
            *page_table = tmp|7;
            page_table = (unsigned long *) tmp;
        }
        if((*page_table)&1)
        {
            i=0;
            continue;
        }
        else
        {
            i++;
        }
        if(i==pageNum)
        {
            return data_base;
        }
    }
    return 0;
}
/*
key：共享内存键值（可以自己定义，也可以依靠ftok函数）
size：共享内存大小
shmflg：  
IPC_CREAT：共享内存不存在则创建
mode：共享内存 rwx 权限
创建内存：1、key==IPC_PRIVATE(0) 2、shmflg=IPC_CREAT且无创建好的内存
shmflg同时IPC_CREAT|IPC_EXCL 且已经存在共享内存，则返回错误
shmflg仅有IPC_CREAT则没有共享内存创建，有的话返回已经存在的内存
返回值：
成功：共享内存id
失败：-1
*/
int sys_shmget(key_t key, size_t size, int shmflg)
{
    unsigned int pageNum;
    int i;
    shmstruct *shmNull=NULL, *shmKey=NULL;
    int shmNullID = -1, shmKeyID = -1;

    if(size<=0)
    {
        printk("shmget size err");
        return -1;
    }

    for(i=0;i<SHM_NUM;i++)
    {
        if(shmnum[i].key==key)
        {
            shmKey = &shmnum[i];
            shmKeyID = i;
        }
        if(shmnum[i].key==-1)
        {
            shmNull = &shmnum[i];
            shmNullID = i;
        }
        if(shmKey && shmNull)
        {
            break;
        }
    }
    if(i>=SHM_NUM)
    {
        printk("creat too much shm\n");
        return -1;
    }
    pageNum = (size+0x0FFF)/4096;
    if(pageNum>PAGE_NUM)
    {
        printk("pageNum too much:%d\n",pageNum);
        return -1;
    }

    if(key==IPC_PRIVATE || (shmflg&IPC_CREAT && shmKey==NULL))
    {
        for(i=0;i<pageNum;i++)
        {
            if(!(shmNull->shmaddr[i] = get_free_page()))
            {
                while(i>=0)
                {
                    free_page(shmNull->shmaddr[i]);
                    shmNull->shmaddr[i] = 0;
                    i--;
                }
                shmNull->pageNum = 0;
                shmNull->key = -1;
                printk("get_free_page fail\n");
                return -1;
            }
        }
        shmNull->pageNum = pageNum;
        shmNull->key = key;
        //shmNull->shm_atime = current->cutime;
        shmNull->shm_nattch = 0;
        return shmNullID;
    }
    else if((shmflg&IPC_CREAT) && !(shmflg&IPC_EXCL) && shmKey!=NULL)
    {
        return shmKeyID;
    }
    else
    {
        printk("key=%d, shmflg=0x%08X\n",key, shmflg);
        return -1;
    }
}

/*
shmid：共享内存id
shmaddr：映射地址，为NULL时表示自动分配
shmflg：
SHM_RDONLY：只读方式映射
0：可读可写
shmflg&SHM_RND —— shmaddr非空  映射的地址是(shmaddr-shmaddr%SHMLBA)
shmflg无SHM_RND ——shmaddr非空 映射的地址是shmaddr
返回值：
成功：共享内存地址
失败：-1
*/
void* sys_shmat(int shmid, const void * shmaddr, int shmflg)
{
    int i = 0;
    unsigned long address, tmpaddr;
    shmstruct *shmKey=NULL;

    if(shmid<0 || shmid>=SHM_NUM)
    {
        printk("shmat para err\n");
        return (void *)-1;
    }
    shmKey = &shmnum[shmid];

    if(shmaddr==NULL)
    {
        //find first available address
        if((address=findEmptyAdress(shmKey->pageNum))>0)
        {
            tmpaddr = address;
            for(i=0;i<shmKey->pageNum;i++)
            {
                (void)put_page(shmKey->shmaddr[i],tmpaddr);
                tmpaddr += PAGE_SIZE;
            }
            shmKey->shm_atime = current->cutime;
            shmKey->shm_nattch ++;
            shmKey->pid[current->pid/8] |= (1<<(current->pid%8));
            return (void *)address;
        }
        else
        {
            printk("shmat have no logic addr\n\r");
            return (void *)-1;
        }
    }
    else
    {
        if(shmflg & SHM_RND)
        {
            address = (unsigned long)shmaddr -((unsigned long)shmaddr %SHMLBA);
            tmpaddr = address;
            for(i=0;i<shmKey->pageNum;i++)
            {
                if(!(put_page(shmKey->shmaddr[i],address)))
                {
                    printk("put_page fail=%d\n",i);
                    return (void *)-1;
                }
                tmpaddr += PAGE_SIZE;
            }
            shmKey->shm_atime = current->cutime;
            shmKey->shm_nattch ++;
            shmKey->pid[current->pid/8] |= (1<<(current->pid%8));
            return (void *)address;
        }
        else
        {
            address = (unsigned long)shmaddr;
            tmpaddr = address;
            for(i=0;i<shmKey->pageNum;i++)
            {
                if(!(put_page(shmKey->shmaddr[i], (unsigned long)shmaddr)))
                {
                    printk("put_page fail=%d\n",i);
                    return (void *)-1;
                }
                tmpaddr += PAGE_SIZE;
            }
            shmKey->shm_atime = current->cutime;
            shmKey->shm_nattch ++;
            shmKey->pid[current->pid/8] |= (1<<(current->pid%8));
            return (void *)address;
        }
    }
}


/*
功能：解除共享内存映射
shmaddr：映射地址

返回值：
成功：0
失败：-1
*/
int sys_shmdt(const void *shmaddr)
{
    int i;
    shmstruct *shmKey;

    if(shmaddr==NULL)
    {
        printk("shmdt para err\n");
        return -1;
    }
    for(i=0;i<SHM_NUM;i++)
    {
        if(shmnum[i].key>=0 && (shmnum[i].pid[current->pid/8]&(1<<(current->pid%8))))
        {
            shmKey=&shmnum[i];
            break;
        }
    }
    if(i>=SHM_NUM)
    {
        printk("shmdt i=%d\n",i);
        return -1;
    }
    if(shmKey->shm_nattch>0)
    {
        shmKey->shm_nattch --;
        shmKey->pid[current->pid/8] &= (~((unsigned char)(1<<(current->pid%8))));


        if(shmKey->shm_nattch==0)
        {
            shmKey->key = -1;
            for(i=0;i<shmKey->pageNum;i++)
            {

                free_page(shmKey->shmaddr[i]);
            }
            shmKey->pageNum = 0;
        }
    }
    
    return 0;
}



/*
获取或设置共享内存的相关属性
shmid：共享内存id
cmd：
IPC_STAT：获取共享内存的属性信息，由参数buf返回
IPC_SET：设置共享内存的属性，由参数buf传入
IPC_RMID：删除共享内存
buf：属性缓冲区

返回值：
成功：由cmd类型决定，上面列出的这三个都返回-1.
失败：-1
*/
//int shmctl(int shmid,int cmd,struct shmid_ds *buf);
