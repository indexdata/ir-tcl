/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 *
 * $Id: tclmain.c,v 1.1 1995-03-06 17:05:34 adam Exp $
 */

#include <tcl.h>

#include "ir-tcl.h"

static char *fileName = NULL;

int Tcl_AppInit (Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
    if (ir_tcl_init(interp) == TCL_ERROR)
        return TCL_ERROR;
    return TCL_OK;
}

int main (int argc, char **argv)
{
    Tcl_Interp *interp;
    int code;

    interp = Tcl_CreateInterp();

    if (argc != 2)
    {
        fprintf (stderr, "Script file expected\n");
        exit (1);
    }
    fileName = argv[1];
    if (fileName == NULL)
    {
        fprintf (stderr, "No filename specified\n");
        exit (1);
    }
    if (Tcl_AppInit(interp) != TCL_OK) {
        fprintf(stderr, "Tcl_AppInit failed: %s\n", interp->result);
    }    
    code = Tcl_EvalFile (interp, fileName);
    if (*interp->result != 0)
        printf ("%s\n", interp->result);
    if (code != TCL_OK)
        exit (1);
    exit (0);
}

