
#include<sys/ipc.h>
#define __LIBRARY__
#include<unistd.h>

// #define __NR_sem_open	72
// #define _syscall3(type,name,atype,a,btype,b,ctype,c) \
// type name(atype a,btype b,ctype c) \
// { \
// long __res; \
// __asm__ volatile ("int $0x80" \
// 	: "=a" (__res) \
// 	: "0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b)),"d" ((long)(c))); \
// if (__res>=0) \
// 	return (type) __res; \
// errno=-__res; \
// return -1; \
// }


_syscall3(sem_t *,sem_open,const char *,name,int,oflag,unsigned int,value)
_syscall1(int,sem_wait,sem_t *,sem)
_syscall1(int,sem_post,sem_t *,sem)
_syscall1(int,sem_unlink,const char *,name)


_syscall2(key_t,ftok,const char *,pathname,int,proj_id)
_syscall3(int,shmget,key_t,key,size_t,size, int,shmflg)
_syscall3(void*,shmat,int,shmid,const void *,shmaddr,int,shmflg)
_syscall1(int,shmdt,const void *,shmaddr)


