#define _SYSTEM		1	/* get OK and negative error codes */

#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>
#include <minix/rmib.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <machine/param.h>
#include <machine/vm.h>
#include <machine/vmparam.h>
#include <errno.h>

/* mutex.c */
int do_mutex_init(message *);
int do_mutex_destroy(message *);
int do_mutex_lock(message *);
int do_mutex_unlock(message *);

/* cond.c */
int do_cond_init(message *);
int do_cond_destroy(message *);
int do_cond_wait(message *);
int do_cond_signal(message *);
int do_cond_broadcast(message *);
