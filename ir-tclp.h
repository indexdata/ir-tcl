/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: ir-tclp.h,v $
 * Revision 1.23  1996-01-19 16:22:40  adam
 * New method: apduDump - returns information about last incoming APDU.
 *
 * Revision 1.22  1996/01/10  09:18:44  adam
 * PDU specific callbacks implemented: initRespnse, searchResponse,
 *  presentResponse and scanResponse.
 * Bug fix in the command line shell (tclmain.c) - discovered on OSF/1.
 *
 * Revision 1.21  1996/01/04  16:12:14  adam
 * Setting PDUType renamed to eventType.
 *
 * Revision 1.20  1996/01/04  11:05:23  adam
 * New setting: PDUType - returns type of last PDU returned from the target.
 * Fixed a bug in configure/Makefile.
 *
 * Revision 1.19  1995/11/13  09:55:46  adam
 * Multiple records at a position in a result-set with differnt
 * element specs.
 *
 * Revision 1.18  1995/10/18  16:42:44  adam
 * New settings: smallSetElementSetNames and mediumSetElementSetNames.
 *
 * Revision 1.17  1995/10/16  17:00:56  adam
 * New setting: elementSetNames.
 * Various client improvements. Medium presentation format looks better.
 *
 * Revision 1.16  1995/09/20  11:37:01  adam
 * Configure searches for tk4.1 and tk7.5.
 * Work on GRS.
 *
 * Revision 1.15  1995/08/29  15:30:15  adam
 * Work on GRS records.
 *
 * Revision 1.14  1995/08/04  11:32:40  adam
 * More work on output queue. Memory related routines moved
 * to mem.c
 *
 * Revision 1.13  1995/08/03  13:23:00  adam
 * Request queue.
 *
 * Revision 1.12  1995/07/28  10:28:38  adam
 * First work on request queue.
 *
 * Revision 1.11  1995/06/20  08:07:35  adam
 * New setting: failInfo.
 * Working on better cancel mechanism.
 *
 * Revision 1.10  1995/06/16  12:28:20  adam
 * Implemented preferredRecordSyntax.
 * Minor changes in diagnostic handling.
 * Record list deleted when connection closes.
 *
 * Revision 1.9  1995/06/14  15:08:01  adam
 * Bug fix in cascade-target-list. Uses yaz-version.h.
 *
 * Revision 1.8  1995/06/14  13:37:18  adam
 * Setting recordType implemented.
 * Setting implementationVersion implemented.
 * Settings implementationId / implementationName edited.
 *
 * Revision 1.7  1995/06/01  07:31:28  adam
 * Rename of many typedefs -> IrTcl_...
 *
 * Revision 1.6  1995/05/31  08:36:40  adam
 * Bug fix in client.tcl: didn't save options on clientrc.tcl.
 * New method: referenceId. More work on scan.
 *
 * Revision 1.5  1995/05/29  08:44:25  adam
 * Work on delete of objects.
 *
 * Revision 1.4  1995/05/26  11:44:10  adam
 * Bugs fixed. More work on MARC utilities and queries. Test
 * client is up-to-date again.
 *
 * Revision 1.3  1995/05/26  08:54:17  adam
 * New MARC utilities. Uses prefix query.
 *
 * Revision 1.2  1995/05/24  14:10:23  adam
 * Work on idAuthentication, protocolVersion and options.
 *
 * Revision 1.1  1995/05/23  15:34:49  adam
 * Many new settings, userInformationField, smallSetUpperBound, etc.
 * A number of settings are inherited when ir-set is executed.
 * This version is incompatible with the graphical test client (client.tcl).
 *
 */

#ifndef IR_TCLP_H
#define IR_TCLP_H

#include <tcl.h>

#include <log.h>
#include <pquery.h>
#if CCL2RPN
#include <yaz-ccl.h>
#endif

#include <comstack.h>
#include <tcpip.h>

#if MOSI
#include <xmosi.h>
#endif

#include <yaz-version.h>
#include <odr.h>
#include <proto.h>
#include <oid.h>
#include <diagbib1.h>

#include "ir-tcl.h"

typedef struct {
    char      **databaseNames;
    int         num_databaseNames;
    char       *queryType;
    enum oid_value *preferredRecordSyntax;
    int         replaceIndicator;
    char       *referenceId;

    char       *elementSetNames;
    char       *smallSetElementSetNames;
    char       *mediumSetElementSetNames;

    int         smallSetUpperBound;
    int         largeSetLowerBound;
    int         mediumSetPresentNumber;
} IrTcl_SetCObj;
    
typedef struct {
    int         ref_count;

    char       *cs_type;
    int         protocol_type;
    int         failInfo;
    COMSTACK    cs_link;
    
    int         state;

    int         preferredMessageSize;
    int         maximumRecordSize;
    Odr_bitmask options;
    Odr_bitmask protocolVersion;

    char       *idAuthenticationOpen;
    char       *idAuthenticationGroupId;
    char       *idAuthenticationUserId;
    char       *idAuthenticationPassword;

    char       *implementationName;
    char       *implementationId;
    char       *implementationVersion;
    int        initResult;
    char       *targetImplementationName;
    char       *targetImplementationId;
    char       *targetImplementationVersion;
    char       *userInformationField;

    char       *hostname;
    char       *eventType;
   
    char       *buf_in;
    int         len_in;
    ODR         odr_in;
    ODR         odr_out;
    ODR         odr_pr;

    Tcl_Interp *interp;
    char       *callback;
    char       *failback;
    char       *initResponse;

    int        apduLen;
    int        apduOffset;

#if CCL2RPN
    CCL_bibset  bibset;
#endif
    struct IrTcl_Request_ *request_queue;

    IrTcl_SetCObj   set_inher;
} IrTcl_Obj;

typedef struct IrTcl_Request_ {
    struct IrTcl_Request_ *next; 

    char       *object_name;
    
    char       *buf_out;
    int         len_out;

    char       *callback;
} IrTcl_Request;

typedef struct {
    int condition;
    char *addinfo;
} IrTcl_Diagnostic;

struct GRS_Record_entry {
    int tagType;
    int tagWhich;
    union {
        int num;
        char *str;
    } tagVal;
    int dataWhich;
    union {
        struct IrTcl_GRS_Record_ *sub;
        char *str;
        struct {
            int len;
            char *buf;
        } octets;
        int num;
        int bool;
    } tagData;
};

typedef struct IrTcl_GRS_Record_ {
    int noTags;
    struct GRS_Record_entry *entries;
} IrTcl_GRS_Record;

typedef struct IrTcl_RecordList_ {
    int no;
    char *elements;
    int which;
    union {
        struct {
	    char *buf;
	    size_t size;
            union {
                IrTcl_GRS_Record *grs1;
            } u;
            enum oid_value type;
        } dbrec;
        struct {
            int num;
            IrTcl_Diagnostic *list;
        } surrogateDiagnostics;
    } u;
    struct IrTcl_RecordList_ *next;
} IrTcl_RecordList;

typedef struct IrTcl_SetObj_ {
    IrTcl_Obj  *parent;
    int         searchStatus;
    int         presentStatus;
    int         resultCount;
    int         nextResultSetPosition;
    int         start;
    int         number;
    int         numberOfRecordsReturned;
    char       *setName;
    char       *recordElements;
    int         recordFlag;
    int         which;
    int         nonSurrogateDiagnosticNum;
    char       *searchResponse;
    char       *presentResponse;
    IrTcl_Diagnostic *nonSurrogateDiagnosticList;
    IrTcl_RecordList *record_list;
    IrTcl_SetCObj set_inher;
} IrTcl_SetObj;

typedef struct IrTcl_ScanEntry_ {
    int         which;
    union {
        struct {
	    char *buf;
	    int  globalOccurrences;
	} term;
	struct {
            IrTcl_Diagnostic *list;
            int num;
	} diag;
    } u;
} IrTcl_ScanEntry;

typedef struct IrTcl_ScanObj_ {
    IrTcl_Obj   *parent;
    int         stepSize;
    int         numberOfTermsRequested;
    int         preferredPositionInResponse;

    int         scanStatus;
    int         numberOfEntriesReturned;
    int         positionOfTerm;

    int         entries_flag;
    int         which;

    int         num_entries;
    int         num_diagRecs;

    char        *scanResponse;
    IrTcl_ScanEntry *entries;
    IrTcl_Diagnostic  *nonSurrogateDiagnosticList;
    int         nonSurrogateDiagnosticNum;
} IrTcl_ScanObj;

struct ir_named_entry {
    char *name;
    int  pos;
};

int ir_tcl_get_marc (Tcl_Interp *interp, const char *buf,
                     int argc, char **argv);
int ir_tcl_send_APDU (Tcl_Interp *interp, IrTcl_Obj *p, Z_APDU *apdu,
                      const char *msg, const char *object_name);
int ir_tcl_send_q (IrTcl_Obj *p, IrTcl_Request *rq, const char *msg);
void ir_tcl_del_q (IrTcl_Obj *p);
void *ir_tcl_malloc (size_t size);
int ir_tcl_strdup (Tcl_Interp *interp, char** p, const char *s);
int ir_tcl_strdel (Tcl_Interp *interp, char **p);

char *ir_tcl_fread_marc (FILE *inf, size_t *size);
void ir_tcl_read_grs (Z_GenericRecord *r, IrTcl_GRS_Record **grs_record);
int ir_tcl_get_grs (Tcl_Interp *interp, IrTcl_GRS_Record *grs_record, 
                     int argc, char **argv);

#define IR_TCL_FAIL_CONNECT      1
#define IR_TCL_FAIL_READ         2
#define IR_TCL_FAIL_WRITE        3
#define IR_TCL_FAIL_IN_APDU      4
#define IR_TCL_FAIL_UNKNOWN_APDU 5

#define IR_TCL_R_Idle            0
#define IR_TCL_R_Writing         1
#define IR_TCL_R_Waiting         2
#define IR_TCL_R_Reading         3
#define IR_TCL_R_Connecting      4
#endif
