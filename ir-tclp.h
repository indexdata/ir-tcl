/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: ir-tclp.h,v $
 * Revision 1.8  1995-06-14 13:37:18  adam
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

#include <odr.h>
#include <proto.h>
#include <oid.h>
#include <diagbib1.h>

#include "ir-tcl.h"

typedef struct {
    char      **databaseNames;
    int         num_databaseNames;

    char       *queryType;
    int         replaceIndicator;
    char       *referenceId;

    int         smallSetUpperBound;
    int         largeSetLowerBound;
    int         mediumSetPresentNumber;
} IrTcl_SetCObj;
    
typedef struct {
    int         ref_count;

    char       *cs_type;
    char       *protocol_type;
    int         connectFlag;
    COMSTACK    cs_link;

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
   
    char       *buf_out;
    int         len_out;
    char       *buf_in;
    int         len_in;
    char       *sbuf;
    int         slen;
    ODR         odr_in;
    ODR         odr_out;
    ODR         odr_pr;

    Tcl_Interp *interp;
    char       *callback;
    char       *failback;

#if CCL2RPN
    CCL_bibset  bibset;
#endif

    struct IrTcl_SetObj_ *set_child;
    struct IrTcl_ScanObj_ *scan_child;

    IrTcl_SetCObj   set_inher;
} IrTcl_Obj;

typedef struct IrTcl_RecordList_ {
    int no;
    int which;
    union {
        struct {
	    char *buf;
	    size_t size;
            enum oid_value type;
        } dbrec;
        struct {
            int  condition;
            char *addinfo;
        } diag;
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
    int         recordFlag;
    int         which;
    int         condition;
    char       *addinfo;
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
	    int  condition;
	    char *addinfo;
	} diag;
    } u;
} IrTcl_ScanEntry;

typedef struct IrTcl_ScanDiag_ {
    int         dummy;
} IrTcl_ScanDiag;

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

    IrTcl_ScanEntry *entries;
    IrTcl_ScanDiag  *nonSurrogateDiagnostics;
} IrTcl_ScanObj;

struct ir_named_entry {
    char *name;
    int  pos;
};

int ir_tcl_get_marc (Tcl_Interp *interp, const char *buf,
                     int argc, char **argv);
char *ir_tcl_fread_marc (FILE *inf, size_t *size);
#endif
