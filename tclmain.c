/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: tclmain.c,v $
 * Revision 1.16  1996-02-05 17:58:05  adam
 * Ported ir-tcl to use the beta releases of tcl7.5/tk4.1.
 *
 * Revision 1.15  1996/01/10  09:18:45  adam
 * PDU specific callbacks implemented: initRespnse, searchResponse,
 *  presentResponse and scanResponse.
 * Bug fix in the command line shell (tclmain.c) - discovered on OSF/1.
 *
 * Revision 1.14  1995/09/21  13:11:53  adam
 * Support of dynamic loading.
 * Test script uses load command if necessary.
 *
 * Revision 1.13  1995/08/28  12:21:22  adam
 * Removed lines and list as synonyms of list in MARC extractron.
 * Configure searches also for tk4.0 / tcl7.4.
 *
 * Revision 1.12  1995/08/28  11:07:16  adam
 * Minor changes.
 *
 * Revision 1.11  1995/08/03  13:23:02  adam
 * Request queue.
 *
 * Revision 1.10  1995/06/30  12:39:28  adam
 * Bug fix: loadFile didn't set record type.
 * The MARC routines are a little less strict in the interpretation.
 * Script display.tcl replaces the old marc.tcl.
 * New interactive script: shell.tcl.
 *
 * Revision 1.9  1995/06/26  10:20:20  adam
 * ir-tk works like wish.
 *
 * Revision 1.8  1995/06/21  15:16:44  adam
 * More work on configuration.
 *
 * Revision 1.7  1995/06/21  11:04:54  adam
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
#ifdef _AIX
#include <sys/select.h>
#endif
#include <assert.h>

#include <tcl.h>
#include <log.h>
#include "ir-tcl.h"

static char *fileName = NULL;

/* select(2) callbacks */
struct callback {
    void (*r_handle)(ClientData);
    void (*w_handle)(ClientData);
    void (*x_handle)(ClientData);
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
    if (Irtcl_Init(interp) == TCL_ERROR)
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
    else if (isatty(0))
    {

        Tcl_SetVar (interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
        tcl_mainloop (interp, 1);
    }
    else
    {
        Tcl_DString command;
        char input_buf[1024];
        int count;

        printf ("xx\n");
        Tcl_DStringInit (&command);
        while (fgets (input_buf, 1024, stdin))
        {
            count = strlen(input_buf);
            Tcl_DStringAppend (&command, input_buf, count);
            if (Tcl_CommandComplete (Tcl_DStringValue (&command)))
            {
                int code = Tcl_Eval (interp, Tcl_DStringValue (&command));
                Tcl_DStringFree (&command);
                if (code)
                    printf ("Error: %s\n", interp->result);
            }
        }
        tcl_mainloop (interp, 0);
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
        printf ("%% "); fflush (stdout);
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
                if (callback_table[i].r_handle)
                    (*callback_table[i].r_handle) (callback_table[i].obj);
            }
            if (FD_ISSET (i, &fdset_tcl_w))
            {
                if (callback_table[i].w_handle)
                    (*callback_table[i].w_handle) (callback_table[i].obj);
            }
            if (FD_ISSET (i, &fdset_tcl_x))
            {
                if (callback_table[i].x_handle)
                    (*callback_table[i].x_handle) (callback_table[i].obj);
            }
        }
        if (interactive && FD_ISSET(0, &fdset_tcl_r))
        {
            char input_buf[1024];
            int count = read (0, input_buf, 1024);

            if (count <= 0)
                exit (0);
            Tcl_DStringAppend (&command, input_buf, count);
            if (Tcl_CommandComplete (Tcl_DStringValue (&command)))
            {
                int code = Tcl_Eval (interp, Tcl_DStringValue (&command));
                Tcl_DStringFree (&command);
                if (code)
                    printf ("Error: %s\n", interp->result);
                else if (*interp->result)
                    printf ("%s\n", interp->result);
                printf ("%% "); fflush (stdout);
            }
        }
    }
}

#if IRTCL_GENERIC_FILES
void ir_select_add (Tcl_File file, void *obj)
{
    int fd = (int) Tcl_GetFileInfo (file, NULL);
#else
void ir_select_add (int fd, void *obj)
{
#endif
    callback_table[fd].obj = obj;
    callback_table[fd].r_handle = ir_select_read;
    callback_table[fd].w_handle = NULL;
    callback_table[fd].x_handle = NULL;
    if (fd > max_fd)
        max_fd = fd;
}

#if IRTCL_GENERIC_FILES
void ir_select_add_write (Tcl_File file, void *obj)
{
    int fd = (int) Tcl_GetFileInfo (file, NULL);
#else
void ir_select_add_write (int fd, void *obj)
{
#endif
    callback_table[fd].w_handle = ir_select_write;
    if (fd > max_fd)
        max_fd = fd;
}

#if IRTCL_GENERIC_FILES
void ir_select_remove_write (Tcl_File file, void *obj)
{
    int fd = (int) Tcl_GetFileInfo (file, NULL);
#else
void ir_select_remove_write (int fd, void *obj)
{
#endif
    callback_table[fd].w_handle = NULL;
}

#if IRTCL_GENERIC_FILES
void ir_select_remove (Tcl_File file, void *obj)
{
    int fd = (int) Tcl_GetFileInfo (file, NULL);
#else
void ir_select_remove (int fd, void *obj)
{
#endif
    callback_table[fd].r_handle = NULL;
    callback_table[fd].w_handle = NULL;
    callback_table[fd].x_handle = NULL;
}
