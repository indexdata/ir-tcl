/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995-1999
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: queue.c,v $
 * Revision 1.13  2003-03-05 21:21:42  adam
 * APDU log. default largeSetLowerBound changed from 2 to 1
 *
 * Revision 1.12  1999/04/20 10:01:46  adam
 * Modified calls to ODR encoders/decoders (name argument).
 *
 * Revision 1.11  1996/07/03 13:31:14  adam
 * The xmalloc/xfree functions from YAZ are used to manage memory.
 *
 * Revision 1.10  1996/06/03  09:04:24  adam
 * Changed a few logf calls.
 *
 * Revision 1.9  1996/03/20  13:54:05  adam
 * The Tcl_File structure is only manipulated in the Tk-event interface
 * in tkinit.c.
 *
 * Revision 1.8  1996/03/05  09:21:20  adam
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

    if (!z_APDU (p->odr_out, &apdu, 0, 0))
    {
        Tcl_AppendResult (interp, odr_errmsg (odr_geterror (p->odr_out)),
                          NULL);
        odr_reset (p->odr_out);
        return TCL_ERROR;
    }
    if (p->odr_pr)
        z_APDU (p->odr_pr, &apdu, 0, 0);
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
        logf (LOG_DEBUG, "APDU send %s", msg);
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
                char buf[100];
	        snprintf(buf, sizeof buf - 1, "cs_put failed in %s", msg);
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION <= 5
	        interp->result = buf;
#else
                Tcl_SetResult(interp, buf, TCL_VOLATILE);
#endif
                return TCL_ERROR;
            }
        } 
    }
    else
        logf (LOG_DEBUG, "APDU pending %s", msg);
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
        ir_select_add_write (cs_fileno (p->cs_link), p);
        logf (LOG_DEBUG, "Send part of %s", msg);
        p->state = IR_TCL_R_Writing;
    }
    else
    {
        logf (LOG_DEBUG, "Send %s (%d bytes) fd=%d", msg, rp->len_out,
              cs_fileno(p->cs_link));
        p->state = IR_TCL_R_Waiting;
        xfree (rp->buf_out);
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
        xfree (rp->object_name);
        xfree (rp->callback);
        xfree (rp->buf_out);
        rp1 = rp->next;
        xfree (rp);
    }
    p->request_queue = NULL;
}



