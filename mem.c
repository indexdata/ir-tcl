/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: mem.c,v $
 * Revision 1.1  1995-08-04 11:32:40  adam
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
    void *p = malloc (n);
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
    if (!s)
    {
        *p = NULL;
        return TCL_OK;
    }
    *p = malloc (strlen(s)+1);
    if (!*p)
    {
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
    free (*p);
    *p = NULL;
    return TCL_OK;
}

