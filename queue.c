
/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: queue.c,v $
 * Revision 1.2  1995-08-03 13:23:01  adam
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

void *ir_tcl_malloc (size_t size)
{
    void *p = malloc (size);
    if (!p)
    {
        logf (LOG_FATAL, "Out of memory. %d bytes requested", size);
        exit (1);
    }
    return p;
}

int ir_tcl_send_APDU (Tcl_Interp *interp, IrTcl_Obj *p, Z_APDU *apdu,
                      const char *msg)
{
    IrTcl_Request **rp;

    if (!z_APDU (p->odr_out, &apdu, 0))
    {
        Tcl_AppendResult (interp, odr_errlist [odr_geterror (p->odr_out)],
                          NULL);
        odr_reset (p->odr_out);
        return TCL_ERROR;
    }
    rp = &p->request_queue;
    while (*rp)
        rp = &(*rp)->next;
    *rp = ir_tcl_malloc (sizeof(**rp));
    (*rp)->next = NULL;
    (*rp)->buf_out = odr_getbuf (p->odr_out, &(*rp)->len_out, NULL);
    odr_setbuf (p->odr_out, NULL, 0, 1);
    odr_reset (p->odr_out);
    if (p->state == IR_TCL_R_Idle)
    {
        if (ir_tcl_send_q (p, *rp, msg) == TCL_ERROR)
        {
            sprintf (interp->result, "cs_put failed in %s", msg);
            return TCL_ERROR;
        } 
    }
    return TCL_OK;
}

int ir_tcl_send_q (IrTcl_Obj *p, IrTcl_Request *rp, const char *msg)
{
    int r;

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
        logf (LOG_DEBUG, "Send %s (%d bytes)", msg, rp->len_out);
        p->state = IR_TCL_R_Waiting;
        free (rp->buf_out);
        rp->buf_out = NULL;
    }
    return TCL_OK;
}

