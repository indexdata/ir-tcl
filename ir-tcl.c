/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 *
 * $Id: ir-tcl.c,v 1.2 1995-03-08 07:28:29 adam Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

#include <comstack.h>
#include <tcpip.h>
#include <xmosi.h>

#include <odr.h>
#include <proto.h>

#include <tcl.h>

#include "ir-tcl.h"

typedef struct {
    COMSTACK cs_link;

    int preferredMessageSize;
    int maximumMessageSize;
    Odr_bitmask options;
    Odr_bitmask protocolVersion;
    char *idAuthentication;
    char *implementationName;
    char *implementationId;

    char *buf_out;
    int  len_out;

    char *buf_in;
    int  len_in;

    ODR odr_in;
    ODR odr_out;
    ODR odr_pr;

    Tcl_Interp *interp;
    char *callback;
} IRObj;

typedef struct {
    IRObj *parent;
} IRSetObj;

typedef struct {
    char *name;
    int (*method) (void * obj, Tcl_Interp *interp, int argc, char **argv);
} IRMethod;

static int do_disconnect (void *obj,Tcl_Interp *interp, int argc, char **argv);

/*
 * get_parent_info: Returns information about parent object.
 */
static int get_parent_info (Tcl_Interp *interp, const char *name,
			    Tcl_CmdInfo *parent_info)
{
    char parent_name[128];
    const char *csep = strrchr (name, '.');
    int pos;

    if (!csep)
    {
        interp->result = "missing .";
        return TCL_ERROR;
    }
    pos = csep-name;
    if (pos > 127)
        pos = 127;
    memcpy (parent_name, name, pos);
    parent_name[pos] = '\0';
    if (!Tcl_GetCommandInfo (interp, parent_name, parent_info))
        return TCL_ERROR;
    return TCL_OK;
}

/*
 * ir_method: Search for method in table and invoke method handler
 */
int ir_method (void *obj, Tcl_Interp *interp, int argc, char **argv,
                   IRMethod *tab)
{
    while (tab->name)
    {
        if (!strcmp (tab->name, argv[1]))
            return (*tab->method)(obj, interp, argc, argv);
        tab++;
    }
    Tcl_AppendResult (interp, "unknown method: ", argv[1], NULL);
    return TCL_ERROR;
}

/*
 * ir_asc2bitmask: Ascii to ODR bitmask conversion
 */
int ir_asc2bitmask (const char *asc, Odr_bitmask *ob)
{
    const char *cp = asc + strlen(asc);
    int bitno = 0;

    ODR_MASK_ZERO (ob);
    do 
    {
        if (*--cp == '1')
            ODR_MASK_SET (ob, bitno);
        bitno++;
    } while (cp != asc);
    return bitno;
}

/*
 * ir_strdup: Duplicate string
 */
int ir_strdup (Tcl_Interp *interp, char** p, char *s)
{
    *p = malloc (strlen(s)+1);
    if (!*p)
    {
        interp->result = "malloc fail";
        return TCL_ERROR;
    }
    strcpy (*p, s);
    return TCL_OK;
}

/* ------------------------------------------------------- */

/*
 * do_init_request: init method on IR object
 */
static int do_init_request (void *obj, Tcl_Interp *interp,
		       int argc, char **argv)
{
    Z_APDU apdu, *apdup;
    IRObj *p = obj;
    Z_InitRequest req;
    char *sbuf;
    int slen;

    req.referenceId = 0;
    req.options = &p->options;
    req.protocolVersion = &p->protocolVersion;
    req.preferredMessageSize = &p->preferredMessageSize;
    req.maximumRecordSize = &p->maximumMessageSize;

    req.idAuthentication = p->idAuthentication;
    req.implementationId = p->implementationId;
    req.implementationName = p->implementationName;
    req.implementationVersion = "0.1";
    req.userInformationField = 0;

    apdu.u.initRequest = &req;
    apdu.which = Z_APDU_initRequest;
    apdup = &apdu;

    if (!z_APDU (p->odr_out, &apdup, 0))
    {
        interp->result = odr_errlist [odr_geterror (p->odr_out)];
        odr_reset (p->odr_out);
        return TCL_ERROR;
    }
    sbuf = odr_getbuf (p->odr_out, &slen);
    if (cs_put (p->cs_link, sbuf, slen) < 0)
    {
        interp->result = "cs_put failed in init";
        return TCL_ERROR;
    }
    printf("Sent initializeRequest (%d bytes).\n", slen);
    return TCL_OK;
}

/*
 * do_protocolVersion: Set protocol Version
 */
static int do_protocolVersion (void *obj, Tcl_Interp *interp,
                               int argc, char **argv)
{
    if (argc == 3)
        ir_asc2bitmask (argv[2], &((IRObj *) obj)->protocolVersion);
    return TCL_OK;
}

/*
 * do_options: Set options
 */
static int do_options (void *obj, Tcl_Interp *interp,
                       int argc, char **argv)
{
    if (argc == 3)
        ir_asc2bitmask (argv[2], &((IRObj *) obj)->options);
    return TCL_OK;
}

/*
 * do_preferredMessageSize: Set preferred message size
 */
static int do_preferredMessageSize (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    if (argc == 3)
    {
        if (Tcl_GetInt (interp, argv[2], 
                        &((IRObj *)obj)->preferredMessageSize)==TCL_ERROR)
            return TCL_ERROR;
    }
    sprintf (interp->result, "%d", ((IRObj *)obj)->preferredMessageSize);
    return TCL_OK;
}

/*
 * do_maximumMessageSize: Set maximum message size
 */
static int do_maximumMessageSize (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    if (argc == 3)
    {
        if (Tcl_GetInt (interp, argv[2], 
                        &((IRObj *)obj)->maximumMessageSize)==TCL_ERROR)
            return TCL_ERROR;
    }
    sprintf (interp->result, "%d", ((IRObj *)obj)->maximumMessageSize);
    return TCL_OK;
}


/*
 * do_implementationName: Set Implementation Name.
 */
static int do_implementationName (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    if (argc == 3)
    {
        free (((IRObj*)obj)->implementationName);
        if (ir_strdup (interp, &((IRObj*) obj)->implementationName, argv[2])
            == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, ((IRObj*)obj)->implementationName,
                      (char*) NULL);
    return TCL_OK;
}

/*
 * do_implementationId: Set Implementation Name.
 */
static int do_implementationId (void *obj, Tcl_Interp *interp,
                                int argc, char **argv)
{
    if (argc == 3)
    {
        free (((IRObj*)obj)->implementationId);
        if (ir_strdup (interp, &((IRObj*) obj)->implementationId, argv[2])
            == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, ((IRObj*)obj)->implementationId,
                      (char*) NULL);
    return TCL_OK;
}

/*
 * do_idAuthentication: Set id Authentication
 */
static int do_idAuthentication (void *obj, Tcl_Interp *interp,
                                int argc, char **argv)
{
    if (argc == 3)
    {
        free (((IRObj*)obj)->idAuthentication);
        if (ir_strdup (interp, &((IRObj*) obj)->idAuthentication, argv[2])
            == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, ((IRObj*)obj)->idAuthentication,
                      (char*) NULL);
    return TCL_OK;
}

/*
 * do_connect: connect method on IR object
 */
static int do_connect (void *obj, Tcl_Interp *interp,
		       int argc, char **argv)
{
    void *addr;
    IRObj *p = obj;

    if (argc < 3)
    {
        interp->result = "missing hostname";
        return TCL_ERROR;
    }
    if (cs_type(p->cs_link) == tcpip_type)
    {
        addr = tcpip_strtoaddr (argv[2]);
        if (!addr)
        {
            interp->result = "tcpip_strtoaddr fail";
            return TCL_ERROR;
        }
        printf ("tcp/ip connect %s\n", argv[2]);
    }
    else if (cs_type (p->cs_link) == mosi_type)
    {
        addr = mosi_strtoaddr (argv[2]);
        if (!addr)
        {
            interp->result = "mosi_strtoaddr fail";
            return TCL_ERROR;
        }
        printf ("mosi connect %s\n", argv[2]);
    }
    if (cs_connect (p->cs_link, addr) < 0)
    {
        interp->result = "cs_connect fail";
        do_disconnect (p, interp, argc, argv);
        return TCL_ERROR;
    }
    ir_select_add (cs_fileno (p->cs_link), p);
    return TCL_OK;
}

/*
 * do_disconnect: disconnect method on IR object
 */
static int do_disconnect (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IRObj *p = obj;

    ir_select_remove (cs_fileno (p->cs_link), p);
    if (cs_type (p->cs_link) == tcpip_type)
    {
        cs_close (p->cs_link);
        p->cs_link = cs_create (tcpip_type);
    }
    else if (cs_type (p->cs_link) == mosi_type)
    {
        cs_close (p->cs_link);
        p->cs_link = cs_create (mosi_type);
    }
    else
    {
        interp->result = "unknown comstack type";
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 * do_comstack: comstack method on IR object
 */
static int do_comstack (void *obj, Tcl_Interp *interp,
		        int argc, char **argv)
{
    if (argc == 3)
    {
        if (!strcmp (argv[2], "tcpip"))
            ((IRObj *)obj)->cs_link = cs_create (tcpip_type);
        else if (!strcmp (argv[2], "mosi"))
            ((IRObj *)obj)->cs_link = cs_create (mosi_type);
        else
        {
            interp->result = "wrong comstack type";
            return TCL_ERROR;
        }
    }
    if (cs_type(((IRObj *)obj)->cs_link) == tcpip_type)
        interp->result = "tcpip";
    else if (cs_type(((IRObj *)obj)->cs_link) == mosi_type)
        interp->result = "comstack";
    return TCL_OK;
}

/*
 * do_callback: add callback
 */
static int do_callback (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IRObj *p = obj;

    if (argc == 3)
    {
        free (p->callback);
        if (ir_strdup (interp, &p->callback, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
        p->interp = interp;
    }
    return TCL_OK;
}

/* 
 * ir_obj_method: IR Object methods
 */
static int ir_obj_method (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    static IRMethod tab[] = {
    { "comstack", do_comstack },
    { "connect", do_connect },
    { "protocolVersion", do_protocolVersion },
    { "options", do_options },
    { "preferredMessageSize", do_preferredMessageSize },
    { "maximumMessageSize",   do_maximumMessageSize },
    { "implementationName", do_implementationName },
    { "implementationId",   do_implementationId },
    { "idAuthentication",   do_idAuthentication },
    { "init", do_init_request },
    { "disconnect", do_disconnect },
    { "callback", do_callback },
    { NULL, NULL}
    };
    if (argc < 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    return ir_method (clientData, interp, argc, argv, tab);
}

/* 
 * ir_obj_delete: IR Object disposal
 */
static void ir_obj_delete (ClientData clientData)
{
    free ( (void*) clientData);
}

/* 
 * ir_obj_mk: IR Object creation
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
    {
        interp->result = "malloc fail";
        return TCL_ERROR;
    }
    obj->cs_link = cs_create (tcpip_type);

    obj->maximumMessageSize = 10000;
    obj->preferredMessageSize = 4096;

    obj->idAuthentication = NULL;

    if (ir_strdup (interp, &obj->implementationName, "TCL/TK on YAZ")
        == TCL_ERROR)
        return TCL_ERROR;

    if (ir_strdup (interp, &obj->implementationId, "TCL/TK/YAZ")
        == TCL_ERROR)
        return TCL_ERROR;

    ODR_MASK_ZERO (&obj->protocolVersion);
    ODR_MASK_SET (&obj->protocolVersion, 0);
    ODR_MASK_SET (&obj->protocolVersion, 1);

    ODR_MASK_ZERO (&obj->options);
    ODR_MASK_SET (&obj->options, 0);

    obj->odr_in = odr_createmem (ODR_DECODE);
    obj->odr_out = odr_createmem (ODR_ENCODE);
    obj->odr_pr = odr_createmem (ODR_PRINT);

    obj->len_out = 10000;
    obj->buf_out = malloc (obj->len_out);
    if (!obj->buf_out)
    {
        interp->result = "malloc fail";
        return TCL_ERROR;
    }
    odr_setbuf (obj->odr_out, obj->buf_out, obj->len_out);

    obj->len_in = 0;
    obj->buf_in = NULL;

    obj->callback = NULL;

    Tcl_CreateCommand (interp, argv[1], ir_obj_method,
                       (ClientData) obj, ir_obj_delete);
    return TCL_OK;
}

/* ------------------------------------------------------- */
/*
 * do_query: Set query for a Set Object
 */
static int do_query (void *obj, Tcl_Interp *interp,
		       int argc, char **argv)
{
    return TCL_OK;
}


/* 
 * ir_set_obj_method: IR Set Object methods
 */
static int ir_set_obj_method (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    static IRMethod tab[] = {
    { "query", do_query },
    { NULL, NULL}
    };

    if (argc < 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    return ir_method (clientData, interp, argc, argv, tab);
}

/* 
 * ir_set_obj_delete: IR Set Object disposal
 */
static void ir_set_obj_delete (ClientData clientData)
{
    free ( (void*) clientData);
}

/* 
 * ir_set_obj_mk: IR Set Object creation
 */
static int ir_set_obj_mk (ClientData clientData, Tcl_Interp *interp,
			     int argc, char **argv)
{
    Tcl_CmdInfo parent_info;
    IRSetObj *obj;

    if (argc != 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (get_parent_info (interp, argv[1], &parent_info) == TCL_ERROR)
    {
        interp->result = "No parent";
        return TCL_ERROR;
    }
    obj = malloc (sizeof(*obj));
    if (!obj)
    {
        interp->result = "malloc fail";
        return TCL_ERROR;
    }
    obj->parent = (IRObj *) parent_info.clientData;
    Tcl_CreateCommand (interp, argv[1], ir_set_obj_method,
                       (ClientData) obj, ir_set_obj_delete);
    return TCL_OK;
}

/* ------------------------------------------------------- */

static void ir_searchResponse (void *obj, Z_SearchResponse *searchrs)
{    
    if (searchrs->searchStatus)
        printf("Search was a success.\n");
    else
            printf("Search was a bloomin' failure.\n");
    printf("Number of hits: %d, setno %d\n",
           *searchrs->resultCount, 1);
#if 0
    if (searchrs->records)
        display_records(searchrs->records);
#endif
}

static void ir_initResponse (void *obj, Z_InitResponse *initrs)
{
    if (!*initrs->result)
        printf("Connection rejected by target.\n");
    else
        printf("Connection accepted by target.\n");
    if (initrs->implementationId)
            printf("ID     : %s\n", initrs->implementationId);
    if (initrs->implementationName)
        printf("Name   : %s\n", initrs->implementationName);
    if (initrs->implementationVersion)
        printf("Version: %s\n", initrs->implementationVersion);
#if 0
    if (initrs->userInformationField)
    {
        printf("UserInformationfield:\n");
        odr_external(&print, (Odr_external**)&initrs->
                         userInformationField, 0);
    }
#endif
}

static void ir_presentResponse (void *obj, Z_PresentResponse *presrs)
{
    printf("Received presentResponse.\n");
    if (presrs->records)
        printf ("Got records\n");
    else
        printf("No records\n");
}

void ir_select_proc (ClientData clientData)
{
    IRObj *p = clientData;
    Z_APDU *apdu;
    int r;
    
    do
    {
        if ((r=cs_get (p->cs_link, &p->buf_in, &p->len_in))  < 0)
        {
            printf ("cs_get failed\n");
            return;
        }        
        odr_setbuf (p->odr_in, p->buf_in, r);
        printf ("cs_get ok, got %d\n", r);
        if (!z_APDU (p->odr_in, &apdu, 0))
        {
            printf ("%s\n", odr_errlist [odr_geterror (p->odr_in)]);
            return;
        }
        if (p->callback)
        {
	    Tcl_Eval (p->interp, p->callback);
	}
        switch(apdu->which)
        {
        case Z_APDU_initResponse:
            ir_initResponse (NULL, apdu->u.initResponse);
            break;
        case Z_APDU_searchResponse:
            ir_searchResponse (NULL, apdu->u.searchResponse);
            break;
        case Z_APDU_presentResponse:
            ir_presentResponse (NULL, apdu->u.presentResponse);
            break;
        default:
            printf("Received unknown APDU type (%d).\n", 
                   apdu->which);
        }
    } while (cs_more (p->cs_link));    
}

/* ------------------------------------------------------- */

/*
 * ir_tcl_init: Registration of TCL commands.
 */
int ir_tcl_init (Tcl_Interp *interp)
{
    Tcl_CreateCommand (interp, "ir", ir_obj_mk, (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "ir-set", ir_set_obj_mk,
    		       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    return TCL_OK;
}


