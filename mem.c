/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: mem.c,v $
 * Revision 1.3  1996-07-03 13:31:14  adam
 * The xmalloc/xfree functions from YAZ are used to manage memory.
 *
 * Revision 1.2  1995/08/29  15:30:15  adam
 * Work on GRS records.
 *
 * Revision 1.1  1995/08/04  11:32:40  adam
 * More work on output queue. Memory related routines moved
 * to mem.c
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "ir-tclp.h"

/*
 * ir_tcl_malloc: Allocate n byte from the heap
 */
void *ir_tcl_malloc (size_t n)
{
    void *p = xmalloc (n);
    if (!p)
    {
        logf (LOG_FATAL, "Out of memory. %ld bytes requested", (long) n);
        exit (1);
    }
    return p;
}

/*
 * ir_tcl_strdup: Duplicate string
 */
int ir_tcl_strdup (Tcl_Interp *interp, char** p, const char *s)
{
    size_t len;

    if (!s)
    {
        *p = NULL;
        return TCL_OK;
    }
    len = strlen(s)+1;
    *p = xmalloc (len);
    if (!*p)
    {
        if (!interp) 
        {
            logf (LOG_FATAL, "Out of memory in strdup. %ld bytes", len);
            exit (1);
        }
        interp->result = "strdup fail";
        return TCL_ERROR;
    }
    strcpy (*p, s);
    return TCL_OK;
}

/*
 * ir_strdel: Delete string
 */
int ir_tcl_strdel (Tcl_Interp *interp, char **p)
{
    xfree (*p);
    *p = NULL;
    return TCL_OK;
}

