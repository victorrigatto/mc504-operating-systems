/* This file contains the main program of the SYNCVAR service. */

#include <stdio.h>
#include <stdlib.h>
//Cabeçalho para funções de sincronização
#include "svar.h"

//System call table
#define CALL(n) [((n)-SYNC_BASE)]
static int (*const call_vec[])(message *) = {
    CALL(MUTEX_INIT)        = do_mutex_init,
    CALL(MUTEX_DESTROY)     = do_mutex_destroy,
    CALL(MUTEX_LOCK)        = do_mutex_lock,
    CALL(MUTEX_UNLOCK)      = do_mutex_unlock,
    CALL(COND_INIT)         = do_cond_init,
    CALL(COND_DESTROY)      = do_cond_destroy,
    CALL(COND_WAIT)         = do_cond_wait,
    CALL(COND_SIGNAL)       = do_cond_signal,
    CALL(COND_BROADCAST)    = do_cond_broadcast,
};

/*===========================================================================*
 *				main					     *
 *===========================================================================*/
int main(int argc, char ** argv){
  /* Main routine of the scheduler. */
	message m; /* the incoming message itself is kept here. */
	unsigned int call_index; /* system call number */
	int r;

	/* SEF local startup. */
	env_setargs(argc, argv);
	sef_startup();

	/* This is SYNCVAR's main loop - get work and do it, forever and forever. */
	while (TRUE){
    int ipc_status;

    /* Wait for the next message and extract useful information from it. */
		if ((r = sef_receive_status(ANY, &m, &ipc_status)) != OK){
			panic("SYNCVAR sef_receive error");
			exit(1);
		}

    /* Check for system notifications first. Special cases. */
		if (is_ipc_notify(ipc_status)){
			printf("SYNCVAR: ignored notification from %d\n", m.m_source);
			continue;
		}

    /*===========================================================================*
     *				request					     *
     *===========================================================================*/
		call_index = (unsigned int)(m.m_type - SYNC_BASE);

		if (call_index < __arraycount(call_vec) && call_vec[call_index] != NULL){
			r = call_vec[call_index](&m);
		} else{
			r = ENOSYS;
		}

    /*===========================================================================*
     *				reply					     *
     *===========================================================================*/
		if (r != SUSPEND){
			m.m_type = r;

			if ((r = ipc_sendnb(m.m_source, &m)) != OK){
				printf("SYNCVAR: unable to send reply to %d: %d\n", m.m_source, r);
      }
		}
	}

	/* NOTREACHED */
	printf("SYNCVAR shutting down\n");
	return (OK);
}
