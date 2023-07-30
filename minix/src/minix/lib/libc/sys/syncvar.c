#define _SYSTEM 1 /* for defining OK */
#include <errno.h>
#include <unistd.h> /* getpid() */
#include <string.h> /* memset() */
#include <stdlib.h> /* exit() */
#include <minix/syncvar.h>

/*
 * syncvar endpoint
 */
int get_syncvar_endpt(endpoint_t *pt){
	int r;
	r = minix_rs_lookup("syncvar", pt);
	if (r != OK) {
		printf("Error trying to find SYNCVAR service: %d\n", r);
		exit(1);
	}
	return r;
}

/* mutex init */
int mutex_init(void){
    message msg;
    int r;
    endpoint_t pt;

    memset(&msg, 0, sizeof(msg));

    get_syncvar_endpt(&pt);
    r = _syscall(pt, MUTEX_INIT, &msg);
    if (r != OK){
        return msg.m_type;
    }
    return msg.m_syncvar.mutex_num;
}

/* mutex destroy */
int mutex_destroy(int mutex_num){
    message msg;
    int r;
    endpoint_t pt;

    if (mutex_num < 0){
        return EINVAL;
		}

    msg.m_syncvar.mutex_num = mutex_num;

    get_syncvar_endpt(&pt);
    r = _syscall(pt, MUTEX_DESTROY, &msg);
    if (r != OK){
        return msg.m_type;
    }
    return OK;
}

/* lock */
int mutex_lock(int mutex_num){
    message msg;
    int r;
    endpoint_t pt;

    if (mutex_num < 0){
        return EINVAL;
		}

    msg.m_syncvar.mutex_num = mutex_num;

    get_syncvar_endpt(&pt);
    r = _syscall(pt, MUTEX_LOCK, &msg);
    if (r != OK) {
        return msg.m_type;
    }
    return OK;
}

/* unlock */
int mutex_unlock(int mutex_num){
    message msg;
    int r;
    endpoint_t pt;

    if (mutex_num < 0){
        return EINVAL;
		}

    msg.m_syncvar.mutex_num = mutex_num;

    get_syncvar_endpt(&pt);
    r = _syscall(pt, MUTEX_UNLOCK, &msg);
    if (r != OK) {
        return msg.m_type;
    }
    return OK;
}

/* cond init */
int cond_init(void){
    message msg;
    int r;
    endpoint_t pt;

    memset(&msg, 0, sizeof(msg));

    get_syncvar_endpt(&pt);
    r = _syscall(pt, COND_INIT, &msg);
    if (r != OK) {
        return r;
    }
    return msg.m_syncvar.cond_num;
}

/* cond destroy */
int cond_destroy(int cond_num){
    message msg;
    int r;
    endpoint_t pt;

    if (cond_num < 0){
        return EINVAL;
		}

    msg.m_syncvar.cond_num = cond_num;

    get_syncvar_endpt(&pt);
    r = _syscall(pt, COND_DESTROY, &msg);
    if (r != OK) {
        return msg.m_type;
    }
    return OK;
}

/* cond wait */
int cond_wait(int cond_num, int mutex_num){
    message msg;
    int r;
    endpoint_t pt;

    if (cond_num < 0){
        return EINVAL;
		}

    msg.m_syncvar.cond_num = cond_num;

    get_syncvar_endpt(&pt);
    r = _syscall(pt, COND_WAIT, &msg);
    if (r != OK) {
        return msg.m_type;
    }
    return OK;
}

/* cond signal */
int cond_signal(int cond_num){
    message msg;
    int r;
    endpoint_t pt;

    if (cond_num < 0){
        return EINVAL;
		}

    msg.m_syncvar.cond_num = cond_num;

    get_syncvar_endpt(&pt);
    r = _syscall(pt, COND_SIGNAL, &msg);
    if (r != OK) {
        return msg.m_type;
    }
    return OK;
}

/* cond broadcast */
int cond_broadcast(int cond_num){
    message msg;
    int r;
    endpoint_t pt;

    if (cond_num < 0){
        return EINVAL;
		}

    msg.m_syncvar.cond_num = cond_num;

    get_syncvar_endpt(&pt);
    r = _syscall(pt, COND_BROADCAST, &msg);
    if (r != OK) {
        return msg.m_type;
    }
    return OK;
}
