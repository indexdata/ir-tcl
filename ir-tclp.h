/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: ir-tclp.h,v $
 * Revision 1.1  1995-05-23 15:34:49  adam
 * Many new settings, userInformationField, smallSetUpperBound, etc.
 * A number of settings are inherited when ir-set is executed.
 * This version is incompatible with the graphical test client (client.tcl).
 *
 */

#ifndef IR_TCLP_H
#define IR_TCLP_H

#include <tcl.h>

#include <yaz-ccl.h>
#include <iso2709.h>
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

    int         smallSetUpperBound;
    int         largeSetLowerBound;
    int         mediumSetPresentNumber;
} IRSetCObj;
    
typedef struct {
    char       *cs_type;
    char       *protocol_type;
    int         connectFlag;
    COMSTACK    cs_link;

    int         preferredMessageSize;
    int         maximumRecordSize;
    Odr_bitmask options;
    Odr_bitmask protocolVersion;
    char       *idAuthentication;
    char       *implementationName;
    char       *implementationId;
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

    CCL_bibset  bibset;
    oident      bib1;

    struct IRSetObj_ *set_child;
    struct IRScanObj_ *scan_child;

    IRSetCObj   set_inher;
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
    IRObj      *parent;
    int         searchStatus;
    int         resultCount;
    int         start;
    int         number;
    int         numberOfRecordsReturned;
    char       *setName;
    int         recordFlag;
    int         which;
    int         condition;
    char       *addinfo;
    IRRecordList *record_list;
    IRSetCObj   set_inher;
} IRSetObj;

typedef struct IRScanEntry_ {
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
} IRScanEntry;

typedef struct IRScanDiag_ {
    int         dummy;
} IRScanDiag;

typedef struct IRScanObj_ {
    IRObj      *parent;
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

    IRScanEntry *entries;
    IRScanDiag  *nonSurrogateDiagnostics;
} IRScanObj;

#endif
