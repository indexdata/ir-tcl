/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 *
 * $Id: ir-tcl.c,v 1.1.1.1 1995-03-06 17:05:35 adam Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include <comstack.h>
#include <tcpip.h>
#include <xmosi.h>

#include <odr.h>
#include <proto.h>

#include <tcl.h>

#include "ir-tcl.h"

typedef struct {
    COMSTACK cs_link;
} IRObj;

/* 
 * Object method
 */
static int ir_obj_handle (ClientData clientData, Tcl_Interp *interp,
                      int argc, char **argv)
{
    IRObj *ir = clientData;
    if (argc < 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (!strcmp (argv[1], "comstack"))
    {
        if (argc == 3)
        {
            if (!strcmp (argv[2], "tcpip"))
                ir->cs_link = cs_create (tcpip_type);
            else if (!strcmp (argv[2], "mosi"))
                ir->cs_link = cs_create (mosi_type);
            else
            {
                interp->result = "wrong comstack type";
                return TCL_ERROR;
            }
        }
        if (cs_type(ir->cs_link) == tcpip_type)
            interp->result = "tcpip";
        else if (cs_type(ir->cs_link) == mosi_type)
            interp->result = "comstack";
    }
    else if (!strcmp (argv[1], "connect"))
    {
        void *addr;

        if (argc < 3)
        {
            interp->result = "missing hostname after connect";
            return TCL_ERROR;
        }
        if (cs_type(ir->cs_link) == tcpip_type)
        {
            addr = tcpip_strtoaddr (argv[2]);
            if (!addr)
            {
                interp->result = "tcpip_strtoaddr fail";
                return TCL_ERROR;
            }
        }
        else if (cs_type (ir->cs_link) == mosi_type)
        {
            addr = mosi_strtoaddr (argv[2]);
            if (!addr)
            {
                interp->result = "mosi_strtoaddr fail";
                return TCL_ERROR;
            }
        }
        if (cs_connect (ir->cs_link, addr) < 0)
        {
            interp->result = "cs_connect fail";
            cs_close (ir->cs_link);
            return TCL_ERROR;
        }
    }
    return TCL_OK;
}

/* 
 * Object disposal
 */
static void ir_obj_delete (ClientData clientData)
{
    free ( (void*) clientData);
}

/* 
 * Object create
 */
static int ir_obj_mk (ClientData clientData, Tcl_Interp *interp,
              int argc, char **argv)
{
    IRObj *obj;

    if (argc != 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    obj = malloc (sizeof(*obj));
    if (!obj)
        return TCL_ERROR;
    obj->cs_link = cs_create (tcpip_type);

    Tcl_CreateCommand (interp, argv[1], ir_obj_handle,
                       (ClientData) obj, ir_obj_delete);
    return TCL_OK;
}

/*
 * Registration
 */
int ir_tcl_init (Tcl_Interp *interp)
{
    Tcl_CreateCommand (interp, "ir", ir_obj_mk, (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
    return TCL_OK;
}
