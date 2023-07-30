/* This file contains the main program of the condition variable for the SYNCVAR service. */

#include <stdlib.h>
#include "svar.h"
#include "queue.h"
#include <sys/errno.h>

#define OK 0
#define ACTIVE 1
#define INACTIVE 0

/*
 * data structure with the necessary info for a single conditional variable
 * identification is given by its index in the cond_vec
 * CHANGE AS NEEDED FOR THE NEXT PARTS
 */
typedef struct process{
    endpoint_t process;  //endpoint da mensagem
} process_t;

typedef struct cond{
    uint8_t cond_status;
    thread_queue_t process_queue;
    thread_queue_t mutex_queue;
} cond_t;

/* file-scoped global variables */
static cond_t * cond_queue;
static unsigned int cond_cap = 0;
static unsigned int cond_count = 0;

/* cond */
int
do_cond_init(message * m){
    cond_t * cond_queue_aux;

    /* init new */
    if (cond_cap == 0 || cond_queue == NULL) {
        cond_cap = 2;
        cond_queue = malloc(cond_cap * sizeof(cond_t));
    }

    /* increase capacity */
    if (cond_count == cond_cap){
        cond_queue_aux = realloc(cond_queue, 2 * cond_cap * sizeof(cond_t));

        if(cond_queue_aux != NULL){
            cond_queue = cond_queue_aux;
            cond_cap *= 2;
        } else{
            /* out of memory */
            return ENOMEM;
        }
    }

    /* out of memory */
    if (cond_queue == NULL){
        return ENOMEM;
    }

    /* return message */
    m->m_syncvar.cond_num = cond_count;
    cond_queue[cond_count].cond_status = ACTIVE;
    cond_queue[cond_count].process_queue = init_queue();
    cond_count++;

    return OK;
}

/* cond destroy */
int do_cond_destroy(message * m){
    endpoint_t pt;

    if(m->m_syncvar.cond_num >= cond_count){
        return EINVAL;
    }

    /* já era inativo */
    if(cond_queue[m->m_syncvar.cond_num].cond_status == INACTIVE){
        return EINVAL;
    }

    if(!peek(&cond_queue[m->m_syncvar.cond_num].process_queue, &pt)){
        return EBUSY;
    }

    cond_queue[m->m_syncvar.cond_num].cond_status = INACTIVE;

    return OK;
}

/* cond wait */
int do_cond_wait(message * m){
    int mutex_r;

    if(m->m_syncvar.cond_num >= cond_count || cond_queue[m->m_syncvar.cond_num].cond_status == INACTIVE){
        return EINVAL;
    }

    mutex_r = do_mutex_unlock(m);

    if (mutex_r != OK){
        return mutex_r;
    }

    enqueue(&cond_queue[m->m_syncvar.cond_num].process_queue, m->m_source);
    enqueue(&cond_queue[m->m_syncvar.cond_num].mutex_queue, (endpoint_t)m->m_syncvar.mutex_num);

    return SUSPEND;
}

/* cond signal */
int do_cond_signal(message * m){
    endpoint_t pt; // endpoint that might be waiting
    endpoint_t source; // original source endpoint
    int mutex_r, mutex_num;

    if(m->m_syncvar.cond_num >= cond_count) {
        return EINVAL;
    }

    source = m->m_source; // store original source
    if (dequeue(&cond_queue[m->m_syncvar.cond_num].process_queue, &pt)){
        dequeue(&cond_queue[m->m_syncvar.cond_num].mutex_queue, (endpoint_t*)&mutex_num);

        m->m_source = pt;
        m->m_syncvar.mutex_num = mutex_num;

        mutex_r = do_mutex_lock(m);
        if (mutex_r == OK) {
            m->m_type = OK;
            ipc_sendnb(pt, m);
        }
        m->m_source = source;
    } else { // queue is empty
        return ENOPROC;
    }
    return OK;
}

/* cond broadcast */
int do_cond_broadcast(message * m){
    int num_it = 0;
    int r;

    /* signal until there is no more process waiting */
    while ((r = do_cond_signal(m)) != ENOPROC && r != EINVAL){
        num_it++;
    }
    /* empty queue*/
    if (num_it == 0)
        return r;

    return OK;
}
