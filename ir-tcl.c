/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: ir-tcl.c,v $
 * Revision 1.28  1995-05-23 15:34:48  adam
 * Many new settings, userInformationField, smallSetUpperBound, etc.
 * A number of settings are inherited when ir-set is executed.
 * This version is incompatible with the graphical test client (client.tcl).
 *
 * Revision 1.27  1995/05/11  15:34:47  adam
 * Scan request changed a bit. This version works with RLG.
 *
 * Revision 1.26  1995/04/18  16:11:51  adam
 * First version of graphical Scan. Some work on query-by-form.
 *
 * Revision 1.25  1995/04/17  09:37:17  adam
 * Further development of scan.
 *
 * Revision 1.24  1995/04/11  14:16:42  adam
 * Further work on scan. Response works. Entries aren't saved yet.
 *
 * Revision 1.23  1995/04/10  10:50:27  adam
 * Result-set name defaults to suffix of ir-set name.
 * Started working on scan. Not finished at this point.
 *
 * Revision 1.22  1995/03/31  10:43:03  adam
 * More robust when getting bad MARC records.
 *
 * Revision 1.21  1995/03/31  08:56:37  adam
 * New button "Search".
 *
 * Revision 1.20  1995/03/29  16:07:09  adam
 * Bug fix: Didn't use setName in present request.
 *
 * Revision 1.19  1995/03/28  12:45:23  adam
 * New ir method failback: called on disconnect/protocol error.
 * New ir set/get method: protocol: SR / Z3950.
 * Simple popup and disconnect when failback is invoked.
 *
 * Revision 1.18  1995/03/21  15:50:12  adam
 * Minor changes.
 *
 * Revision 1.17  1995/03/21  13:41:03  adam
 * Comstack cs_create not used too often. Non-blocking connect.
 *
 * Revision 1.16  1995/03/21  08:26:06  adam
 * New method, setName, to specify the result set name (other than Default).
 * New method, responseStatus, which returns diagnostic info, if any, after
 * present response / search response.
 *
 * Revision 1.15  1995/03/20  15:24:07  adam
 * Diagnostic records saved on searchResponse.
 *
 * Revision 1.14  1995/03/20  08:53:22  adam
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

#define CS_BLOCK 0

#include "ir-tclp.h"

typedef struct {
    int type;
    char *name;
    int (*method) (void *obj, Tcl_Interp *interp, int argc, char **argv);
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
 * getsetint: Set/get integer value
 */
static int get_set_int (int *val, Tcl_Interp *interp, int argc, char **argv)
{
    char buf[20];
    
    if (argc == 3)
    {
        if (Tcl_GetInt (interp, argv[2], val)==TCL_ERROR)
            return TCL_ERROR;
    }
    sprintf (buf, "%d", *val);
    Tcl_AppendResult (interp, buf, NULL);
    return TCL_OK;
}

/*
 * mk_nonSurrogateDiagnostics: Make Tcl result with diagnostic info
 */
static int mk_nonSurrogateDiagnostics (Tcl_Interp *interp, 
                                       int condition,
				       const char *addinfo)
{
    char buf[20];
    const char *cp;

    Tcl_AppendElement (interp, "NSD");
    sprintf (buf, "%d", condition);
    Tcl_AppendElement (interp, buf);
    cp = diagbib1_str (condition);
    if (cp)
        Tcl_AppendElement (interp, (char*) cp);
    else
        Tcl_AppendElement (interp, "");
    if (addinfo)
        Tcl_AppendElement (interp, (char*) addinfo);
    else
        Tcl_AppendElement (interp, "");
    return TCL_OK;
}

/*
 * get_parent_info: Returns information about parent object.
 */
static int get_parent_info (Tcl_Interp *interp, const char *name,
			    Tcl_CmdInfo *parent_info,
			    const char **suffix)
{
    char parent_name[128];
    const char *csep = strrchr (name, '.');
    int pos;

    if (!csep)
    {
        interp->result = "missing .";
        return TCL_ERROR;
    }
    if (suffix)
        *suffix = csep+1;
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
               IRMethod *tab, int sigerr)
{
    IRMethod *t;
    for (t = tab; t->name; t++)
        if (!strcmp (t->name, argv[1]))
            return (*t->method)(obj, interp, argc, argv);
    if (sigerr)
        return TCL_ERROR;
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
int ir_strdup (Tcl_Interp *interp, char** p, const char *s)
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
    Z_APDU apdu, *apdup = &apdu;
    IRObj *p = obj;
    Z_InitRequest req;
    int r;

    if (!p->cs_link)
    {
        interp->result = "not connected";
        return TCL_ERROR;
    }
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

    if (!z_APDU (p->odr_out, &apdup, 0))
    {
        Tcl_AppendResult (interp, odr_errlist [odr_geterror (p->odr_out)],
                          NULL);
        odr_reset (p->odr_out);
        return TCL_ERROR;
    }
    p->sbuf = odr_getbuf (p->odr_out, &p->slen, NULL);
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {     
        interp->result = "cs_put failed in init";
        do_disconnect (p, NULL, 0, NULL);
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
    IRObj *p = obj;
    return get_set_int (&p->preferredMessageSize, interp, argc, argv);
}

/*
 * do_maximumRecordSize: Set/get maximum record size
 */
static int do_maximumRecordSize (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    IRObj *p = obj;
    return get_set_int (&p->maximumRecordSize, interp, argc, argv);
}

/*
 * do_initResult: Get init result
 */
static int do_initResult (void *o, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IRObj *obj = o;
    
    return get_set_int (&obj->initResult, interp, argc, argv);
}


/*
 * do_implementationName: Set/get Implementation Name.
 */
static int do_implementationName (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    IRObj *p = obj;

    if (argc == 3)
    {
        free (((IRObj*)obj)->implementationName);
        if (ir_strdup (interp, &p->implementationName, argv[2])
            == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->implementationName,
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
 * do_targetImplementationName: Get Implementation Name of target.
 */
static int do_targetImplementationName (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    IRObj *p = obj;

    Tcl_AppendResult (interp, p->targetImplementationName,
                      (char*) NULL);
    return TCL_OK;
}

/*
 * do_targetImplementationId: Get Implementation Id of target
 */
static int do_targetImplementationId (void *obj, Tcl_Interp *interp,
                                      int argc, char **argv)
{
    Tcl_AppendResult (interp, ((IRObj*)obj)->targetImplementationId,
                      (char*) NULL);
    return TCL_OK;
}

/*
 * do_targetImplementationVersion: Get Implementation Version of target
 */
static int do_targetImplementationVersion (void *obj, Tcl_Interp *interp,
                                           int argc, char **argv)
{
    Tcl_AppendResult (interp, ((IRObj*)obj)->targetImplementationVersion,
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
    int r;
    int protocol_type = CS_Z3950;

    if (argc == 3)
    {
        if (p->hostname)
        {
            interp->result = "already connected";
            return TCL_ERROR;
        }
        if (!strcmp (p->protocol_type, "Z3950"))
            protocol_type = CS_Z3950;
        else if (!strcmp (p->protocol_type, "SR"))
            protocol_type = CS_SR;
        else
        {
            interp->result = "bad protocol type";
            return TCL_ERROR;
        }
        if (!strcmp (p->cs_type, "tcpip"))
        {
            p->cs_link = cs_create (tcpip_type, CS_BLOCK, protocol_type);
            addr = tcpip_strtoaddr (argv[2]);
            if (!addr)
            {
                interp->result = "tcpip_strtoaddr fail";
                return TCL_ERROR;
            }
            printf ("tcp/ip connect %s\n", argv[2]);
        }
#if MOSI
        else if (!strcmp (p->cs_type, "mosi"))
        {
            p->cs_link = cs_create (mosi_type, CS_BLOCK, protocol_type);
            addr = mosi_strtoaddr (argv[2]);
            if (!addr)
            {
                interp->result = "mosi_strtoaddr fail";
                return TCL_ERROR;
            }
            printf ("mosi connect %s\n", argv[2]);
        }
#endif
        else 
        {
            interp->result = "unknown comstack type";
            return TCL_ERROR;
        }
        if (ir_strdup (interp, &p->hostname, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
        if ((r=cs_connect (p->cs_link, addr)) < 0)
        {
            interp->result = "cs_connect fail";
            do_disconnect (p, NULL, 0, NULL);
            return TCL_ERROR;
        }
        ir_select_add (cs_fileno (p->cs_link), p);
        if (r == 1)
        {
            ir_select_add_write (cs_fileno (p->cs_link), p);
            p->connectFlag = 1;
        }
        else
        {
            p->connectFlag = 0;
            if (p->callback)
                Tcl_Eval (p->interp, p->callback);
        }
    }
    if (p->hostname)
        Tcl_AppendElement (interp, p->hostname);
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
        ir_select_remove_write (cs_fileno (p->cs_link), p);
        ir_select_remove (cs_fileno (p->cs_link), p);

        assert (p->cs_link);
        cs_close (p->cs_link);
        p->cs_link = NULL;
    }
    assert (!p->cs_link);
    return TCL_OK;
}

/*
 * do_comstack: Set/get comstack method on IR object
 */
static int do_comstack (void *o, Tcl_Interp *interp,
		        int argc, char **argv)
{
    IRObj *obj = o;

    if (argc == 3)
    {
        free (obj->cs_type);
        if (ir_strdup (interp, &obj->cs_type, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendElement (interp, obj->cs_type);
    return TCL_OK;
}

/*
 * do_protocol: Set/get protocol method on IR object
 */
static int do_protocol (void *o, Tcl_Interp *interp,
		        int argc, char **argv)
{
    IRObj *obj = o;

    if (argc == 3)
    {
        free (obj->protocol_type);
        if (ir_strdup (interp, &obj->protocol_type, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendElement (interp, obj->protocol_type);
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
 * do_failback: add error handle callback
 */
static int do_failback (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IRObj *p = obj;

    if (argc == 3)
    {
        free (p->failback);
        if (ir_strdup (interp, &p->failback, argv[2]) == TCL_ERROR)
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
    IRSetCObj *p = obj;

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
 * do_replaceIndicator: Set/get replace Set indicator
 */
static int do_replaceIndicator (void *obj, Tcl_Interp *interp,
                                int argc, char **argv)
{
    IRSetCObj *p = obj;

    return get_set_int (&p->replaceIndicator, interp, argc, argv);
}

/*
 * do_queryType: Set/Get query method
 */
static int do_queryType (void *obj, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRSetCObj *p = obj;

    if (argc == 3)
    {
        free (p->queryType);
        if (ir_strdup (interp, &p->queryType, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->queryType, NULL);
    return TCL_OK;
}

/*
 * do_userInformationField: Get User information field
 */
static int do_userInformationField (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    IRObj *p = obj;

    Tcl_AppendResult (interp, p->userInformationField, NULL);
    return TCL_OK;
}

/*
 * do_smallSetUpperBound: Set/get small set upper bound
 */
static int do_smallSetUpperBound (void *o, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRSetCObj *obj = o;

    return get_set_int (&obj->smallSetUpperBound, interp, argc, argv);
}

/*
 * do_largeSetLowerBound: Set/get large set lower bound
 */
static int do_largeSetLowerBound (void *o, Tcl_Interp *interp,
                                  int argc, char **argv)
{
    IRSetCObj *obj = o;

    return get_set_int (&obj->largeSetLowerBound, interp, argc, argv);
}

/*
 * do_mediumSetPresentNumber: Set/get large set lower bound
 */
static int do_mediumSetPresentNumber (void *o, Tcl_Interp *interp,
                                      int argc, char **argv)
{
    IRSetCObj *obj = o;
    
    return get_set_int (&obj->mediumSetPresentNumber, interp, argc, argv);
}


static IRMethod ir_method_tab[] = {
{ 1, "comstack",                    do_comstack },
{ 1, "protocol",                    do_protocol },
{ 0, "failback",                    do_failback },

{ 1, "connect",                     do_connect },
{ 0, "protocolVersion",             do_protocolVersion },
{ 1, "preferredMessageSize",        do_preferredMessageSize },
{ 1, "maximumRecordSize",           do_maximumRecordSize },
{ 1, "implementationName",          do_implementationName },
{ 1, "implementationId",            do_implementationId },
{ 0, "targetImplementationName",    do_targetImplementationName },
{ 0, "targetImplementationId",      do_targetImplementationId },
{ 0, "targetImplementationVersion", do_targetImplementationVersion },
{ 0, "userInformationField",        do_userInformationField },
{ 1, "idAuthentication",            do_idAuthentication },
{ 0, "options",                     do_options },
{ 0, "init",                        do_init_request },
{ 0, "initResult",                  do_initResult },
{ 0, "disconnect",                  do_disconnect },
{ 0, "callback",                    do_callback },
{ 0, NULL, NULL}
};

static IRMethod ir_set_c_method_tab[] = {
{ 0, "databaseNames",           do_databaseNames},
{ 0, "replaceIndicator",        do_replaceIndicator},
{ 0, "queryType",               do_queryType },
{ 0, "smallSetUpperBound",      do_smallSetUpperBound},
{ 0, "largeSetLowerBound",      do_largeSetLowerBound},
{ 0, "mediumSetPresentNumber",  do_mediumSetPresentNumber},
{ 0, NULL, NULL}
};

/* 
 * ir_obj_method: IR Object methods
 */
static int ir_obj_method (ClientData clientData, Tcl_Interp *interp,
int argc, char **argv)
{
    if (argc < 2)
        return ir_method_r (clientData, interp, argc, argv, ir_method_tab);
    if (ir_method (clientData, interp, argc, argv,
                   ir_method_tab, 1) == TCL_OK)
        return TCL_OK;
    return ir_method (&((IRObj*) clientData)->set_inher, interp,
                      argc, argv, ir_set_c_method_tab, 0);
}

/* 
 * ir_obj_delete: IR Object disposal
 */
static void ir_obj_delete (ClientData clientData)
{
    free ( (void*) clientData);
}

static int ir_reset_inher (Tcl_Interp *interp, IRSetCObj *o)
{
    o->smallSetUpperBound = 0;
    o->largeSetLowerBound = 2;
    o->mediumSetPresentNumber = 0;
    o->replaceIndicator = 1;
#if 0
    obj->databaseNames = NULL;
    obj->num_databaseNames = 0; 
#else
    o->num_databaseNames = 1;
    if (!(o->databaseNames =
          ir_malloc (interp, sizeof(*o->databaseNames))))
        return TCL_ERROR;
    if (ir_strdup (interp, &o->databaseNames[0], "Default")
        == TCL_ERROR)
        return TCL_ERROR;
#endif
    if (ir_strdup (interp, &o->queryType, "rpn") == TCL_ERROR)
        return TCL_ERROR;
    return TCL_OK;
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
    if (ir_strdup (interp, &obj->cs_type, "tcpip") == TCL_ERROR)
        return TCL_ERROR;
    if (ir_strdup (interp, &obj->protocol_type, "Z3950") == TCL_ERROR)
        return TCL_ERROR;
    obj->cs_link = NULL;
    obj->bib1.proto = PROTO_Z3950;
    obj->bib1.class = CLASS_ATTSET;
    obj->bib1.value = VAL_BIB1;

    obj->maximumRecordSize = 32768;
    obj->preferredMessageSize = 4096;
    obj->connectFlag = 0;

    obj->idAuthentication = NULL;

    if (ir_strdup (interp, &obj->implementationName, "TCL/TK on YAZ")
        == TCL_ERROR)
        return TCL_ERROR;

    if (ir_strdup (interp, &obj->implementationId, "TCL/TK/YAZ")
        == TCL_ERROR)
        return TCL_ERROR;

    obj->targetImplementationName = NULL;
    obj->targetImplementationId = NULL;
    obj->targetImplementationVersion = NULL;
    obj->userInformationField = NULL;
    
    obj->hostname = NULL;

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
    odr_setbuf (obj->odr_out, obj->buf_out, obj->len_out, 0);

    obj->len_in = 0;
    obj->buf_in = NULL;

    obj->callback = NULL;
    obj->failback = NULL;

    if (ir_reset_inher (interp, &obj->set_inher) == TCL_ERROR)
        return TCL_ERROR;
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
    Z_APDU apdu, *apdup = &apdu;
    Odr_oct ccl_query;
    IRSetObj *obj = o;
    IRObj *p = obj->parent;
    int r;

    p->set_child = o;
    if (argc != 3)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (!p->set_inher.num_databaseNames)
    {
        interp->result = "no databaseNames";
        return TCL_ERROR;
    }
    if (!p->cs_link)
    {
        interp->result = "not connected";
        return TCL_ERROR;
    }
    apdu.which = Z_APDU_searchRequest;
    apdu.u.searchRequest = &req;

    req.referenceId = 0;
    req.smallSetUpperBound = &p->set_inher.smallSetUpperBound;
    req.largeSetLowerBound = &p->set_inher.largeSetLowerBound;
    req.mediumSetPresentNumber = &p->set_inher.mediumSetPresentNumber;
    req.replaceIndicator = &p->set_inher.replaceIndicator;
    req.resultSetName = obj->setName ? obj->setName : "Default";
    req.num_databaseNames = p->set_inher.num_databaseNames;
    req.databaseNames = p->set_inher.databaseNames;
    printf ("Search:");
    for (r=0; r < p->set_inher.num_databaseNames; r++)
    {
        printf (" %s", p->set_inher.databaseNames[r]);
    }
    req.smallSetElementSetNames = 0;
    req.mediumSetElementSetNames = 0;
    req.preferredRecordSyntax = 0;
    req.query = &query;

    if (!strcmp (p->set_inher.queryType, "rpn"))
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
        ccl_pr_tree (rpn, stderr);
        fprintf (stderr, "\n");
        query.which = Z_Query_type_1;
        assert((RPNquery = ccl_rpn_query(rpn)));
        RPNquery->attributeSetId = oid_getoidbyent (&p->bib1);
        query.u.type_1 = RPNquery;
        printf ("- RPN\n");
    }
    else if (!strcmp (p->set_inher.queryType, "ccl"))
    {
        query.which = Z_Query_type_2;
        query.u.type_2 = &ccl_query;
        ccl_query.buf = (unsigned char *) argv[2];
        ccl_query.len = strlen (argv[2]);
        printf ("- CCL\n");
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
    p->sbuf = odr_getbuf (p->odr_out, &p->slen, NULL);
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {
        interp->result = "cs_put failed in search";
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

    return get_set_int (&obj->resultCount, interp, argc, argv);
}

/*
 * do_searchStatus: Get search status (after search response)
 */
static int do_searchStatus (void *o, Tcl_Interp *interp,
		            int argc, char **argv)
{
    IRSetObj *obj = o;

    return get_set_int (&obj->searchStatus, interp, argc, argv);
}

/*
 * do_setName: Set result Set name
 */
static int do_setName (void *o, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRSetObj *obj = o;

    if (argc == 3)
    {
        free (obj->setName);
        if (ir_strdup (interp, &obj->setName, argv[2])
            == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendElement (interp, obj->setName);
    return TCL_OK;
}

/*
 * do_numberOfRecordsReturned: Get number of records returned
 */
static int do_numberOfRecordsReturned (void *o, Tcl_Interp *interp,
		       int argc, char **argv)
{
    IRSetObj *obj = o;

    return get_set_int (&obj->numberOfRecordsReturned, interp, argc, argv);
}

static int get_marc_fields(Tcl_Interp *interp, Iso2709Rec rec,
                           int argc, char **argv)
{
    Iso2709Anchor a;
    char *data;

    if (!rec)
        return TCL_OK;
    a = iso2709_a_mk (rec);
    while (iso2709_a_search (a, argv[4], argv[5], argv[6]))
    {
        if (!(iso2709_a_info_field (a, NULL, NULL, NULL, &data)))
            break;
        Tcl_AppendElement (interp, data);
        iso2709_a_next (a);
    }

    iso2709_a_rm (a);
    return TCL_OK;
}

static int get_marc_lines(Tcl_Interp *interp, Iso2709Rec rec,
                         int argc, char **argv)
{
    Iso2709Anchor a;
    char *tag;
    char *indicator;
    char *identifier;
    char *data;
    char *ptag = "";
    
    if (!rec)
        return TCL_OK;
    a = iso2709_a_mk (rec);
    while (iso2709_a_search (a, argv[4], argv[5], argv[6]))
    {
        if (!(iso2709_a_info_field (a, &tag, &indicator, &identifier, &data)))
            break;
        if (strcmp (tag, ptag))
        {
            if (*ptag)
                Tcl_AppendResult (interp, "}} ", NULL);
            if (!indicator)
                Tcl_AppendResult (interp, "{", tag, " {} {", NULL);
            else
                Tcl_AppendResult (interp, "{", tag, " {", indicator, 
                                  "} {", NULL);
            ptag = tag;
        }
        if (!identifier)
            Tcl_AppendResult (interp, "{{}", NULL);
        else
            Tcl_AppendResult (interp, "{", identifier, NULL);
        Tcl_AppendElement (interp, data);
        Tcl_AppendResult (interp, "} ", NULL);
        iso2709_a_next (a);
    }
    if (*ptag)
        Tcl_AppendResult (interp, "}} ", NULL);
    iso2709_a_rm (a);
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
 * do_responseStatus: Return response status (present or search)
 */
static int do_responseStatus (void *o, Tcl_Interp *interp, 
                             int argc, char **argv)
{
    IRSetObj *obj = o;

    if (!obj->recordFlag)
    {
        Tcl_AppendElement (interp, "OK");
        return TCL_OK;
    }
    switch (obj->which)
    {
    case Z_Records_DBOSD:
    	Tcl_AppendElement (interp, "DBOSD");
        break;
    case Z_Records_NSD:
        return mk_nonSurrogateDiagnostics (interp, obj->condition, 
	                                   obj->addinfo);
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
    Z_APDU apdu, *apdup = &apdu;
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
    if (!p->cs_link)
    {
        interp->result = "not connected";
        return TCL_ERROR;
    }
    obj->start = start;
    obj->number = number;

    apdu.which = Z_APDU_presentRequest;
    apdu.u.presentRequest = &req;
    req.referenceId = 0;
    /* sprintf(setstring, "%d", setnumber); */

    req.resultSetId = obj->setName ? obj->setName : "Default";
    
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
    p->sbuf = odr_getbuf (p->odr_out, &p->slen, NULL);
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {
        interp->result = "cs_put failed in present";
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
    { 0, "setName",                 do_setName },
    { 0, "resultCount",             do_resultCount },
    { 0, "numberOfRecordsReturned", do_numberOfRecordsReturned },
    { 0, "present",                 do_present },
    { 0, "recordType",              do_recordType },
    { 0, "recordMarc",              do_recordMarc },
    { 0, "recordDiag",              do_recordDiag },
    { 0, "responseStatus",          do_responseStatus },
    { 0, "loadFile",                do_loadFile },
    { 0, NULL, NULL}
    };

    if (argc < 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (ir_method (clientData, interp, argc, argv, tab, 1) == TCL_OK)
        return TCL_OK;
    return ir_method (&((IRSetObj *)clientData)->set_inher, interp, argc,
                      argv, ir_set_c_method_tab, 0);
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
    IRSetObj *obj;

    if (argc < 2 || argc > 3)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    else if (argc == 3)
    {
        Tcl_CmdInfo parent_info;
        int i;
        IRSetCObj *dst;
        IRSetCObj *src;

        if (!Tcl_GetCommandInfo (interp, argv[2], &parent_info))
        {
            interp->result = "No parent";
            return TCL_ERROR;
        }
        if (!(obj = ir_malloc (interp, sizeof(*obj))))
            return TCL_ERROR;
        obj->parent = (IRObj *) parent_info.clientData;

        dst = &obj->set_inher;
        src = &obj->parent->set_inher;

        dst->num_databaseNames = src->num_databaseNames;
        if (!(dst->databaseNames =
              ir_malloc (interp, sizeof (*dst->databaseNames)
                         * dst->num_databaseNames)))
            return TCL_ERROR;
        for (i = 0; i < dst->num_databaseNames; i++)
        {
            printf ("database %i %s\n", i, src->databaseNames[i]);
            if (ir_strdup (interp, &dst->databaseNames[i],
                           src->databaseNames[i]) == TCL_ERROR)
                return TCL_ERROR;
        }
        if (ir_strdup (interp, &dst->queryType, src->queryType)
            == TCL_ERROR)
            return TCL_ERROR;
        
        dst->smallSetUpperBound = src->smallSetUpperBound;
        dst->largeSetLowerBound = src->largeSetLowerBound;
        dst->mediumSetPresentNumber = src->mediumSetPresentNumber;
        printf ("ssu lsl msp %d %d %d\n", dst->smallSetUpperBound,
                dst->largeSetLowerBound, dst->mediumSetPresentNumber);
    }   
    else
        obj->parent = NULL;
    if (ir_strdup (interp, &obj->setName, argv[2]) == TCL_ERROR)
        return TCL_ERROR;
    obj->record_list = NULL;
    obj->addinfo = NULL;
    Tcl_CreateCommand (interp, argv[1], ir_set_obj_method,
                       (ClientData) obj, ir_set_obj_delete);
    return TCL_OK;
}

/* ------------------------------------------------------- */

/*
 * do_scan: Perform scan 
 */
static int do_scan (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    Z_ScanRequest req;
    Z_APDU apdu, *apdup = &apdu;
    IRScanObj *obj = o;
    IRObj *p = obj->parent;
    int r, pos;
    struct ccl_rpn_node *rpn;

    p->scan_child = o;
    if (argc != 3)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (!p->set_inher.num_databaseNames)
    {
        interp->result = "no databaseNames";
	return TCL_ERROR;
    }
    if (!p->cs_link)
    {
        interp->result = "not connected";
	return TCL_ERROR;
    }
    apdu.which = Z_APDU_scanRequest;
    apdu.u.scanRequest = &req;
    req.referenceId = NULL;
    req.num_databaseNames = p->set_inher.num_databaseNames;
    req.databaseNames = p->set_inher.databaseNames;
    req.attributeSet = oid_getoidbyent (&p->bib1);

#if 0
    if (!(req.termListAndStartPoint =
          ir_malloc (interp, sizeof(*req.termListAndStartPoint))))
        return TCL_ERROR;
    req.termListAndStartPoint->num_attributes = 0;
    req.termListAndStartPoint->attributeList = NULL;
    if (!(req.termListAndStartPoint->term = ir_malloc (interp,
                                                       sizeof(Z_Term))))
        return TCL_ERROR;
    req.termListAndStartPoint->term->which = Z_Term_general;
    if (!(req.termListAndStartPoint->term->u.general = 
        ir_malloc (interp, sizeof(*req.termListAndStartPoint->
                                  term->u.general))))
        return TCL_ERROR;
    if (ir_strdup (interp, &req.termListAndStartPoint->term->u.general->buf,
                   argv[2]) == TCL_ERROR)
        return TCL_ERROR;
    req.termListAndStartPoint->term->u.general->len = 
        req.termListAndStartPoint->term->u.general->size = strlen(argv[2]);
#else
    rpn = ccl_find_str(p->bibset, argv[2], &r, &pos);
    if (r)
    {
        Tcl_AppendResult (interp, "CCL error: ", ccl_err_msg (r), NULL);
        return TCL_ERROR;
    }
    ccl_pr_tree (rpn, stderr);
    fprintf (stderr, "\n");
    if (!(req.termListAndStartPoint = ccl_scan_query (rpn)))
        return TCL_ERROR;
#endif
    req.stepSize = &obj->stepSize;
    req.numberOfTermsRequested = &obj->numberOfTermsRequested;
    req.preferredPositionInResponse = &obj->preferredPositionInResponse;
    printf ("stepSize=%d\n", *req.stepSize);
    printf ("numberOfTermsRequested=%d\n", *req.numberOfTermsRequested);
    printf ("preferredPositionInResponse=%d\n",
            *req.preferredPositionInResponse);

    if (!z_APDU (p->odr_out, &apdup, 0))
    {
        interp->result = odr_errlist [odr_geterror (p->odr_out)];
        odr_reset (p->odr_out);
        return TCL_ERROR;
    } 
    p->sbuf = odr_getbuf (p->odr_out, &p->slen, NULL);
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {
        interp->result = "cs_put failed in scan";
        return TCL_ERROR;
    }
    else if (r == 1)
    {
        ir_select_add_write (cs_fileno(p->cs_link), p);
        printf("Sent part of scanRequest (%d bytes).\n", p->slen);
    }
    else
    {
        printf ("Whole scan request\n");
    }
    return TCL_OK;
}

/*
 * do_stepSize: Set/get replace Step Size
 */
static int do_stepSize (void *obj, Tcl_Interp *interp,
                        int argc, char **argv)
{
    IRScanObj *p = obj;
    return get_set_int (&p->stepSize, interp, argc, argv);
}

/*
 * do_numberOfTermsRequested: Set/get Number of Terms requested
 */
static int do_numberOfTermsRequested (void *obj, Tcl_Interp *interp,
                                      int argc, char **argv)
{
    IRScanObj *p = obj;
    return get_set_int (&p->numberOfTermsRequested, interp, argc, argv);
}


/*
 * do_preferredPositionInResponse: Set/get preferred Position
 */
static int do_preferredPositionInResponse (void *obj, Tcl_Interp *interp,
                                           int argc, char **argv)
{
    IRScanObj *p = obj;
    return get_set_int (&p->preferredPositionInResponse, interp, argc, argv);
}

/*
 * do_scanStatus: Get scan status
 */
static int do_scanStatus (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IRScanObj *p = obj;
    return get_set_int (&p->scanStatus, interp, argc, argv);
}

/*
 * do_numberOfEntriesReturned: Get number of Entries returned
 */
static int do_numberOfEntriesReturned (void *obj, Tcl_Interp *interp,
                                       int argc, char **argv)
{
    IRScanObj *p = obj;
    return get_set_int (&p->numberOfEntriesReturned, interp, argc, argv);
}

/*
 * do_positionOfTerm: Get position of Term
 */
static int do_positionOfTerm (void *obj, Tcl_Interp *interp,
                              int argc, char **argv)
{
    IRScanObj *p = obj;
    return get_set_int (&p->positionOfTerm, interp, argc, argv);
}

/*
 * do_scanLine: get Scan Line (surrogate or normal) after response
 */
static int do_scanLine (void *obj, Tcl_Interp *interp, int argc, char **argv)
{
    IRScanObj *p = obj;
    int i;
    char numstr[20];

    if (argc != 3)
    {
        interp->result = "wrong # args";
	return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &i) == TCL_ERROR)
        return TCL_ERROR;
    if (!p->entries_flag || p->which != Z_ListEntries_entries || !p->entries
        || i >= p->num_entries || i < 0)
        return TCL_OK;
    switch (p->entries[i].which)
    {
    case Z_Entry_termInfo:
        Tcl_AppendElement (interp, "T");
	if (p->entries[i].u.term.buf)
	    Tcl_AppendElement (interp, p->entries[i].u.term.buf);
	else
	    Tcl_AppendElement (interp, "");
	sprintf (numstr, "%d", p->entries[i].u.term.globalOccurrences);
	Tcl_AppendElement (interp, numstr);
	break;
    case Z_Entry_surrogateDiagnostic:
        return 
	    mk_nonSurrogateDiagnostics (interp, p->entries[i].u.diag.condition,
	                                p->entries[i].u.diag.addinfo);
	break;
    }
    return TCL_OK;
}

/* 
 * ir_scan_obj_method: IR Scan Object methods
 */
static int ir_scan_obj_method (ClientData clientData, Tcl_Interp *interp,
                               int argc, char **argv)
{
    static IRMethod tab[] = {
    { 0, "scan",                    do_scan },
    { 0, "stepSize",                do_stepSize },
    { 0, "numberOfTermsRequested",  do_numberOfTermsRequested },
    { 0, "preferredPositionInResponse", do_preferredPositionInResponse },
    { 0, "scanStatus",              do_scanStatus },
    { 0, "numberOfEntriesReturned", do_numberOfEntriesReturned },
    { 0, "positionOfTerm",          do_positionOfTerm },
    { 0, "scanLine",                do_scanLine },
    { 0, NULL, NULL}
    };

    if (argc < 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    return ir_method (clientData, interp, argc, argv, tab, 0);
}

/* 
 * ir_scan_obj_delete: IR Scan Object disposal
 */
static void ir_scan_obj_delete (ClientData clientData)
{
    free ( (void*) clientData);
}

/* 
 * ir_scan_obj_mk: IR Scan Object creation
 */
static int ir_scan_obj_mk (ClientData clientData, Tcl_Interp *interp,
                           int argc, char **argv)
{
    Tcl_CmdInfo parent_info;
    IRScanObj *obj;

    if (argc != 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    if (get_parent_info (interp, argv[1], &parent_info, NULL) == TCL_ERROR)
        return TCL_ERROR;
    if (!(obj = ir_malloc (interp, sizeof(*obj))))
        return TCL_ERROR;

    obj->stepSize = 0;
    obj->numberOfTermsRequested = 20;
    obj->preferredPositionInResponse = 1;

    obj->entries = NULL;
    obj->nonSurrogateDiagnostics = NULL;

    obj->parent = (IRObj *) parent_info.clientData;
    Tcl_CreateCommand (interp, argv[1], ir_scan_obj_method,
                       (ClientData) obj, ir_scan_obj_delete);
    return TCL_OK;
}

/* ------------------------------------------------------- */

static void ir_initResponse (void *obj, Z_InitResponse *initrs)
{
    IRObj *p = obj;

    p->initResult = *initrs->result ? 1 : 0;
    if (!*initrs->result)
        printf("Connection rejected by target.\n");
    else
        printf("Connection accepted by target.\n");

    free (p->targetImplementationId);
    ir_strdup (p->interp, &p->targetImplementationId,
               initrs->implementationId);
    free (p->targetImplementationName);
    ir_strdup (p->interp, &p->targetImplementationName,
               initrs->implementationName);
    free (p->targetImplementationVersion);
    ir_strdup (p->interp, &p->targetImplementationVersion,
               initrs->implementationVersion);

    p->maximumRecordSize = *initrs->maximumRecordSize;
    p->preferredMessageSize = *initrs->preferredMessageSize;

    free (p->userInformationField);
    p->userInformationField = NULL;
    if (initrs->userInformationField)
    {
        int len;

        if (initrs->userInformationField->which == ODR_EXTERNAL_octet && 
            (p->userInformationField =
             malloc ((len = 
                      initrs->userInformationField->u.octet_aligned->len)
                     +1)))
        {
            memcpy (p->userInformationField,
                    initrs->userInformationField->u.octet_aligned->buf,
                        len);
            (p->userInformationField)[len] = '\0';
        }
    }
}

static void ir_handleRecords (void *o, Z_Records *zrs)
{
    IRObj *p = o;
    IRSetObj *setobj = p->set_child;

    setobj->which = zrs->which;
    setobj->recordFlag = 1;
    if (zrs->which == Z_Records_NSD)
    {
        const char *addinfo;
        
        setobj->numberOfRecordsReturned = 0;
        setobj->condition = *zrs->u.nonSurrogateDiagnostic->condition;
        free (setobj->addinfo);
        setobj->addinfo = NULL;
        addinfo = zrs->u.nonSurrogateDiagnostic->addinfo;
        if (addinfo && (setobj->addinfo = malloc (strlen(addinfo) + 1)))
            strcpy (setobj->addinfo, addinfo);
        printf ("Diagnostic response. %s (%d): %s\n",
                diagbib1_str (setobj->condition),
                setobj->condition,
                setobj->addinfo ? setobj->addinfo : "");
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

static void ir_searchResponse (void *o, Z_SearchResponse *searchrs)
{    
    IRObj *p = o;
    IRSetObj *setobj = p->set_child;
    Z_Records *zrs = searchrs->records;

    if (setobj)
    {
        setobj->searchStatus = searchrs->searchStatus ? 1 : 0;
        setobj->resultCount = *searchrs->resultCount;
        printf ("Search response %d, %d hits\n", 
                 setobj->searchStatus, setobj->resultCount);
        if (zrs)
            ir_handleRecords (o, zrs);
        else
            setobj->recordFlag = 0;
    }
    else
        printf ("Search response, no object!\n");
}


static void ir_presentResponse (void *o, Z_PresentResponse *presrs)
{
    IRObj *p = o;
    IRSetObj *setobj = p->set_child;
    Z_Records *zrs = presrs->records;
    
    printf ("Received presentResponse\n");
    if (zrs)
        ir_handleRecords (o, zrs);
    else
    {
        setobj->recordFlag = 0;
        printf ("No records!\n");
    }
}

static void ir_scanResponse (void *o, Z_ScanResponse *scanrs)
{
    IRObj *p = o;
    IRScanObj *scanobj = p->scan_child;
    
    printf ("Received scanResponse\n");

    scanobj->scanStatus = *scanrs->scanStatus;
    printf ("scanStatus=%d\n", scanobj->scanStatus);

    if (scanrs->stepSize)
        scanobj->stepSize = *scanrs->stepSize;
    printf ("stepSize=%d\n", scanobj->stepSize);

    scanobj->numberOfEntriesReturned = *scanrs->numberOfEntriesReturned;
    printf ("numberOfEntriesReturned=%d\n", scanobj->numberOfEntriesReturned);

    if (scanrs->positionOfTerm)
        scanobj->positionOfTerm = *scanrs->positionOfTerm;
    else
        scanobj->positionOfTerm = -1;
    printf ("positionOfTerm=%d\n", scanobj->positionOfTerm);

    free (scanobj->entries);
    scanobj->entries = NULL;
    free (scanobj->nonSurrogateDiagnostics);
    scanobj->nonSurrogateDiagnostics = NULL;

    if (scanrs->entries)
    {
        int i;
	Z_Entry *ze;

        scanobj->entries_flag = 1;
        scanobj->which = scanrs->entries->which;
	switch (scanobj->which)
	{
	case Z_ListEntries_entries:
	    scanobj->num_entries = scanrs->entries->u.entries->num_entries;
	    scanobj->entries = malloc (scanobj->num_entries * 
	                               sizeof(*scanobj->entries));
            for (i=0; i<scanobj->num_entries; i++)
	    {
	        ze = scanrs->entries->u.entries->entries[i];
                scanobj->entries[i].which = ze->which;
		switch (ze->which)
		{
		case Z_Entry_termInfo:
		    if (ze->u.termInfo->term->which == Z_Term_general)
		    {
                        int l = ze->u.termInfo->term->u.general->len;
                        scanobj->entries[i].u.term.buf = malloc (1+l);
			memcpy (scanobj->entries[i].u.term.buf, 
			        ze->u.termInfo->term->u.general->buf,
                                l);
                        scanobj->entries[i].u.term.buf[l] = '\0';
		    }
		    else
                        scanobj->entries[i].u.term.buf = NULL;
		    if (ze->u.termInfo->globalOccurrences)
		        scanobj->entries[i].u.term.globalOccurrences = 
		            *ze->u.termInfo->globalOccurrences;
		    else
		        scanobj->entries[i].u.term.globalOccurrences = 0;
                    break;
		case Z_Entry_surrogateDiagnostic:
		    scanobj->entries[i].u.diag.addinfo = 
		            malloc (1+strlen(ze->u.surrogateDiagnostic->
			                     addinfo));
                    strcpy (scanobj->entries[i].u.diag.addinfo,
		            ze->u.surrogateDiagnostic->addinfo);
		    scanobj->entries[i].u.diag.condition = 
		        *ze->u.surrogateDiagnostic->condition;
		    break;
		}
	    }
            break;
	case Z_ListEntries_nonSurrogateDiagnostics:
	    scanobj->num_diagRecs = scanrs->entries->
	                          u.nonSurrogateDiagnostics->num_diagRecs;
	    scanobj->nonSurrogateDiagnostics = malloc (scanobj->num_diagRecs *
	                          sizeof(*scanobj->nonSurrogateDiagnostics));
            break;
	}
    }
    else
        scanobj->entries_flag = 0;
}

/*
 * ir_select_read: handle incoming packages
 */
void ir_select_read (ClientData clientData)
{
    IRObj *p = clientData;
    Z_APDU *apdu;
    int r;

    if (p->connectFlag)
    {
        r = cs_rcvconnect (p->cs_link);
        if (r == 1)
            return;
        p->connectFlag = 0;
        ir_select_remove_write (cs_fileno (p->cs_link), p);
        if (r < 0)
        {
            printf ("cs_rcvconnect error\n");
            if (p->failback)
                Tcl_Eval (p->interp, p->failback);
            do_disconnect (p, NULL, 0, NULL);
            return;
        }
        if (p->callback)
	    Tcl_Eval (p->interp, p->callback);
        return;
    }
    do
    {
        if ((r=cs_get (p->cs_link, &p->buf_in, &p->len_in))  <= 0)
        {
            printf ("cs_get failed\n");
            ir_select_remove (cs_fileno (p->cs_link), p);
            if (p->failback)
                Tcl_Eval (p->interp, p->failback);
            do_disconnect (p, NULL, 0, NULL);
            return;
        }        
        if (r == 1)
            return ;
        odr_setbuf (p->odr_in, p->buf_in, r, 0);
        printf ("cs_get ok, got %d\n", r);
        if (!z_APDU (p->odr_in, &apdu, 0))
        {
            printf ("%s\n", odr_errlist [odr_geterror (p->odr_in)]);
            if (p->failback)
                Tcl_Eval (p->interp, p->failback);
            do_disconnect (p, NULL, 0, NULL);
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
        case Z_APDU_scanResponse:
            ir_scanResponse (p, apdu->u.scanResponse);
            break;
        default:
            printf("Received unknown APDU type (%d).\n", 
                   apdu->which);
            if (p->failback)
                Tcl_Eval (p->interp, p->failback);
            do_disconnect (p, NULL, 0, NULL);
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

    printf ("In write handler.....\n");
    if (p->connectFlag)
    {
        r = cs_rcvconnect (p->cs_link);
        if (r == 1)
            return;
        p->connectFlag = 0;
        if (r < 0)
        {
            printf ("cs_rcvconnect error\n");
            ir_select_remove_write (cs_fileno (p->cs_link), p);
            if (p->failback)
                Tcl_Eval (p->interp, p->failback);
            do_disconnect (p, NULL, 0, NULL);
            return;
        }
        ir_select_remove_write (cs_fileno (p->cs_link), p);
        if (p->callback)
	    Tcl_Eval (p->interp, p->callback);
        return;
    }
    if ((r=cs_put (p->cs_link, p->sbuf, p->slen)) < 0)
    {   
        printf ("select write fail\n");
        if (p->failback)
            Tcl_Eval (p->interp, p->failback);
        do_disconnect (p, NULL, 0, NULL);
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
    Tcl_CreateCommand (interp, "ir-scan", ir_scan_obj_mk,
    		       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    return TCL_OK;
}


