/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 *
 * $Log: ir-tcl.c,v $
 * Revision 1.5  1995-03-10 18:00:15  adam
 * Actual presentation in line-by-line format. RPN query support.
 *
 * Revision 1.4  1995/03/09  16:15:08  adam
 * First presentRequest attempts. Hot-target list.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

#include <yaz-ccl.h>
#include <iso2709p.h>
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

    int smallSetUpperBound;
    int largeSetLowerBound;
    int mediumSetPresentNumber;
    int replaceIndicator;
    char **databaseNames;
    int num_databaseNames;
    char *query_method;

    CCL_bibset bibset;

    struct IRSetObj_ *child;
} IRObj;

typedef struct IRRecordList_ {
    int status;
    Iso2709Rec rec;
    int no;
    struct IRRecordList_ *next;
} IRRecordList;

typedef struct IRSetObj_ {
    IRObj *parent;
    int resultCount;
    int start;
    int number;
    int numberOfRecordsReturned;
    Z_Records *z_records;
    IRRecordList *record_list;
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
    {
        interp->result = "No parent";
        return TCL_ERROR;
    }
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
 * do_databaseNames: specify database names
 */
static int do_databaseNames (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    int i;
    IRObj *p = obj;

    if (argc < 3)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (p->databaseNames)
    {
        for (i=0; i<p->num_databaseNames; i++)
            free (p->databaseNames[i]);
        free (p->databaseNames);
    }
    p->num_databaseNames = argc - 2;
    if (!(p->databaseNames = malloc (sizeof(*p->databaseNames) *
                               p->num_databaseNames)))
    {
        interp->result = "malloc fail";
        return TCL_ERROR;
    }
    for (i=0; i<p->num_databaseNames; i++)
    {
        if (ir_strdup (interp, &p->databaseNames[i], argv[2+i]) 
            == TCL_ERROR)
            return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 * do_query: Set/Get query mothod
 */
static int do_query (void *obj, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRObj *p = obj;
    if (argc == 3)
    {
        free (p->query_method);
        if (ir_strdup (interp, &p->query_method, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->query_method, NULL);
    return TCL_OK;
}

/* 
 * ir_obj_method: IR Object methods
 */
static int ir_obj_method (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    static IRMethod tab[] = {
    { "comstack",                do_comstack },
    { "connect",                 do_connect },
    { "protocolVersion",         do_protocolVersion },
    { "options",                 do_options },
    { "preferredMessageSize",    do_preferredMessageSize },
    { "maximumMessageSize",      do_maximumMessageSize },
    { "implementationName",      do_implementationName },
    { "implementationId",        do_implementationId },
    { "idAuthentication",        do_idAuthentication },
    { "init",                    do_init_request },
    { "disconnect",              do_disconnect },
    { "callback",                do_callback },
    { "databaseNames",           do_databaseNames},
    { "query",                   do_query },
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
    FILE *inf;

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

    obj->maximumMessageSize = 32768;
    obj->preferredMessageSize = 4096;

    obj->idAuthentication = NULL;

    if (ir_strdup (interp, &obj->implementationName, "TCL/TK on YAZ")
        == TCL_ERROR)
        return TCL_ERROR;

    if (ir_strdup (interp, &obj->implementationId, "TCL/TK/YAZ")
        == TCL_ERROR)
        return TCL_ERROR;
    
    obj->smallSetUpperBound = 0;
    obj->largeSetLowerBound = 2;
    obj->mediumSetPresentNumber = 0;
    obj->replaceIndicator = 1;
    obj->databaseNames = NULL;
    obj->num_databaseNames = 0; 
    if (ir_strdup (interp, &obj->query_method, "rpn") == TCL_ERROR)
        return TCL_ERROR;
    obj->bibset = ccl_qual_mk (); 
    if ((inf = fopen ("default.bib", "r")))
    {
    	ccl_qual_file (obj->bibset, inf);
    	fclose (inf);
    }
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
 * do_search: Do search request
 */
static int do_search (void *o, Tcl_Interp *interp,
		       int argc, char **argv)
{
    Z_SearchRequest req;
    Z_Query query;
    Z_APDU apdu, *apdup;
    static Odr_oid bib1[] = {1, 2, 840, 10003, 3, 1, -1};
    Odr_oct ccl_query;
    IRSetObj *obj = o;
    IRObj *p = obj->parent;
    char *sbuf;
    int slen;

    p->child = o;
    if (argc != 3)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (!p->num_databaseNames)
    {
        interp->result = "no databaseNames";
        return TCL_ERROR;
    }
    apdu.which = Z_APDU_searchRequest;
    apdu.u.searchRequest = &req;
    apdup = &apdu;

    req.referenceId = 0;
    req.smallSetUpperBound = &p->smallSetUpperBound;
    req.largeSetLowerBound = &p->largeSetLowerBound;
    req.mediumSetPresentNumber = &p->mediumSetPresentNumber;
    req.replaceIndicator = &p->replaceIndicator;
    req.resultSetName = "Default";
    req.num_databaseNames = p->num_databaseNames;
    req.databaseNames = p->databaseNames;
    req.smallSetElementSetNames = 0;
    req.mediumSetElementSetNames = 0;
    req.preferredRecordSyntax = 0;
    req.query = &query;

    if (!strcmp (p->query_method, "rpn"))
    {
        int error;
        int pos;
        struct ccl_rpn_node *rpn;
        Z_RPNQuery *RPNquery;

        rpn = ccl_find_str(p->bibset, argv[2], &error, &pos);
        if (error)
        {
            Tcl_AppendResult (interp, "CCL error: ", ccl_err_msg(error),NULL);
            return TCL_ERROR;
        }
        query.which = Z_Query_type_1;
        assert((RPNquery = ccl_rpn_query(rpn)));
        RPNquery->attributeSetId = bib1;
        query.u.type_1 = RPNquery;
    }
    else if (!strcmp (p->query_method, "ccl"))
    {
        query.which = Z_Query_type_2;
        query.u.type_2 = &ccl_query;
        ccl_query.buf = argv[2];
        ccl_query.len = strlen (argv[2]);
    }
    else
    {
        interp->result = "unknown query method";
        return TCL_ERROR;
    }
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
    printf ("Search request\n");
    return TCL_OK;
}

/*
 * do_resultCount: Get number of hits
 */
static int do_resultCount (void *o, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRSetObj *obj = o;

    sprintf (interp->result, "%d", obj->resultCount);
    return TCL_OK;
}

/*
 * do_numberOfRecordsReturned: Get number of records returned
 */
static int do_numberOfRecordsReturned (void *o, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRSetObj *obj = o;

    sprintf (interp->result, "%d", obj->numberOfRecordsReturned);
    return TCL_OK;
}

static int get_marc_record(Tcl_Interp *interp, Iso2709Rec rec,
                           int argc, char **argv)
{
    struct iso2709_dir *dir;
    struct iso2709_field *field;
    
    for (dir = rec->directory; dir; dir = dir->next)
    {
        if (strcmp (dir->tag, argv[3]))
            continue;
        for (field = dir->fields; field; field = field->next)
        {
            if (argc > 4 && strcmp (field->identifier, argv[4]))
                continue;
            Tcl_AppendElement (interp, field->data);
        }
    }
    return TCL_OK;
}

/*
 * do_getRecord: Get an ISO2709 Record
 */
static int do_getRecord (void *o, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRSetObj *obj = o;
    int offset;
    IRRecordList *rl;

    if (argc < 3)
    {
        sprintf (interp->result, "wrong # args");
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;
    for (rl = obj->record_list; rl; rl = rl->next)
    {
        if (rl->no == offset)
            break;
    }
    if (!rl)
    {
        Tcl_AppendResult (interp, "No record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (!rl->rec)
    {
        Tcl_AppendResult (interp, "Not a MARC record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    return get_marc_record (interp, rl->rec, argc, argv);
}

/*
 * do_present: Perform Present Request
 */

static int do_present (void *o, Tcl_Interp *interp,
                       int argc, char **argv)
{
    IRSetObj *obj = o;
    IRObj *p = obj->parent;
    Z_APDU apdu, *apdup;
    Z_PresentRequest req;
    int start;
    int number;
    char *sbuf;
    int slen;

    if (argc >= 3)
    {
        if (Tcl_GetInt (interp, argv[2], &start) == TCL_ERROR)
            return TCL_ERROR;
    }
    else
        start = 1;
    if (argc >= 4)
    {
        if (Tcl_GetInt (interp, argv[3], &number) == TCL_ERROR)
            return TCL_ERROR;
    }
    else 
        number = 10;
    obj->start = start;
    obj->number = number;

    apdup = &apdu;
    apdu.which = Z_APDU_presentRequest;
    apdu.u.presentRequest = &req;
    req.referenceId = 0;
    /* sprintf(setstring, "%d", setnumber); */
    req.resultSetId = "Default";
    req.resultSetStartPoint = &start;
    req.numberOfRecordsRequested = &number;
    req.elementSetNames = 0;
    req.preferredRecordSyntax = 0;

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
    printf ("Present request, start=%d, num=%d\n", start, number);
    return TCL_OK;
}

/* 
 * ir_set_obj_method: IR Set Object methods
 */
static int ir_set_obj_method (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    static IRMethod tab[] = {
    { "search",                  do_search },
    { "resultCount",             do_resultCount },
    { "numberOfRecordsReturned", do_numberOfRecordsReturned },
    { "present",                 do_present },
    { "getRecord",               do_getRecord },
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
        return TCL_ERROR;
    obj = malloc (sizeof(*obj));
    if (!obj)
    {
        interp->result = "malloc fail";
        return TCL_ERROR;
    }
    obj->z_records = NULL;
    obj->record_list = NULL;
    obj->parent = (IRObj *) parent_info.clientData;
    Tcl_CreateCommand (interp, argv[1], ir_set_obj_method,
                       (ClientData) obj, ir_set_obj_delete);
    return TCL_OK;
}

/* ------------------------------------------------------- */

static void ir_searchResponse (void *o, Z_SearchResponse *searchrs)
{    
    IRObj *p = o;
    IRSetObj *obj = p->child;

    if (obj)
        obj->resultCount = *searchrs->resultCount;
    if (searchrs->searchStatus)
        printf("Search was a success.\n");
    else
        printf("Search was a bloomin' failure.\n");
    printf("Number of hits: %d\n", *searchrs->resultCount);
#if 0
    if (searchrs->records)
        display_records(searchrs->records);
#endif
}

static void ir_initResponse (void *obj, Z_InitResponse *initrs)
{
    IRObj *p = obj;

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

static void ir_presentResponse (void *o, Z_PresentResponse *presrs)
{
    IRObj *p = o;
    IRSetObj *setobj = p->child;
    Z_Records *zrs = presrs->records;
    setobj->z_records = presrs->records;
    
    printf ("Received presentResponse\n");
    if (zrs)
    {
        if (zrs->which == Z_Records_NSD)
        {
            setobj->numberOfRecordsReturned = 0;
            printf ("They are diagnostic!!!\n");
            /*            
               char buf[16];
               sprintf (buf, "%d", *zrs->u.nonSurrogateDiagnostic->condition);
               Tcl_AppendResult (interp, "Diagnostic message: ", buf,
               " : ",
               zrs->u.nonSurrogateDiagnostic->addinfo, NULL);
               return TCL_ERROR;
               */
            return;
        }
        else
        {
            int offset;
            IRRecordList *rl;
            
            setobj->numberOfRecordsReturned = 
                zrs->u.databaseOrSurDiagnostics->num_records;
            printf ("Got %d records\n", setobj->numberOfRecordsReturned);
            for (offset = 0; offset<setobj->numberOfRecordsReturned; offset++)
            {
                int no = setobj->start + offset;
                
                for (rl = setobj->record_list; rl; rl = rl->next)
                {
                    if (no == rl->no)
                    {
                        if (rl->rec)
                            iso2709_rm (rl->rec);
                        break;
                    }
                }
                if (!rl)
                {
                    rl = malloc (sizeof(*rl));
                    assert (rl);
                    rl->next = setobj->record_list;
                    rl->no = no;
                    rl->status = 0;
                    setobj->record_list = rl;
                }
                if (zrs->u.databaseOrSurDiagnostics->records[offset]->which ==
                    Z_NamePlusRecord_surrogateDiagnostic)
                {
                    rl->status = -1;
                    rl->rec = NULL;
                }
                else
                {
                    Z_DatabaseRecord *zr; 
                    Odr_external *oe;
                    
                    rl->status = 0;
                    zr = zrs->u.databaseOrSurDiagnostics->records[offset]
                        ->u.databaseRecord;
                    oe = (Odr_external*) zr;
                    if (oe->which == ODR_EXTERNAL_octet
                        && zr->u.octet_aligned->len)
                    {
                        const char *buf = (char*) zr->u.octet_aligned->buf;
                        rl->rec = iso2709_cvt (buf);
                    }
                }
            }
        }
    }
    else
    {
        printf ("No records!\n");
    }
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
            ir_select_remove (cs_fileno (p->cs_link), p);
            return;
        }        
        odr_setbuf (p->odr_in, p->buf_in, r);
        printf ("cs_get ok, got %d\n", r);
        if (!z_APDU (p->odr_in, &apdu, 0))
        {
            printf ("%s\n", odr_errlist [odr_geterror (p->odr_in)]);
            return;
        }
        switch(apdu->which)
        {
        case Z_APDU_initResponse:
            ir_initResponse (p, apdu->u.initResponse);
            break;
        case Z_APDU_searchResponse:
            ir_searchResponse (p, apdu->u.searchResponse);
            break;
        case Z_APDU_presentResponse:
            ir_presentResponse (p, apdu->u.presentResponse);
            break;
        default:
            printf("Received unknown APDU type (%d).\n", 
                   apdu->which);
        }
        if (p->callback)
	    Tcl_Eval (p->interp, p->callback);
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
