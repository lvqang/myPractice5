#ifndef _IPC_H
#define _IPC_H

#ifdef __cplusplus/*告诉C++代码使用C的方式链接这个库函数-即这段代码要被C++使用，但是用C的链接方式*/
extern "C" {
#endif

#include<string.h>

typedef unsigned long key_t;
typedef unsigned long sem_t;

/*sem*/
#define SEM_FAILED  ((void *) 0)

/*extern sem_t *sem_open(const char *name, int oflag, unsigned int value);*/
/*extern int sem_wait(sem_t *sem);*/
/*extern int sem_post(sem_t *sem);*/
/*extern int sem_unlink(const char *name);*/
extern int sys_inisem();

/*shm*/
#define IPC_PRIVATE	((key_t) 0)	/* Private key.  */
#define IPC_CREAT	01000		/* Create key if key does not exist. */
#define IPC_EXCL	02000		/* Fail if key exists.  */
#define SHM_RND     00001
#define SHMLBA      1

/*extern key_t ftok(const char *pathname, int proj_id);*/
/*extern int shmget(key_t key, size_t size, int shmflg);*/
/*extern void* shmat(int shmid, const void * shmaddr, int shmflg);*/
/*extern int shmdt(const void *shmaddr);*/
extern int sys_inishm();

#ifdef __cplusplus 
} 
#endif 


#endif
