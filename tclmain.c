/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995-1996
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: tclmain.c,v $
 * Revision 1.21  2000-02-22 23:11:03  adam
 * Fixed include statements.
 *
 * Revision 1.20  1997/04/30 07:26:08  adam
 * Added support for shared libaries (if supported by Tcl itself).
 *
 * Revision 1.19  1996/08/20 09:27:49  adam
 * More work on explain.
 * Renamed tkinit.c to tkmain.c. The tcl shell uses the Tcl 7.5 interface
 * for socket i/o instead of the handcrafted one (for Tcl 7.3 and Tcl7.4).
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
#include <yaz/log.h>
#include "ir-tcl.h"

int Tcl_AppInit (Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
    if (Irtcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
#if USE_WAIS
    if (Waistcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
#endif
    return TCL_OK;
}

#if TCL_MAJOR_VERSION > 7 || (TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION > 4)
/* new version of tcl: version > 7.4 */
extern int matherr ();
int *tclDummyMathPtr = (int*) matherr;

int main (int argc, char **argv)
{
    Tcl_Main (argc, argv, Tcl_AppInit);
    return 0;
}

#else
/* old version of tcl: version <= 7.4 */

static char *fileName = NULL;
extern int main ();
int *tclDummyMainPtr = (int*) main;

/* select(2) callbacks */
struct callback {
    void (*handle)(ClientData, int, int, int);
    int r, w, e;
    ClientData obj;
};
#define MAX_CALLBACK 200

static struct callback callback_table[MAX_CALLBACK];
static int max_fd = 3;            /* don't worry: it will grow... */

void tcl_mainloop (Tcl_Interp *interp, int interactive);

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
        callback_table[i].handle = NULL;
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
            if (callback_table[i].handle && callback_table[i].w)
            {
                FD_SET (i, &fdset_tcl_w);
                res++;
            }
            if (callback_table[i].handle && callback_table[i].r)
            {
                FD_SET (i, &fdset_tcl_r);
                res++;
            }
            if (callback_table[i].handle && callback_table[i].e)
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
            int r_flag = 0;
            int w_flag = 0;
            int e_flag = 0;

            if (!callback_table[i].handle)
                continue;
            if (FD_ISSET (i, &fdset_tcl_r) && callback_table[i].r)
                r_flag = 1;
            if (FD_ISSET (i, &fdset_tcl_w) && callback_table[i].w)
                w_flag = 1;
            if (FD_ISSET (i, &fdset_tcl_x) && callback_table[i].e)
                e_flag = 1;
            if (r_flag || w_flag || e_flag)
                (*callback_table[i].handle)(callback_table[i].obj,
                 r_flag, w_flag, e_flag);
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

void ir_tcl_select_set (void (*f)(ClientData clientData, int r, int w, int e),
                        int fd, ClientData clientData, int r, int w, int e)
{
    callback_table[fd].handle = f;
    callback_table[fd].obj = clientData;
    callback_table[fd].r = r;
    callback_table[fd].w = w;
    callback_table[fd].e = e;
    if (fd > max_fd)
        max_fd = fd;
}

#endif
