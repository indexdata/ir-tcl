/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1996
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: select.c,v $
 * Revision 1.4  1997-08-28 20:20:48  adam
 * Added support for Tk8.0/Tcl8.0. Since Tcl_File handlers are gone
 * we've moved to Tcl_Channel handlers instead.
 *
 * Revision 1.3  1997/04/13 18:57:28  adam
 * Better error reporting and aligned with Tcl/Tk style.
 * Rework of notifier code with Tcl_File handles.
 *
 * Revision 1.2  1996/09/13 10:51:48  adam
 * Bug fix: ir_tcl_select_set called Tcl_GetFile at disconnect.
 *
 * Revision 1.1  1996/08/20  09:33:23  adam
 * Tcl7.5 Generic file handling.
 *
 */

#include <tcl.h>
#include <log.h>
#include "ir-tcl.h"

#if TCL_MAJOR_VERSION == 8
struct sel_proc {
    void (*f)(ClientData clientData, int r, int w, int e);
    ClientData clientData;
    int fd;
    Tcl_Channel tcl_Channel;
    struct sel_proc *next;
};

static struct sel_proc *sel_proc_list = NULL;

static void ir_tcl_tk_select_proc (ClientData clientData, int mask)
{
    struct sel_proc *sp = (struct sel_proc *) clientData;

    if (!sp->f)
        return ;
    (*sp->f)(sp->clientData, mask & TCL_READABLE, mask & TCL_WRITABLE,
             mask & TCL_EXCEPTION);
}

void ir_tcl_select_set (void (*f)(ClientData clientData, int r, int w, int e),
                        int fd, ClientData clientData, int r, int w, int e)
{
    int mask = 0;
    struct sel_proc **sp = &sel_proc_list;

    if (r)
        mask |= TCL_READABLE;
    if (w)
        mask |= TCL_WRITABLE;
    if (e)
        mask |= TCL_EXCEPTION;
    while (*sp)
    {
        if ((*sp)->fd == fd)
             break;
        sp = &(*sp)->next;
    }
    logf (LOG_DEBUG, "r=%d w=%d e=%d sp=%p", r, w, e, *sp);
    if (*sp)
        Tcl_DeleteChannelHandler ((*sp)->tcl_Channel, ir_tcl_tk_select_proc,
                                  (*sp)->clientData);
    if (!f)
    {
        if (*sp)
        {
            Tcl_Close (NULL, (*sp)->tcl_Channel);
            *sp = (*sp)->next;
        }
        return ;
    }
    if (!*sp)
    {
        *sp = ir_tcl_malloc (sizeof(**sp));
        (*sp)->next = NULL;
        (*sp)->fd = fd;
        (*sp)->tcl_Channel = Tcl_MakeTcpClientChannel ((ClientData) fd);
    }
    (*sp)->f = f;
    (*sp)->clientData = clientData;
    Tcl_CreateChannelHandler ((*sp)->tcl_Channel, mask,
                              ir_tcl_tk_select_proc, *sp);
}
#endif

#if (TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION > 4)

struct sel_proc {
    void (*f)(ClientData clientData, int r, int w, int e);
    ClientData clientData;
    int fd;
    Tcl_File tcl_File;
    struct sel_proc *next;
};

static struct sel_proc *sel_proc_list = NULL;

static void ir_tcl_tk_select_proc (ClientData clientData, int mask)
{
    struct sel_proc *sp = (struct sel_proc *) clientData;

    if (!sp->f)
        return ;
    (*sp->f)(sp->clientData, mask & TCL_READABLE, mask & TCL_WRITABLE,
             mask & TCL_EXCEPTION);
}

void ir_tcl_select_set (void (*f)(ClientData clientData, int r, int w, int e),
                        int fd, ClientData clientData, int r, int w, int e)
{
    int mask = 0;
    struct sel_proc **sp = &sel_proc_list;

    if (r)
        mask |= TCL_READABLE;
    if (w)
        mask |= TCL_WRITABLE;
    if (e)
        mask |= TCL_EXCEPTION;
    while (*sp)
    {
        if ((*sp)->fd == fd)
             break;
        sp = &(*sp)->next;
    }
    logf (LOG_DEBUG, "r=%d w=%d e=%d sp=%p", r, w, e, *sp);
    if (!f)
    {
        if (*sp)
        {
            Tcl_DeleteFileHandler ((*sp)->tcl_File);
            Tcl_FreeFile ((*sp)->tcl_File);
            *sp = (*sp)->next;
        }
        return ;
    }
    if (!*sp)
    {
        *sp = ir_tcl_malloc (sizeof(**sp));
        (*sp)->next = NULL;
        (*sp)->fd = fd;
#if WINDOWS
        (*sp)->tcl_File = Tcl_GetFile ((ClientData) fd, TCL_WIN_SOCKET);
#else
        (*sp)->tcl_File = Tcl_GetFile ((ClientData) fd, TCL_UNIX_FD);
#endif
    }
    (*sp)->f = f;
    (*sp)->clientData = clientData;
    Tcl_CreateFileHandler ((*sp)->tcl_File, mask, ir_tcl_tk_select_proc, *sp);
}
#endif

