/*
 * NWI - Nordic Web Index 
 * Technical Knowledge Centre & Library of Denmark (DTV)
 *
 * Wais extension to IrTcl
 *
 * $Log: wais-tcl.c,v $
 * Revision 1.3  1996-03-08 16:46:44  adam
 * Doesn't use documentID to determine positions in present-response.
 *
 * Revision 1.2  1996/03/07  12:43:44  adam
 * Better error handling. WAIS target closed before failback is invoked.
 *
 * Revision 1.1  1996/02/29  15:28:08  adam
 * First version of Wais extension to IrTcl.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* YAZ headers ... */
#include <comstack.h>
#include <tcpip.h>
#include <oid.h>

/* IrTcl internal header */
#include <ir-tclp.h>

/* FreeWAIS-sf header */
#include <ui.h>

typedef struct {
    int           position;
    any           *documentID;
    long          score;
    long          documentLength;
    long          lines;
    char          *headline;
    char          *documentText;
} WaisTcl_Record;

typedef struct WaisTcl_Records {
    WaisTcl_Record *record;
    struct WaisTcl_Records *next;
} WaisTcl_Records;

typedef struct {
    IrTcl_Obj     *irtcl_obj;
    Tcl_Interp    *interp;
    int           ref_count;
    COMSTACK      wais_link;
    char          *hostname;
    char          *buf_out;
    int           len_out;
    int           max_out;
    char          *object;
} WaisTcl_Obj;

typedef struct {
    WaisTcl_Obj   *parent;
    IrTcl_SetObj  *irtcl_set_obj;
    Tcl_Interp    *interp;
    WaisTcl_Records *records;
    char          *diag;
    char          *addinfo;
    int           maxDocs;
    int           presentOffset;
} WaisSetTcl_Obj;

static void wais_obj_delete (ClientData clientData);
static void wais_select_notify (ClientData clientData, int r, int w, int e);
static int do_disconnect (void *obj, Tcl_Interp *interp,
                          int argc, char **argv);

/* --- N E T W O R K    I / O ----------------------------------------- */

static void wais_select_write (ClientData clientData)
{
    WaisTcl_Obj *p = clientData;
    int r;
    
    logf (LOG_DEBUG, "Wais write handler fd=%d", cs_fileno(p->wais_link));
    switch (p->irtcl_obj->state)
    {
    case IR_TCL_R_Connecting:
	logf(LOG_DEBUG, "write wais: connect");
        r = cs_rcvconnect (p->wais_link);
        if (r == 1)
            return;
        p->irtcl_obj->state = IR_TCL_R_Idle;
        if (r < 0)
        {
            logf (LOG_DEBUG, "cs_rcvconnect error");
            do_disconnect (p, NULL, 2, NULL);
            p->irtcl_obj->failInfo = IR_TCL_FAIL_CONNECT;
            if (p->irtcl_obj->failback)
                ir_tcl_eval (p->interp, p->irtcl_obj->failback);
            return;
        }
        ir_tcl_select_set (wais_select_notify, cs_fileno(p->wais_link),
                           clientData, 1, 0, 0);
        if (p->irtcl_obj->callback)
            ir_tcl_eval (p->interp, p->irtcl_obj->callback);
        break;
    case IR_TCL_R_Writing:
        if ((r=cs_put (p->wais_link, p->buf_out, p->len_out)) < 0)
        {
            logf (LOG_DEBUG, "cs_put write fail");
            do_disconnect (p, NULL, 2, NULL);
            if (p->irtcl_obj->failback)
            {
                p->irtcl_obj->failInfo = IR_TCL_FAIL_WRITE;
                ir_tcl_eval (p->interp, p->irtcl_obj->failback);
            }
        }
        else if (r == 0)            /* remove select bit */
        {
            logf(LOG_DEBUG, "Write completed");
            p->irtcl_obj->state = IR_TCL_R_Waiting;
            
            ir_tcl_select_set (wais_select_notify, cs_fileno(p->wais_link),
                               clientData, 1, 0, 0);        
        }
        break;
    default:
        logf (LOG_FATAL|LOG_ERRNO, "Wais read. state=%d", p->irtcl_obj->state);
        abort ();
    }
}

static WaisTcl_Record *wais_lookup_record_pos (WaisSetTcl_Obj *p, int pos)
{
    WaisTcl_Records *recs;

    for (recs = p->records; recs; recs = recs->next)
        if (recs->record->position == pos)
            return recs->record;
    return NULL;
}

static WaisTcl_Record *wais_lookup_record_pos_bf (WaisSetTcl_Obj *p, int pos)
{
    WaisTcl_Record *rec;

    rec = wais_lookup_record_pos (p, pos);
    if (!rec)
    {
        return NULL;
    }
    if (rec->documentText || 
        !p->irtcl_set_obj->recordElements ||
        !*p->irtcl_set_obj->recordElements ||
        strcmp (p->irtcl_set_obj->recordElements, "F"))
        return rec;
    return NULL;
}

static void wais_delete_record (WaisTcl_Record *rec)
{
    freeAny (rec->documentID);
    free (rec->headline);
    free (rec->documentText);
    free (rec);
}

static void wais_delete_records (WaisSetTcl_Obj *p)
{
    WaisTcl_Records *recs, *recs1;

    for (recs = p->records; recs; recs = recs1)
    {
        recs1 = recs->next;
        wais_delete_record (recs->record);
        free (recs);
    }
    p->records = NULL;
}

static void wais_add_record_brief (WaisSetTcl_Obj *p,
                                   int position,
                                   any *documentID,
                                   long score,
                                   long documentLength,
                                   long lines,
                                   char *headline)
{
    WaisTcl_Record *rec;
    WaisTcl_Records *recs;
    
    rec = wais_lookup_record_pos (p, position);
    if (!rec)
    {
        rec = ir_tcl_malloc (sizeof(*rec));

        recs = ir_tcl_malloc (sizeof(*recs));
        recs->record = rec;
        recs->next = p->records;
        p->records = recs;
    }
    else
    {
        freeAny (rec->documentID);
        free (rec->headline);
        if (rec->documentText)
            free (rec->documentText);
    }
    rec->position = position;
    rec->documentID = duplicateAny (documentID);
    rec->score = score;
    rec->documentLength = documentLength;
    rec->lines = lines;
    ir_tcl_strdup (NULL, &rec->headline, headline);
    rec->documentText = NULL;
}

static void wais_add_record_full (WaisSetTcl_Obj *p,
                                  int position, 
                                  any *documentText)
{
    WaisTcl_Record *rec;
    rec = wais_lookup_record_pos (p, position);

    if (!rec)
    {
        logf (LOG_DEBUG, "Adding text. Didn't find corresponding brief");
        return ;
    }
    if (rec->documentText)
        free (rec->documentText);
    rec->documentText = ir_tcl_malloc (documentText->size+1);
    memcpy (rec->documentText, documentText->bytes, documentText->size);
    rec->documentText[documentText->size] = '\0';
    logf (LOG_DEBUG, "Adding text record: \n%.20s", rec->documentText);
}

static void wais_handle_search_response (WaisSetTcl_Obj *p,
                                         SearchResponseAPDU *responseAPDU)
{
    logf (LOG_DEBUG, "- SearchStatus=%d", responseAPDU->SearchStatus);
    logf (LOG_DEBUG, "- ResultCount=%d", responseAPDU->ResultCount);
    logf (LOG_DEBUG, "- NumberOfRecordsReturned=%d",
          responseAPDU->NumberOfRecordsReturned);
    logf (LOG_DEBUG, "- ResultSetStatus=%d", responseAPDU->ResultSetStatus);
    logf (LOG_DEBUG, "- PresentStatus=%d", responseAPDU->PresentStatus);

    if (responseAPDU->DatabaseDiagnosticRecords)
    {
        WAISSearchResponse *ddr = responseAPDU->DatabaseDiagnosticRecords;

        p->irtcl_set_obj->searchStatus = 1;

        p->irtcl_set_obj->nextResultSetPosition =
            responseAPDU->NextResultSetPosition;
        p->irtcl_set_obj->numberOfRecordsReturned =
            responseAPDU->NumberOfRecordsReturned;

        if (!p->irtcl_set_obj->resultCount)
        {
#if 1
            if (responseAPDU->NumberOfRecordsReturned >
                responseAPDU->ResultCount)
                p->irtcl_set_obj->resultCount =
                    responseAPDU->NumberOfRecordsReturned;
            else
#endif
                p->irtcl_set_obj->resultCount =
                    responseAPDU->ResultCount;
        }
        logf (LOG_DEBUG, "resultCount=%d", p->irtcl_set_obj->resultCount);
        free (p->diag);
        p->diag = NULL;
        free (p->addinfo);
        p->addinfo = NULL;
        if (ddr->Diagnostics)
        {
            diagnosticRecord **dr = ddr->Diagnostics;
            if (dr[0])
            {
                logf (LOG_DEBUG, "Diagnostic response. %s : %s",
                      dr[0]->DIAG ? dr[0]->DIAG : "<null>",
                      dr[0]->ADDINFO ? dr[0]->ADDINFO : "<null>");
                ir_tcl_strdup (NULL, &p->diag, dr[0]->DIAG);
                ir_tcl_strdup (NULL, &p->addinfo, dr[0]->ADDINFO);
            }
            else
                logf (LOG_DEBUG, "Diagnostic response");
        }
        if (ddr->DocHeaders)
        {
            int i;
            logf (LOG_DEBUG, "Adding doc header entries");
            for (i = 0; ddr->DocHeaders[i]; i++)
            {
                WAISDocumentHeader *head = ddr->DocHeaders[i];

                logf (LOG_DEBUG, "%4d -->%.*s<--", i+1,
                      head->DocumentID->size, head->DocumentID->bytes);
                wais_add_record_brief (p, i+1, head->DocumentID,
                                       head->Score, head->DocumentLength,
                                       head->Lines, head->Headline);
            }
            logf (LOG_DEBUG, "got %d DBOSD records", i);
        }
        if (ddr->Text)
        {
            int i;
            logf (LOG_DEBUG, "Adding text entries");
            for (i = 0; ddr->Text[i]; i++)
            {
                logf (LOG_DEBUG, " size=%d", ddr->Text[i]->DocumentID->size);
#if 0
                logf (LOG_DEBUG, "-->%.*s<--",
                      ddr->Text[i]->DocumentID->size,
                      ddr->Text[i]->DocumentID->bytes);
#endif
                wais_add_record_full (p,
                                      p->presentOffset + i,
                                      ddr->Text[i]->DocumentText);
            }
        }
        freeWAISSearchResponse (ddr);
    }
    else
    {
        logf (LOG_DEBUG, "No records!");
    }
    freeSearchResponseAPDU (responseAPDU);
}


static void wais_select_read (ClientData clientData)
{
    SearchResponseAPDU *searchRAPDU;
    ClientData objectClientData;
    WaisTcl_Obj *p = clientData;
    char *pdup;
    int r;

    logf (LOG_DEBUG, "Wais read handler fd=%d", cs_fileno(p->wais_link));
    do
    {
        /* signal one more use of ir object - callbacks must not
           release the ir memory (p pointer) */
        p->irtcl_obj->state = IR_TCL_R_Reading;

        /* read incoming APDU */
        if ((r=cs_get (p->wais_link, &p->irtcl_obj->buf_in,
                       &p->irtcl_obj->len_in)) <= 0)
        {
            p->ref_count = 2;
            logf (LOG_DEBUG, "cs_get failed, code %d", r);
            do_disconnect (p, NULL, 2, NULL);
            p->irtcl_obj->failInfo = IR_TCL_FAIL_READ;
            if (p->irtcl_obj->failback)
                ir_tcl_eval (p->interp, p->irtcl_obj->failback);
            /* release wais object now if callback deleted it */
            wais_obj_delete (p);
            return;
        }        
        if (r == 1)
	{
	    logf(LOG_DEBUG, "PDU Fraction read");
            return ;
	}
        logf (LOG_DEBUG, "cs_get ok, total size %d", r);
        /* got complete APDU. Now decode */

        p->ref_count = 2;
        /* determine set/ir object corresponding to response */
        objectClientData = 0;
        if (p->object)
        {
            Tcl_CmdInfo cmd_info;
            
            if (Tcl_GetCommandInfo (p->interp, p->object, &cmd_info))
                objectClientData = cmd_info.clientData;
            free (p->object);
            p->object = NULL;
        }
        pdup = p->irtcl_obj->buf_in + HEADER_LENGTH;
        switch (peekPDUType (pdup))
        {
        case initResponseAPDU:
            p->irtcl_obj->eventType = "init";
            logf (LOG_DEBUG, "Got Wais Init response");
            break;
        case searchResponseAPDU:
            p->irtcl_obj->eventType = "search";
            logf (LOG_DEBUG, "Got Wais Search response");
            
            readSearchResponseAPDU (&searchRAPDU, pdup);
            if (!searchRAPDU)
            {
                logf (LOG_WARN, "Couldn't decode Wais search APDU",
                      peekPDUType (pdup));
                p->irtcl_obj->failInfo = IR_TCL_FAIL_IN_APDU;
                do_disconnect (p, NULL, 2, NULL);
                if (p->irtcl_obj->failback)
                    ir_tcl_eval (p->interp, p->irtcl_obj->failback);
                wais_obj_delete (p);
                return ;
            }
            if (objectClientData)
                wais_handle_search_response (objectClientData, searchRAPDU);
            break;
        default:
            logf (LOG_WARN, "Received unknown Wais APDU type %d",
                  peekPDUType (pdup));
            do_disconnect (p, NULL, 2, NULL);
            p->irtcl_obj->failInfo = IR_TCL_FAIL_UNKNOWN_APDU;
            if (p->irtcl_obj->failback)
                ir_tcl_eval (p->interp, p->irtcl_obj->failback);
            wais_obj_delete (p);
            return ;
        }
        p->irtcl_obj->state = IR_TCL_R_Idle;
        
        if (p->irtcl_obj->callback)
            ir_tcl_eval (p->interp, p->irtcl_obj->callback);
        if (p->ref_count == 1)
        {
            wais_obj_delete (p);
            return;
        }
        --(p->ref_count);
    } while (p->wais_link && cs_more (p->wais_link));
}

static void wais_select_notify (ClientData clientData, int r, int w, int e)
{
    if (w)
        wais_select_write (clientData);
    if (r)
        wais_select_read (clientData);
}

static int wais_send_apdu (Tcl_Interp *interp, WaisTcl_Obj *p,
                           const char *msg, const char *object)
{
    int r;

    if (p->object)
    {
        logf (LOG_DEBUG, "Cannot send. object=%s", p->object);
        return TCL_ERROR;
    }
    r = cs_put (p->wais_link, p->buf_out, p->len_out);
    if (r < 0)
    {
        p->irtcl_obj->state = IR_TCL_R_Idle;
        p->irtcl_obj->failInfo = IR_TCL_FAIL_WRITE;
        do_disconnect (p, NULL, 2, NULL);
        if (p->irtcl_obj->failback)
        {
            ir_tcl_eval (p->interp, p->irtcl_obj->failback);
            return TCL_OK;
        }
        else
        {
            interp->result = "Write failed when sending Wais PDU";
            return TCL_ERROR;
        }
    }
    ir_tcl_strdup (NULL, &p->object, object);
    if (r == 1)
    {
        ir_tcl_select_set (wais_select_notify, cs_fileno(p->wais_link),
                           p, 1, 1, 0);
        logf (LOG_DEBUG, "Send part of wais %s APDU", msg);
        p->irtcl_obj->state = IR_TCL_R_Writing;
    }
    else
    {
        logf (LOG_DEBUG, "Send %s (%d bytes) fd=%d", msg, p->len_out,
              cs_fileno(p->wais_link));
        p->irtcl_obj->state = IR_TCL_R_Waiting;
    }
    return TCL_OK;
}

/* --- A S S O C I A T I O N S ----------------------------------------- */

static int do_connect (void *obj, Tcl_Interp *interp,
                       int argc, char **argv)
{
    void *addr;
    WaisTcl_Obj *p = obj;
    int r;

    if (argc <= 0)
        return TCL_OK;
    else if (argc == 2)
    {
        Tcl_AppendResult (interp, p->hostname, NULL);
        return TCL_OK;
    }
    if (p->hostname)
    {
        interp->result = "already connected";
        return TCL_ERROR;
    }
    if (strcmp (p->irtcl_obj->comstackType, "wais"))
    {
        interp->result = "only wais comstack supported";
        return TCL_ERROR;
    }
    p->wais_link = cs_create (tcpip_type, 0, PROTO_WAIS);
    addr = tcpip_strtoaddr (argv[2]);
    if (!addr)
    {
        interp->result = "tcpip_strtoaddr fail";
        return TCL_ERROR;
    }
    logf (LOG_DEBUG, "tcp/ip wais connect %s", argv[2]);

    if (ir_tcl_strdup (interp, &p->hostname, argv[2]) == TCL_ERROR)
        return TCL_ERROR;
    r = cs_connect (p->wais_link, addr);
    logf(LOG_DEBUG, "cs_connect returned %d fd=%d", r,
         cs_fileno(p->wais_link));
    if (r < 0)
    {
        interp->result = "wais connect fail";
        do_disconnect (p, NULL, 2, NULL);
        return TCL_ERROR;
    }
    p->irtcl_obj->eventType = "connect";
    if (r == 1)
    {
        p->irtcl_obj->state = IR_TCL_R_Connecting;
        ir_tcl_select_set (wais_select_notify, cs_fileno(p->wais_link),
                           p, 1, 1, 0);
    }
    else
    {
        p->irtcl_obj->state = IR_TCL_R_Idle;
        ir_tcl_select_set (wais_select_notify, cs_fileno(p->wais_link),
                           p, 1, 0, 0);
        if (p->irtcl_obj->callback)
            ir_tcl_eval (p->interp, p->irtcl_obj->callback);
    }
    return TCL_OK;
}

static int do_disconnect (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    WaisTcl_Obj *p = obj;
    
    if (argc == 0)
    {
        p->wais_link = NULL;
        p->hostname = NULL;
        p->object = NULL;
        return TCL_OK;
    }
    if (p->hostname)
    {
        ir_tcl_select_set (NULL, cs_fileno(p->wais_link), NULL, 0, 0, 0);

        free (p->hostname);
        p->hostname = NULL;
        cs_close (p->wais_link);
        p->wais_link = NULL;
        free (p->object);
        p->object = NULL;
    }
    return TCL_OK;
}

static int do_init (void *obj, Tcl_Interp *interp, int argc, char **argv)
{
    WaisTcl_Obj *p = obj;

    if (argc <= 0)
        return TCL_OK;
    p->irtcl_obj->initResult = 0;
    if (!p->hostname)
    {
        interp->result = "not connected";
        return TCL_ERROR;
    }
    p->irtcl_obj->initResult = 1;
    p->irtcl_obj->eventType = "init";
    if (p->irtcl_obj->callback)
        ir_tcl_eval (p->interp, p->irtcl_obj->callback);
    return TCL_OK;
}

static int do_options (void *obj, Tcl_Interp *interp, int argc, char **argv)
{
    WaisTcl_Obj *p = obj;

    if (argc <= 0)
        return TCL_OK;
    if (argc != 2)
        return TCL_OK;
    Tcl_AppendElement (p->interp, "search");
    Tcl_AppendElement (p->interp, "present");
    return TCL_OK;
}


static IrTcl_Method wais_method_tab[] = {
{ "connect",                     do_connect, NULL },
{ "disconnect",                  do_disconnect, NULL },
{ "init",                        do_init, NULL },
{ "options",                     do_options, NULL },
{ NULL, NULL}
};


int wais_obj_init(ClientData clientData, Tcl_Interp *interp,
                  int argc, char **argv, ClientData *subData,
                  ClientData parentData)
{
    IrTcl_Methods tab[3];
    WaisTcl_Obj *obj;
    ClientData subP;
    int r;
    
    if (argc != 2)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    obj = ir_tcl_malloc (sizeof(*obj));
    obj->ref_count = 1;
    obj->interp = interp;
    
    logf (LOG_DEBUG, "wais object create %s", argv[1]);

    r = (*ir_obj_class.ir_init)(clientData, interp, argc, argv, &subP, 0);
    if (r == TCL_ERROR)
        return TCL_ERROR;
    obj->irtcl_obj = subP;

    obj->max_out = 2048;
    obj->buf_out = ir_tcl_malloc (obj->max_out);

    free (obj->irtcl_obj->comstackType);
    ir_tcl_strdup (NULL, &obj->irtcl_obj->comstackType, "wais");

    tab[0].tab = wais_method_tab;
    tab[0].obj = obj;
    tab[1].tab = NULL;

    if (ir_tcl_method (interp, 0, NULL, tab, NULL) == TCL_ERROR)
    {
        Tcl_AppendResult (interp, "Failed to initialize ", argv[1], NULL);
        /* cleanup missing ... */
        return TCL_ERROR;
    }
    *subData = obj;
    return TCL_OK;
}


/* 
 * wais_obj_delete: Wais Object disposal
 */
static void wais_obj_delete (ClientData clientData)
{
    WaisTcl_Obj *obj = clientData;
    IrTcl_Methods tab[3];

    --(obj->ref_count);
    if (obj->ref_count > 0)
        return;

    logf (LOG_DEBUG, "wais object delete");

    tab[0].tab = wais_method_tab;
    tab[0].obj = obj;
    tab[1].tab = NULL;

    ir_tcl_method (NULL, -1, NULL, tab, NULL);

    (*ir_obj_class.ir_delete)((ClientData) obj->irtcl_obj);

    free (obj->buf_out);
    free (obj);
}

/* 
 * wais_obj_method: Wais Object methods
 */
static int wais_obj_method (ClientData clientData, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_Methods tab[3];
    WaisTcl_Obj *p = clientData;
    int r;

    if (argc < 2)
        return TCL_ERROR;

    tab[0].tab = wais_method_tab;
    tab[0].obj = p;
    tab[1].tab = NULL;

    if (ir_tcl_method (interp, argc, argv, tab, &r) == TCL_ERROR)
    {
        return (*ir_obj_class.ir_method)((ClientData) p->irtcl_obj,
                                         interp, argc, argv);
    }
    return r;
}

/* 
 * wais_obj_mk: Wais Object creation
 */
static int wais_obj_mk (ClientData clientData, Tcl_Interp *interp,
                        int argc, char **argv)
{
    ClientData subData;
    int r = wais_obj_init (clientData, interp, argc, argv, &subData, 0);
    
    if (r == TCL_ERROR)
        return TCL_ERROR;
    Tcl_CreateCommand (interp, argv[1], wais_obj_method,
                       subData, wais_obj_delete);
    return TCL_OK;
}

/* --- S E T S ---------------------------------------------------------- */

static int do_present (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    WaisSetTcl_Obj *obj = o;
    WaisTcl_Obj *p = obj->parent;
    int i, start, number;
    static char *element_names[3];
    long left;
    char *retp;
    any *waisQuery;
    SearchAPDU *waisSearch;
    DocObj **docObjs;
    any refID;
    
    if (argc <= 0)
        return TCL_OK;
    if (argc >= 3)
    {
        if (Tcl_GetInt (interp, argv[2], &start) == TCL_ERROR)
            return TCL_ERROR;
    }
    else
        start = 1;
    obj->presentOffset = start;
    if (argc >= 4)
    {
        if (Tcl_GetInt (interp, argv[3], &number) == TCL_ERROR)
            return TCL_ERROR;
    }
    else 
        number = 10;
    if (!p->wais_link)
    {
        interp->result = "present: not connected";
        return TCL_ERROR;
    }
    element_names[0] = " ";
    element_names[1] = ES_DocumentText;
    element_names[2] = NULL;

    refID.size = 1;
    refID.bytes = "3";

    docObjs = ir_tcl_malloc (sizeof(*docObjs) * (number+1));
    for (i = 0; i<number; i++)
    {
        WaisTcl_Record *rec;

        rec = wais_lookup_record_pos (obj, i+start);
        if (!rec)
        {
            interp->result = "present request out of range";
            return TCL_ERROR;
        }
        docObjs[i] = makeDocObjUsingBytes (rec->documentID, "TEXT", 0,
                                           rec->documentLength);
    }
    docObjs[i] = NULL;
    waisQuery = makeWAISTextQuery (docObjs);
    waisSearch =
        makeSearchAPDU (30L,                          /* small */
                        5000L,                        /* large */
                        30L,                          /* medium */
                        (boolean) obj->irtcl_set_obj->
                        set_inher.replaceIndicator,   /* replace indicator */
                        obj->irtcl_set_obj->
                        setName,                      /* result set name */
                        obj->irtcl_set_obj->set_inher.databaseNames,
                        QT_TextRetrievalQuery,        /* query type */
                        element_names,                /* element name */
                        &refID,                       /* reference ID */
                        waisQuery);

    left = p->max_out;
    retp = writeSearchAPDU (waisSearch, p->buf_out + HEADER_LENGTH, &left);
    p->len_out = p->max_out - left;

    for (i = 0; i<number; i++)
        CSTFreeDocObj (docObjs[i]);
    free (docObjs);

    CSTFreeWAISTextQuery (waisQuery);
    freeSearchAPDU (waisSearch);
    if (!retp)
    {
        interp->result = "Couldn't encode Wais text search APDU";
        return TCL_ERROR;
    }
    writeWAISPacketHeader (p->buf_out, (long) (p->len_out), (long) 'z', "wais",
                           (long) NO_COMPRESSION,
                           (long) NO_ENCODING,
                           (long) HEADER_VERSION);

    p->len_out += HEADER_LENGTH;
    return wais_send_apdu (interp, p, "search", argv[0]);
}

static int do_search (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    WaisSetTcl_Obj *obj = o;
    WaisTcl_Obj *p = obj->parent;
    WAISSearch *waisQuery;
    SearchAPDU *waisSearch;
    char *retp;
    long left;
    DocObj **docObjs = NULL;

    if (argc <= 0)
        return TCL_OK;
    if (argc < 3 || argc > 4)
    {
        interp->result = "wrong # args";
        return TCL_ERROR;
    }
    obj->presentOffset = 1;
    if (argc == 4)
    {
        docObjs = ir_tcl_malloc (2 * sizeof(*docObjs));

        docObjs[0] = ir_tcl_malloc (sizeof(**docObjs));
        docObjs[0]->DocumentID = stringToAny (argv[3]);
        docObjs[0]->Type = NULL;
        docObjs[0]->ChunkCode = (long) CT_document;

        docObjs[1] = NULL;
    }
    if (!obj->irtcl_set_obj->set_inher.num_databaseNames)
    {
        interp->result = "no databaseNames";
        return TCL_ERROR;
    }
    logf (LOG_DEBUG, "parent = %p", p);
    if (!p->hostname)
    {
        interp->result = "not connected";
        return TCL_ERROR;
    }
    obj->irtcl_set_obj->resultCount = 0;
    obj->irtcl_set_obj->searchStatus = 0;
    waisQuery = 
        makeWAISSearch (argv[2],         /* seed words */
                        docObjs,         /* doc ptrs */
                        0,               /* text list */
                        1L,              /* date factor */
                        0L,              /* begin date range */
                        0L,              /* end date range */
                        obj->maxDocs);   /* max docs retrieved */

    waisSearch =
        makeSearchAPDU (30L,                          /* small */
                        5000L,                        /* large */
                        30L,                          /* medium */
                        (boolean) obj->irtcl_set_obj->
                        set_inher.replaceIndicator,   /* replace indicator */
                        obj->irtcl_set_obj->
                        setName,                      /* result set name */
                        obj->irtcl_set_obj->set_inher.databaseNames,
                        QT_RelevanceFeedbackQuery,
                                                     /* query type */
                        NULL,                         /* element name */
                        NULL,                         /* reference ID */
                        waisQuery);

    left = p->max_out;
    retp = writeSearchAPDU (waisSearch, p->buf_out + HEADER_LENGTH, &left);
    p->len_out = p->max_out - left;

    CSTFreeWAISSearch (waisQuery);
    freeSearchAPDU (waisSearch);
    if (docObjs)
    {
        CSTFreeDocObj (docObjs[0]);
        free (docObjs);
    }
    if (!retp)
    {
        interp->result = "Couldn't encode Wais search APDU";
        return TCL_ERROR;
    }
    writeWAISPacketHeader (p->buf_out, (long) (p->len_out), (long) 'z', "wais",
                           (long) NO_COMPRESSION,
                           (long) NO_ENCODING,
                           (long) HEADER_VERSION);

    p->len_out += HEADER_LENGTH;
    return wais_send_apdu (interp, p, "search", argv[0]);
}

/*
 * do_responseStatus: Return response status (present or search)
 */
static int do_responseStatus (void *o, Tcl_Interp *interp, 
                             int argc, char **argv)
{
    WaisSetTcl_Obj *obj = o;

    if (argc == 0)
    {
        obj->diag = NULL;
        obj->addinfo = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
    {
        free (obj->diag);
        free (obj->addinfo);
    }
    if (obj->diag)
    {
        Tcl_AppendElement (interp, "NSD");

        Tcl_AppendElement (interp, obj->diag);
        Tcl_AppendElement (interp, obj->diag);
        
        Tcl_AppendElement (interp, obj->addinfo ? obj->addinfo : "");
        return TCL_OK;
    }
    Tcl_AppendElement (interp, "DBOSD");
    return TCL_OK; 
}

/*
 * do_maxDocs: Set number of documents to be retrieved in ranked query
 */
static int do_maxDocs (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    WaisSetTcl_Obj *obj = o;

    if (argc <= 0)
    {
        obj->maxDocs = 100;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&obj->maxDocs, interp, argc, argv);
}


/*
 * do_type: Return type (if any) at position.
 */
static int do_type (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    WaisSetTcl_Obj *obj = o;
    int offset;
    WaisTcl_Record *rec;

    if (argc == 0)
    {
        obj->records = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
    {
        wais_delete_records (obj);
        return TCL_OK;
    }
    if (argc != 3)
    {
        sprintf (interp->result, "wrong # args");
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;
    rec = wais_lookup_record_pos_bf (obj, offset);
    if (!rec)
    {
        logf (LOG_DEBUG, "No record at position %d", offset);
        return TCL_OK;
    }
    interp->result = "DB";
    return TCL_OK;
}


/*
 * do_recordType: Return record type (if any) at position.
 */
static int do_recordType (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    WaisSetTcl_Obj *obj = o;
    int offset;
    WaisTcl_Record *rec;

    if (argc <= 0)
    {
        return TCL_OK;
    }
    if (argc != 3)
    {
        sprintf (interp->result, "wrong # args");
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;

    rec = wais_lookup_record_pos_bf (obj, offset);
    if (!rec)
        return TCL_OK;

    Tcl_AppendElement (interp, "WAIS");
    return TCL_OK;
}

/*
 * do_getWAIS: Return WAIS record at position.
 */
static int do_getWAIS (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    WaisSetTcl_Obj *obj = o;
    int offset;
    WaisTcl_Record *rec;
    char prbuf[1024];

    if (argc <= 0)
    {
        return TCL_OK;
    }
    if (argc != 4)
    {
        sprintf (interp->result, "wrong # args: should be"
                 " \"assoc getWAIS pos field\"\n"
                 " field is one of:\n"
                 " score headline documentLength text lines documentID");
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;
    rec = wais_lookup_record_pos_bf (obj, offset);
    if (!rec)
        return TCL_OK;
    if (!strcmp (argv[3], "score"))
    {
        sprintf (prbuf, "%ld", (long) rec->score);
        Tcl_AppendElement (interp, prbuf);
    }
    else if (!strcmp (argv[3], "headline"))
    {
        Tcl_AppendElement (interp, rec->headline);
    }
    else if (!strcmp (argv[3], "documentLength"))
    {
        sprintf (prbuf, "%ld", (long) rec->documentLength);
        Tcl_AppendElement (interp, prbuf);
    }
    else if (!strcmp (argv[3], "text"))
    {
        Tcl_AppendElement (interp, rec->documentText);
    }
    else if (!strcmp (argv[3], "lines"))
    {
        sprintf (prbuf, "%ld", (long) rec->lines);
        Tcl_AppendElement (interp, prbuf);
    }
    else if (!strcmp (argv[3], "documentID"))
    {
        if (rec->documentID->size >= sizeof(prbuf))
        {
            interp->result = "bad documentID";
            return TCL_ERROR;
        }
        memcpy (prbuf, rec->documentID->bytes, rec->documentID->size);
        prbuf[rec->documentID->size] = '\0';
        Tcl_AppendElement (interp, prbuf);
    }
    return TCL_OK;
}


static IrTcl_Method wais_set_method_tab[] = {
{ "maxDocs",                     do_maxDocs, NULL },
{ "search",                      do_search, NULL },
{ "present",                     do_present, NULL },
{ "responseStatus",              do_responseStatus, NULL },
{ "type",                        do_type, NULL },
{ "recordType",                  do_recordType, NULL },
{ "getWAIS",                     do_getWAIS, NULL },
{ NULL, NULL}
};

/* 
 * wais_obj_method: Wais Set Object methods
 */
static int wais_set_obj_method (ClientData clientData, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_Methods tab[3];
    WaisSetTcl_Obj *p = clientData;
    int r;

    if (argc < 2)
        return TCL_ERROR;

    tab[0].tab = wais_set_method_tab;
    tab[0].obj = p;
    tab[1].tab = NULL;

    if (ir_tcl_method (interp, argc, argv, tab, &r) == TCL_ERROR)
    {
        return (*ir_set_obj_class.ir_method)((ClientData) p->irtcl_set_obj,
                                             interp, argc, argv);
    }
    return r;
}

int wais_set_obj_init (ClientData clientData, Tcl_Interp *interp,
                       int argc, char **argv, ClientData *subData,
                       ClientData parentData)
{
    IrTcl_Methods tab[3];
    WaisSetTcl_Obj *obj;
    ClientData subP;
    int r;
    
    assert (parentData);
    if (argc != 3)
        return TCL_ERROR;
    obj = ir_tcl_malloc (sizeof(*obj));
    obj->parent = (WaisTcl_Obj *) parentData;
    logf (LOG_DEBUG, "parent = %p", obj->parent);
    obj->interp = interp;
    obj->diag = NULL;
    obj->addinfo = NULL;
    
    logf (LOG_DEBUG, "wais set object create %s", argv[1]);

    r = (*ir_set_obj_class.ir_init)(clientData, interp, argc, argv, &subP,
                                    obj->parent->irtcl_obj);
    if (r == TCL_ERROR)
        return TCL_ERROR;
    obj->irtcl_set_obj = subP;

    tab[0].tab = wais_set_method_tab;
    tab[0].obj = obj;
    tab[1].tab = NULL;

    if (ir_tcl_method (interp, 0, NULL, tab, NULL) == TCL_ERROR)
    {
        Tcl_AppendResult (interp, "Failed to initialize ", argv[1], NULL);
        /* cleanup missing ... */
        return TCL_ERROR;
    }
    *subData = obj;
    return TCL_OK;
}


/* 
 * wais_set_obj_delete: Wais Set Object disposal
 */
static void wais_set_obj_delete (ClientData clientData)
{
    WaisSetTcl_Obj *obj = clientData;
    IrTcl_Methods tab[3];

    logf (LOG_DEBUG, "wais set object delete");

    tab[0].tab = wais_set_method_tab;
    tab[0].obj = obj;
    tab[1].tab = NULL;

    ir_tcl_method (NULL, -1, NULL, tab, NULL);

    (*ir_set_obj_class.ir_delete)((ClientData) obj->irtcl_set_obj);

    free (obj);
}

/*
 * wais_set_obj_mk: Wais Set Object creation
 */
static int wais_set_obj_mk (ClientData clientData, Tcl_Interp *interp,
                            int argc, char **argv)
{
    int r;
    ClientData subData;
    Tcl_CmdInfo parent_info;

    if (argc != 3)
    {
        interp->result = "wrong # args: should be"
            " \"wais-set set assoc?\"";
        return TCL_ERROR;
    }
    parent_info.clientData = 0;
    if (!Tcl_GetCommandInfo (interp, argv[2], &parent_info))
    {
        interp->result = "No parent";
        return TCL_ERROR;
    }
    r = wais_set_obj_init (clientData, interp, argc, argv, &subData,
                           parent_info.clientData);
    if (r == TCL_ERROR)
        return TCL_ERROR;
    Tcl_CreateCommand (interp, argv[1], wais_set_obj_method,
                       subData, wais_set_obj_delete);
    return TCL_OK;
}


/*
 * do_htmlToken
 */
int do_htmlToken (ClientData clientData, Tcl_Interp *interp,
                  int argc, char **argv)
{
    const char *src;
    char *tmp_buf = NULL;
    int tmp_size = 0;
    int r;
    
    if (argc != 4)
    {
        interp->result = "wrong # args: should be"
            " \"htmlToken var list command\"";
        return TCL_ERROR;
    }
    src = argv[2];
    while (*src)
    {
        const char *src1;

        if (*src == ' ' || *src == '\t' || *src == '\n' ||
            *src == '\r' || *src == '\f')
        {
            src++;
            continue;
        }
        src1 = src + 1;
        if (*src == '<')
        {
            while (*src1 != '>' && *src1 != '\n' ** src1)
                src1++;
            if (*src1 == '>')
                src1++;
        }
        else
        {
            while (*src1 != '<' && *src1)
                src1++;
        }
        if (src1 - src >= tmp_size)
        {
            free (tmp_buf);
            tmp_size = src1 - src + 256;
            tmp_buf = ir_tcl_malloc (tmp_size);
        }
        memcpy (tmp_buf, src, src1 - src);
        tmp_buf[src1-src] = '\0';
        Tcl_SetVar (interp, argv[1], tmp_buf, 0);
        r = Tcl_Eval (interp, argv[3]);
        if (r != TCL_OK && r != TCL_CONTINUE)
            break;
        src = src1;
    }
    if (r == TCL_CONTINUE)
        r = TCL_OK;
    free (tmp_buf);
    return r;
}

/* --- R E G I S T R A T I O N ---------------------------------------- */
/*
 * Waistcl_init: Registration of TCL commands.
 */
int Waistcl_Init (Tcl_Interp *interp)
{
    Tcl_CreateCommand (interp,  "wais", wais_obj_mk, (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp,  "wais-set", wais_set_obj_mk,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "htmlToken", do_htmlToken,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    return TCL_OK;
}

