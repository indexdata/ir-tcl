/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: ir-tcl.c,v $
 * Revision 1.14  1995-03-20 08:53:22  adam
 * Event loop in tclmain.c rewritten. New method searchStatus.
 *
 * Revision 1.13  1995/03/17  18:26:17  adam
 * Non-blocking i/o used now. Database names popup as cascade items.
 *
 * Revision 1.12  1995/03/17  15:45:00  adam
 * Improved target/database setup.
 *
 * Revision 1.11  1995/03/16  17:54:03  adam
 * Minor changes really.
 *
 * Revision 1.10  1995/03/15  16:14:50  adam
 * Blocking arg in cs_create changed.
 *
 * Revision 1.9  1995/03/15  13:59:24  adam
 * Minor changes.
 *
 * Revision 1.8  1995/03/15  08:25:16  adam
 * New method presentStatus to check for error on present. Misc. cleanup
 * of IRRecordList manipulations. Full MARC record presentation in
 * search.tcl.
 *
 * Revision 1.7  1995/03/14  17:32:29  adam
 * Presentation of full Marc record in popup window.
 *
 * Revision 1.6  1995/03/12  19:31:55  adam
 * Pattern matching implemented when retrieving MARC records. More
 * diagnostic functions.
 *
 * Revision 1.5  1995/03/10  18:00:15  adam
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
#include <diagbib1.h>

#include <tcl.h>

#include "ir-tcl.h"

typedef struct {
    COMSTACK cs_link;

    int preferredMessageSize;
    int maximumRecordSize;
    Odr_bitmask options;
    Odr_bitmask protocolVersion;
    char *idAuthentication;
    char *implementationName;
    char *implementationId;

    char *hostname;
   
    char *buf_out;
    int  len_out;

    char *buf_in;
    int  len_in;

    char *sbuf;
    int  slen;

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
    int no;
    int which;
    union {
        struct {
            Iso2709Rec rec;
        } marc;
        struct {
            int  condition;
            char *addinfo;
        } diag;
    } u;
    struct IRRecordList_ *next;
} IRRecordList;

typedef struct IRSetObj_ {
    IRObj *parent;
    int searchStatus;
    int resultCount;
    int start;
    int number;
    int numberOfRecordsReturned;
    Z_Records *z_records;
    int which;
    int condition;
    char *addinfo;
    IRRecordList *record_list;
} IRSetObj;

typedef struct {
    int type;
    char *name;
    int (*method) (void * obj, Tcl_Interp *interp, int argc, char **argv);
} IRMethod;

static int do_disconnect (void *obj,Tcl_Interp *interp, int argc, char **argv);

static IRRecordList *new_IR_record (IRSetObj *setobj, int no, int which)
{
    IRRecordList *rl;

    for (rl = setobj->record_list; rl; rl = rl->next)
    {
        if (no == rl->no)
        {
            switch (rl->which)
            {
            case Z_NamePlusRecord_databaseRecord:
                iso2709_rm (rl->u.marc.rec);
                break;
            case Z_NamePlusRecord_surrogateDiagnostic:
                free (rl->u.diag.addinfo);
                rl->u.diag.addinfo = NULL;
                break;
            }
            break;
        }
    }
    if (!rl)
    {
        rl = malloc (sizeof(*rl));
        assert (rl);
        rl->next = setobj->record_list;
        rl->no = no;
        setobj->record_list = rl;
    }
    rl->which = which;
    return rl;
}

static IRRecordList *find_IR_record (IRSetObj *setobj, int no)
{
    IRRecordList *rl;

    for (rl = setobj->record_list; rl; rl = rl->next)
        if (no == rl->no)
            return rl;
    return NULL;
}

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
    IRMethod *t;
    for (t = tab; t->name; t++)
        if (!strcmp (t->name, argv[1]))
            return (*t->method)(obj, interp, argc, argv);
    Tcl_AppendResult (interp, "Bad method. Possible values:", NULL);
    for (t = tab; t->name; t++)
        Tcl_AppendResult (interp, " ", t->name, NULL);
    return TCL_ERROR;
}

/*
 * ir_method_r: Get status for all readable elements
 */
int ir_method_r (void *obj, Tcl_Interp *interp, int argc, char **argv,
                 IRMethod *tab)
{
    char *argv_n[3];
    int argc_n;

    argv_n[0] = argv[0];
    argc_n = 2;
    for (; tab->name; tab++)
        if (tab->type)
        {
            argv_n[1] = tab->name;
            Tcl_AppendResult (interp, "{", NULL);
            (*tab->method)(obj, interp, argc_n, argv_n);
            Tcl_AppendResult (interp, "} ", NULL);
        }
    return TCL_OK;
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
        interp->result = "strdup fail";
        return TCL_ERROR;
    }
    strcpy (*p, s);
    return TCL_OK;
}

/*
 * ir_malloc: Malloc function
 */
void *ir_malloc (Tcl_Interp *interp, size_t size)
{
    static char buf[128];
    void *p = malloc (size);

    if (!p)
    {
        sprintf (buf, "Malloc fail. %ld bytes requested", (long) size);
        interp->result = buf;
        return NULL;
    }
    return p;
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
    int r;

    req.referenceId = 0;
    req.options = &p->options;
    req.protocolVersion = &p->protocolVersion;
    req.preferredMessageSize = &p->preferredMessageSize;
    req.maximumRecordSize = &p->maximumRecordSize;

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
        Tcl_AppendResult (interp, odr_errlist [odr_geterror (p->odr_out)],
                          NULL);
        odr_reset (p->odr_out);
        return TCL_ERROR;
    }
    p->sbuf = odr_getbuf (p->odr_out, &p->slen);
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {     
        interp->result = "cs_put failed in init";
        return TCL_ERROR;
    }
    else if (r == 1)
    {
        ir_select_add_write (cs_fileno(p->cs_link), p);
        printf("Sent part of initializeRequest (%d bytes).\n", p->slen);
    }
    else
        printf("Sent whole initializeRequest (%d bytes).\n", p->slen);
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
 * do_preferredMessageSize: Set/get preferred message size
 */
static int do_preferredMessageSize (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    char buf[20];
    if (argc == 3)
    {
        if (Tcl_GetInt (interp, argv[2], 
                        &((IRObj *)obj)->preferredMessageSize)==TCL_ERROR)
            return TCL_ERROR;
    }
    sprintf (buf, "%d", ((IRObj *)obj)->preferredMessageSize);
    Tcl_AppendResult (interp, buf, NULL);
    return TCL_OK;
}

/*
 * do_maximumRecordSize: Set/get maximum record size
 */
static int do_maximumRecordSize (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    char buf[20];
    if (argc == 3)
    {
        if (Tcl_GetInt (interp, argv[2], 
                        &((IRObj *)obj)->maximumRecordSize)==TCL_ERROR)
            return TCL_ERROR;
    }
    sprintf (buf, "%d", ((IRObj *)obj)->maximumRecordSize);
    Tcl_AppendResult (interp, buf, NULL);
    return TCL_OK;
}


/*
 * do_implementationName: Set/get Implementation Name.
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
 * do_implementationId: Set/get Implementation Id.
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
 * do_idAuthentication: Set/get id Authentication
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

    if (argc == 3)
    {
        if (p->hostname)
        {
            interp->result = "already connected";
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
        if (ir_strdup (interp, &p->hostname, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
        ir_select_add (cs_fileno (p->cs_link), p);
    }
    Tcl_AppendResult (interp, p->hostname, NULL);
    return TCL_OK;
}

/*
 * do_disconnect: disconnect method on IR object
 */
static int do_disconnect (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IRObj *p = obj;

    if (p->hostname)
    {
        free (p->hostname);
        p->hostname = NULL;
        ir_select_remove (cs_fileno (p->cs_link), p);
    }
    if (cs_type (p->cs_link) == tcpip_type)
    {
        cs_close (p->cs_link);
        p->cs_link = cs_create (tcpip_type, 0);
    }
    else if (cs_type (p->cs_link) == mosi_type)
    {
        cs_close (p->cs_link);
        p->cs_link = cs_create (mosi_type, 0);
    }
    else
    {
        interp->result = "unknown comstack type";
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 * do_comstack: Set/get comstack method on IR object
 */
static int do_comstack (void *obj, Tcl_Interp *interp,
		        int argc, char **argv)
{
    char *cs_type = NULL;
    if (argc == 3)
    {
        cs_close (((IRObj*) obj)->cs_link);
        if (!strcmp (argv[2], "tcpip"))
            ((IRObj *)obj)->cs_link = cs_create (tcpip_type, 0);
        else if (!strcmp (argv[2], "mosi"))
            ((IRObj *)obj)->cs_link = cs_create (mosi_type, 0);
        else
        {
            interp->result = "wrong comstack type";
            return TCL_ERROR;
        }
    }
    if (cs_type(((IRObj *)obj)->cs_link) == tcpip_type)
        cs_type = "tcpip";
    else if (cs_type(((IRObj *)obj)->cs_link) == mosi_type)
        cs_type = "comstack";
    Tcl_AppendResult (interp, cs_type, NULL);
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
        for (i=0; i<p->num_databaseNames; i++)
            Tcl_AppendElement (interp, p->databaseNames[i]);
        return TCL_OK;
    }
    if (p->databaseNames)
    {
        for (i=0; i<p->num_databaseNames; i++)
            free (p->databaseNames[i]);
        free (p->databaseNames);
    }
    p->num_databaseNames = argc - 2;
    if (!(p->databaseNames = ir_malloc (interp, 
          sizeof(*p->databaseNames) * p->num_databaseNames)))
        return TCL_ERROR;
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
    { 1, "comstack",                do_comstack },
    { 1, "connect",                 do_connect },
    { 0, "protocolVersion",         do_protocolVersion },
    { 0, "options",                 do_options },
    { 1, "preferredMessageSize",    do_preferredMessageSize },
    { 1, "maximumRecordSize",       do_maximumRecordSize },
    { 1, "implementationName",      do_implementationName },
    { 1, "implementationId",        do_implementationId },
    { 1, "idAuthentication",        do_idAuthentication },
    { 0, "init",                    do_init_request },
    { 0, "disconnect",              do_disconnect },
    { 0, "callback",                do_callback },
    { 1, "databaseNames",           do_databaseNames},
    { 1, "query",                   do_query },
    { 0, NULL, NULL}
    };
    if (argc < 2)
        return ir_method_r (clientData, interp, argc, argv, tab);
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
    if (!(obj = ir_malloc (interp, sizeof(*obj))))
        return TCL_ERROR;
    obj->cs_link = cs_create (tcpip_type, 0);

    obj->maximumRecordSize = 32768;
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

    obj->hostname = NULL;

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
    if (!(obj->buf_out = ir_malloc (interp, obj->len_out)))
        return TCL_ERROR;
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
    int r;

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
    p->sbuf = odr_getbuf (p->odr_out, &p->slen);
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {
        interp->result = "cs_put failed in init";
        return TCL_ERROR;
    }
    else if (r == 1)
    {
        ir_select_add_write (cs_fileno(p->cs_link), p);
        printf("Sent part of searchRequest (%d bytes).\n", p->slen);
    }
    else
    {
        printf ("Whole search request\n");
    }
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
 * do_searchStatus: Get search status (after search response)
 */
static int do_searchStatus (void *o, Tcl_Interp *interp,
		            int argc, char **argv)
{
    IRSetObj *obj = o;

    sprintf (interp->result, "%d", obj->searchStatus);
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

static int marc_cmp (const char *field, const char *pattern)
{
    if (*pattern == '*')
        return 0;
    for (; *field && *pattern; field++, pattern++)
    {
        if (*pattern == '?')
            continue;
        if (*pattern != *field)
            break;
    }
    return *field - *pattern;
}

static int get_marc_fields(Tcl_Interp *interp, Iso2709Rec rec,
                           int argc, char **argv)
{
    struct iso2709_dir *dir;
    struct iso2709_field *field;

    for (dir = rec->directory; dir; dir = dir->next)
    {
        if (argc > 4 && marc_cmp (dir->tag, argv[4]))
            continue;
        if (argc > 5 && marc_cmp (dir->indicator, argv[5]))
            continue;
        for (field = dir->fields; field; field = field->next)
        {
            if (argc > 6 && marc_cmp (field->identifier, argv[6]))
                continue;
            Tcl_AppendElement (interp, field->data);
        }
    }
    return TCL_OK;
}

static int get_marc_lines (Tcl_Interp *interp, Iso2709Rec rec,
                           int argc, char **argv)
{
    struct iso2709_dir *dir;
    struct iso2709_field *field;
    
    for (dir = rec->directory; dir; dir = dir->next)
    {
        if (argc > 4 && marc_cmp (dir->tag, argv[4]))
            continue;
        if (!dir->indicator)
            Tcl_AppendResult (interp, "{", dir->tag, " {} {", NULL);
        else
        {
            if (argc > 5 && marc_cmp (dir->indicator, argv[5]))
                continue;
            Tcl_AppendResult (interp, "{", dir->tag, " {", dir->indicator, 
                              "} {", NULL);
        }
        for (field = dir->fields; field; field = field->next)
        {
            if (!field->identifier)
                Tcl_AppendResult (interp, "{{}", NULL);
            else
            {
                if (argc > 6 && marc_cmp (field->identifier, argv[6]))
                    continue;
                Tcl_AppendResult (interp, "{", field->identifier, NULL);
            }
            Tcl_AppendElement (interp, field->data);
            Tcl_AppendResult (interp, "} ", NULL);
        }
        Tcl_AppendResult (interp, "}} ", NULL);
    }
    return TCL_OK;
}

/*
 * do_recordType: Return record type (if any) at position.
 */
static int do_recordType (void *o, Tcl_Interp *interp, int argc, char **argv)
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
    rl = find_IR_record (obj, offset);
    if (!rl)
        return TCL_OK;
    switch (rl->which)
    {
    case Z_NamePlusRecord_databaseRecord:
        interp->result = "databaseRecord";
        break;
    case Z_NamePlusRecord_surrogateDiagnostic:
        interp->result = "surrogateDiagnostic";
        break;
    }
    return TCL_OK;
}

/*
 * do_recordDiag: Return diagnostic record info
 */
static int do_recordDiag (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IRSetObj *obj = o;
    int offset;
    IRRecordList *rl;
    char buf[20];

    if (argc < 3)
    {
        sprintf (interp->result, "wrong # args");
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;
    rl = find_IR_record (obj, offset);
    if (!rl)
    {
        Tcl_AppendResult (interp, "No record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (rl->which != Z_NamePlusRecord_surrogateDiagnostic)
    {
        Tcl_AppendResult (interp, "No Diagnostic record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    sprintf (buf, "%d", rl->u.diag.condition);
    Tcl_AppendResult (interp, buf, " {", 
                      (rl->u.diag.addinfo ? rl->u.diag.addinfo : ""),
                      "}", NULL);
    return TCL_OK;
}

/*
 * do_recordMarc: Get ISO2709 Record lines/fields
 */
static int do_recordMarc (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IRSetObj *obj = o;
    int offset;
    IRRecordList *rl;

    if (argc < 4)
    {
        sprintf (interp->result, "wrong # args");
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;
    rl = find_IR_record (obj, offset);
    if (!rl)
    {
        Tcl_AppendResult (interp, "No record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (rl->which != Z_NamePlusRecord_databaseRecord)
    {
        Tcl_AppendResult (interp, "No MARC record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (!strcmp (argv[3], "field"))
        return get_marc_fields (interp, rl->u.marc.rec, argc, argv);
    else if (!strcmp (argv[3], "line"))
        return get_marc_lines (interp, rl->u.marc.rec, argc, argv);
    else
    {
        Tcl_AppendResult (interp, "field/line expected", NULL);
        return TCL_ERROR;
    }
}

/*
 * do_presentStatus: Return present status (after present response)
 */
static int do_presentStatus (void *o, Tcl_Interp *interp, 
                             int argc, char **argv)
{
    IRSetObj *obj = o;
    const char *cp;
    char buf[28];

    switch (obj->which)
    {
    case Z_Records_DBOSD:
    	Tcl_AppendElement (interp, "DBOSD");
        break;
    case Z_Records_NSD:
        Tcl_AppendElement (interp, "NSD");
        sprintf (buf, "%d", obj->condition);
        Tcl_AppendElement (interp, buf);
        cp = diagbib1_str (obj->condition);
        if (cp)
            Tcl_AppendElement (interp, (char*) cp);
        else
            Tcl_AppendElement (interp, "");
        if (obj->addinfo)
            Tcl_AppendElement (interp, obj->addinfo);
        else
            Tcl_AppendElement (interp, "");
        break;
    }
    return TCL_OK;
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
    int r;

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
    p->sbuf = odr_getbuf (p->odr_out, &p->slen);
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {
        interp->result = "cs_put failed in init";
        return TCL_ERROR;
    }
    else if (r == 1)
    {
        ir_select_add_write (cs_fileno(p->cs_link), p);
        printf ("Part of present request, start=%d, num=%d (%d bytes)\n",
                start, number, p->slen);
    }
    else
    {
        printf ("Whole present request, start=%d, num=%d (%d bytes)\n",
                start, number, p->slen);
    }
    return TCL_OK;
}

/*
 * do_loadFile: Load result set from file
 */

static int do_loadFile (void *o, Tcl_Interp *interp,
                        int argc, char **argv)
{
    IRSetObj *setobj = o;
    FILE *inf;
    int  no = 1;
    const char *buf;

    if (argc < 3)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    inf = fopen (argv[2], "r");
    if (!inf)
    {
        Tcl_AppendResult (interp, "Cannot open ", argv[2], NULL);
        return TCL_ERROR;
    }
    while ((buf = iso2709_read (inf)))
    {
        IRRecordList *rl;
        Iso2709Rec rec;

        rec = iso2709_cvt (buf);
        if (!rec)
            break;
        rl = new_IR_record (setobj, no, Z_NamePlusRecord_databaseRecord);
        rl->u.marc.rec = rec;
        no++;
    }
    setobj->numberOfRecordsReturned = no-1;
    fclose (inf);
    return TCL_OK;
}


/* 
 * ir_set_obj_method: IR Set Object methods
 */
static int ir_set_obj_method (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    static IRMethod tab[] = {
    { 0, "search",                  do_search },
    { 0, "searchStatus",            do_searchStatus },
    { 0, "resultCount",             do_resultCount },
    { 0, "numberOfRecordsReturned", do_numberOfRecordsReturned },
    { 0, "present",                 do_present },
    { 0, "recordType",              do_recordType },
    { 0, "recordMarc",              do_recordMarc },
    { 0, "recordDiag",              do_recordDiag },
    { 0, "presentStatus",           do_presentStatus },
    { 0, "loadFile",                do_loadFile },
    { 0, NULL, NULL}
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
    if (!(obj = ir_malloc (interp, sizeof(*obj))))
        return TCL_ERROR;
    obj->z_records = NULL;
    obj->record_list = NULL;
    obj->addinfo = NULL;
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
    {
        obj->searchStatus = searchrs->searchStatus ? 1 : 0;
        obj->resultCount = *searchrs->resultCount;
        printf ("Search response %d, %d hits\n", 
                 obj->searchStatus, obj->resultCount);
    }
    else
        printf ("Search response, no object!\n");
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
    if (initrs->maximumRecordSize)
        printf ("MaximumRecordSize=%d\n", *initrs->maximumRecordSize);
    if (initrs->preferredMessageSize)
        printf ("PreferredMessageSize=%d\n", *initrs->preferredMessageSize);
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
        setobj->which = zrs->which;
        if (zrs->which == Z_Records_NSD)
        {
            const char *addinfo;

            printf ("They are diagnostic!!!\n");

            setobj->numberOfRecordsReturned = 0;
            setobj->condition = *zrs->u.nonSurrogateDiagnostic->condition;
            free (setobj->addinfo);
            setobj->addinfo = NULL;
            addinfo = zrs->u.nonSurrogateDiagnostic->addinfo;
            if (addinfo && (setobj->addinfo = malloc (strlen(addinfo) + 1)))
                strcpy (setobj->addinfo, addinfo);
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
                rl = new_IR_record (setobj, setobj->start + offset,
                                    zrs->u.databaseOrSurDiagnostics->
                                    records[offset]->which);
                if (rl->which == Z_NamePlusRecord_surrogateDiagnostic)
                {
                    Z_DiagRec *diagrec;

                    diagrec = zrs->u.databaseOrSurDiagnostics->
                              records[offset]->u.surrogateDiagnostic;

                    rl->u.diag.condition = *diagrec->condition;
                    if (diagrec->addinfo && (rl->u.diag.addinfo =
                        malloc (strlen (diagrec->addinfo)+1)))
                        strcpy (rl->u.diag.addinfo, diagrec->addinfo);
                }
                else
                {
                    Z_DatabaseRecord *zr; 
                    Odr_external *oe;
                    
                    zr = zrs->u.databaseOrSurDiagnostics->records[offset]
                        ->u.databaseRecord;
                    oe = (Odr_external*) zr;
                    if (oe->which == ODR_EXTERNAL_octet
                        && zr->u.octet_aligned->len)
                    {
                        const char *buf = (char*) zr->u.octet_aligned->buf;
                        rl->u.marc.rec = iso2709_cvt (buf);
                    }
                    else
                        rl->u.marc.rec = NULL;
                }
            }
        }
    }
    else
    {
        printf ("No records!\n");
    }
}

/*
 * ir_select_read: handle incoming packages
 */
void ir_select_read (ClientData clientData)
{
    IRObj *p = clientData;
    Z_APDU *apdu;
    int r;
    
    do
    {
        if ((r=cs_get (p->cs_link, &p->buf_in, &p->len_in))  <= 0)
        {
            printf ("cs_get failed\n");
            ir_select_remove (cs_fileno (p->cs_link), p);
            return;
        }        
        if (r == 1)
            return ;
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

/*
 * ir_select_write: handle outgoing packages - not yet written.
 */
void ir_select_write (ClientData clientData)
{
    IRObj *p = clientData;
    int r;
    
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {   
        printf ("select write fail\n");
        cs_close (p->cs_link);
    }
    else if (r == 0)            /* remove select bit */
    {
        ir_select_remove_write (cs_fileno (p->cs_link), p);
    }
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


