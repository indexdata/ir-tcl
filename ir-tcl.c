/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995-2003
 * See the file LICENSE for details.
 *
 * $Log: ir-tcl.c,v $
 * Revision 1.128  2005-03-10 13:54:56  adam
 * Remove CCL support for scan
 *
 * Revision 1.127  2004/05/10 08:38:45  adam
 * Do not use obsolete YAZ defines
 *
 * Revision 1.126  2003/11/29 17:24:09  adam
 * Added getXml method (Franck Falcoz)
 *
 * Revision 1.125  2003/04/29 10:51:23  adam
 * Null terminate octet aligned records
 *
 * Revision 1.124  2003/03/05 22:02:47  adam
 * Add Tcl_InitStubs
 *
 * Revision 1.123  2003/03/05 21:21:41  adam
 * APDU log. default largeSetLowerBound changed from 2 to 1
 *
 * Revision 1.122  2003/03/05 18:02:08  adam
 * Fix bug with idAuthentication that didn't work for empty group.
 *
 * Revision 1.121  2003/01/30 13:27:07  adam
 * Changed version to 1.4.1. Added WIN32 version resource.
 * IrTcl ignores unexpected PDU's, rather than die.
 *
 * Revision 1.120  2002/03/20 14:48:54  adam
 * implemented USR.1 SearchResult-1
 *
 * Revision 1.119  2001/12/03 00:31:06  adam
 * Towards 1.4. Configure updates.
 *
 * Revision 1.118  2001/03/27 16:27:21  adam
 * Fixed bug in do_responseStatus.
 *
 * Revision 1.117  2001/03/26 11:39:34  adam
 * Fixed bug in ir_deleteDiags - crash when receiving multiple diags.
 *
 * Revision 1.116  2001/02/09 11:58:04  adam
 * Updated for Tcl8.1 and higher where internal encoding is UTF-8.
 *
 * Revision 1.115  2000/09/13 12:18:49  adam
 * Logging utility patch (YAZ version 1.7).
 *
 * Revision 1.114  1999/05/17 20:37:41  adam
 * Fixed problem with ASN code.
 *
 * Revision 1.113  1999/04/20 10:01:46  adam
 * Modified calls to ODR encoders/decoders (name argument).
 *
 * Revision 1.112  1999/03/22 06:51:34  adam
 * Implemented sort.
 *
 * Revision 1.111  1999/02/11 11:30:09  adam
 * Updated for WIN32.
 *
 * Revision 1.110  1998/10/20 15:15:31  adam
 * Changed scan response handler.
 *
 * Revision 1.109  1998/10/13 21:23:26  adam
 * Fixed searchStatus method.
 *
 * Revision 1.108  1998/10/12 11:48:08  adam
 * Removed printf call.
 *
 * Revision 1.107  1998/06/10 13:00:46  adam
 * Added ir-version command.
 *
 * Revision 1.106  1998/05/20 12:25:35  adam
 * Fixed bug that occurred in rare cases when encoding of incoming
 * records failed.
 *
 * Revision 1.105  1998/04/02 14:31:08  adam
 * This version works with compiled ASN.1 code.
 *
 * Revision 1.104  1998/02/27 14:26:07  adam
 * Changed client so that it still works if target sets numberOfRecords
 * in response to an illegal value.
 *
 * Revision 1.103  1997/11/19 11:22:10  adam
 * Object identifiers can be accessed in GRS-1 records.
 *
 * Revision 1.102  1997/09/17 12:22:40  adam
 * Changed to use YAZ version 1.4. The new comstack utility, cs_straddr,
 * is used.
 *
 * Revision 1.101  1997/09/09 10:19:53  adam
 * New MSV5.0 port with fewer warnings.
 *
 * Revision 1.100  1997/05/01 15:04:05  adam
 * Added ir-log command.
 *
 * Revision 1.99  1997/04/30 07:24:47  adam
 * Spell fix of an error message.
 *
 * Revision 1.98  1997/04/13 18:57:20  adam
 * Better error reporting and aligned with Tcl/Tk style.
 * Rework of notifier code with Tcl_File handles.
 *
 * Revision 1.97  1996/11/14 17:11:07  adam
 * Added Explain documentaion.
 *
 * Revision 1.96  1996/10/08  13:02:50  adam
 * When dealing with records, odr_choice_enable_bias function is used to
 * prevent decoding of externals.
 *
 * Revision 1.95  1996/09/13  10:51:49  adam
 * Bug fix: ir_tcl_select_set called Tcl_GetFile at disconnect.
 *
 * Revision 1.94  1996/08/21  13:32:53  adam
 * Implemented saveFile method and extended loadFile method to work with it.
 *
 * Revision 1.93  1996/08/16  15:07:45  adam
 * First work on Explain.
 *
 * Revision 1.92  1996/08/09  15:33:07  adam
 * Modified the code to use tk4.1/tcl7.5 patch level 1. The time-driven
 * polling is no longer activated on Windows since asynchrounous I/O works
 * better.
 *
 * Revision 1.91  1996/07/03  13:31:11  adam
 * The xmalloc/xfree functions from YAZ are used to manage memory.
 *
 * Revision 1.90  1996/06/27  14:21:00  adam
 * Yet another Windows port.
 *
 * Revision 1.89  1996/06/11  15:27:15  adam
 * Event type set to connect a little earlier in the do_connect function.
 *
 * Revision 1.88  1996/06/03  09:04:22  adam
 * Changed a few logf calls.
 *
 * Revision 1.87  1996/05/29  06:37:51  adam
 * Function ir_tcl_get_grs_r enhanced so that specific elements can be
 * extracted.
 *
 * Revision 1.86  1996/03/20 13:54:04  adam
 * The Tcl_File structure is only manipulated in the Tk-event interface
 * in tkinit.c.
 *
 * Revision 1.85  1996/03/15  11:15:48  adam
 * Modified to use new prototypes for p_query_rpn and p_query_scan.
 *
 * Revision 1.84  1996/03/07  12:42:49  adam
 * Better logging when callback is invoked.
 *
 * Revision 1.83  1996/03/05  09:21:09  adam
 * Bug fix: memory used by GRS records wasn't freed.
 * Rewrote some of the error handling code - the connection is always
 * closed before failback is called.
 * If failback is defined the send APDU methods (init, search, ...) will
 * return OK but invoke failback (as is the case if the write operation
 * fails).
 * Bug fix: ref_count in assoc object could grow if fraction of PDU was
 * read.
 *
 * Revision 1.82  1996/02/29  15:30:21  adam
 * Export of IrTcl functionality to extensions.
 *
 * Revision 1.81  1996/02/26  18:38:32  adam
 * Work on export of set methods.
 *
 * Revision 1.80  1996/02/23  17:31:39  adam
 * More functions made available to the wais tcl extension.
 *
 * Revision 1.79  1996/02/23  13:41:38  adam
 * Work on public access to simple ir class system.
 *
 * Revision 1.78  1996/02/21  10:16:08  adam
 * Simplified select handling. Only one function ir_tcl_select_set has
 * to be externally defined.
 *
 * Revision 1.77  1996/02/20  17:52:58  adam
 * Uses the YAZ oid system to name record syntax object identifiers.
 *
 * Revision 1.76  1996/02/20  16:09:51  adam
 * Bug fix: didn't set element set names stamp correctly on result
 * set records when element set names were set to the empty string.
 *
 * Revision 1.75  1996/02/19  15:41:53  adam
 * Better log messages.
 * Minor improvement of connect method.
 *
 * Revision 1.74  1996/02/05  17:58:03  adam
 * Ported ir-tcl to use the beta releases of tcl7.5/tk4.1.
 *
 * Revision 1.73  1996/01/29  11:35:19  adam
 * Bug fix: cs_type member renamed to comstackType to avoid conflict with
 * cs_type macro defined by YAZ.
 *
 * Revision 1.72  1996/01/19  17:45:34  quinn
 * Added debugging output
 *
 * Revision 1.71  1996/01/19  16:22:38  adam
 * New method: apduDump - returns information about last incoming APDU.
 *
 * Revision 1.70  1996/01/10  09:18:34  adam
 * PDU specific callbacks implemented: initRespnse, searchResponse,
 *  presentResponse and scanResponse.
 * Bug fix in the command line shell (tclmain.c) - discovered on OSF/1.
 *
 * Revision 1.69  1996/01/04  16:12:12  adam
 * Setting PDUType renamed to eventType.
 *
 * Revision 1.68  1996/01/04  11:05:22  adam
 * New setting: PDUType - returns type of last PDU returned from the target.
 * Fixed a bug in configure/Makefile.
 *
 * Revision 1.67  1996/01/03  09:00:51  adam
 * Updated to use new version of Yaz (names changed to avoid C++ conflict).
 *
 * Revision 1.66  1995/11/28  17:26:39  adam
 * Removed Carriage return from ir-tcl.c!
 * Removed misc. debug logs.
 *
 * Revision 1.65  1995/11/28  13:53:00  quinn
 * Windows port.
 *
 * Revision 1.64  1995/11/13  15:39:18  adam
 * Bug fix: {small,medium}SetElementSetNames weren't set correctly.
 * Bug fix: idAuthentication weren't set correctly.
 *
 * Revision 1.63  1995/11/13  09:55:39  adam
 * Multiple records at a position in a result-set with differnt
 * element specs.
 *
 * Revision 1.62  1995/10/18  17:20:33  adam
 * Work on target setup in client.tcl.
 *
 * Revision 1.61  1995/10/18  16:42:42  adam
 * New settings: smallSetElementSetNames and mediumSetElementSetNames.
 *
 * Revision 1.60  1995/10/18  15:43:31  adam
 * In search: mediumSetElementSetNames and smallSetElementSetNames are
 * set to elementSetNames.
 *
 * Revision 1.59  1995/10/17  12:18:58  adam
 * Bug fix: when target connection closed, the connection was not
 * properly reestablished.
 *
 * Revision 1.58  1995/10/16  17:00:55  adam
 * New setting: elementSetNames.
 * Various client improvements. Medium presentation format looks better.
 *
 * Revision 1.57  1995/09/21  13:11:51  adam
 * Support of dynamic loading.
 * Test script uses load command if necessary.
 *
 * Revision 1.56  1995/08/29  15:30:14  adam
 * Work on GRS records.
 *
 * Revision 1.55  1995/08/28  09:43:25  adam
 * Minor changes. configure only searches for yaz beta 3 and versions after
 * that.
 *
 * Revision 1.54  1995/08/24  12:25:16  adam
 * Modified to work with yaz 1.0b3.
 *
 * Revision 1.53  1995/08/04  12:49:26  adam
 * Bug fix: reading uninitialized variable p.
 *
 * Revision 1.52  1995/08/04  11:32:38  adam
 * More work on output queue. Memory related routines moved
 * to mem.c
 *
 * Revision 1.51  1995/08/03  13:22:54  adam
 * Request queue.
 *
 * Revision 1.50  1995/07/20  08:09:49  adam
 * client.tcl: Targets removed from hotTargets list when targets
 *  are removed/modified.
 * ir-tcl.c: More work on triggerResourceControl.
 *
 * Revision 1.49  1995/06/30  12:39:21  adam
 * Bug fix: loadFile didn't set record type.
 * The MARC routines are a little less strict in the interpretation.
 * Script display.tcl replaces the old marc.tcl.
 * New interactive script: shell.tcl.
 *
 * Revision 1.48  1995/06/27  19:03:50  adam
 * Bug fix in do_present in ir-tcl.c: p->set_child member weren't set.
 * nextResultSetPosition used instead of setOffset.
 *
 * Revision 1.47  1995/06/25  10:25:04  adam
 * Working on triggerResourceControl. Description of compile/install
 * procedure moved to ir-tcl.sgml.
 *
 * Revision 1.46  1995/06/22  13:15:06  adam
 * Feature: SUTRS. Setting getSutrs implemented.
 * Work on display formats.
 * Preferred record syntax can be set by the user.
 *
 * Revision 1.45  1995/06/20  08:07:30  adam
 * New setting: failInfo.
 * Working on better cancel mechanism.
 *
 * Revision 1.44  1995/06/19  17:01:20  adam
 * Minor changes.
 *
 * Revision 1.43  1995/06/19  13:06:08  adam
 * New define: IR_TCL_VERSION.
 *
 * Revision 1.42  1995/06/19  08:08:52  adam
 * client.tcl: hotTargets now contain both database and target name.
 * ir-tcl.c: setting protocol edited. Errors in callbacks are logged
 * by logf(LOG_WARN, ...) calls.
 *
 * Revision 1.41  1995/06/16  12:28:16  adam
 * Implemented preferredRecordSyntax.
 * Minor changes in diagnostic handling.
 * Record list deleted when connection closes.
 *
 * Revision 1.40  1995/06/14  13:37:18  adam
 * Setting recordType implemented.
 * Setting implementationVersion implemented.
 * Settings implementationId / implementationName edited.
 *
 * Revision 1.39  1995/06/08  10:26:32  adam
 * Bug fix in ir_strdup.
 *
 * Revision 1.38  1995/06/01  16:36:47  adam
 * About buttons. Minor bug fixes.
 *
 * Revision 1.37  1995/06/01  07:31:20  adam
 * Rename of many typedefs -> IrTcl_...
 *
 * Revision 1.36  1995/05/31  13:09:59  adam
 * Client searches/presents may be interrupted.
 * New moving book-logo.
 *
 * Revision 1.35  1995/05/31  08:36:33  adam
 * Bug fix in client.tcl: didn't save options on clientrc.tcl.
 * New method: referenceId. More work on scan.
 *
 * Revision 1.34  1995/05/29  10:33:42  adam
 * README and rename of startup script.
 *
 * Revision 1.33  1995/05/29  09:15:11  quinn
 * Changed CS_SR to PROTO_SR, etc.
 *
 * Revision 1.32  1995/05/29  08:44:16  adam
 * Work on delete of objects.
 *
 * Revision 1.31  1995/05/26  11:44:10  adam
 * Bugs fixed. More work on MARC utilities and queries. Test
 * client is up-to-date again.
 *
 * Revision 1.30  1995/05/26  08:54:11  adam
 * New MARC utilities. Uses prefix query.
 *
 * Revision 1.29  1995/05/24  14:10:22  adam
 * Work on idAuthentication, protocolVersion and options.
 *
 * Revision 1.28  1995/05/23  15:34:48  adam
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
 * of IrTcl_RecordList manipulations. Full MARC record presentation in
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
#ifdef WIN32

#else
#include <unistd.h>
#endif
#include <time.h>
#include <assert.h>

#define CS_BLOCK 0

#include "ir-tclp.h"

#if defined(__WIN32__)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   undef WIN32_LEAN_AND_MEAN
 
/*
 * VC++ has an alternate entry point called DllMain, so we need to rename
 * our entry point.
 */
 
#   if defined(_MSC_VER)
#     define EXPORT(a,b) __declspec(dllexport) a b
#     define DllEntryPoint DllMain
#   else
#     if defined(__BORLANDC__)
#         define EXPORT(a,b) a _export b
#     else
#         define EXPORT(a,b) a b
#     endif
#   endif
#else
#   define EXPORT(a,b) a b
#endif

static char *wrongArgs = "wrong # args: should be \"";
static FILE *odr_print_file = 0;

static int ir_tcl_error_exec (Tcl_Interp *interp, int argc, char **argv)
{
    int i;
    Tcl_AppendResult (interp, " while executing ", NULL);
    for (i = 0; i<argc; i++)
        Tcl_AppendResult (interp, (i ? " " : "\""), argv[i], NULL);
    Tcl_AppendResult (interp, "\"", NULL);
    return TCL_ERROR;
}


static void ir_deleteDiags (IrTcl_Diagnostic **dst_list, int *dst_num);

static void ir_select_notify (ClientData clientData, int r, int w, int e);

void ir_select_add (int fd, void *obj)
{
    ir_tcl_select_set (ir_select_notify, fd, obj, 1, 0, 0);
}

void ir_select_add_write (int fd, void *obj)
{
    ir_tcl_select_set (ir_select_notify, fd, obj, 1, 1, 0);
}

void ir_select_remove (int fd, void *obj)
{
    ir_tcl_select_set (NULL, fd, obj, 0, 0, 0);
}

void ir_select_remove_write (int fd, void *obj)
{
    ir_tcl_select_set (ir_select_notify, fd, obj, 1, 0, 0);
}

static void delete_IR_record (IrTcl_RecordList *rl)
{
    switch (rl->which)
    {
    case Z_NamePlusRecord_databaseRecord:
        switch (rl->u.dbrec.type)
        {
        case VAL_GRS1:
            ir_tcl_grs_del (&rl->u.dbrec.u.grs1);
            break;
        default:
            break;
        }
        xfree (rl->u.dbrec.buf);
        rl->u.dbrec.buf = NULL;
        break;
    case Z_NamePlusRecord_surrogateDiagnostic:
        ir_deleteDiags (&rl->u.surrogateDiagnostics.list,
                        &rl->u.surrogateDiagnostics.num);
        break;
    }
    xfree (rl->elements);
}

static void purge_IR_records (IrTcl_SetObj *setobj)
{
    IrTcl_RecordList *rl;
    while ((rl = setobj->record_list))
    {
        setobj->record_list = rl->next;
        delete_IR_record (rl);
        xfree (rl);
    } 
}

static IrTcl_RecordList *new_IR_record (IrTcl_SetObj *setobj, 
                                        int no, int which, 
                                        const char *elements)
{
    IrTcl_RecordList *rl;

    if (elements && !*elements)
        elements = NULL;
    for (rl = setobj->record_list; rl; rl = rl->next)
    {
        if (no == rl->no && (!rl->elements || !elements ||
                             !strcmp(elements, rl->elements)))
        {
            delete_IR_record (rl);
            break;
        }
    }
    if (!rl)
    {
        rl = ir_tcl_malloc (sizeof(*rl));
        rl->next = setobj->record_list;
        rl->no = no;
        setobj->record_list = rl;
    }
    rl->which = which;
    ir_tcl_strdup (NULL, &rl->elements, elements);
    return rl;
}

/* 
 * ir_tcl_eval
 */
int ir_tcl_eval (Tcl_Interp *interp, const char *command)
{
    char *tmp = ir_tcl_malloc (strlen(command)+1);
    int r;

    logf (LOG_DEBUG, "Invoking %.23s ...", command);
    strcpy (tmp, command);
    r = Tcl_Eval (interp, tmp);
    if (r == TCL_ERROR)
    {
	const char *errorInfo = Tcl_GetVar (interp, "errorInfo", 0);
        logf (LOG_WARN, "Tcl error in line %d: %s\n%s",
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION <= 5
	      interp->errorLine,
              interp->result,
#else
	      Tcl_GetErrorLine(interp),
              Tcl_GetStringResult(interp),
#endif
	      errorInfo ? errorInfo : "<null>");
    }
    Tcl_FreeResult (interp);
    xfree (tmp);
    return r;
}

/*
 * IrTcl_getRecordSyntaxStr: Return record syntax name of object id
 */
static char *IrTcl_getRecordSyntaxStr (enum oid_value value)
{
    int *o;
    struct oident ent, *entp;

    ent.proto = PROTO_Z3950;
    ent.oclass = CLASS_RECSYN;
    ent.value = value;

    o = oid_getoidbyent (&ent);
    entp = oid_getentbyoid (o);
    
    if (!entp)
        return "";
    return entp->desc;
}

/*
 * IrTcl_getRecordSyntaxVal: Return record syntax value of string
 */
static enum oid_value IrTcl_getRecordSyntaxVal (const char *name)
{
    return oid_getvalbyname (name);
}

static IrTcl_RecordList *find_IR_record (IrTcl_SetObj *setobj, int no)
{
    IrTcl_RecordList *rl;

    for (rl = setobj->record_list; rl; rl = rl->next)
        if (no == rl->no && 
            (!setobj->recordElements || !rl->elements || 
             !strcmp (setobj->recordElements, rl->elements)))
            return rl;
    return NULL;
}

static void delete_IR_records (IrTcl_SetObj *setobj)
{
    IrTcl_RecordList *rl, *rl1;

    for (rl = setobj->record_list; rl; rl = rl1)
    {
        delete_IR_record (rl);
        rl1 = rl->next;
        xfree (rl);
    }
    setobj->record_list = NULL;
}

/*
 * ir_tcl_get_set_int: Set/get integer value
 */
int ir_tcl_get_set_int (int *val, Tcl_Interp *interp, int argc, char **argv)
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
 * ir_tcl_method_error
 */
int ir_tcl_method_error (Tcl_Interp *interp, int argc, char **argv,
                         IrTcl_Methods *tab)
{
    IrTcl_Methods *tab_i = tab;
    IrTcl_Method *t;

    Tcl_AppendResult (interp, "bad method: \"", *argv, " ", argv[1],
                      "\"\nmethod should be  of:", NULL);
    for (tab_i = tab; tab_i->tab; tab_i++)
        for (t = tab_i->tab; t->name; t++)
            Tcl_AppendResult (interp, " ", t->name, NULL);
    return TCL_ERROR;
}

/*
 * ir_tcl_method: Search for method in table and invoke method handler
 */
int ir_tcl_method (Tcl_Interp *interp, int argc, char **argv,
                   IrTcl_Methods *tab, int *ret)
{
    IrTcl_Methods *tab_i = tab;
    IrTcl_Method *t;

    for (tab_i = tab; tab_i->tab; tab_i++)
        for (t = tab_i->tab; t->name; t++)
            if (argc <= 0)
            {
                if ((*t->method)(tab_i->obj, interp, argc, argv) == TCL_ERROR)
                    return TCL_ERROR;
            }
            else
                if (!strcmp (t->name, argv[1]))
                {
                    *ret = (*t->method)(tab_i->obj, interp, argc, argv);
                    return TCL_OK;
                }

    if (argc <= 0)
        return TCL_OK;
    *ret = TCL_ERROR;
    return TCL_ERROR;
}

/*
 *  ir_tcl_named_bits: get/set named bits
 */
int ir_tcl_named_bits (struct ir_named_entry *tab, Odr_bitmask *ob,
                       Tcl_Interp *interp, int argc, char **argv)
{
    struct ir_named_entry *ti;
    if (argc > 0)
    {
        int no;
        ODR_MASK_ZERO (ob);
        for (no = 0; no < argc; no++)
        {
	    int ok = 0;
            for (ti = tab; ti->name; ti++)
                if (!strcmp(argv[no], "@all") || !strcmp (argv[no], ti->name))
                {
                    ODR_MASK_SET (ob, ti->pos);
                    ok = 1;
                }
            if (!ok)
            {
                Tcl_AppendResult (interp, "bad bit mask ", argv[no], NULL);
                return ir_tcl_error_exec (interp, argc, argv);
            }
        }
        return TCL_OK;
    }
    for (ti = tab; ti->name; ti++)
        if (ODR_MASK_GET (ob, ti->pos))
            Tcl_AppendElement (interp, ti->name);
    return TCL_OK;
}

static void set_referenceId (ODR o, Z_ReferenceId **dst, const char *src)
{
    if (!src || !*src)
        *dst = NULL;
    else
    {
        *dst = odr_malloc (o, sizeof(**dst));
        (*dst)->size = (*dst)->len = strlen(src);
        (*dst)->buf = odr_malloc (o, (*dst)->len);
        memcpy ((*dst)->buf, src, (*dst)->len);
    }
}

static void get_referenceId (char **dst, Z_ReferenceId *src)
{
    xfree (*dst);
    if (!src)
    {
        *dst = NULL;
        return;
    }
    *dst = ir_tcl_malloc (src->len+1);
    memcpy (*dst, src->buf, src->len);
    (*dst)[src->len] = '\0';
}

/* ------------------------------------------------------- */

/*
 * do_init_request: init method on IR object
 */
static int do_init_request (void *obj, Tcl_Interp *interp,
                            int argc, char **argv)
{
    Z_APDU *apdu;
    IrTcl_Obj *p = obj;
    Z_InitRequest *req;

    if (argc <= 0)
        return TCL_OK;
    logf (LOG_DEBUG, "init %s", *argv);
    if (!p->cs_link)
    {
        Tcl_AppendResult (interp, "not connected", NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    apdu = zget_APDU (p->odr_out, Z_APDU_initRequest);
    req = apdu->u.initRequest;

    set_referenceId (p->odr_out, &req->referenceId, p->set_inher.referenceId);
    req->options = &p->options;
    req->protocolVersion = &p->protocolVersion;
    req->preferredMessageSize = &p->preferredMessageSize;
    req->maximumRecordSize = &p->maximumRecordSize;

    if (p->idAuthenticationGroupId || p->idAuthenticationUserId)
    {
        Z_IdPass *pass = odr_malloc (p->odr_out, sizeof(*pass));
        Z_IdAuthentication *auth = odr_malloc (p->odr_out, sizeof(*auth));

        logf (LOG_DEBUG, "using pass authentication");

        auth->which = Z_IdAuthentication_idPass;
        auth->u.idPass = pass;
        if (p->idAuthenticationGroupId && *p->idAuthenticationGroupId)
            pass->groupId = p->idAuthenticationGroupId;
        else
            pass->groupId = NULL;
        if (p->idAuthenticationUserId && *p->idAuthenticationUserId)
            pass->userId = p->idAuthenticationUserId;
        else
            pass->userId = NULL;
        if (p->idAuthenticationPassword && *p->idAuthenticationPassword)
            pass->password = p->idAuthenticationPassword;
        else
            pass->password = NULL;
        req->idAuthentication = auth;
    }
    else if (p->idAuthenticationOpen && *p->idAuthenticationOpen)
    {
        Z_IdAuthentication *auth = odr_malloc (p->odr_out, sizeof(*auth));

        logf (LOG_DEBUG, "using open authentication");
        auth->which = Z_IdAuthentication_open;
        auth->u.open = p->idAuthenticationOpen;
        req->idAuthentication = auth;
    }
    else
        req->idAuthentication = NULL;
    req->implementationId = p->implementationId;
    req->implementationName = p->implementationName;
    req->implementationVersion = p->implementationVersion;
    req->userInformationField = 0;

    return ir_tcl_send_APDU (interp, p, apdu, "init", *argv);
}

/*
 * do_protocolVersion: Set protocol Version
 */
static int do_protocolVersion (void *obj, Tcl_Interp *interp,
                               int argc, char **argv)
{
    int version, i;
    char buf[10];
    IrTcl_Obj *p = obj;

    if (argc <= 0)
    {
        ODR_MASK_ZERO (&p->protocolVersion);
        ODR_MASK_SET (&p->protocolVersion, 0);
        ODR_MASK_SET (&p->protocolVersion, 1);
        return TCL_OK;
    }
    if (argc == 3)
    {
        if (Tcl_GetInt (interp, argv[2], &version)==TCL_ERROR)
            return TCL_ERROR;
        ODR_MASK_ZERO (&p->protocolVersion);
        for (i = 0; i<version; i++)
            ODR_MASK_SET (&p->protocolVersion, i);
    }
    for (i = 4; --i >= 0; )
        if (ODR_MASK_GET (&p->protocolVersion, i))
            break;
    sprintf (buf, "%d", i+1);
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION <= 5
    interp->result = buf;
#else
    Tcl_SetResult(interp, buf, TCL_VOLATILE);
#endif
    return TCL_OK;
}

/*
 * do_options: Set options
 */
static int do_options (void *obj, Tcl_Interp *interp,
                       int argc, char **argv)
{
    static struct ir_named_entry options_tab[] = {
    { "search", 0 },
    { "present", 1 },
    { "delSet", 2 },
    { "resourceReport", 3 },
    { "triggerResourceCtrl", 4},
    { "resourceCtrl", 5},
    { "accessCtrl", 6},
    { "scan", 7},
    { "sort", 8},
    { "extendedServices", 10},
    { "level-1Segmentation", 11},
    { "level-2Segmentation", 12},
    { "concurrentOperations", 13},
    { "namedResultSets", 14},
    { NULL, 0}
    };
    IrTcl_Obj *p = obj;

    if (argc <= 0)
    {
        ODR_MASK_ZERO (&p->options);
        ODR_MASK_SET (&p->options, 0);
        ODR_MASK_SET (&p->options, 1);
        ODR_MASK_SET (&p->options, 4);
        ODR_MASK_SET (&p->options, 7);
        ODR_MASK_SET (&p->options, 14);
        return TCL_OK;
    }
    return ir_tcl_named_bits (options_tab, &p->options, interp, argc-2, argv+2);
}

/*
 * do_apduInfo: Get APDU information
 */
static int do_apduInfo (void *obj, Tcl_Interp *interp, int argc, char **argv)
{
    char buf[16];
    FILE *apduf;
    IrTcl_Obj *p = obj;

    if (argc <= 0)
        return TCL_OK;
    sprintf (buf, "%d", p->apduLen);
    Tcl_AppendElement (interp, buf);
    sprintf (buf, "%d", p->apduOffset);
    Tcl_AppendElement (interp, buf);
    if (!p->buf_in)
    {
        Tcl_AppendElement (interp, "");
        return TCL_OK;
    }
    apduf = fopen ("apdu.tmp", "w");
    if (!apduf)
    {
        Tcl_AppendElement (interp, "");
        return TCL_OK;
    }
    odr_dumpBER (apduf, p->buf_in, p->apduLen);
    fclose (apduf);
    if (!(apduf = fopen ("apdu.tmp", "r")))
        Tcl_AppendElement (interp, "");
    else
    {
        int c;
        
        Tcl_AppendResult (interp, " {", NULL);
        while ((c = getc (apduf)) != EOF)
        {
            buf[0] = c;
            buf[1] = '\0';
            Tcl_AppendResult (interp, buf, NULL);
        }
        fclose (apduf);
        Tcl_AppendResult (interp, "}", NULL);
    }
    unlink ("apdu.tmp");
    return TCL_OK;
}

/*
 * do_failInfo: Get fail information
 */
static int do_failInfo (void *obj, Tcl_Interp *interp, int argc, char **argv)
{
    char buf[16], *cp;
    IrTcl_Obj *p = obj;

    if (argc <= 0)
    {
        p->failInfo = 0;
        return TCL_OK;
    }
    sprintf (buf, "%d", p->failInfo);
    switch (p->failInfo)
    {
    case 0:
        cp = "ok";
        break;
    case IR_TCL_FAIL_CONNECT:
        cp = "connect failed";
        break;
    case IR_TCL_FAIL_READ:
        cp = "connection closed";
        break;
    case IR_TCL_FAIL_WRITE:
        cp = "connection closed";
        break;
    case IR_TCL_FAIL_IN_APDU:
        cp = "failed to decode incoming APDU";
        break;
    case IR_TCL_FAIL_UNKNOWN_APDU:
        cp = "unknown APDU";
        break;
    default:
        cp = "";
    } 
    Tcl_AppendElement (interp, buf);
    Tcl_AppendElement (interp, cp);
    return TCL_OK;
}

/*
 * do_preferredMessageSize: Set/get preferred message size
 */
static int do_preferredMessageSize (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc <= 0)
    {
        p->preferredMessageSize = 30000;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->preferredMessageSize, interp, argc, argv);
}

/*
 * do_maximumRecordSize: Set/get maximum record size
 */
static int do_maximumRecordSize (void *obj, Tcl_Interp *interp,
                                 int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc <= 0)
    {
        p->maximumRecordSize = 30000;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->maximumRecordSize, interp, argc, argv);
}

/*
 * do_initResult: Get init result
 */
static int do_initResult (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IrTcl_Obj *p = obj;
   
    if (argc <= 0)
        return TCL_OK;
    return ir_tcl_get_set_int (&p->initResult, interp, argc, argv);
}


/*
 * do_implementationName: Set/get Implementation Name.
 */
static int do_implementationName (void *obj, Tcl_Interp *interp,
                                    int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
        return ir_tcl_strdup (interp, &p->implementationName,
                          "IrTcl/YAZ");
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->implementationName);
    if (argc == 3)
    {
        xfree (p->implementationName);
        if (ir_tcl_strdup (interp, &p->implementationName, argv[2])
            == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->implementationName, (char*) NULL);
    return TCL_OK;
}

/*
 * do_implementationId: Get Implementation Id.
 */
static int do_implementationId (void *obj, Tcl_Interp *interp,
                                int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
        return ir_tcl_strdup (interp, &p->implementationId, "81");
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->implementationId);
    Tcl_AppendResult (interp, p->implementationId, (char*) NULL);
    return TCL_OK;
}

/*
 * do_implementationVersion: get Implementation Version.
 */
static int do_implementationVersion (void *obj, Tcl_Interp *interp,
                                     int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
        return ir_tcl_strdup (interp, &p->implementationVersion, 
#ifdef IR_TCL_VERSION
                          IR_TCL_VERSION "/"
#endif
                          YAZ_VERSION
                          );
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->implementationVersion);
    Tcl_AppendResult (interp, p->implementationVersion, (char*) NULL);
    return TCL_OK;
}

/*
 * do_targetImplementationName: Get Implementation Name of target.
 */
static int do_targetImplementationName (void *obj, Tcl_Interp *interp,
                                        int argc, char **argv)
{
    IrTcl_Obj *p = obj;
    
    if (argc == 0)
    {
        p->targetImplementationName = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->targetImplementationName);
    Tcl_AppendResult (interp, p->targetImplementationName, (char*) NULL);
    return TCL_OK;
}

/*
 * do_targetImplementationId: Get Implementation Id of target
 */
static int do_targetImplementationId (void *obj, Tcl_Interp *interp,
                                      int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
    {
        p->targetImplementationId = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->targetImplementationId);
    Tcl_AppendResult (interp, p->targetImplementationId, (char*) NULL);
    return TCL_OK;
}

/*
 * do_targetImplementationVersion: Get Implementation Version of target
 */
static int do_targetImplementationVersion (void *obj, Tcl_Interp *interp,
                                           int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
    {
        p->targetImplementationVersion = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->targetImplementationVersion);
    Tcl_AppendResult (interp, p->targetImplementationVersion, (char*) NULL);
    return TCL_OK;
}

/*
 * do_idAuthentication: Set/get id Authentication
 */
static int do_idAuthentication (void *obj, Tcl_Interp *interp,
                                int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc >= 3 || argc == -1)
    {
        xfree (p->idAuthenticationOpen);
        xfree (p->idAuthenticationGroupId);
        xfree (p->idAuthenticationUserId);
        xfree (p->idAuthenticationPassword);
    }
    if (argc >= 3 || argc <= 0)
    {
        p->idAuthenticationOpen = NULL;
        p->idAuthenticationGroupId = NULL;
        p->idAuthenticationUserId = NULL;
        p->idAuthenticationPassword = NULL;
    }
    if (argc <= 0)
        return TCL_OK;
    if (argc >= 3)
    {
        if (argc == 3)
        {
            xfree (p->idAuthenticationGroupId);
            xfree (p->idAuthenticationUserId);
            xfree (p->idAuthenticationPassword);
            p->idAuthenticationGroupId = NULL;
            p->idAuthenticationUserId = NULL;
            p->idAuthenticationPassword = NULL;
            if (argv[2][0] && 
                ir_tcl_strdup (interp, &p->idAuthenticationOpen, argv[2])
                == TCL_ERROR)
                return TCL_ERROR;
        }
        else if (argc == 5)
        {
            xfree (p->idAuthenticationOpen);
            p->idAuthenticationOpen = NULL;
            if (argv[2][0] && 
                ir_tcl_strdup (interp, &p->idAuthenticationGroupId, argv[2])
                == TCL_ERROR)
                return TCL_ERROR;
            if (argv[3][0] && 
                ir_tcl_strdup (interp, &p->idAuthenticationUserId, argv[3])
                == TCL_ERROR)
                return TCL_ERROR;
            if (argv[4][0] &&
                ir_tcl_strdup (interp, &p->idAuthenticationPassword, argv[4])
                == TCL_ERROR)
                return TCL_ERROR;
        }
    }
    if (p->idAuthenticationOpen)
        Tcl_AppendElement (interp, p->idAuthenticationOpen);
    else if (p->idAuthenticationGroupId || p->idAuthenticationUserId)
    {
        Tcl_AppendElement (interp, p->idAuthenticationGroupId);
        Tcl_AppendElement (interp, p->idAuthenticationUserId);
        Tcl_AppendElement (interp, p->idAuthenticationPassword);
    }
    return TCL_OK;
}

/*
 * do_connect: connect method on IR object
 */
static int do_connect (void *obj, Tcl_Interp *interp,
                       int argc, char **argv)
{
    void *addr;
    IrTcl_Obj *p = obj;
    int r;

    if (argc <= 0)
        return TCL_OK;
    if (argc > 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " ?hostname?\"", NULL);
        return TCL_ERROR;
    }
    else if (argc < 3)
    {
        Tcl_AppendResult (interp, p->hostname, NULL);
    }
    else
    {
        logf (LOG_DEBUG, "connect %s %s", *argv, argv[2]);
        if (p->hostname)
        {
            Tcl_AppendResult (interp, "already connected", NULL);
            return ir_tcl_error_exec (interp, argc, argv);
        }
        if (!strcmp (p->comstackType, "tcpip"))
        {
            p->cs_link = cs_create (tcpip_type, CS_BLOCK, p->protocol_type);
            logf (LOG_DEBUG, "tcp/ip connect %s", argv[2]);
        }
        else if (!strcmp (p->comstackType, "mosi"))
        {
#if MOSI
            p->cs_link = cs_create (mosi_type, CS_BLOCK, p->protocol_type);
            logf (LOG_DEBUG, "mosi connect %s", argv[2]);
#else
            Tcl_AppendResult (interp, "mosi not supported", NULL);
            return ir_tcl_error_exec (interp, argc, argv);
#endif
        }
        else 
        {
            Tcl_AppendResult (interp, "bad comstack type ", 
                              p->comstackType, NULL);
            return ir_tcl_error_exec (interp, argc, argv);
        }
        if (ir_tcl_strdup (interp, &p->hostname, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
        p->eventType = "connect";
	addr = cs_straddr (p->cs_link, argv[2]);
	if (!addr)
	{
	    ir_tcl_disconnect (p);
	    Tcl_AppendResult (interp, "cs_straddr fail", NULL);
	    return ir_tcl_error_exec (interp, argc, argv);
	}
        if ((r=cs_connect (p->cs_link, addr)) < 0)
        {
            ir_tcl_disconnect (p);
            Tcl_AppendResult (interp, "connect fail", NULL);
            return ir_tcl_error_exec (interp, argc, argv);
        }
        ir_select_add (cs_fileno (p->cs_link), p);
        if (r == 1)
        {
            logf (LOG_DEBUG, "connect pending fd=%d", cs_fileno(p->cs_link));
            ir_select_add_write (cs_fileno (p->cs_link), p);
            p->state = IR_TCL_R_Connecting;
        }
        else
        {
            logf (LOG_DEBUG, "connect ok fd=%d", cs_fileno(p->cs_link));
            p->state = IR_TCL_R_Idle;
            if (p->callback)
                ir_tcl_eval (p->interp, p->callback);
        }
    }
    return TCL_OK;
}

/* 
 * ir_tcl_disconnect: close connection
 */
void ir_tcl_disconnect (IrTcl_Obj *p)
{
    if (p->hostname)
    {
        logf(LOG_DEBUG, "Closing connection to %s", p->hostname);
        xfree (p->hostname);
        p->hostname = NULL;
        assert (p->cs_link);
        ir_select_remove (cs_fileno (p->cs_link), p);

        odr_reset (p->odr_in);

#if TCL_MAJOR_VERSION == 8
	cs_fileno(p->cs_link) = -1;
#endif
        cs_close (p->cs_link);
        p->cs_link = NULL;

        ODR_MASK_ZERO (&p->options);
        ODR_MASK_SET (&p->options, 0);
        ODR_MASK_SET (&p->options, 1);
        ODR_MASK_SET (&p->options, 4);
        ODR_MASK_SET (&p->options, 7);
        ODR_MASK_SET (&p->options, 14);

        ODR_MASK_ZERO (&p->protocolVersion);
        ODR_MASK_SET (&p->protocolVersion, 0);
        ODR_MASK_SET (&p->protocolVersion, 1);
        ir_tcl_del_q (p);
    }
    assert (!p->cs_link);
}

/*
 * do_disconnect: disconnect method on IR object
 */
static int do_disconnect (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
    {
        p->state = IR_TCL_R_Idle;
        p->eventType = NULL;
        p->hostname = NULL;
        p->cs_link = NULL;
        return TCL_OK;
    }
    ir_tcl_disconnect (p);
    return TCL_OK;
}

/*
 * do_comstack: Set/get comstack method on IR object
 */
static int do_comstack (void *o, Tcl_Interp *interp,
                        int argc, char **argv)
{
    IrTcl_Obj *obj = o;

    if (argc == 0)
        return ir_tcl_strdup (interp, &obj->comstackType, "tcpip");
    else if (argc == -1)
        return ir_tcl_strdel (interp, &obj->comstackType);
    else if (argc == 3)
    {
        xfree (obj->comstackType);
        if (ir_tcl_strdup (interp, &obj->comstackType, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendElement (interp, obj->comstackType);
    return TCL_OK;
}

/*
 * do_logLevel: Set log level
 */
static int do_logLevel (void *o, Tcl_Interp *interp,
                        int argc, char **argv)
{
    if (argc <= 2)
        return TCL_OK;
    if (argc == 3)
        yaz_log_init (yaz_log_mask_str (argv[2]), "", NULL);
    else if (argc == 4)
        yaz_log_init (yaz_log_mask_str (argv[2]), argv[3], NULL);
    else if (argc == 5)
        yaz_log_init (yaz_log_mask_str (argv[2]), argv[3], argv[4]);
    return TCL_OK;
}


/*
 * do_eventType: Return type of last event
 */
static int do_eventType (void *obj, Tcl_Interp *interp,
                         int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc <= 0)
    {
        p->eventType = NULL;
        return TCL_OK;
    }
    Tcl_AppendElement (interp, p->eventType ? p->eventType : "");
    return TCL_OK;
}


/*
 * do_callback: add callback
 */
static int do_callback (void *obj, Tcl_Interp *interp,
                        int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
    {
        p->callback = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->callback);
    if (argc == 3)
    {
        xfree (p->callback);
        if (argv[2][0])
        {
            if (ir_tcl_strdup (interp, &p->callback, argv[2]) == TCL_ERROR)
                return TCL_ERROR;
        }
        else
            p->callback = NULL;
    }
    return TCL_OK;
}

/*
 * do_failback: add error handle callback
 */
static int do_failback (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
    {
        p->failback = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->failback);
    else if (argc == 3)
    {
        xfree (p->failback);
        if (argv[2][0])
        {
            if (ir_tcl_strdup (interp, &p->failback, argv[2]) == TCL_ERROR)
                return TCL_ERROR;
        }
        else
            p->failback = NULL;
    }
    return TCL_OK;
}

/*
 * do_initResponse: add init response handler
 */
static int do_initResponse (void *obj, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_Obj *p = obj;

    if (argc == 0)
    {
        p->initResponse = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->initResponse);
    if (argc == 3)
    {
        xfree (p->initResponse);
        if (argv[2][0])
        {
            if (ir_tcl_strdup (interp, &p->initResponse, argv[2]) == TCL_ERROR)
                return TCL_ERROR;
        }
        else
            p->initResponse = NULL;
    }
    return TCL_OK;
}
/*
 * do_protocol: Set/get protocol method on IR object
 */
static int do_protocol (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_Obj *p = o;

    if (argc <= 0)
    {
        p->protocol_type = PROTO_Z3950;
        return TCL_OK;
    }
    else if (argc == 3)
    {
        if (!strcmp (argv[2], "Z39"))
            p->protocol_type = PROTO_Z3950;
        else if (!strcmp (argv[2], "SR"))
            p->protocol_type = PROTO_SR;
        else
        {
            Tcl_AppendResult (interp, "bad protocol ", argv[2], NULL);
            return ir_tcl_error_exec (interp, argc, argv);
        }
        return TCL_OK;
    }
    switch (p->protocol_type)
    {
    case PROTO_Z3950:
        Tcl_AppendElement (interp, "Z39");
        break;
    case PROTO_SR:
        Tcl_AppendElement (interp, "SR");
        break;
    }
    return TCL_OK;
}

/*
 * do_triggerResourceControl:
 */
static int do_triggerResourceControl (void *obj, Tcl_Interp *interp,
                                      int argc, char **argv)
{
    IrTcl_Obj *p = obj;
    Z_APDU *apdu;
    Z_TriggerResourceControlRequest *req;
    bool_t is_false = 0;

    if (argc <= 0)
        return TCL_OK;
    if (!p->cs_link)
    {
        Tcl_AppendResult (interp, "not connected", NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    apdu = zget_APDU (p->odr_out, Z_APDU_triggerResourceControlRequest);
    req = apdu->u.triggerResourceControlRequest;
    *req->requestedAction = Z_TriggerResourceControlRequest_cancel;
    req->resultSetWanted = &is_false; 
    
    return ir_tcl_send_APDU (interp, p, apdu, "triggerResourceControl",
                             argv[0]);
}

/*
 * do_databaseNames: specify database names
 */
static int do_databaseNames (void *obj, Tcl_Interp *interp,
                             int argc, char **argv)
{
    int i;
    IrTcl_SetCObj *p = obj;

    if (argc == -1)
    {
        for (i=0; i<p->num_databaseNames; i++)
            xfree (p->databaseNames[i]);
        xfree (p->databaseNames);
    }
    if (argc <= 0)
    {
        p->num_databaseNames = 0;
        p->databaseNames = NULL;
        return TCL_OK;
    }
    if (argc < 3)
    {
        for (i=0; i<p->num_databaseNames; i++)
            Tcl_AppendElement (interp, p->databaseNames[i]);
        return TCL_OK;
    }
    if (p->databaseNames)
    {
        for (i=0; i<p->num_databaseNames; i++)
            xfree (p->databaseNames[i]);
        xfree (p->databaseNames);
    }
    p->num_databaseNames = argc - 2;
    p->databaseNames =
        ir_tcl_malloc (sizeof(*p->databaseNames) * (1+p->num_databaseNames));
    for (i=0; i<p->num_databaseNames; i++)
    {
        if (ir_tcl_strdup (interp, &p->databaseNames[i], argv[2+i]) 
            == TCL_ERROR)
            return TCL_ERROR;
    }
    p->databaseNames[i] = NULL;
    return TCL_OK;
}

/*
 * do_replaceIndicator: Set/get replace Set indicator
 */
static int do_replaceIndicator (void *obj, Tcl_Interp *interp,
                                int argc, char **argv)
{
    IrTcl_SetCObj *p = obj;

    if (argc <= 0)
    {
        p->replaceIndicator = 1;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->replaceIndicator, interp, argc, argv);
}

/*
 * do_queryType: Set/Get query method
 */
static int do_queryType (void *obj, Tcl_Interp *interp,
                       int argc, char **argv)
{
    IrTcl_SetCObj *p = obj;

    if (argc == 0)
        return ir_tcl_strdup (interp, &p->queryType, "rpn");
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->queryType);
    if (argc == 3)
    {
        xfree (p->queryType);
        if (ir_tcl_strdup (interp, &p->queryType, argv[2]) == TCL_ERROR)
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
    IrTcl_Obj *p = obj;
    
    if (argc == 0)
    {
        p->userInformationField = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->userInformationField);
    Tcl_AppendResult (interp, p->userInformationField, NULL);
    return TCL_OK;
}

/*
 * do_smallSetUpperBound: Set/get small set upper bound
 */
static int do_smallSetUpperBound (void *o, Tcl_Interp *interp,
                       int argc, char **argv)
{
    IrTcl_SetCObj *p = o;

    if (argc <= 0)
    {
        p->smallSetUpperBound = 0;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->smallSetUpperBound, interp, argc, argv);
}

/*
 * do_largeSetLowerBound: Set/get large set lower bound
 */
static int do_largeSetLowerBound (void *o, Tcl_Interp *interp,
                                  int argc, char **argv)
{
    IrTcl_SetCObj *p = o;

    if (argc <= 0)
    {
        p->largeSetLowerBound = 1;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->largeSetLowerBound, interp, argc, argv);
}

/*
 * do_mediumSetPresentNumber: Set/get large set lower bound
 */
static int do_mediumSetPresentNumber (void *o, Tcl_Interp *interp,
                                      int argc, char **argv)
{
    IrTcl_SetCObj *p = o;
   
    if (argc <= 0)
    {
        p->mediumSetPresentNumber = 0;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->mediumSetPresentNumber, interp, argc, argv);
}

/*
 * do_referenceId: Set/Get referenceId
 */
static int do_referenceId (void *obj, Tcl_Interp *interp,
                           int argc, char **argv)
{
    IrTcl_SetCObj *p = obj;

    if (argc == 0)
    {
        p->referenceId = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->referenceId);
    if (argc == 3)
    {
        xfree (p->referenceId);
        if (ir_tcl_strdup (interp, &p->referenceId, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->referenceId, NULL);
    return TCL_OK;
}

/*
 * do_preferredRecordSyntax: Set/get preferred record syntax
 */
static int do_preferredRecordSyntax (void *obj, Tcl_Interp *interp,
                                     int argc, char **argv)
{
    IrTcl_SetCObj *p = obj;

    if (argc == 0)
    {
        p->preferredRecordSyntax = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
    {
        xfree (p->preferredRecordSyntax);
        p->preferredRecordSyntax = NULL;
        return TCL_OK;
    }
    if (argc == 3)
    {
        xfree (p->preferredRecordSyntax);
        p->preferredRecordSyntax = NULL;
        if (argv[2][0] && (p->preferredRecordSyntax = 
                           ir_tcl_malloc (sizeof(*p->preferredRecordSyntax))))
            *p->preferredRecordSyntax = IrTcl_getRecordSyntaxVal (argv[2]);
    }
    else if (argc == 2)
    {
        Tcl_AppendElement
            (interp,!p->preferredRecordSyntax ? "" :
             IrTcl_getRecordSyntaxStr(*p->preferredRecordSyntax));
    }
    return TCL_OK;
            
}

/*
 * do_elementSetNames: Set/Get element Set Names
 */
static int do_elementSetNames (void *obj, Tcl_Interp *interp,
                               int argc, char **argv)
{
    IrTcl_SetCObj *p = obj;

    if (argc == 0)
    {
        p->elementSetNames = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->elementSetNames);
    if (argc == 3)
    {
        xfree (p->elementSetNames);
        if (ir_tcl_strdup (interp, &p->elementSetNames, argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->elementSetNames, NULL);
    return TCL_OK;
}

/*
 * do_smallSetElementSetNames: Set/Get small Set Element Set Names
 */
static int do_smallSetElementSetNames (void *obj, Tcl_Interp *interp,
                               int argc, char **argv)
{
    IrTcl_SetCObj *p = obj;

    if (argc == 0)
    {
        p->smallSetElementSetNames = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->smallSetElementSetNames);
    if (argc == 3)
    {
        xfree (p->smallSetElementSetNames);
        if (ir_tcl_strdup (interp, &p->smallSetElementSetNames,
                           argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->smallSetElementSetNames, NULL);
    return TCL_OK;
}

/*
 * do_mediumSetElementSetNames: Set/Get medium Set Element Set Names
 */
static int do_mediumSetElementSetNames (void *obj, Tcl_Interp *interp,
                               int argc, char **argv)
{
    IrTcl_SetCObj *p = obj;

    if (argc == 0)
    {
        p->mediumSetElementSetNames = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &p->mediumSetElementSetNames);
    if (argc == 3)
    {
        xfree (p->mediumSetElementSetNames);
        if (ir_tcl_strdup (interp, &p->mediumSetElementSetNames,
                           argv[2]) == TCL_ERROR)
            return TCL_ERROR;
    }
    Tcl_AppendResult (interp, p->mediumSetElementSetNames, NULL);
    return TCL_OK;
}

static IrTcl_Method ir_method_tab[] = {
{ "comstack",                    do_comstack, NULL },
{ "protocol",                    do_protocol, NULL },
{ "failback",                    do_failback, NULL },
{ "failInfo",                    do_failInfo, NULL },
{ "apduInfo",                    do_apduInfo, NULL },
{ "logLevel",                    do_logLevel, NULL },

{ "eventType",                   do_eventType, NULL },
{ "connect",                     do_connect, NULL },
{ "protocolVersion",             do_protocolVersion, NULL },
{ "preferredMessageSize",        do_preferredMessageSize, NULL },
{ "maximumRecordSize",           do_maximumRecordSize, NULL },
{ "implementationName",          do_implementationName, NULL },
{ "implementationId",            do_implementationId, NULL },
{ "implementationVersion",       do_implementationVersion, NULL },
{ "targetImplementationName",    do_targetImplementationName, NULL },
{ "targetImplementationId",      do_targetImplementationId, NULL },
{ "targetImplementationVersion", do_targetImplementationVersion, NULL},
{ "userInformationField",        do_userInformationField, NULL},
{ "idAuthentication",            do_idAuthentication, NULL},
{ "options",                     do_options, NULL},
{ "init",                        do_init_request, NULL},
{ "initResult",                  do_initResult, NULL},
{ "disconnect",                  do_disconnect, NULL},
{ "callback",                    do_callback, NULL},
{ "initResponse",                do_initResponse, NULL},
{ "triggerResourceControl",      do_triggerResourceControl, NULL},
{ "initResponse",                do_initResponse, NULL},
{ NULL, NULL}
};

static IrTcl_Method ir_set_c_method_tab[] = {
{ "databaseNames",               do_databaseNames, NULL},
{ "replaceIndicator",            do_replaceIndicator, NULL},
{ "queryType",                   do_queryType, NULL},
{ "preferredRecordSyntax",       do_preferredRecordSyntax, NULL},
{ "smallSetUpperBound",          do_smallSetUpperBound, NULL},
{ "largeSetLowerBound",          do_largeSetLowerBound, NULL},
{ "mediumSetPresentNumber",      do_mediumSetPresentNumber, NULL},
{ "referenceId",                 do_referenceId, NULL},
{ "elementSetNames",             do_elementSetNames, NULL},
{ "smallSetElementSetNames",     do_smallSetElementSetNames, NULL},
{ "mediumSetElementSetNames",    do_mediumSetElementSetNames, NULL},
{ NULL, NULL}
};

/* 
 * ir_obj_method: IR Object methods
 */
static int ir_obj_method (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IrTcl_Methods tab[3];
    IrTcl_Obj *p = (IrTcl_Obj *) clientData;
    int r;

    if (argc < 2)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, "method args...\"", NULL);
        return TCL_ERROR;
    }
    
    tab[0].tab = ir_method_tab;
    tab[0].obj = p;
    tab[1].tab = ir_set_c_method_tab;
    tab[1].obj = &p->set_inher;
    tab[2].tab = NULL;
    
    if (ir_tcl_method (interp, argc, argv, tab, &r) == TCL_ERROR)
        return ir_tcl_method_error (interp, argc, argv, tab);
    return r;
}

/* 
 * ir_obj_delete: IR Object disposal
 */
static void ir_obj_delete (ClientData clientData)
{
    IrTcl_Obj *obj = (IrTcl_Obj *) clientData;
    IrTcl_Methods tab[3];

    --(obj->ref_count);
    if (obj->ref_count > 0)
        return;
    assert (obj->ref_count == 0);

    logf (LOG_DEBUG, "ir object delete");
    tab[0].tab = ir_method_tab;
    tab[0].obj = obj;
    tab[1].tab = ir_set_c_method_tab;
    tab[1].obj = &obj->set_inher;
    tab[2].tab = NULL;

    ir_tcl_method (NULL, -1, NULL, tab, NULL);

    ir_tcl_del_q (obj);
    odr_destroy (obj->odr_in);
    odr_destroy (obj->odr_out);
    if (obj->odr_pr)
    {
        obj->odr_pr->print = 0;
        odr_destroy (obj->odr_pr);
    }
    xfree (obj);
}

/* 
 * ir_obj_init: IR Object initialization
 */
int ir_obj_init (ClientData clientData, Tcl_Interp *interp,
                 int argc, char **argv, ClientData *subData,
                 ClientData parentData)
{
    IrTcl_Methods tab[3];
    IrTcl_Obj *obj;
#if CCL2RPN
    FILE *inf;
#endif

    if (argc != 2)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " objName\"", NULL);
        return TCL_ERROR;
    }
    obj = ir_tcl_malloc (sizeof(*obj));
    obj->ref_count = 1;
#if CCL2RPN
    obj->bibset = ccl_qual_mk (); 
    if ((inf = fopen ("default.bib", "r")))
    {
        ccl_qual_file (obj->bibset, inf);
        fclose (inf);
    }
#endif

    logf (LOG_DEBUG, "ir object create %s", argv[1]);
    obj->odr_in = odr_createmem (ODR_DECODE);
    odr_choice_enable_bias (obj->odr_in, 0);
    obj->odr_out = odr_createmem (ODR_ENCODE);
    obj->odr_pr = 0;
    if (odr_print_file)
    {
        obj->odr_pr = odr_createmem (ODR_PRINT);
	odr_setprint(obj->odr_pr, odr_print_file);
    }
    obj->state = IR_TCL_R_Idle;
    obj->interp = interp;

    obj->len_in = 0;
    obj->buf_in = NULL;
    obj->request_queue = NULL;

    tab[0].tab = ir_method_tab;
    tab[0].obj = obj;
    tab[1].tab = ir_set_c_method_tab;
    tab[1].obj = &obj->set_inher;
    tab[2].tab = NULL;

    if (ir_tcl_method (interp, 0, NULL, tab, NULL) == TCL_ERROR)
    {
        Tcl_AppendResult (interp, "Failed to initialize ", argv[1], NULL);
        return TCL_ERROR;
    }
    *subData = (ClientData) obj;
    return TCL_OK;
}


/* 
 * ir_obj_mk: IR Object creation
 */
static int ir_obj_mk (ClientData clientData, Tcl_Interp *interp,
                      int argc, char **argv)
{
    ClientData subData;
    int r = ir_obj_init (clientData, interp, argc, argv, &subData, 0);
    
    if (r == TCL_ERROR)
        return TCL_ERROR;
    Tcl_CreateCommand (interp, argv[1], ir_obj_method,
                       subData, ir_obj_delete);
    return TCL_OK;
}

IrTcl_Class ir_obj_class = {
    "ir",
    ir_obj_init,
    ir_obj_method,
    ir_obj_delete
};


/* ------------------------------------------------------- */
/*
 * do_search: Do search request
 */
static int do_search (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    Z_SearchRequest *req;
    Z_Query query;
    Z_APDU *apdu;
    Odr_oct ccl_query;
    IrTcl_SetObj *obj = o;
    IrTcl_Obj *p;
    int r, code;
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION > 0)
    Tcl_DString ds;
#endif
    char *query_str;

    if (argc <= 0)
        return TCL_OK;

    p = obj->parent;
    assert (argc > 1);
    if (argc != 3)
    {
	Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1], "query\"",
                          NULL);
        return TCL_ERROR;
    }
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION > 0)
    query_str = Tcl_UtfToExternalDString(0, argv[2], -1, &ds);
#else
    query_str = argv[2];
#endif
    logf (LOG_DEBUG, "search %s %s", *argv, query_str);
    if (!obj->set_inher.num_databaseNames)
    {
        Tcl_AppendResult (interp, "no databaseNames", NULL);
        code = ir_tcl_error_exec (interp, argc, argv);
	goto out;
    }
    if (!p->cs_link)
    {
        Tcl_AppendResult (interp, "not connected", NULL);
        code = ir_tcl_error_exec (interp, argc, argv);
	goto out;
    }
    apdu = zget_APDU (p->odr_out, Z_APDU_searchRequest);
    req = apdu->u.searchRequest;

    obj->start = 1;

    set_referenceId (p->odr_out, &req->referenceId,
                     obj->set_inher.referenceId);

    req->smallSetUpperBound = &obj->set_inher.smallSetUpperBound;
    req->largeSetLowerBound = &obj->set_inher.largeSetLowerBound;
    req->mediumSetPresentNumber = &obj->set_inher.mediumSetPresentNumber;
    req->replaceIndicator = &obj->set_inher.replaceIndicator;
    req->resultSetName = obj->setName ? obj->setName : "default";
    logf (LOG_DEBUG, "Search, resultSetName %s", req->resultSetName);
    req->num_databaseNames = obj->set_inher.num_databaseNames;
    req->databaseNames = obj->set_inher.databaseNames;
    for (r=0; r < obj->set_inher.num_databaseNames; r++)
        logf (LOG_DEBUG, " Database %s", obj->set_inher.databaseNames[r]);
    if (obj->set_inher.preferredRecordSyntax)
    {
        struct oident ident;

        ident.proto = p->protocol_type;
        ident.oclass = CLASS_RECSYN;
        ident.value = *obj->set_inher.preferredRecordSyntax;
        logf (LOG_DEBUG, "Preferred record syntax is %d", ident.value);
        req->preferredRecordSyntax = odr_oiddup (p->odr_out, 
                                                 oid_getoidbyent (&ident));
    }
    else
        req->preferredRecordSyntax = 0;

    if (obj->set_inher.smallSetElementSetNames &&
        *obj->set_inher.smallSetElementSetNames)
    {
        Z_ElementSetNames *esn = odr_malloc (p->odr_out, sizeof(*esn));
        
        esn->which = Z_ElementSetNames_generic;
        esn->u.generic = obj->set_inher.smallSetElementSetNames;
        req->smallSetElementSetNames = esn;
    }
    else
        req->smallSetElementSetNames = NULL; 
    
    if (obj->set_inher.mediumSetElementSetNames &&
        *obj->set_inher.mediumSetElementSetNames)
    {
        Z_ElementSetNames *esn = odr_malloc (p->odr_out, sizeof(*esn));
        
        esn->which = Z_ElementSetNames_generic;
        esn->u.generic = obj->set_inher.mediumSetElementSetNames;
        req->mediumSetElementSetNames = esn;
    }
    else
        req->mediumSetElementSetNames = NULL; 
    
    req->query = &query;
   
    logf (LOG_DEBUG, "queryType %s", obj->set_inher.queryType);
    if (!strcmp (obj->set_inher.queryType, "rpn"))
    {
        Z_RPNQuery *RPNquery;

        RPNquery = p_query_rpn (p->odr_out, p->protocol_type, query_str);
        if (!RPNquery)
        {
            Tcl_AppendResult (interp, "query syntax error", NULL);
            code = ir_tcl_error_exec (interp, argc, argv);
	    goto out;
        }
        query.which = Z_Query_type_1;
        query.u.type_1 = RPNquery;
    }
#if CCL2RPN
    else if (!strcmp (obj->set_inher.queryType, "cclrpn"))
    {
        int error;
        int pos;
        struct ccl_rpn_node *rpn;
        Z_RPNQuery *RPNquery;
        oident bib1;

        bib1.proto = p->protocol_type;
        bib1.oclass = CLASS_ATTSET;
        bib1.value = VAL_BIB1;

        rpn = ccl_find_str(p->bibset, query_str, &error, &pos);
        if (error)
        {
            Tcl_AppendResult (interp, "ccl syntax error ", ccl_err_msg(error),
                              NULL);
            code = ir_tcl_error_exec (interp, argc, argv);
	    goto out;
        }
#if 0
        ccl_pr_tree (rpn, stderr);
        fprintf (stderr, "\n");
#endif
        RPNquery = ccl_rpn_query(p->odr_out, rpn);
        RPNquery->attributeSetId = oid_getoidbyent (&bib1);
        query.which = Z_Query_type_1;
        query.u.type_1 = RPNquery;
    }
#endif
    else if (!strcmp (obj->set_inher.queryType, "ccl"))
    {
        query.which = Z_Query_type_2;
        query.u.type_2 = &ccl_query;
        ccl_query.buf = (unsigned char *) query_str;
        ccl_query.len = strlen (query_str);
    }
    else
    {
        Tcl_AppendResult (interp, "invalid query method ",
                          obj->set_inher.queryType, NULL);
        code = ir_tcl_error_exec (interp, argc, argv);
	goto out;
    }
    code = ir_tcl_send_APDU (interp, p, apdu, "search", *argv);
 out:
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION > 0)
    Tcl_DStringFree (&ds);
#endif
    return code;
}

/*
 * do_searchResponse: add search response handler
 */
static int do_searchResponse (void *o, Tcl_Interp *interp,
                              int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc == 0)
    {
        obj->searchResponse = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &obj->searchResponse);
    if (argc == 3)
    {
        xfree (obj->searchResponse);
        if (argv[2][0])
        {
            if (ir_tcl_strdup (interp, &obj->searchResponse, argv[2])
                == TCL_ERROR)
                return TCL_ERROR;
        }
        else
            obj->searchResponse = NULL;
    }
    return TCL_OK;
}

/*
 * do_presentResponse: add present response handler
 */
static int do_presentResponse (void *o, Tcl_Interp *interp,
                               int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc == 0)
    {
        obj->presentResponse = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &obj->presentResponse);
    if (argc == 3)
    {
        xfree (obj->presentResponse);
        if (argv[2][0])
        {
            if (ir_tcl_strdup (interp, &obj->presentResponse, argv[2])
                == TCL_ERROR)
                return TCL_ERROR;
        }
        else
            obj->presentResponse = NULL;
    }
    return TCL_OK;
}

/*
 * do_sortResponse: add sort response handler
 */
static int do_sortResponse (void *o, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc == 0)
    {
        obj->sortResponse = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &obj->sortResponse);
    if (argc == 3)
    {
        xfree (obj->sortResponse);
        if (argv[2][0])
        {
            if (ir_tcl_strdup (interp, &obj->sortResponse, argv[2])
                == TCL_ERROR)
                return TCL_ERROR;
        }
        else
            obj->sortResponse = NULL;
    }
    return TCL_OK;
}

/*
 * do_resultCount: Get number of hits
 */
static int do_resultCount (void *o, Tcl_Interp *interp,
                       int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc <= 0)
    {
        obj->resultCount = 0;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&obj->resultCount, interp, argc, argv);
}

/*
 * do_searchStatus: Get search status (after search response)
 */
static int do_searchStatus (void *o, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc <= 0)
        return TCL_OK;
    return ir_tcl_get_set_int (&obj->searchStatus, interp, argc, argv);
}

static void reset_searchResult (IrTcl_SetObj *setobj)
{
    int i;
    for (i = 0; i<setobj->searchResult_num; i++)
        xfree (setobj->searchResult_terms[i]);
    xfree (setobj->searchResult_terms);
    xfree (setobj->searchResult_count);
    setobj->searchResult_terms = 0;
    setobj->searchResult_num = 0;   
}


/*
 * do_searchResult Get USR:Search-Result1 (after search response)
 */
static int do_searchResult (void *o, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int i;
    
    if (argc == 0)
    {
        obj->searchResult_num = 0;
        obj->searchResult_terms = 0;
        obj->searchResult_count = 0;
        return TCL_OK;
    }
    else if (argc == -1)
    {
        reset_searchResult (obj);
        return TCL_OK;
    }
    for (i = 0; i<obj->searchResult_num; i++)
    {
        char str[40];
        sprintf (str, "%d", obj->searchResult_count[i]);
        Tcl_AppendResult (interp, "{", NULL);
        if (obj->searchResult_terms[i])
            Tcl_AppendElement (interp, obj->searchResult_terms[i]); 
        else
            Tcl_AppendElement (interp, ""); 
        Tcl_AppendElement (interp, str);
        Tcl_AppendResult (interp, "} ", NULL);
    }
    return TCL_OK;
}

/*
 * do_presentStatus: Get search status (after search/present response)
 */
static int do_presentStatus (void *o, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc <= 0)
        return TCL_OK;
    return ir_tcl_get_set_int (&obj->presentStatus, interp, argc, argv);
}

/*
 * do_sortStatus: Get sort status (after sort response)
 */
static int do_sortStatus (void *o, Tcl_Interp *interp,
			  int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    char *res;

    if (argc <= 0)
    {
	obj->sortStatus = Z_SortResponse_failure;
        return TCL_OK;
    }
    switch (obj->sortStatus)
    {
    case Z_SortResponse_success:
        res = "success"; break;
    case Z_SortResponse_partial_1:
        res = "partial"; break;
    case Z_SortResponse_failure:
        res = "failure"; break;
    default:
	res = "unknown"; break;
    }
    Tcl_AppendElement (interp, res);
    return TCL_OK;
}

/*
 * do_nextResultSetPosition: Get next result set position
 *       (after search/present response)
 */
static int do_nextResultSetPosition (void *o, Tcl_Interp *interp,
                                     int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc <= 0)
    {
        obj->nextResultSetPosition = 0;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&obj->nextResultSetPosition, interp,
                               argc, argv);
}

/*
 * do_setName: Set result Set name
 */
static int do_setName (void *o, Tcl_Interp *interp,
                       int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc == 0)
        return ir_tcl_strdup (interp, &obj->setName, "Default");
    else if (argc == -1)
        return ir_tcl_strdel (interp, &obj->setName);
    if (argc == 3)
    {
        xfree (obj->setName);
        if (ir_tcl_strdup (interp, &obj->setName, argv[2])
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
    IrTcl_SetObj *obj = o;

    if (argc <= 0)
    {
        obj->numberOfRecordsReturned = 0;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&obj->numberOfRecordsReturned, interp,
                               argc, argv);
}

/*
 * do_type: Return type (if any) at position.
 */
static int do_type (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int offset;
    IrTcl_RecordList *rl;
    const char *type = 0;

    if (argc == 0)
    {
        obj->record_list = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
    {
        delete_IR_records (obj);
        return TCL_OK;
    }
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position\"", NULL);
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;
    rl = find_IR_record (obj, offset);
    if (!rl)
    {
        logf (LOG_DEBUG, "%s %s %s: no record", argv[0], argv[1], argv[2]);
        return TCL_OK;
    }
    switch (rl->which)
    {
    case Z_NamePlusRecord_databaseRecord:
        type = "DB";
        break;
    case Z_NamePlusRecord_surrogateDiagnostic:
        type = "SD";
        break;
    }
    if (type) {
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION <= 5
        interp->result = type;
#else
	Tcl_SetResult(interp, (char *) type, TCL_STATIC);
#endif
    }
    return TCL_OK;
}


/*
 * do_recordType: Return record type (if any) at position.
 */
static int do_recordType (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int offset;
    IrTcl_RecordList *rl;

    if (argc == 0)
    {
        return TCL_OK;
    }
    else if (argc == -1)
    {
        return TCL_OK;
    }
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position\"", NULL);
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &offset)==TCL_ERROR)
        return TCL_ERROR;
    rl = find_IR_record (obj, offset);
    if (!rl)
    {
        logf (LOG_DEBUG, "%s %s %s: no record", argv[0], argv[1], argv[2]);
        return TCL_OK;
    }
    if (rl->which != Z_NamePlusRecord_databaseRecord)
    {
        Tcl_AppendResult (interp, "No DB record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    Tcl_AppendElement (interp, (char*)
                       IrTcl_getRecordSyntaxStr (rl->u.dbrec.type));
    return TCL_OK;
}

/*
 * set record elements (for record extraction)
 */
static int do_recordElements (void *o, Tcl_Interp *interp,
                              int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc == 0)
    {
        obj->recordElements = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (NULL, &obj->recordElements);
    if (argc > 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " ?position?\"", NULL);
        return TCL_ERROR;
    }
    if (argc == 3)
    {
        xfree (obj->recordElements);
        return ir_tcl_strdup (NULL, &obj->recordElements, 
                              (*argv[2] ? argv[2] : NULL));
    }
    Tcl_AppendResult (interp, obj->recordElements, NULL);
    return TCL_OK;
}

/*
 * ir_diagResult 
 */
static int ir_diagResult (Tcl_Interp *interp, IrTcl_Diagnostic *list, int num)
{
    char buf[20];
    int i;
    const char *cp;

    for (i = 0; i<num; i++)
    {
        sprintf (buf, "%d", list[i].condition);
        Tcl_AppendElement (interp, buf);
        cp = diagbib1_str (list[i].condition);
        if (cp)
            Tcl_AppendElement (interp, (char*) cp);
        else
            Tcl_AppendElement (interp, "");
        if (list[i].addinfo)
            Tcl_AppendElement (interp, (char*) list[i].addinfo);
        else
            Tcl_AppendElement (interp, "");
    }
    return TCL_OK;
}

/*
 * do_diag: Return diagnostic record info
 */
static int do_diag (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int offset;
    IrTcl_RecordList *rl;

    if (argc <= 0)
        return TCL_OK;
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position\"", NULL);
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
    return ir_diagResult (interp, rl->u.surrogateDiagnostics.list,
                          rl->u.surrogateDiagnostics.num);
}

/*
 * do_getMarc: Get ISO2709 Record lines/fields
 */
static int do_getMarc (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int offset;
    IrTcl_RecordList *rl;

    if (argc <= 0)
        return TCL_OK;
    if (argc < 7)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position line|field tag ind field\"", NULL);
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
        Tcl_AppendResult (interp, "No DB record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    return ir_tcl_get_marc (interp, rl->u.dbrec.buf, argc, argv);
}

/*
 * do_getSutrs: Get SUTRS Record
 */
static int do_getSutrs (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int offset;
    IrTcl_RecordList *rl;

    if (argc <= 0)
        return TCL_OK;
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position\"", NULL);
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
        Tcl_AppendResult (interp, "No DB record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (!rl->u.dbrec.buf || rl->u.dbrec.type != VAL_SUTRS)
        return TCL_OK;
    Tcl_AppendElement (interp, rl->u.dbrec.buf);
    return TCL_OK;
}

/*
 * do_getXml: Get XML Record
 */
static int do_getXml (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int offset;
    IrTcl_RecordList *rl;

    if (argc <= 0)
        return TCL_OK;
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position\"", NULL);
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
        Tcl_AppendResult (interp, "No DB record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (!rl->u.dbrec.buf || rl->u.dbrec.type != VAL_TEXT_XML)
        return TCL_OK;
    Tcl_AppendElement (interp, rl->u.dbrec.buf);
    return TCL_OK;
}

/*
 * do_getGrs: Get a GRS-1 Record
 */
static int do_getGrs (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    int offset;
    IrTcl_RecordList *rl;

    if (argc <= 0)
        return TCL_OK;
    if (argc < 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position ?(set,tag) (set,tag) ...?\"", NULL);
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
        Tcl_AppendResult (interp, "No DB record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (rl->u.dbrec.type != VAL_GRS1)
        return TCL_OK;
    return ir_tcl_get_grs (interp, rl->u.dbrec.u.grs1, argc, argv);
}


/*
 * do_getExplain: Get an Explain Record
 */
static int do_getExplain (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    IrTcl_Obj *p = obj->parent;
    void *rr;
    Z_ext_typeent *etype;
    int offset;
    IrTcl_RecordList *rl;

    if (argc <= 0)
        return TCL_OK;
    if (argc < 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position ?mask? ...\"", NULL);
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
        Tcl_AppendResult (interp, "No DB record at #", argv[2], NULL);
        return TCL_ERROR;
    }
    if (!rl->u.dbrec.buf || rl->u.dbrec.type != VAL_EXPLAIN)
        return TCL_OK;

    if (!(etype = z_ext_getentbyref (VAL_EXPLAIN)))
        return TCL_OK;
    assert (rl->u.dbrec.buf);
    odr_setbuf (p->odr_in, rl->u.dbrec.buf, rl->u.dbrec.size, 0);
    if (!(*etype->fun)(p->odr_in, (char **) &rr, 0, 0))
        return TCL_OK;
    
    if (etype->what != Z_External_explainRecord)
        return TCL_OK;

    return ir_tcl_get_explain (interp, rr, argc, argv);
}

/*
 * do_responseStatus: Return response status (present or search)
 */
static int do_responseStatus (void *o, Tcl_Interp *interp, 
                             int argc, char **argv)
{
    IrTcl_SetObj *obj = o;

    if (argc == 0)
    {
        obj->recordFlag = 0;
        obj->nonSurrogateDiagnosticNum = 0;
        obj->nonSurrogateDiagnosticList = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
    {
        ir_deleteDiags (&obj->nonSurrogateDiagnosticList,
                        &obj->nonSurrogateDiagnosticNum);
        return TCL_OK;
    }
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
        Tcl_AppendElement (interp, "NSD");
        return ir_diagResult (interp, obj->nonSurrogateDiagnosticList,
                              obj->nonSurrogateDiagnosticNum);
    case Z_Records_multipleNSD:
        Tcl_AppendElement (interp, "NSD");
        return ir_diagResult (interp, obj->nonSurrogateDiagnosticList,
                               obj->nonSurrogateDiagnosticNum);
    }
    return TCL_OK;
}

/*
 * do_present: Perform Present Request
 */

static int do_present (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_SetObj *obj = o;
    IrTcl_Obj *p;
    Z_APDU *apdu;
    Z_PresentRequest *req;
    int start;
    int number;

    if (argc <= 0)
        return TCL_OK;
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
    logf (LOG_DEBUG, "present %s %d %d", *argv, start, number);
    p = obj->parent;
    if (!p->cs_link)
    {
        Tcl_AppendResult (interp, "not connected", NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    obj->start = start;
    obj->number = number;

    apdu = zget_APDU (p->odr_out, Z_APDU_presentRequest);
    req = apdu->u.presentRequest;

    set_referenceId (p->odr_out, &req->referenceId,
                     obj->set_inher.referenceId);

    req->resultSetId = obj->setName ? obj->setName : "Default";
    
    req->resultSetStartPoint = &start;
    req->numberOfRecordsRequested = &number;
    if (obj->set_inher.preferredRecordSyntax)
    {
        struct oident ident;

        ident.proto = p->protocol_type;
        ident.oclass = CLASS_RECSYN;
        ident.value = *obj->set_inher.preferredRecordSyntax;
        logf (LOG_DEBUG, "Preferred record syntax is %d", ident.value);
        req->preferredRecordSyntax = odr_oiddup (p->odr_out, 
                                                 oid_getoidbyent (&ident));
    }
    else
        req->preferredRecordSyntax = 0;

    if (obj->set_inher.elementSetNames && *obj->set_inher.elementSetNames)
    {
        Z_ElementSetNames *esn = odr_malloc (p->odr_out, sizeof(*esn));
        Z_RecordComposition *compo = odr_malloc (p->odr_out, sizeof(*compo));

        esn->which = Z_ElementSetNames_generic;
        esn->u.generic = obj->set_inher.elementSetNames;

        req->recordComposition = compo;
        compo->which = Z_RecordComp_simple;
        compo->u.simple = esn;
    }
    else
        req->recordComposition = NULL;
    return ir_tcl_send_APDU (interp, p, apdu, "present", *argv);
}

#define IR_TCL_RECORD_ENCODING_ISO2709  1
#define IR_TCL_RECORD_ENCODING_RAW      2

typedef struct {
    int encoding;
    int syntax;
    size_t size;
} IrTcl_FileRecordHead;

/*
 * do_loadFile: Load result set from file
 */
static int do_loadFile (void *o, Tcl_Interp *interp,
                        int argc, char **argv)
{
    IrTcl_SetObj *setobj = o;
    FILE *inf;
    size_t size;
    int offset;
    int start = 1;
    int number = 30000;
    char *buf;
    
    if (argc <= 0)
        return TCL_OK;
    if (argc < 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " filename ?start? ?number?\"", NULL);
        return TCL_ERROR;
    }
    if (argc > 3)
        start = atoi (argv[3]);
    if (argc > 4)
        number = atoi (argv[4]);
    offset = start;

    inf = fopen (argv[2], "r");
    if (!inf)
    {
        Tcl_AppendResult (interp, "Cannot open ", argv[2], NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    while (offset < (start+number))
    {
        IrTcl_FileRecordHead head;
        IrTcl_RecordList *rl;

        if (fread (&head, sizeof(head), 1, inf) < 1)
            break;
        rl = new_IR_record (setobj, offset,
                            Z_NamePlusRecord_databaseRecord,
                            (argc > 5) ? argv[5] : NULL);
        rl->u.dbrec.type = head.syntax;
        if (head.encoding == IR_TCL_RECORD_ENCODING_ISO2709)
        {
            if (!(buf = ir_tcl_fread_marc (inf, &size)))
                break;
            rl->u.dbrec.buf = buf;
            rl->u.dbrec.size = size;
            if (size != head.size)
            {
                fclose (inf);
                Tcl_AppendResult (interp, "bad ISO2709 encoding", NULL);
                return ir_tcl_error_exec (interp, argc, argv);
            }
        } 
        else if (head.encoding == IR_TCL_RECORD_ENCODING_RAW)
        {
            rl->u.dbrec.size = head.size;
            rl->u.dbrec.buf = ir_tcl_malloc (head.size + 1);
            if (fread (rl->u.dbrec.buf, rl->u.dbrec.size, 1, inf) < 1)
            {
                fclose (inf);
                Tcl_AppendResult (interp, "bad raw encoding", NULL);
                return ir_tcl_error_exec (interp, argc, argv);
            }
            rl->u.dbrec.buf[rl->u.dbrec.size] = '\0';
        }
        else
        {
            rl->u.dbrec.buf = NULL;
            rl->u.dbrec.size = 0;
            fclose (inf);
            Tcl_AppendResult (interp, "bad encoding", NULL);
            return ir_tcl_error_exec (interp, argc, argv);
        }
        offset++;
    }
    setobj->numberOfRecordsReturned = offset - start;
    fclose (inf);
    return TCL_OK;
}

/*
 * do_saveFile: Save result set on file
 */
static int do_saveFile (void *o, Tcl_Interp *interp,
                        int argc, char **argv)
{
    IrTcl_SetObj *setobj = o;
    FILE *outf;
    int offset;
    int start = 1;
    int number = 30000;
    IrTcl_RecordList *rl;
    
    if (argc <= 0)
        return TCL_OK;
    if (argc < 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " filename ?start? ?number?\"", NULL);
        return TCL_ERROR;
    }
    if (argc > 3)
        start = atoi (argv[3]);
    if (argc > 4)
        number = atoi (argv[4]);
    offset = start;

    outf = fopen (argv[2], "w");
    if (!outf)
    {
        Tcl_AppendResult (interp, "cannot open file", NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    while (offset < (start+number) && (rl = find_IR_record (setobj, offset)))
    {
        if (rl->which == Z_NamePlusRecord_databaseRecord &&
            rl->u.dbrec.buf && rl->u.dbrec.size)
        {
            IrTcl_FileRecordHead head;

            head.encoding = IR_TCL_RECORD_ENCODING_RAW;
            head.syntax = rl->u.dbrec.type;
            head.size = rl->u.dbrec.size;
            if (fwrite (&head, sizeof(head), 1, outf) < 1)
            {
                Tcl_AppendResult (interp, "cannot write", NULL);
                return ir_tcl_error_exec (interp, argc, argv);
            }
            if (fwrite (rl->u.dbrec.buf, rl->u.dbrec.size, 1, outf) < 1)
            {
                Tcl_AppendResult (interp, "cannot write", NULL);
                return ir_tcl_error_exec (interp, argc, argv);
            }
        }
        offset++;
    }
    if (fclose (outf))
    {
        Tcl_AppendResult (interp, "cannot write ", NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    return TCL_OK;
}


/* ------------------------------------------------------- */
/*
 * do_sort: Do sort request
 */
static int do_sort (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    Z_SortRequest *req;
    Z_APDU *apdu;
    IrTcl_SetObj *obj = o;
    IrTcl_Obj *p;
    char sort_string[64], sort_flags[64];
    char *arg;
    int off;
    Z_SortKeySpecList *sksl;
    int oid[OID_SIZE];
    oident bib1;

    if (argc <= 0)
        return TCL_OK;

    p = obj->parent;
    assert (argc > 1);
    if (argc != 3)
    {
	Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1], "query\"",
                          NULL);
        return TCL_ERROR;
    }
    logf (LOG_DEBUG, "sort %s %s", *argv, argv[2]);
    if (!p->cs_link)
    {
        Tcl_AppendResult (interp, "not connected", NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    apdu = zget_APDU (p->odr_out, Z_APDU_sortRequest);
    sksl = (Z_SortKeySpecList *) odr_malloc (p->odr_out, sizeof(*sksl));
    req = apdu->u.sortRequest;

    set_referenceId (p->odr_out, &req->referenceId,
                     obj->set_inher.referenceId);

#ifdef ASN_COMPILED
    req->num_inputResultSetNames = 1;
    req->inputResultSetNames = (Z_InternationalString **)
        odr_malloc (p->odr_out, sizeof(*req->inputResultSetNames));
    req->inputResultSetNames[0] = obj->setName;
#else
    req->inputResultSetNames =
        (Z_StringList *)odr_malloc (p->odr_out,
				    sizeof(*req->inputResultSetNames));
    req->inputResultSetNames->num_strings = 1;
    req->inputResultSetNames->strings =
        (char **)odr_malloc (p->odr_out,
			     sizeof(*req->inputResultSetNames->strings));
    req->inputResultSetNames->strings[0] = obj->setName;
#endif

    req->sortedResultSetName = (char *) obj->setName;


    req->sortSequence = sksl;
    sksl->num_specs = 0;
    sksl->specs = (Z_SortKeySpec **)
	odr_malloc (p->odr_out, sizeof(sksl->specs) * 20);
    
    bib1.proto = PROTO_Z3950;
    bib1.oclass = CLASS_ATTSET;
    bib1.value = VAL_BIB1;
    arg = argv[2];
    while ((sscanf (arg, "%63s %63s%n", sort_string, sort_flags, &off)) == 2 
           && off > 1)
    {
        int i;
        char *sort_string_sep;
        Z_SortKeySpec *sks = (Z_SortKeySpec *)
	    odr_malloc (p->odr_out, sizeof(*sks));
        Z_SortKey *sk = (Z_SortKey *)
	    odr_malloc (p->odr_out, sizeof(*sk));

        arg += off;
        sksl->specs[sksl->num_specs++] = sks;
        sks->sortElement = (Z_SortElement *)
	    odr_malloc (p->odr_out, sizeof(*sks->sortElement));
        sks->sortElement->which = Z_SortElement_generic;
        sks->sortElement->u.generic = sk;
        
        if ((sort_string_sep = strchr (sort_string, '=')))
        {
            Z_AttributeElement *el = (Z_AttributeElement *)
		odr_malloc (p->odr_out, sizeof(*el));
            sk->which = Z_SortKey_sortAttributes;
            sk->u.sortAttributes =
                (Z_SortAttributes *)
		odr_malloc (p->odr_out, sizeof(*sk->u.sortAttributes));
            sk->u.sortAttributes->id = oid_ent_to_oid(&bib1, oid);
            sk->u.sortAttributes->list =
                (Z_AttributeList *)
		odr_malloc (p->odr_out, sizeof(*sk->u.sortAttributes->list));
            sk->u.sortAttributes->list->num_attributes = 1;
            sk->u.sortAttributes->list->attributes =
                (Z_AttributeElement **)odr_malloc (p->odr_out,
                            sizeof(*sk->u.sortAttributes->list->attributes));
            sk->u.sortAttributes->list->attributes[0] = el;
            el->attributeSet = 0;
            el->attributeType = (int *)
		odr_malloc (p->odr_out, sizeof(*el->attributeType));
            *el->attributeType = atoi (sort_string);
            el->which = Z_AttributeValue_numeric;
            el->value.numeric = (int *)
		odr_malloc (p->odr_out, sizeof(*el->value.numeric));
            *el->value.numeric = atoi (sort_string_sep + 1);
        }
        else
        {
            sk->which = Z_SortKey_sortField;
            sk->u.sortField = (char *)odr_malloc (p->odr_out, strlen(sort_string)+1);
            strcpy (sk->u.sortField, sort_string);
        }
        sks->sortRelation = (int *)
	    odr_malloc (p->odr_out, sizeof(*sks->sortRelation));
        *sks->sortRelation = Z_SortKeySpec_ascending;
        sks->caseSensitivity = (int *)
	    odr_malloc (p->odr_out, sizeof(*sks->caseSensitivity));
        *sks->caseSensitivity = Z_SortKeySpec_caseSensitive;
	
#ifdef ASN_COMPILED
        sks->which = Z_SortKeySpec_null;
        sks->u.null = odr_nullval ();
#else
        sks->missingValueAction = NULL;
#endif
	
        for (i = 0; sort_flags[i]; i++)
        {
            switch (sort_flags[i])
            {
            case 'a':
            case 'A':
            case '>':
                *sks->sortRelation = Z_SortKeySpec_descending;
                break;
            case 'd':
            case 'D':
            case '<':
                *sks->sortRelation = Z_SortKeySpec_ascending;
                break;
            case 'i':
            case 'I':
                *sks->caseSensitivity = Z_SortKeySpec_caseInsensitive;
                break;
            case 'S':
            case 's':
                *sks->caseSensitivity = Z_SortKeySpec_caseSensitive;
                break;
            }
        }
    }
    if (!sksl->num_specs)
    {
        printf ("Missing sort specifications\n");
        return -1;
    }
    return ir_tcl_send_APDU (interp, p, apdu, "sort", *argv);
}

static IrTcl_Method ir_set_method_tab[] = {
    { "search",                  do_search, NULL},
    { "searchResponse",          do_searchResponse, NULL},
    { "presentResponse",         do_presentResponse, NULL},
    { "searchStatus",            do_searchStatus, NULL},
    { "presentStatus",           do_presentStatus, NULL},
    { "nextResultSetPosition",   do_nextResultSetPosition, NULL},
    { "setName",                 do_setName, NULL},
    { "resultCount",             do_resultCount, NULL},
    { "numberOfRecordsReturned", do_numberOfRecordsReturned, NULL},
    { "present",                 do_present, NULL},
    { "type",                    do_type, NULL},
    { "getMarc",                 do_getMarc, NULL},
    { "getSutrs",                do_getSutrs, NULL},
    { "getXml",                  do_getXml, NULL},
    { "getGrs",                  do_getGrs, NULL},
    { "getExplain",              do_getExplain, NULL},
    { "recordType",              do_recordType, NULL},
    { "recordElements",          do_recordElements, NULL},
    { "diag",                    do_diag, NULL},
    { "responseStatus",          do_responseStatus, NULL},
    { "loadFile",                do_loadFile, NULL},
    { "saveFile",                do_saveFile, NULL},
    { "sort",                    do_sort, NULL },
    { "sortResponse",            do_sortResponse, NULL},
    { "sortStatus",              do_sortStatus, NULL},
    { "searchResult",            do_searchResult, NULL},
    { NULL, NULL}
};

/* 
 * ir_set_obj_method: IR Set Object methods
 */
static int ir_set_obj_method (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IrTcl_Methods tabs[3];
    IrTcl_SetObj *p = (IrTcl_SetObj *) clientData;
    int r;

    if (argc < 2)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " method args...\"", NULL);
        return TCL_ERROR;
    }
    tabs[0].tab = ir_set_method_tab;
    tabs[0].obj = p;
    tabs[1].tab = ir_set_c_method_tab;
    tabs[1].obj = &p->set_inher;
    tabs[2].tab = NULL;

    if (ir_tcl_method (interp, argc, argv, tabs, &r) == TCL_ERROR)
        return ir_tcl_method_error (interp, argc, argv, tabs);
    return r;
}

/* 
 * ir_set_obj_delete: IR Set Object disposal
 */
static void ir_set_obj_delete (ClientData clientData)
{
    IrTcl_Methods tabs[3];
    IrTcl_SetObj *p = (IrTcl_SetObj *) clientData;

    logf (LOG_DEBUG, "ir set delete");

    tabs[0].tab = ir_set_method_tab;
    tabs[0].obj = p;
    tabs[1].tab = ir_set_c_method_tab;
    tabs[1].obj = &p->set_inher;
    tabs[2].tab = NULL;

    ir_tcl_method (NULL, -1, NULL, tabs, NULL);

    xfree (p);
}

/*
 * ir_set_obj_init: IR Set Object initialization
 */
static int ir_set_obj_init (ClientData clientData, Tcl_Interp *interp,
                            int argc, char **argv, ClientData *subData,
                            ClientData parentData)
{
    IrTcl_Methods tabs[3];
    IrTcl_SetObj *obj;

    if (argc < 2 || argc > 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv,
                          " objSetName ?objParent?\"", NULL);
        return TCL_ERROR;
    }
    obj = ir_tcl_malloc (sizeof(*obj));
    logf (LOG_DEBUG, "ir set create %s", argv[1]);
    if (parentData)
    {
        int i;
        IrTcl_SetCObj *dst;
        IrTcl_SetCObj *src;

        obj->parent = (IrTcl_Obj *) parentData;

        dst = &obj->set_inher;
        src = &obj->parent->set_inher;

        if ((dst->num_databaseNames = src->num_databaseNames))
        {
            dst->databaseNames =
                ir_tcl_malloc (sizeof (*dst->databaseNames)
                               * (1+dst->num_databaseNames));
            for (i = 0; i < dst->num_databaseNames; i++)
                if (ir_tcl_strdup (interp, &dst->databaseNames[i],
                                   src->databaseNames[i]) == TCL_ERROR)
                    return TCL_ERROR;
            dst->databaseNames[i] = NULL;
        }
        else
            dst->databaseNames = NULL;
        if (ir_tcl_strdup (interp, &dst->queryType, src->queryType)
            == TCL_ERROR)
            return TCL_ERROR;

        if (ir_tcl_strdup (interp, &dst->referenceId, src->referenceId)
            == TCL_ERROR)
            return TCL_ERROR;

        if (ir_tcl_strdup (interp, &dst->elementSetNames, src->elementSetNames)
            == TCL_ERROR)
            return TCL_ERROR;

        if (ir_tcl_strdup (interp, &dst->smallSetElementSetNames,
                           src->smallSetElementSetNames)
            == TCL_ERROR)
            return TCL_ERROR;

        if (ir_tcl_strdup (interp, &dst->mediumSetElementSetNames,
                           src->mediumSetElementSetNames)
            == TCL_ERROR)
            return TCL_ERROR;

        if (src->preferredRecordSyntax && 
            (dst->preferredRecordSyntax 
             = ir_tcl_malloc (sizeof(*dst->preferredRecordSyntax))))
            *dst->preferredRecordSyntax = *src->preferredRecordSyntax;
        else
            dst->preferredRecordSyntax = NULL;
        dst->replaceIndicator = src->replaceIndicator;
        dst->smallSetUpperBound = src->smallSetUpperBound;
        dst->largeSetLowerBound = src->largeSetLowerBound;
        dst->mediumSetPresentNumber = src->mediumSetPresentNumber;
    }   
    else
        obj->parent = NULL;

    tabs[0].tab = ir_set_method_tab;
    tabs[0].obj = obj;
    tabs[1].tab = NULL;

    if (ir_tcl_method (interp, 0, NULL, tabs, NULL) == TCL_ERROR)
        return TCL_ERROR;

    *subData = (ClientData) obj;
    return TCL_OK;
}

/*
 * ir_set_obj_mk: IR Set Object creation
 */
static int ir_set_obj_mk (ClientData clientData, Tcl_Interp *interp,
                          int argc, char **argv)
{
    ClientData subData;
    ClientData parentData = 0;
    int r;

    if (argc == 3)
    {
        Tcl_CmdInfo parent_info;
        if (!Tcl_GetCommandInfo (interp, argv[2], &parent_info))
        {
            Tcl_AppendResult (interp, "no object parent", NULL);
            return ir_tcl_error_exec (interp, argc, argv);
        }
        parentData = parent_info.clientData;
    }
    r = ir_set_obj_init (clientData, interp, argc, argv, &subData, parentData);
    if (r == TCL_ERROR)
        return TCL_ERROR;
    Tcl_CreateCommand (interp, argv[1], ir_set_obj_method,
                       subData, ir_set_obj_delete);
    return TCL_OK;
}

IrTcl_Class ir_set_obj_class = {
    "ir-set",
    ir_set_obj_init,
    ir_set_obj_method,
    ir_set_obj_delete
};

/* ------------------------------------------------------- */

/*
 * do_scan: Perform scan 
 */
static int do_scan (void *o, Tcl_Interp *interp, int argc, char **argv)
{
    Z_ScanRequest *req;
    Z_APDU *apdu;
    IrTcl_ScanObj *obj = o;
    IrTcl_Obj *p = obj->parent;
    char *start_term;
    int code;
#if CCL2RPN
    oident bib1;
    struct ccl_rpn_node *rpn;
    int pos;
    int r;
#endif
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION > 0)
    Tcl_DString ds;
#endif

    if (argc <= 0)
        return TCL_OK;
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " scanQuery\"", NULL);
        return TCL_ERROR;
    }
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION > 0)
    start_term = Tcl_UtfToExternalDString(0, argv[2], -1, &ds);
#else
    start_term = argv[2];
#endif
    logf (LOG_DEBUG, "scan %s %s", *argv, start_term);
    if (!p->set_inher.num_databaseNames)
    {
        Tcl_AppendResult (interp, "no databaseNames", NULL);
        code = ir_tcl_error_exec (interp, argc, argv);
	goto out;
    }
    if (!p->cs_link)
    {
        Tcl_AppendResult (interp, "not connected", NULL);
        code = ir_tcl_error_exec (interp, argc, argv);
	goto out;
    }

    apdu = zget_APDU (p->odr_out, Z_APDU_scanRequest);
    req = apdu->u.scanRequest;

    set_referenceId (p->odr_out, &req->referenceId, p->set_inher.referenceId);
    req->num_databaseNames = p->set_inher.num_databaseNames;
    req->databaseNames = p->set_inher.databaseNames;

    if (!(req->termListAndStartPoint =
          p_query_scan (p->odr_out, p->protocol_type,
                        &req->attributeSet, start_term)))
    {
        Tcl_AppendResult (interp, "query syntax error", NULL);
	code = ir_tcl_error_exec (interp, argc, argv);
	goto out;
    }
    req->stepSize = &obj->stepSize;
    req->numberOfTermsRequested = &obj->numberOfTermsRequested;
    req->preferredPositionInResponse = &obj->preferredPositionInResponse;
    logf (LOG_DEBUG, "stepSize=%d", *req->stepSize);
    logf (LOG_DEBUG, "numberOfTermsRequested=%d",
          *req->numberOfTermsRequested);
    logf (LOG_DEBUG, "preferredPositionInResponse=%d",
          *req->preferredPositionInResponse);
    
    code = ir_tcl_send_APDU (interp, p, apdu, "scan", *argv);
 out:
#if TCL_MAJOR_VERSION > 8 || (TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION > 0)
    Tcl_DStringFree (&ds);
#endif
    return code;
}

/*
 * do_scanResponse: add scan response handler
 */
static int do_scanResponse (void *o, Tcl_Interp *interp,
                            int argc, char **argv)
{
    IrTcl_ScanObj *obj = o;

    if (argc == 0)
    {
        obj->scanResponse = NULL;
        return TCL_OK;
    }
    else if (argc == -1)
        return ir_tcl_strdel (interp, &obj->scanResponse);
    if (argc == 3)
    {
        xfree (obj->scanResponse);
        if (argv[2][0])
        {
            if (ir_tcl_strdup (interp, &obj->scanResponse, argv[2])
                == TCL_ERROR)
                return TCL_ERROR;
        }
        else
            obj->scanResponse = NULL;
    }
    return TCL_OK;
}

/*
 * do_stepSize: Set/get replace Step Size
 */
static int do_stepSize (void *obj, Tcl_Interp *interp,
                        int argc, char **argv)
{
    IrTcl_ScanObj *p = obj;
    if (argc <= 0)
    {
        p->stepSize = 0;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->stepSize, interp, argc, argv);
}

/*
 * do_numberOfTermsRequested: Set/get Number of Terms requested
 */
static int do_numberOfTermsRequested (void *obj, Tcl_Interp *interp,
                                      int argc, char **argv)
{
    IrTcl_ScanObj *p = obj;

    if (argc <= 0)
    {
        p->numberOfTermsRequested = 20;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->numberOfTermsRequested, interp, argc, argv);
}


/*
 * do_preferredPositionInResponse: Set/get preferred Position
 */
static int do_preferredPositionInResponse (void *obj, Tcl_Interp *interp,
                                           int argc, char **argv)
{
    IrTcl_ScanObj *p = obj;

    if (argc <= 0)
    {
        p->preferredPositionInResponse = 1;
        return TCL_OK;
    }
    return ir_tcl_get_set_int (&p->preferredPositionInResponse, interp,
                               argc, argv);
}

/*
 * do_scanStatus: Get scan status
 */
static int do_scanStatus (void *obj, Tcl_Interp *interp,
                          int argc, char **argv)
{
    IrTcl_ScanObj *p = obj;

    if (argc <= 0)
        return TCL_OK;
    return ir_tcl_get_set_int (&p->scanStatus, interp, argc, argv);
}

/*
 * do_numberOfEntriesReturned: Get number of Entries returned
 */
static int do_numberOfEntriesReturned (void *obj, Tcl_Interp *interp,
                                       int argc, char **argv)
{
    IrTcl_ScanObj *p = obj;

    if (argc <= 0)
        return TCL_OK;
    return ir_tcl_get_set_int (&p->numberOfEntriesReturned, interp,
                               argc, argv);
}

/*
 * do_positionOfTerm: Get position of Term
 */
static int do_positionOfTerm (void *obj, Tcl_Interp *interp,
                              int argc, char **argv)
{
    IrTcl_ScanObj *p = obj;

    if (argc <= 0)
        return TCL_OK;
    return ir_tcl_get_set_int (&p->positionOfTerm, interp, argc, argv);
}

/*
 * do_scanLine: get Scan Line (surrogate or normal) after response
 */
static int do_scanLine (void *obj, Tcl_Interp *interp, int argc, char **argv)
{
    IrTcl_ScanObj *p = obj;
    int i;
    char numstr[20];

    if (argc == 0)
    {
        p->entries_flag = 0;
        p->entries = NULL;
        p->nonSurrogateDiagnosticNum = 0;
        p->nonSurrogateDiagnosticList = 0;
        return TCL_OK;
    }
    else if (argc == -1)
    {
        p->entries_flag = 0;
        /* release entries */
        p->entries = NULL;

        ir_deleteDiags (&p->nonSurrogateDiagnosticList, 
                        &p->nonSurrogateDiagnosticNum);
        return TCL_OK;
    }
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " ", argv[1],
                          " position\"", NULL);
        return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &i) == TCL_ERROR)
        return TCL_ERROR;
    if (!p->entries_flag || !p->entries || i >= p->num_entries || i < 0)
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
        Tcl_AppendElement (interp, "SD");
        return ir_diagResult (interp, p->entries[i].u.diag.list,
                              p->entries[i].u.diag.num);
        break;
    }
    return TCL_OK;
}

static IrTcl_Method ir_scan_method_tab[] = {
    { "scan",                    do_scan, NULL},
    { "scanResponse",            do_scanResponse, NULL},
    { "stepSize",                do_stepSize, NULL},
    { "numberOfTermsRequested",  do_numberOfTermsRequested, NULL},
    { "preferredPositionInResponse", do_preferredPositionInResponse, NULL},
    { "scanStatus",              do_scanStatus, NULL},
    { "numberOfEntriesReturned", do_numberOfEntriesReturned, NULL},
    { "positionOfTerm",          do_positionOfTerm, NULL},
    { "scanLine",                do_scanLine, NULL},
    { NULL, NULL}
};

/* 
 * ir_scan_obj_method: IR Scan Object methods
 */
static int ir_scan_obj_method (ClientData clientData, Tcl_Interp *interp,
                               int argc, char **argv)
{
    IrTcl_Methods tabs[2];
    int r;

    if (argc < 2)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv, " method args...\"", NULL);
        return TCL_ERROR;
    }
    tabs[0].tab = ir_scan_method_tab;
    tabs[0].obj = clientData;
    tabs[1].tab = NULL;

    if (ir_tcl_method (interp, argc, argv, tabs, &r) == TCL_ERROR)
        return ir_tcl_method_error (interp, argc, argv, tabs);
    return r;
}

/* 
 * ir_scan_obj_delete: IR Scan Object disposal
 */
static void ir_scan_obj_delete (ClientData clientData)
{
    IrTcl_Methods tabs[2];
    IrTcl_ScanObj *obj = (IrTcl_ScanObj *) clientData;

    tabs[0].tab = ir_scan_method_tab;
    tabs[0].obj = obj;
    tabs[1].tab = NULL;

    ir_tcl_method (NULL, -1, NULL, tabs, NULL);
    xfree (obj);
}

/* 
 * ir_scan_obj_mk: IR Scan Object creation
 */
static int ir_scan_obj_mk (ClientData clientData, Tcl_Interp *interp,
                           int argc, char **argv)
{
    Tcl_CmdInfo parent_info;
    IrTcl_ScanObj *obj;
    IrTcl_Methods tabs[2];

    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv,
                          "objScanName objParentName\"", NULL);
        return TCL_ERROR;
    }
    logf (LOG_DEBUG, "ir scan create %s", argv[1]);
    if (!Tcl_GetCommandInfo (interp, argv[2], &parent_info))
    {
        Tcl_AppendResult (interp, "no object parent", NULL);
        return ir_tcl_error_exec (interp, argc, argv);
    }
    obj = ir_tcl_malloc (sizeof(*obj));
    obj->parent = (IrTcl_Obj *) parent_info.clientData;

    tabs[0].tab = ir_scan_method_tab;
    tabs[0].obj = obj;
    tabs[1].tab = NULL;

    if (ir_tcl_method (interp, 0, NULL, tabs, NULL) == TCL_ERROR)
        return TCL_ERROR;
    Tcl_CreateCommand (interp, argv[1], ir_scan_obj_method,
                       (ClientData) obj, ir_scan_obj_delete);
    return TCL_OK;
}

/* ------------------------------------------------------- */

/* 
 * ir_log_init_proc: set yaz log level
 */
static int ir_log_init_proc (ClientData clientData, Tcl_Interp *interp,
                             int argc, char **argv)
{
    int lev;
    if (argc <= 1 || argc > 4)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv,
                          " ?level ?prefix ?filename\"", NULL);
        return TCL_OK;
    }
    lev = yaz_log_mask_str (argv[1]);
    if (argc == 2)
        yaz_log_init (lev, "", NULL);
    else if (argc == 3)
        yaz_log_init (lev, argv[2], NULL);
    else 
        yaz_log_init (lev, argv[2], argv[3]);
    if (lev & LOG_DEBUG)
        odr_print_file = yaz_log_file();
    else
        odr_print_file = 0;
    return TCL_OK;
}

/* 
 * ir_log_proc: log yaz message
 */
static int ir_log_proc (ClientData clientData, Tcl_Interp *interp,
                        int argc, char **argv)
{
    int mask;
    if (argc != 3)
    {
        Tcl_AppendResult (interp, wrongArgs, *argv,
                          " level string\"", NULL);
        return TCL_OK;
    }
    mask = yaz_log_mask_str_x (argv[1], 0);
    logf (LOG_DEBUG, "%s", argv[2]);
    return TCL_OK;
}


/* 
 * ir_version: log ir version
 */
static int ir_version (ClientData clientData, Tcl_Interp *interp,
                        int argc, char **argv)
{
    Tcl_AppendElement (interp, IR_TCL_VERSION);
    Tcl_AppendElement (interp, YAZ_VERSION);
    return TCL_OK;
}


/* ------------------------------------------------------- */
static void ir_initResponse (void *obj, Z_InitResponse *initrs)
{
    IrTcl_Obj *p = obj;

    p->initResult = *initrs->result ? 1 : 0;
    if (!*initrs->result)
        logf (LOG_DEBUG, "Connection rejected by target");
    else
        logf (LOG_DEBUG, "Connection accepted by target");

    get_referenceId (&p->set_inher.referenceId, initrs->referenceId);

    xfree (p->targetImplementationId);
    ir_tcl_strdup (p->interp, &p->targetImplementationId,
               initrs->implementationId);
    xfree (p->targetImplementationName);
    ir_tcl_strdup (p->interp, &p->targetImplementationName,
               initrs->implementationName);
    xfree (p->targetImplementationVersion);
    ir_tcl_strdup (p->interp, &p->targetImplementationVersion,
               initrs->implementationVersion);

    p->maximumRecordSize = *initrs->maximumRecordSize;
    p->preferredMessageSize = *initrs->preferredMessageSize;
    
    memcpy (&p->options, initrs->options, sizeof(initrs->options));
    memcpy (&p->protocolVersion, initrs->protocolVersion,
            sizeof(initrs->protocolVersion));
    xfree (p->userInformationField);
    p->userInformationField = NULL;
    if (initrs->userInformationField)
    {
        int len;

        if (initrs->userInformationField->which == ODR_EXTERNAL_octet && 
            (p->userInformationField =
             ir_tcl_malloc ((len = 
                             initrs->userInformationField->
                             u.octet_aligned->len) +1)))
        {
            memcpy (p->userInformationField,
                    initrs->userInformationField->u.octet_aligned->buf,
                        len);
            (p->userInformationField)[len] = '\0';
        }
    }
}

static void ir_deleteDiags (IrTcl_Diagnostic **dst_list, int *dst_num)
{
    int i;
    for (i = 0; i<*dst_num; i++)
        xfree ((*dst_list)[i].addinfo);
    xfree (*dst_list);
    *dst_list = NULL;
    *dst_num = 0;
}

static void ir_handleDiags (IrTcl_Diagnostic **dst_list, int *dst_num,
                            Z_DiagRec **list, int num)
{
    int i;
    char *addinfo = NULL;

    *dst_num = num;
    *dst_list = ir_tcl_malloc (sizeof(**dst_list) * num);
    for (i = 0; i<num; i++)
    {
        const char *cp;
        switch (list[i]->which)
        {
        case Z_DiagRec_defaultFormat:
            (*dst_list)[i].condition = *list[i]->u.defaultFormat->condition;
#ifdef ASN_COMPILED
	    switch (list[i]->u.defaultFormat->which)
	    {
	    case Z_DefaultDiagFormat_v2Addinfo:
		addinfo = list[i]->u.defaultFormat->u.v2Addinfo;
		break;
	    case Z_DefaultDiagFormat_v3Addinfo:
		addinfo = list[i]->u.defaultFormat->u.v3Addinfo;
		break;
	    }
#else
            addinfo = list[i]->u.defaultFormat->addinfo;
#endif
            if (addinfo && 
                ((*dst_list)[i].addinfo = ir_tcl_malloc (strlen(addinfo)+1)))
                strcpy ((*dst_list)[i].addinfo, addinfo);
            cp = diagbib1_str ((*dst_list)[i].condition);
            logf (LOG_DEBUG, "Diag %d %s %s", (*dst_list)[i].condition,
                  cp ? cp : "", addinfo ? addinfo : "");
            break;
        default:
            (*dst_list)[i].addinfo = NULL;
            (*dst_list)[i].condition = 0;
        }
    }
}

static void ir_handleDBRecord (IrTcl_Obj *p, IrTcl_RecordList *rl,
                               Z_External *oe)
{
    struct oident *ident;
    Z_ext_typeent *etype;

    logf (LOG_DEBUG, "handleDBRecord size=%d", oe->u.octet_aligned->len);
    rl->u.dbrec.size = oe->u.octet_aligned->len;
    rl->u.dbrec.buf = NULL;
    
    if ((ident = oid_getentbyoid (oe->direct_reference)))
        rl->u.dbrec.type = ident->value;
    else
        rl->u.dbrec.type = VAL_USMARC;

    if (ident && (oe->which == Z_External_single ||
                  oe->which == Z_External_octet)
        && (etype = z_ext_getentbyref (ident->value)))
    {
        void *rr;
        
        odr_setbuf (p->odr_in, (char*) oe->u.octet_aligned->buf,
                    oe->u.octet_aligned->len, 0);
        if (!(*etype->fun)(p->odr_in, (char **) &rr, 0, 0))
        {
            rl->u.dbrec.type = VAL_NONE;
            return;
        }
        switch (etype->what)
        {
        case Z_External_sutrs:
            logf (LOG_DEBUG, "Z_External_sutrs");
            oe->u.sutrs = rr;
            if ((rl->u.dbrec.buf = ir_tcl_malloc (oe->u.sutrs->len+1)))
            {
                memcpy (rl->u.dbrec.buf, oe->u.sutrs->buf,
                        oe->u.sutrs->len);
                rl->u.dbrec.buf[oe->u.sutrs->len] = '\0';
            }
            rl->u.dbrec.size = oe->u.sutrs->len;
            break;
        case Z_External_grs1:
            logf (LOG_DEBUG, "Z_External_grs1");
            oe->u.grs1 = rr;
            ir_tcl_grs_mk (oe->u.grs1, &rl->u.dbrec.u.grs1);
            break;
        case Z_External_explainRecord:
            logf (LOG_DEBUG, "Z_External_explainRecord");
            if ((rl->u.dbrec.buf = ir_tcl_malloc (rl->u.dbrec.size)))
            {
                memcpy (rl->u.dbrec.buf, oe->u.octet_aligned->buf,
                        rl->u.dbrec.size);
            }
            break;
        }
    }
    else
    {
        if (oe->which == Z_External_octet && rl->u.dbrec.size > 0)
        {
            char *buf = (char*) oe->u.octet_aligned->buf;
            if ((rl->u.dbrec.buf = ir_tcl_malloc (rl->u.dbrec.size+1)))
	    {
                memcpy (rl->u.dbrec.buf, buf, rl->u.dbrec.size);
                rl->u.dbrec.buf[rl->u.dbrec.size] = '\0';
	    }
        }
        else if (rl->u.dbrec.type == VAL_SUTRS && 
                 oe->which == Z_External_sutrs)
        {
            if ((rl->u.dbrec.buf = ir_tcl_malloc (oe->u.sutrs->len+1)))
            {
                memcpy (rl->u.dbrec.buf, oe->u.sutrs->buf,
                        oe->u.sutrs->len);
                rl->u.dbrec.buf[oe->u.sutrs->len] = '\0';
            }
            rl->u.dbrec.size = oe->u.sutrs->len;
        }
        else if (rl->u.dbrec.type == VAL_GRS1 && 
                 oe->which == Z_External_grs1)
        {
            ir_tcl_grs_mk (oe->u.grs1, &rl->u.dbrec.u.grs1);
        }
    }
}

static void ir_handleZRecords (void *o, Z_Records *zrs, IrTcl_SetObj *setobj,
                              const char *elements)
{
    IrTcl_Obj *p = o;

    int offset;
    IrTcl_RecordList *rl;

    setobj->which = zrs->which;
    setobj->recordFlag = 1;
    
    ir_deleteDiags (&setobj->nonSurrogateDiagnosticList,
                    &setobj->nonSurrogateDiagnosticNum);
    if (zrs->which == Z_Records_DBOSD)
    {
	int num_rec = zrs->u.databaseOrSurDiagnostics->num_records;

        if (num_rec != setobj->numberOfRecordsReturned)
        {
	    logf (LOG_WARN, "numberOfRecordsReturned=%d but num records=%d",
			setobj->numberOfRecordsReturned, num_rec);
            setobj->numberOfRecordsReturned = num_rec;
        }

        for (offset = 0; offset < num_rec; offset++)
        {
            Z_NamePlusRecord *znpr = zrs->u.databaseOrSurDiagnostics->
                records[offset];
            
            rl = new_IR_record (setobj, setobj->start + offset, znpr->which,
                                elements);
            if (rl->which == Z_NamePlusRecord_surrogateDiagnostic)
                ir_handleDiags (&rl->u.surrogateDiagnostics.list,
                                &rl->u.surrogateDiagnostics.num,
                                &znpr->u.surrogateDiagnostic,
                                1);
            else
                ir_handleDBRecord (p, rl,
                                   (Z_External*) (znpr->u.databaseRecord));
        }
    }
    else if (zrs->which == Z_Records_multipleNSD)
    {
        logf (LOG_DEBUG, "multipleNonSurrogateDiagnostic %d",
              zrs->u.multipleNonSurDiagnostics->num_diagRecs);
        setobj->numberOfRecordsReturned = 0;
        ir_handleDiags (&setobj->nonSurrogateDiagnosticList,
                        &setobj->nonSurrogateDiagnosticNum,
                        zrs->u.multipleNonSurDiagnostics->diagRecs,
                        zrs->u.multipleNonSurDiagnostics->num_diagRecs);
    }
    else
    {
#ifdef ASN_COMPILED
        Z_DiagRec dr, *dr_p = &dr;
        dr.which = Z_DiagRec_defaultFormat;
        dr.u.defaultFormat = zrs->u.nonSurrogateDiagnostic;
#else
        Z_DiagRec *dr_p = zrs->u.nonSurrogateDiagnostic;
#endif
        logf (LOG_DEBUG, "NonSurrogateDiagnostic");

        setobj->numberOfRecordsReturned = 0;
        ir_handleDiags (&setobj->nonSurrogateDiagnosticList,
                        &setobj->nonSurrogateDiagnosticNum,
                        &dr_p, 1);
    }
}

static char *set_queryExpression (Z_QueryExpression *qe)
{
    char *termz = 0;
    if (!qe)
        return 0;
    if (qe->which == Z_QueryExpression_term)
    {
        if (qe->u.term->queryTerm)
        {
            Z_Term *term = qe->u.term->queryTerm;
            if (term->which == Z_Term_general)
            {
                termz = xmalloc (term->u.general->len+1);
                memcpy (termz, term->u.general->buf, term->u.general->len);
                termz[term->u.general->len] = 0;
            }
        }
    }
    return termz;
}

static void set_searchResult (Z_OtherInformation *o,
                              IrTcl_SetObj *setobj)
{
    int i;
    if (!o)
        return ;
    for (i = 0; i < o->num_elements; i++)
    {
        if (o->list[i]->which == Z_OtherInfo_externallyDefinedInfo)
        {
            ODR odr = odr_createmem (ODR_DECODE);
            Z_External *ext = o->list[i]->information.externallyDefinedInfo;
            Z_SearchInfoReport *sr = 0;

            if (ext->which == Z_External_single)
            {
                odr_setbuf (odr, ext->u.single_ASN1_type->buf,
                            ext->u.single_ASN1_type->len, 0);
                z_SearchInfoReport (odr, &sr, 0, "searchInfo");
            }
            if (ext->which == Z_External_searchResult1)
                sr = ext->u.searchResult1;
            if (sr)
            {
                int j;
                reset_searchResult (setobj);

                setobj->searchResult_num = sr->num;
                setobj->searchResult_terms =
                    xmalloc (sr->num * sizeof(*setobj->searchResult_terms));
                setobj->searchResult_count =
                    xmalloc (sr->num * sizeof(*setobj->searchResult_count));

                for (j = 0; j < sr->num; j++)
                {
                    setobj->searchResult_terms[j] =
                        set_queryExpression (
                            sr->elements[j]->subqueryExpression);
                    if (sr->elements[j]->subqueryCount)
                        setobj->searchResult_count[j] = 
                            *sr->elements[j]->subqueryCount;
                    else
                        setobj->searchResult_count[j] =  0;
                }
            }
            odr_destroy (odr);
        }
    }
}

static void ir_searchResponse (void *o, Z_SearchResponse *searchrs,
                               IrTcl_SetObj *setobj)
{    
    Z_Records *zrs = searchrs->records;

    logf (LOG_DEBUG, "Received search response");
    if (!setobj)
    {
        logf (LOG_DEBUG, "Search response, no object!");
        return;
    }
    setobj->searchStatus = *searchrs->searchStatus;
    get_referenceId (&setobj->set_inher.referenceId, searchrs->referenceId);
    setobj->resultCount = *searchrs->resultCount;
    if (searchrs->presentStatus)
        setobj->presentStatus = *searchrs->presentStatus;
    else
        setobj->presentStatus = Z_SearchResponse_none;
    if (searchrs->nextResultSetPosition)
        setobj->nextResultSetPosition = *searchrs->nextResultSetPosition;

    logf (LOG_DEBUG, "status %d hits %d", 
          setobj->searchStatus, setobj->resultCount);
    set_searchResult (searchrs->additionalSearchInfo, setobj);
    if (zrs)
    {
        const char *es;
        if (setobj->resultCount <= setobj->set_inher.smallSetUpperBound)
            es = setobj->set_inher.smallSetElementSetNames;
        else 
            es = setobj->set_inher.mediumSetElementSetNames;
	setobj->numberOfRecordsReturned = *searchrs->numberOfRecordsReturned;
        ir_handleZRecords (o, zrs, setobj, es);
    }
    else
    {
	setobj->numberOfRecordsReturned = 0;
        setobj->recordFlag = 0;
    }
}


static void ir_presentResponse (void *o, Z_PresentResponse *presrs,
                                IrTcl_SetObj *setobj)
{
    Z_Records *zrs = presrs->records;
    
    logf (LOG_DEBUG, "Received present response");
    if (!setobj)
    {
        logf (LOG_DEBUG, "Present response, no object!");
        return;
    }
    setobj->presentStatus = *presrs->presentStatus;
    get_referenceId (&setobj->set_inher.referenceId, presrs->referenceId);
    setobj->nextResultSetPosition = *presrs->nextResultSetPosition;
    if (zrs)
    {
	setobj->numberOfRecordsReturned = *presrs->numberOfRecordsReturned;
        ir_handleZRecords (o, zrs, setobj, setobj->set_inher.elementSetNames);
    }
    else
    {
	setobj->numberOfRecordsReturned = 0;
        setobj->recordFlag = 0;
        logf (LOG_DEBUG, "No records!");
    }
}

static void ir_scanResponse (void *o, Z_ScanResponse *scanrs,
                             IrTcl_ScanObj *scanobj)
{
    IrTcl_Obj *p = o;
    
    logf (LOG_DEBUG, "Received scanResponse");

    get_referenceId (&p->set_inher.referenceId, scanrs->referenceId);
    scanobj->scanStatus = *scanrs->scanStatus;
    logf (LOG_DEBUG, "scanStatus=%d", scanobj->scanStatus);

    if (scanrs->stepSize)
        scanobj->stepSize = *scanrs->stepSize;
    logf (LOG_DEBUG, "stepSize=%d", scanobj->stepSize);

    scanobj->numberOfEntriesReturned = *scanrs->numberOfEntriesReturned;
    logf (LOG_DEBUG, "numberOfEntriesReturned=%d",
          scanobj->numberOfEntriesReturned);

    if (scanrs->positionOfTerm)
        scanobj->positionOfTerm = *scanrs->positionOfTerm;
    else
        scanobj->positionOfTerm = -1;
    logf (LOG_DEBUG, "positionOfTerm=%d", scanobj->positionOfTerm);

    xfree (scanobj->entries);
    scanobj->entries = NULL;
    scanobj->num_entries = 0;
    scanobj->entries_flag = 0;
	
    ir_deleteDiags (&scanobj->nonSurrogateDiagnosticList,
                    &scanobj->nonSurrogateDiagnosticNum);
    if (scanrs->entries)
    {
        int i;
        Z_Entry **ze;

        scanobj->entries_flag = 1;
	if (scanrs->entries)
	{
            scanobj->num_entries = scanrs->entries->num_entries;
            scanobj->entries = ir_tcl_malloc (scanobj->num_entries * 
					      sizeof(*scanobj->entries));
	    ze = scanrs->entries->entries;
	}
	for (i=0; i<scanobj->num_entries; i++, ze++)
	{
	    scanobj->entries[i].which = (*ze)->which;
	    switch ((*ze)->which)
	    {
	    case Z_Entry_termInfo:
		if ((*ze)->u.termInfo->term->which == Z_Term_general)
		{
		    int l = (*ze)->u.termInfo->term->u.general->len;
		    scanobj->entries[i].u.term.buf = ir_tcl_malloc (1+l);
		    memcpy (scanobj->entries[i].u.term.buf, 
			    (*ze)->u.termInfo->term->u.general->buf,
			    l);
		    scanobj->entries[i].u.term.buf[l] = '\0';
		}
		else
		    scanobj->entries[i].u.term.buf = NULL;
		if ((*ze)->u.termInfo->globalOccurrences)
		    scanobj->entries[i].u.term.globalOccurrences = 
			*(*ze)->u.termInfo->globalOccurrences;
		else
		    scanobj->entries[i].u.term.globalOccurrences = 0;
		break;
	    case Z_Entry_surrogateDiagnostic:
		ir_handleDiags (&scanobj->entries[i].u.diag.list,
				&scanobj->entries[i].u.diag.num,
				&(*ze)->u.surrogateDiagnostic,
				1);
		break;
	    }
	}
	if (scanrs->entries->nonsurrogateDiagnostics)
	    ir_handleDiags (&scanobj->nonSurrogateDiagnosticList,
                            &scanobj->nonSurrogateDiagnosticNum,
                            scanrs->entries->nonsurrogateDiagnostics,
                            scanrs->entries->num_nonsurrogateDiagnostics);
    }
}


static void ir_sortResponse (void *o, Z_SortResponse *sortrs,
                             IrTcl_SetObj *setobj)
{
    IrTcl_Obj *p = o;
    
    logf (LOG_DEBUG, "Received sortResponse");
    
    if (!setobj)
	return;

    purge_IR_records (setobj);

    get_referenceId (&p->set_inher.referenceId, sortrs->referenceId);
    
    setobj->sortStatus = *sortrs->sortStatus;

    ir_deleteDiags (&setobj->nonSurrogateDiagnosticList,
                    &setobj->nonSurrogateDiagnosticNum);
#ifdef ASN_COMPILED
    if (sortrs->diagnostics)
	ir_handleDiags (&setobj->nonSurrogateDiagnosticList,
			&setobj->nonSurrogateDiagnosticNum,
			sortrs->diagnostics,
			sortrs->num_diagnostics);
#else
    if (sortrs->diagnostics)
	ir_handleDiags (&setobj->nonSurrogateDiagnosticList,
			&setobj->nonSurrogateDiagnosticNum,
			sortrs->diagnostics->diagRecs,
			sortrs->diagnostics->num_diagRecs);
#endif
}

/*
 * ir_select_read: handle incoming packages
 */
static void ir_select_read (ClientData clientData)
{
    IrTcl_Obj *p = (IrTcl_Obj *) clientData;
    Z_APDU *apdu;
    int r;
    IrTcl_Request *rq;
    char *object_name;
    Tcl_CmdInfo cmd_info;
    const char *apdu_call;
    int round = 0;

    logf(LOG_DEBUG, "Read handler fd=%d", cs_fileno(p->cs_link));
    if (p->state == IR_TCL_R_Connecting)
    {
        logf(LOG_DEBUG, "read: connect");
        r = cs_rcvconnect (p->cs_link);
        if (r == 1)
        {
            logf (LOG_WARN, "cs_rcvconnect returned 1");
            return;
        }
        p->state = IR_TCL_R_Idle;
        p->ref_count = 2;
        ir_select_remove_write (cs_fileno (p->cs_link), p);
        if (r < 0)
        {
            logf (LOG_DEBUG, "cs_rcvconnect error");
            ir_tcl_disconnect (p);
            if (p->failback)
            {
                p->failInfo = IR_TCL_FAIL_CONNECT;
                ir_tcl_eval (p->interp, p->failback);
            }
            ir_obj_delete ((ClientData) p);
            return;
        }
        if (p->callback)
            ir_tcl_eval (p->interp, p->callback);
        if (p->ref_count == 2 && p->cs_link && p->request_queue
            && p->state == IR_TCL_R_Idle)
            ir_tcl_send_q (p, p->request_queue, "x");
        ir_obj_delete ((ClientData) p);
        return;
    }
    do
    {
        p->state = IR_TCL_R_Reading;

	  round++;
	  yaz_log(LOG_DEBUG, "round %d", round);
        /* read incoming APDU */
        if ((r=cs_get (p->cs_link, &p->buf_in, &p->len_in)) == 1)
        {
            logf(LOG_DEBUG, "PDU Fraction read");
            return ;
        }
        /* signal one more use of ir object - callbacks must not
           release the ir memory (p pointer) */
        p->ref_count = 2;
        if (r <= 0)
        {
            logf (LOG_DEBUG, "cs_get failed, code %d", r);
            ir_tcl_disconnect (p);
            if (p->failback)
            {
                p->failInfo = IR_TCL_FAIL_READ;
                ir_tcl_eval (p->interp, p->failback);
            }
            /* release ir object now if callback deleted it */
            ir_obj_delete ((ClientData) p);
            return;
        }        
        /* got complete APDU. Now decode */
        p->apduLen = r;
        p->apduOffset = -1;
        odr_setbuf (p->odr_in, p->buf_in, r, 0);
        logf (LOG_DEBUG, "cs_get ok, total size %d", r);
        if (!z_APDU (p->odr_in, &apdu, 0, 0))
        {
            logf (LOG_DEBUG, "cs_get failed: %s",
                odr_errmsg (odr_geterror (p->odr_in)));
            ir_tcl_disconnect (p);
            if (p->failback)
            {
                p->failInfo = IR_TCL_FAIL_IN_APDU;
                p->apduOffset = odr_offset (p->odr_in);
                ir_tcl_eval (p->interp, p->failback);
            }
            /* release ir object now if failback deleted it */
            ir_obj_delete ((ClientData) p);
            return;
        }
	if (p->odr_pr)
	    z_APDU(p->odr_pr, &apdu, 0, 0);
        /* handle APDU and invoke callback */
        rq = p->request_queue;
	if (!rq)
	{
	    /* no corresponding request. Skip it. */
	    logf(LOG_DEBUG, "no corresponding request. Skipping it");
            p->state = IR_TCL_R_Idle;
	    return;
        }
        object_name = rq->object_name;
        logf (LOG_DEBUG, "Object %s", object_name);
        apdu_call = NULL;
        if (Tcl_GetCommandInfo (p->interp, object_name, &cmd_info))
        {
            switch(apdu->which)
            {
            case Z_APDU_initResponse:
                p->eventType = "init";
                ir_initResponse (p, apdu->u.initResponse);
                apdu_call = p->initResponse;
                break;
            case Z_APDU_searchResponse:
                p->eventType = "search";
                ir_searchResponse (p, apdu->u.searchResponse,
                                   (IrTcl_SetObj *) cmd_info.clientData);
                apdu_call = ((IrTcl_SetObj *) 
                             cmd_info.clientData)->searchResponse;
                break;
            case Z_APDU_presentResponse:
                p->eventType = "present";
                ir_presentResponse (p, apdu->u.presentResponse,
                                    (IrTcl_SetObj *) cmd_info.clientData);
                apdu_call = ((IrTcl_SetObj *) 
                             cmd_info.clientData)->presentResponse;
                break;
            case Z_APDU_scanResponse:
                p->eventType = "scan";
                ir_scanResponse (p, apdu->u.scanResponse, 
                                 (IrTcl_ScanObj *) cmd_info.clientData);
                apdu_call = ((IrTcl_ScanObj *) 
                             cmd_info.clientData)->scanResponse;
                break;
	    case Z_APDU_sortResponse:
		p->eventType = "sort";
                ir_sortResponse (p, apdu->u.sortResponse, 
                                 (IrTcl_SetObj *) cmd_info.clientData);
                apdu_call = ((IrTcl_SetObj *) 
                             cmd_info.clientData)->sortResponse;
                break;		
            default:
                logf (LOG_WARN, "Received unknown APDU type (%d)",
                      apdu->which);
                ir_tcl_disconnect (p);
                if (p->failback)
                {
                    p->failInfo = IR_TCL_FAIL_UNKNOWN_APDU;
                    ir_tcl_eval (p->interp, p->failback);
                }
                return;
            }
        }
        p->request_queue = rq->next;
        p->state = IR_TCL_R_Idle;
       
        if (apdu_call)
            ir_tcl_eval (p->interp, apdu_call);
        else if (rq->callback)
            ir_tcl_eval (p->interp, rq->callback);
        xfree (rq->buf_out);
        xfree (rq->callback);
        xfree (rq->object_name);
        xfree (rq);
        odr_reset (p->odr_in);
        if (p->ref_count == 1)
        {
            ir_obj_delete ((ClientData) p);
            return;
        }
        ir_obj_delete ((ClientData) p);
    } while (p->cs_link && cs_more (p->cs_link));
    if (p->cs_link && p->request_queue && p->state == IR_TCL_R_Idle)
        ir_tcl_send_q (p, p->request_queue, "x");
}

/*
 * ir_select_write: handle outgoing packages - not yet written.
 */
static int ir_select_write (ClientData clientData)
{
    IrTcl_Obj *p = (IrTcl_Obj *) clientData;
    int r;
    IrTcl_Request *rq;

    logf (LOG_DEBUG, "Write handler fd=%d", cs_fileno(p->cs_link));
    if (p->state == IR_TCL_R_Connecting)
    {
        logf(LOG_DEBUG, "write: connect");
        r = cs_rcvconnect (p->cs_link);
        if (r == 1)
        {
            logf (LOG_DEBUG, "cs_rcvconnect returned 1");
            return 2;
        }
        p->state = IR_TCL_R_Idle;
        p->ref_count = 2;
        ir_select_remove_write (cs_fileno (p->cs_link), p);
        if (r < 0)
        {
            logf (LOG_DEBUG, "cs_rcvconnect error");
            ir_tcl_disconnect (p);
            if (p->failback)
            {
                p->failInfo = IR_TCL_FAIL_CONNECT;
                ir_tcl_eval (p->interp, p->failback);
            }
            ir_obj_delete ((ClientData) p);
            return 2;
        }
        if (p->callback)
            ir_tcl_eval (p->interp, p->callback);
        ir_obj_delete ((ClientData) p);
        return 2;
    }
    rq = p->request_queue;
    if (!rq || !rq->buf_out)
        return 0;
    assert (rq);
    if ((r=cs_put (p->cs_link, rq->buf_out, rq->len_out)) < 0)
    {
        logf (LOG_DEBUG, "cs_put write fail");
        p->ref_count = 2;
        xfree (rq->buf_out);
        rq->buf_out = NULL;
        ir_tcl_disconnect (p);
        if (p->failback)
        {
            p->failInfo = IR_TCL_FAIL_WRITE;
            ir_tcl_eval (p->interp, p->failback);
        }
        ir_obj_delete ((ClientData) p);
    }
    else if (r == 0)            /* remove select bit */
    {
        logf (LOG_DEBUG, "Write completed");
        p->state = IR_TCL_R_Waiting;
        ir_select_remove_write (cs_fileno (p->cs_link), p);
        xfree (rq->buf_out);
        rq->buf_out = NULL;
    }
    return 1;
}

static void ir_select_notify (ClientData clientData, int r, int w, int e)
{
    if (w)
    {
        if (!ir_select_write (clientData) && r)
            ir_select_read (clientData);
    } 
    else if (r)
    {
        ir_select_read (clientData);
    }
}

/*----------------------------------------------------------- */
/* 
 * DllEntryPoint --
 * 
 *    This wrapper function is used by Windows to invoke the
 *    initialization code for the DLL.  If we are compiling
 *    with Visual C++, this routine will be renamed to DllMain.
 *    routine.
 * 
 * Results:
 *    Returns TRUE;
 * 
 * Side effects:
 *    None.
 */
 
#ifdef __WIN32__
BOOL APIENTRY
DllEntryPoint(hInst, reason, reserved)
    HINSTANCE hInst;          /* Library instance handle. */
    DWORD reason;             /* Reason this function is being called. */
    LPVOID reserved;          /* Not used. */
{
    return TRUE;
}
#endif

/* ------------------------------------------------------- */
/*
 * Irtcl_init: Registration of TCL commands.
 */
EXPORT (int,Irtcl_Init) (Tcl_Interp *interp)
{
#if USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
        return TCL_ERROR;
    }
#endif
    Tcl_CreateCommand (interp, "ir", ir_obj_mk, (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "ir-set", ir_set_obj_mk,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "ir-scan", ir_scan_obj_mk,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "ir-log-init", ir_log_init_proc,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "ir-log", ir_log_proc,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "ir-version", ir_version, (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
    nmem_init ();
    return TCL_OK;
}

