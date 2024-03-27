/*
 *  linux/fs/proc.c
 *
 *  (C) 2014  bai ao
 */

#include <signal.h>

#include <linux/sched.h>
#include <asm/segment.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <linux/kernel.h>
#include <string.h> 


extern int vsprintf(char * buf, const char * fmt, va_list args);
static int sprint(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);

	i=vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}

int pro_ps(struct m_inode * inode, unsigned long * pos, char * buf, int count)
{
#define TABLE  10 
#define MAXBUF  1024  
    int i;
    static int p=0;
    char tmpbuf[MAXBUF], *tbuf = tmpbuf;

    if((*pos)==0)
    {
        p=0;
        //                10byte
        sprint(tbuf, "pid       state     father    conter    start_time\n");
        p += (5*TABLE+1);tbuf += (5*TABLE+1);
        for (i = 0; i < NR_TASKS; i++)
        {
            if (task[i])
            {
                sprint(tbuf, "%-010d%-010d%-010d%-010d%-010d\n", task[i]->pid,task[i]->state,
                task[i]->father,task[i]->counter,task[i]->start_time);
                tbuf += (5*TABLE+1);

                p += (5 * TABLE+1);
                if ((p+(5*TABLE+1)) >= MAXBUF)
                {
                    break;
                }
            }
        }
        tmpbuf[p + 1] = '\0';
    }

    if(p>count)
    {
        //memcpy(buf, tmpbuf+(*pos), count);
        for(i=0;i<count;i++)
        {
            put_fs_byte(*(tmpbuf + (*pos)), buf + i);
            (*pos) += 1;
            p--;
        }
        return i;
    }
    else
    {
        //memcpy(buf, tmpbuf+(*pos), p);
        for(i=0;i<p;i++)
        {
            put_fs_byte(*(tmpbuf+(*pos)), buf+i);
            (*pos) += 1;
        }
        p=0;    
        return i;
    }
    
#undef MAXBUF    
#undef TABLE
}

int pro_hd(struct m_inode * inode, unsigned long * pos, char * buf, int count)
{
#define TABLE  23 
#define MAXBUF  256  
    int i,j;
    static int p=0;
    char tmpbuf[MAXBUF], *tbuf = tmpbuf;
    struct super_block * sb;
    struct buffer_head * bh;
    int total_blocks = 0, used_block = 0;

    if((*pos)==0)
    {
        sb = get_super(inode->i_dev);
        total_blocks = sb->s_nzones;
        for(i=0;i<sb->s_zmap_blocks;i++)
        {
            bh=sb->s_zmap[i];
            for(j=0;j<(BLOCK_SIZE*8);j++)
            {
                if((bh->b_data[j>>3]>>(j%8)))
                {
                    used_block++;
                }
            }
        }
        sprint(tbuf, "total_blocks:  %-08d\n",total_blocks);
        tbuf += (TABLE+1);
        sprint(tbuf, "used_blocks:   %-08d\n",used_block);
        tbuf += (TABLE+1);
        sprint(tbuf, "free_blocks:   %-08d\n",total_blocks-used_block);
        p += 3*(TABLE+1);
    }

    if(p>count)
    {
        //memcpy(buf, tmpbuf+(*pos), count);
        for(i=0;i<count;i++)
        {
            put_fs_byte(*(tmpbuf + (*pos)), buf + i);
            (*pos) += 1;
            p--;
        }
        return i;
    }
    else
    {
        //memcpy(buf, tmpbuf+(*pos), p);
        for(i=0;i<p;i++)
        {
            put_fs_byte(*(tmpbuf+(*pos)), buf+i);
            (*pos) += 1;
        }
        p=0;    
        return i;
    }
    
#undef MAXBUF    
#undef TABLE
}

procfs nodeproc[] =
{
    {1, "/proc/psinfo",    S_IRUSR | S_IRGRP | S_IROTH | S_ISPRO, 1, pro_ps},
    {1, "/proc/hdinfo",    S_IRUSR | S_IRGRP | S_IROTH | S_ISPRO, 2, pro_hd},
    {0, "/proc/inodeinfo", S_IRUSR | S_IRGRP | S_IROTH | S_ISPRO, 3},
    {0, "/proc/meminfo",   S_IRUSR | S_IRGRP | S_IROTH | S_ISPRO, 4},
};
unsigned int procLen = sizeof(nodeproc)/sizeof(procfs);

int proc_read(struct m_inode * inode, unsigned long * pos, char * buf, int count)
{
    int i, read = 0;
    if(inode->i_zone[0]<=0 || pos==NULL || buf==NULL)
    {
        printk("%s para err\r\n",__FUNCTION__);
        return -ERROR;
    }
    if(count<1)
    {
        return 0;
    }
    for(i=0;i<procLen;i++)
    {
        if(inode->i_zone[0]==nodeproc[i].dev)
        {
            if (!nodeproc[i].creat)
            {
                break;
            }
            read = nodeproc[i].func(inode,pos,buf,count);
        }
        
    }
    return read;
}


int proc_write()
{
    return 0;
}
