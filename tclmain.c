/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: tclmain.c,v $
 * Revision 1.7  1995-06-21 11:04:54  adam
 * Uses GNU autoconf 2.3.
 * Install procedure implemented.
 * boook bitmaps moved to sub directory bitmaps.
 *
 * Revision 1.6  1995/05/29  08:44:28  adam
 * Work on delete of objects.
 *
 * Revision 1.5  1995/03/20  08:53:30  adam
 * Event loop in tclmain.c rewritten. New method searchStatus.
 *
 * Revision 1.4  1995/03/17  07:50:31  adam
 * Headers have changed a little.
 *
 */

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <assert.h>

#include <tcl.h>

#include "ir-tcl.h"

static char *fileName = NULL;

/* select(2) callbacks */
struct callback {
    void (*r_handle)(void *p);
    void (*w_handle)(void *p);
    void (*x_handle)(void *p);
    void *obj;
};
#define MAX_CALLBACK 200

static struct callback callback_table[MAX_CALLBACK];
static int max_fd = 3;            /* don't worry: it will grow... */

void tcl_mainloop (Tcl_Interp *interp, int interactive);

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
    int i;

    interp = Tcl_CreateInterp();
    Tcl_SetVar (interp, "tcl_interactive", "0", TCL_GLOBAL_ONLY);
    if (argc == 2)
        fileName = argv[1];

    if (Tcl_AppInit(interp) != TCL_OK) {
        fprintf(stderr, "Tcl_AppInit failed: %s\n", interp->result);
    }
    for (i=0; i<MAX_CALLBACK; i++)
    {
        callback_table[i].r_handle = NULL;
        callback_table[i].w_handle = NULL;
        callback_table[i].x_handle = NULL;
    }
    if (fileName)
    {
        code = Tcl_EvalFile (interp, fileName);
        if (*interp->result != 0)
            printf ("%s\n", interp->result);
        if (code != TCL_OK)
            exit (1);
        tcl_mainloop (interp, 0);
    }
    else
    {
        Tcl_SetVar (interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
        tcl_mainloop (interp, 1);
    }
    exit (0);
}

void tcl_mainloop (Tcl_Interp *interp, int interactive)
{
    int i;
    int res;
    Tcl_DString command;
    static fd_set fdset_tcl_r;
    static fd_set fdset_tcl_w;
    static fd_set fdset_tcl_x;
    int min_fd;

    min_fd = interactive ? 3 : 0;
    if (interactive)
    {
        Tcl_DStringInit (&command);
        printf ("[TCL]"); fflush (stdout);
    }
    while (1)
    {
        FD_ZERO (&fdset_tcl_r);
        FD_ZERO (&fdset_tcl_w);
        FD_ZERO (&fdset_tcl_x);
        if (interactive)
            FD_SET (0, &fdset_tcl_r);
        for (res=0, i=min_fd; i<=max_fd; i++)
        {
            if (callback_table[i].w_handle)
            {
                FD_SET (i, &fdset_tcl_w);
                res++;
            }
            if (callback_table[i].r_handle)
            {
                FD_SET (i, &fdset_tcl_r);
                res++;
            }
            if (callback_table[i].x_handle)
            {
                FD_SET (i, &fdset_tcl_x);
                res++;
            }
        }
        if (!interactive && !res)
            return;
        if ((res = select(max_fd+1, &fdset_tcl_r, &fdset_tcl_w, 
                          &fdset_tcl_x, 0)) < 0)
        {
            perror("select");
            exit(1);
        }
        if (!res)
            continue;
        for (i=min_fd; i<=max_fd; i++)
        {
            if (FD_ISSET (i, &fdset_tcl_r))
            {
                assert (callback_table[i].r_handle);
                (*callback_table[i].r_handle) (callback_table[i].obj);
            }
            if (FD_ISSET (i, &fdset_tcl_w))
            {
                assert (callback_table[i].w_handle);
                (*callback_table[i].w_handle) (callback_table[i].obj);
            }
            if (FD_ISSET (i, &fdset_tcl_x))
            {
                assert (callback_table[i].x_handle);
                (*callback_table[i].x_handle) (callback_table[i].obj);
            }
        }
        if (interactive && FD_ISSET(0, &fdset_tcl_r))
        {
            char input_buf[256];
            int count = read (0, input_buf, 256);

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
    callback_table[fd].r_handle = ir_select_read;
    callback_table[fd].w_handle = NULL;
    callback_table[fd].x_handle = NULL;
    if (fd > max_fd)
        max_fd = fd;
}

void ir_select_add_write (int fd, void *obj)
{
    callback_table[fd].w_handle = ir_select_write;
    if (fd > max_fd)
        max_fd = fd;
}

void ir_select_remove_write (int fd, void *obj)
{
    callback_table[fd].w_handle = NULL;
}

void ir_select_remove (int fd, void *obj)
{
    callback_table[fd].r_handle = NULL;
    callback_table[fd].w_handle = NULL;
    callback_table[fd].x_handle = NULL;
}
