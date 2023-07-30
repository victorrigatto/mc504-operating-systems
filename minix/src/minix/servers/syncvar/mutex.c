/* This file contains the main program of the mutex for the SYNCVAR service. */

#include <stdlib.h>
#include "svar.h"
#include "queue.h"
#include <sys/errno.h>

#define OK 0
#define ACTIVE 1
#define INACTIVE 0
#define UNLOCKED 1
#define BLOCKED 0

/*
 * data structure with the necessary info for a single mutex variable
 * identification is given by its index in the mtx_vec
 * CHANGE AS NEEDED FOR THE NEXT PARTS
 */
typedef struct process{
    endpoint_t process;  //endpoint da mensagem
} process_t;

typedef struct mutex{
    uint8_t lock_status;
    uint8_t mutex_status;
    thread_queue_t process_queue;
} mutex_t;

/* file-scoped global variables */
static mutex_t * mutex_queue;
static unsigned long mtx_cap = 0;
static unsigned long mtx_count = 0;

/* mutex init */
int do_mutex_init(message *m){
    static mutex_t * mutex_queue_aux;

    /* novo mutex */
    if (mtx_cap == 0 || mutex_queue == NULL){
        mtx_cap = 2;
        mutex_queue = malloc(mtx_cap * sizeof(mutex_t));
    }

    /* aumenta fila de mutex até o limite de memória do sistema */
    if (mtx_count == mtx_cap){
        mutex_queue_aux = realloc(mutex_queue, 2 * mtx_cap * sizeof(mutex_t));
        if(mutex_queue_aux != NULL){
            mutex_queue = mutex_queue_aux;
            mtx_cap *= 2;
        } else{
            return ENOMEM;
        }
    }

    if (mutex_queue == NULL){
        return ENOMEM;
    }

    /* retorna a mensagem */
    m->m_syncvar.mutex_num = mtx_count;
    mutex_queue[mtx_count].mutex_status = ACTIVE;
    mutex_queue[mtx_count].lock_status = UNLOCKED;
    mutex_queue[mtx_count].process_queue = init_queue();
    mtx_count++;

    return OK;
}

/* mutex destroy */
int do_mutex_destroy(message * m){
    if(m->m_syncvar.mutex_num >= mtx_count || mutex_queue[m->m_syncvar.mutex_num].mutex_status == INACTIVE){
        return EINVAL;
    }

    if(mutex_queue[m->m_syncvar.mutex_num].lock_status == BLOCKED){
        return EBUSY;
    }

    mutex_queue[m->m_syncvar.mutex_num].mutex_status = INACTIVE;

    return OK;
}

/* lock */
int do_mutex_lock(message *m){
    endpoint_t pt = m->m_source;

    if(m->m_syncvar.mutex_num >= mtx_count || mutex_queue[m->m_syncvar.mutex_num].mutex_status==INACTIVE){
        return EINVAL;
    }

    mutex_queue[m->m_syncvar.mutex_num].lock_status = BLOCKED;

    if(peek(&mutex_queue[m->m_syncvar.mutex_num].process_queue, &pt) && pt == m->m_source){
        return ELOCKED;
    }

    enqueue(&mutex_queue[m->m_syncvar.mutex_num].process_queue, m->m_source);

    peek(&mutex_queue[m->m_syncvar.mutex_num].process_queue, &pt);

    if(pt != m->m_source){
        return SUSPEND;
    }

    return OK;
}

/* unlock */
int do_mutex_unlock(message * m){
    endpoint_t pt = (endpoint_t)0;

    if(m->m_syncvar.mutex_num >= mtx_count || mutex_queue[m->m_syncvar.mutex_num].mutex_status == INACTIVE){
        return EINVAL;
    }

    if(mutex_queue[m->m_syncvar.mutex_num].lock_status == UNLOCKED){
        return ENOLOCK;
    }

    peek(&mutex_queue[m->m_syncvar.mutex_num].process_queue, &pt);
    if(pt == m->m_source){
        /* dequeue do próximo processo */
        dequeue(&mutex_queue[m->m_syncvar.mutex_num].process_queue, &pt);
        /* próximo processo recebe lock */
        if(peek(&mutex_queue[m->m_syncvar.mutex_num].process_queue, &pt)){
            m->m_type = OK;
            ipc_sendnb(pt, m);
        } else{ /* não tinha próximo, lock livre */
            mutex_queue[m->m_syncvar.mutex_num].lock_status = UNLOCKED;
        }
    } else{
        return ENOLOCK;
    }
    return OK;
}
