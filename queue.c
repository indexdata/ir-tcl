
/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: queue.c,v $
 * Revision 1.8  1996-03-05 09:21:20  adam
 * Bug fix: memory used by GRS records wasn't freed.
 * Rewrote some of the error handling code - the connection is always
 * closed before failback is called.
 * If failback is defined the send APDU methods (init, search, ...) will
 * return OK but invoke failback (as is the case if the write operation
 * fails).
 * Bug fix: ref_count in assoc object could grow if fraction of PDU was
 * read.
 *
 * Revision 1.7  1996/02/19  15:41:55  adam
 * Better log messages.
 * Minor improvement of connect method.
 *
 * Revision 1.6  1996/02/06  09:22:54  adam
 * Ported ir-tcl to use beta releases of tcl7.5/tk4.1.
 *
 * Revision 1.5  1995/11/28  13:53:40  quinn
 * Windows port.
 *
 * Revision 1.4  1995/10/17  12:18:59  adam
 * Bug fix: when target connection closed, the connection was not
 * properly reestablished.
 *
 * Revision 1.3  1995/08/04  11:32:40  adam
 * More work on output queue. Memory related routines moved
 * to mem.c
 *
 * Revision 1.2  1995/08/03  13:23:01  adam
 * Request queue.
 *
 * Revision 1.1  1995/07/28  10:28:39  adam
 * First work on request queue.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "ir-tclp.h"

int ir_tcl_send_APDU (Tcl_Interp *interp, IrTcl_Obj *p, Z_APDU *apdu,
                      const char *msg, const char *object_name)
{
    IrTcl_Request **rp;

    if (!z_APDU (p->odr_out, &apdu, 0))
    {
        Tcl_AppendResult (interp, odr_errmsg (odr_geterror (p->odr_out)),
                          NULL);
        odr_reset (p->odr_out);
        return TCL_ERROR;
    }
    rp = &p->request_queue;
    while (*rp)
        rp = &(*rp)->next;
    *rp = ir_tcl_malloc (sizeof(**rp));
    (*rp)->next = NULL;
    
    if (ir_tcl_strdup (interp, &(*rp)->object_name, object_name) == TCL_ERROR)
        return TCL_ERROR;
    if (ir_tcl_strdup (interp, &(*rp)->callback, p->callback) == TCL_ERROR)
        return TCL_ERROR;
    
    (*rp)->buf_out = odr_getbuf (p->odr_out, &(*rp)->len_out, NULL);
    odr_setbuf (p->odr_out, NULL, 0, 1);
    odr_reset (p->odr_out);
    if (p->state == IR_TCL_R_Idle)
    {
        logf (LOG_DEBUG, "send_apdu. Sending %s", msg);
        if (ir_tcl_send_q (p, p->request_queue, msg) == TCL_ERROR)
        {
            if (p->failback)
            {
                ir_tcl_disconnect (p);
                p->failInfo = IR_TCL_FAIL_WRITE;
                ir_tcl_eval (interp, p->failback);
                return TCL_OK;
            }
            else 
            {
                sprintf (interp->result, "cs_put failed in %s", msg);
                return TCL_ERROR;
            }
        } 
    }
    else
        logf (LOG_DEBUG, "send_apdu. Not idle (%s)", msg);
    return TCL_OK;
}

int ir_tcl_send_q (IrTcl_Obj *p, IrTcl_Request *rp, const char *msg)
{
    int r;

    assert (rp);
    r = cs_put (p->cs_link, rp->buf_out, rp->len_out);
    if (r < 0)
        return TCL_ERROR;
    else if (r == 1)
    {
#if IRTCL_GENERIC_FILES
        ir_select_add_write (p->csFile, p);
#else
        ir_select_add_write (cs_fileno (p->cs_link), p);
#endif
        logf (LOG_DEBUG, "Send part of %s", msg);
        p->state = IR_TCL_R_Writing;
    }
    else
    {
        logf (LOG_DEBUG, "Send %s (%d bytes) fd=%d", msg, rp->len_out,
              cs_fileno(p->cs_link));
        p->state = IR_TCL_R_Waiting;
        free (rp->buf_out);
        rp->buf_out = NULL;
    }
    return TCL_OK;
}

void ir_tcl_del_q (IrTcl_Obj *p)
{
    IrTcl_Request *rp, *rp1;

    p->state = IR_TCL_R_Idle;
    for (rp = p->request_queue; rp; rp = rp1)
    {
        free (rp->object_name);
        free (rp->callback);
        free (rp->buf_out);
        rp1 = rp->next;
        free (rp);
    }
    p->request_queue = NULL;
}



