/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 *
 * $Id: tclmain.c,v 1.3 1995-03-09 08:35:58 adam Exp $
 */

#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>
#include <unistd.h>

#include <tcl.h>

#include "ir-tcl.h"

static char *fileName = NULL;

static fd_set fdset_tcl;

void tcl_mainloop (Tcl_Interp *interp);

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
    Tcl_SetVar (interp, "tcl_interactive", "0", TCL_GLOBAL_ONLY);
    if (argc == 2)
        fileName = argv[1];

    if (Tcl_AppInit(interp) != TCL_OK) {
        fprintf(stderr, "Tcl_AppInit failed: %s\n", interp->result);
    }
    if (fileName)
    {
        code = Tcl_EvalFile (interp, fileName);
        if (*interp->result != 0)
            printf ("%s\n", interp->result);
        if (code != TCL_OK)
            exit (1);
    }
    Tcl_SetVar (interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
    tcl_mainloop (interp);
    exit (0);
}

struct callback {
    void (*handle)(void *p);
    void *obj;
};

#define MAX_CALLBACK 20

struct callback callback_table[MAX_CALLBACK];

void tcl_mainloop (Tcl_Interp *interp)
{
    int i;
    int res;
    int count;
    char input_buf[256];
    Tcl_DString command;

    for (i=0; i<MAX_CALLBACK; i++)
        callback_table[i].handle = NULL;
    Tcl_DStringInit (&command);
    printf ("[TCL]"); fflush (stdout);
    while (1)
    {
        FD_ZERO (&fdset_tcl);
        FD_SET (0, &fdset_tcl);
        for (i=3; i<MAX_CALLBACK; i++)
            if (callback_table[i].handle)
                FD_SET (i, &fdset_tcl);
        if ((res = select(MAX_CALLBACK+1, &fdset_tcl, 0, 0, 0)) < 0)
        {
            perror("select");
            exit(1);
        }
        if (!res)
            continue;
        for (i=3; i<MAX_CALLBACK; i++)
            if (FD_ISSET (i, &fdset_tcl))
            {
                assert (callback_table[i].handle);
                (*callback_table[i].handle) (callback_table[i].obj);
            }
        if (FD_ISSET(0, &fdset_tcl))
        {
            count = read (0, input_buf, 256);
            if (count <= 0)
                exit (0);
            Tcl_DStringAppend (&command, input_buf, count);
            if (Tcl_CommandComplete (Tcl_DStringValue (&command)))
            {
                int code = Tcl_Eval (interp, Tcl_DStringValue (&command));
                Tcl_DStringFree (&command);
                if (code)
                    printf ("[ERR:%s]\n", interp->result);
                else
                    printf ("[RES:%s]\n", interp->result);
                printf ("[TCL]"); fflush (stdout);
            }
        }
    }
}

void ir_select_add (int fd, void *obj)
{
    callback_table[fd].obj = obj;
    callback_table[fd].handle = ir_select_proc;
}

void ir_select_remove (int fd, void *obj)
{
    callback_table[fd].handle = NULL;
}
