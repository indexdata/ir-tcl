/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995-1996
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: tkmain.c,v $
 * Revision 1.1  1996-08-20 09:27:49  adam
 * More work on explain.
 * Renamed tkinit.c to tkmain.c. The tcl shell uses the Tcl 7.5 interface
 * for socket i/o instead of the handcrafted one (for Tcl 7.3 and Tcl7.4).
 *
 */

#include <tk.h>
#include <log.h>
#include "ir-tcl.h"

/* socket layer code for tk3.x and tk4.0 */
#if TK_MAJOR_VERSION < 4 || (TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0)

struct sel_proc {
    void (*f)(ClientData clientData, int r, int w, int e);
    ClientData clientData;
    int fd;
    struct sel_proc *next;
};

static struct sel_proc *sel_proc_list = NULL;

static void ir_tcl_tk_select_proc (ClientData clientData, int mask)
{
    struct sel_proc *sp = (struct sel_proc *) clientData;

    if (!sp->f)
        return ;
    (*sp->f)(sp->clientData, mask & TK_READABLE, mask & TK_WRITABLE,
             mask & TK_EXCEPTION);
}

void ir_tcl_select_set (void (*f)(ClientData clientData, int r, int w, int e),
                        int fd, ClientData clientData, int r, int w, int e)
{
    int mask = 0;
    struct sel_proc *sp = sel_proc_list;

    if (r)
        mask |= TK_READABLE;
    if (w)
        mask |= TK_WRITABLE;
    if (e)
        mask |= TK_EXCEPTION;
    while (sp)
    {
        if (sp->fd == fd)
             break;
        sp = sp->next;
    }
    if (!sp)
    {
        sp = ir_tcl_malloc (sizeof(*sp));
        sp->next = sel_proc_list;
        sel_proc_list = sp;
        sp->fd = fd;
    }
    sp->f = f;
    sp->clientData = clientData;
    if (f)
        Tk_CreateFileHandler (fd, mask, ir_tcl_tk_select_proc, sp);
    else
        Tk_DeleteFileHandler (fd);
}
#endif

#if TK_MAJOR_VERSION >= 4

extern int matherr ();
int *tclDummyMathPtr = (int*) matherr;

int main (int argc, char **argv)
{
    Tk_Main (argc, argv, Tcl_AppInit);
    return 0;
}

#else

extern int main ();
int *tclDummyMainPtr = (int*) main;

#endif

int Tcl_AppInit (Tcl_Interp *interp)
{
#if TK_MAJOR_VERSION < 4 
    Tk_Window mainw;

    if (!(mainw = Tk_MainWindow(interp)))
        return TCL_ERROR;
#endif
    if (Tcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
    if (Tk_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
    if (Irtcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
    return TCL_OK;
}
