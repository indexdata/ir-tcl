/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1996-1998
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: explain.c,v $
 * Revision 1.13  2003-02-18 10:31:38  adam
 * Minor change
 *
 * Revision 1.12  2003/02/18 10:28:00  adam
 * Fix for new schema definition
 *
 * Revision 1.11  1998/05/20 12:24:41  adam
 * Fixed bug regaring missing element languages in TargetInfo.
 * Changed code so that it works with ASN.1 compiled YAZ code.
 *
 * Revision 1.10  1998/04/02 14:31:08  adam
 * This version works with compiled ASN.1 code.
 *
 * Revision 1.9  1997/11/24 11:34:38  adam
 * Using odr_nullval() instead of ODR_NULLVAL when appropriate.
 *
 * Revision 1.8  1997/09/09 10:19:51  adam
 * New MSV5.0 port with fewer warnings.
 *
 * Revision 1.7  1997/08/28 20:17:36  adam
 * Fixed small bug.
 *
 * Revision 1.6  1997/05/14 06:57:14  adam
 * Adopted to use YAZ with C++ support.
 *
 * Revision 1.5  1996/08/23 11:59:47  adam
 * Bug fix: infinite look in ir_oid.
 *
 * Revision 1.4  1996/08/22  13:39:31  adam
 * More work on explain.
 *
 * Revision 1.3  1996/08/21  13:32:50  adam
 * Implemented saveFile method and extended loadFile method to work with it.
 *
 * Revision 1.2  1996/08/20  09:27:48  adam
 * More work on explain.
 * Renamed tkinit.c to tkmain.c. The tcl shell uses the Tcl 7.5 interface
 * for socket i/o instead of the handcrafted one (for Tcl 7.3 and Tcl7.4).
 *
 * Revision 1.1  1996/08/16  15:07:43  adam
 * First work on Explain.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "ir-tclp.h"

typedef struct {
    int argc;
    char **argv;
    Tcl_Interp *interp;
} IrExpArg;

typedef struct {
    char *name;
    int id;
    int (*handle)();
} IrExpChoice;

#ifdef ASN_COMPILED
#else
typedef char *Z_ElementSetName;
typedef Odr_oid *Z_AttributeSetId;
typedef char *Z_InternationalString;
typedef char *Z_LanguageCode;
#endif

static int ir_UnitType (IrExpArg *iea,
            Z_UnitType *p, const char *name, int argi);
static int ir_Unit (IrExpArg *iea,
            Z_Unit *p, const char *name, int argi);
static int ir_VariantType (IrExpArg *iea,
            Z_VariantType *p, const char *name, int argi);
static int ir_OmittedAttributeInterpretation (IrExpArg *iea,
            Z_OmittedAttributeInterpretation *p, const char *name, int argi);
static int ir_AttributeTypeDetails (IrExpArg *iea,
            Z_AttributeTypeDetails *p, const char *name, int argi);
static int ir_Specification (IrExpArg *iea,
            Z_Specification *p, const char *name, int argi);
static int ir_RetrievalRecordDetails (IrExpArg *iea,
            Z_RetrievalRecordDetails *p, const char *name, int argi);
static int ir_ElementInfo (IrExpArg *iea,
            Z_ElementInfo *p, const char *name, int argi);
static int ir_InternationalString (IrExpArg *iea,
            char *p, const char *name, int argi);
static int ir_TagSetInfo (IrExpArg *iea,
            Z_TagSetInfo *p, const char *name, int argi);
static int ir_DatabaseName (IrExpArg *iea,
            char *p, const char *name, int argi);
static int ir_AccessInfo (IrExpArg *iea,
            Z_AccessInfo *p, const char *name, int argi);
static int ir_bool (IrExpArg *iea,
            bool_t *p, const char *name, int argi);
static int ir_LanguageCode (IrExpArg *iea,
            char *p, const char *name, int argi);
static int ir_Units (IrExpArg *iea,
            Z_Units *p, const char *name, int argi);
static int ir_SortDetails (IrExpArg *iea,
            Z_SortDetails *p, const char *name, int argi);
static int ir_ElementSetDetails (IrExpArg *iea,
            Z_ElementSetDetails *p, const char *name, int argi);
static int ir_TermListDetails (IrExpArg *iea,
            Z_TermListDetails *p, const char *name, int argi);
static int ir_AttributeValue (IrExpArg *iea,
            Z_AttributeValue *p, const char *name, int argi);
static int ir_ElementDataType (IrExpArg *iea,
            Z_ElementDataType *p, const char *name, int argi);
static int ir_ProximitySupport (IrExpArg *iea,
            Z_ProximitySupport *p, const char *name, int argi);
static int ir_ProcessingInformation (IrExpArg *iea,
            Z_ProcessingInformation *p, const char *name, int argi);
static int ir_AttributeCombinations (IrExpArg *iea,
            Z_AttributeCombinations *p, const char *name, int argi);
static int ir_AttributeSetDetails (IrExpArg *iea,
            Z_AttributeSetDetails *p, const char *name, int argi);
static int ir_DatabaseInfo (IrExpArg *iea,
            Z_DatabaseInfo *p, const char *name, int argi);
static int ir_IconObject (IrExpArg *iea,
            Z_IconObject *p, const char *name, int argi);
static int ir_RpnCapabilities (IrExpArg *iea,
            Z_RpnCapabilities *p, const char *name, int argi);
static int ir_QueryTypeDetails (IrExpArg *iea,
            Z_QueryTypeDetails *p, const char *name, int argi);
static int ir_ValueDescription (IrExpArg *iea,
            Z_ValueDescription *p, const char *name, int argi);
static int ir_AttributeSetInfo (IrExpArg *iea,
            Z_AttributeSetInfo *p, const char *name, int argi);
static int ir_SchemaInfo (IrExpArg *iea,
            Z_SchemaInfo *p, const char *name, int argi);
static int ir_AttributeOccurrence (IrExpArg *iea,
            Z_AttributeOccurrence *p, const char *name, int argi);
static int ir_AttributeCombination (IrExpArg *iea,
            Z_AttributeCombination *p, const char *name, int argi);
static int ir_UnitInfo (IrExpArg *iea,
            Z_UnitInfo *p, const char *name, int argi);
static int ir_VariantClass (IrExpArg *iea,
            Z_VariantClass *p, const char *name, int argi);
static int ir_VariantSetInfo (IrExpArg *iea,
            Z_VariantSetInfo *p, const char *name, int argi);
static int ir_RecordTag (IrExpArg *iea,
            Z_RecordTag *p, const char *name, int argi);
static int ir_TermListInfo (IrExpArg *iea,
            Z_TermListInfo *p, const char *name, int argi);
static int ir_StringOrNumeric (IrExpArg *iea,
            Z_StringOrNumeric *p, const char *name, int argi);
static int ir_CategoryInfo (IrExpArg *iea,
            Z_CategoryInfo *p, const char *name, int argi);
static int ir_ValueRange (IrExpArg *iea,
            Z_ValueRange *p, const char *name, int argi);
static int ir_Term (IrExpArg *iea,
            Z_Term *p, const char *name, int argi);
static int ir_DatabaseList (IrExpArg *iea,
            Z_DatabaseList *p, const char *name, int argi);
static int ir_HumanString (IrExpArg *iea,
            Z_HumanString *p, const char *name, int argi);
static int ir_CommonInfo (IrExpArg *iea,
            Z_CommonInfo *p, const char *name, int argi);
static int ir_NetworkAddress (IrExpArg *iea,
            Z_NetworkAddress *p, const char *name, int argi);
static int ir_Costs (IrExpArg *iea,
            Z_Costs *p, const char *name, int argi);
static int ir_RecordSyntaxInfo (IrExpArg *iea,
            Z_RecordSyntaxInfo *p, const char *name, int argi);
static int ir_OtherInformation (IrExpArg *iea,
            Z_OtherInformation *p, const char *name, int argi);
static int ir_CategoryList (IrExpArg *iea,
            Z_CategoryList *p, const char *name, int argi);
static int ir_VariantValue (IrExpArg *iea,
            Z_VariantValue *p, const char *name, int argi);
static int ir_PerElementDetails (IrExpArg *iea,
            Z_PerElementDetails *p, const char *name, int argi);
static int ir_AttributeDetails (IrExpArg *iea,
            Z_AttributeDetails *p, const char *name, int argi);
static int ir_ExtendedServicesInfo (IrExpArg *iea,
            Z_ExtendedServicesInfo *p, const char *name, int argi);
static int ir_AttributeType (IrExpArg *iea,
            Z_AttributeType *p, const char *name, int argi);
static int ir_IntUnit (IrExpArg *iea,
            Z_IntUnit *p, const char *name, int argi);
static int ir_Charge (IrExpArg *iea,
            Z_Charge *p, const char *name, int argi);
static int ir_PrivateCapabilities (IrExpArg *iea,
            Z_PrivateCapabilities *p, const char *name, int argi);
static int ir_ValueSet (IrExpArg *iea,
            Z_ValueSet *p, const char *name, int argi);
static int ir_AttributeDescription (IrExpArg *iea,
            Z_AttributeDescription *p, const char *name, int argi);
static int ir_Path (IrExpArg *iea,
            Z_Path *p, const char *name, int argi);
static int ir_ContactInfo (IrExpArg *iea,
            Z_ContactInfo *p, const char *name, int argi);
static int ir_SearchKey (IrExpArg *iea,
            Z_SearchKey *p, const char *name, int argi);
static int ir_Iso8777Capabilities (IrExpArg *iea,
            Z_Iso8777Capabilities *p, const char *name, int argi);
static int ir_AccessRestrictions (IrExpArg *iea,
            Z_AccessRestrictions *p, const char *name, int argi);
static int ir_SortKeyDetails (IrExpArg *iea,
            Z_SortKeyDetails *p, const char *name, int argi);


static int 
ir_match_start (const char *name, void *p, IrExpArg *iea, int argi)
{
    if (!p)
    {
        if (argi >= iea->argc)
            Tcl_AppendResult (iea->interp, name, " ", NULL);
        return 0;
    }
    if (argi < iea->argc)
    {
        if (strcmp (name, iea->argv[argi]))
            return 0;
    }
    else if (*name)
        Tcl_AppendResult (iea->interp, "{", name, " ", NULL);
    else
        Tcl_AppendResult (iea->interp, "{", NULL);
    return 1;
}

static int 
ir_match_end (const char *name, IrExpArg *iea, int argi)
{
    if (argi >= iea->argc)
        Tcl_AppendResult (iea->interp, "} ", NULL);
    return TCL_OK;
}

static int
ir_choice (IrExpArg *iea, IrExpChoice *clist, int *what, void *p,
           int argi)
{
    if (p)
    {
        while (clist->name)
        {
            if (clist->id == *what)
                return (*clist->handle)(iea, p, clist->name, argi);
            clist++;
        }
    }
    if (argi >= iea->argc)
        Tcl_AppendResult (iea->interp, "{} ", NULL);
    return TCL_OK;
}

static int ir_null (IrExpArg *iea,
            Odr_null *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    Tcl_AppendResult (iea->interp, "1 ", NULL);
    return ir_match_end (name, iea, argi);
}

static int ir_CString (IrExpArg *iea,
            char *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    Tcl_AppendElement (iea->interp, p);
    return ir_match_end (name, iea, argi);
}

static int ir_ElementSetName (IrExpArg *iea,
            char *p, const char *name, int argi)
{
    return ir_CString (iea, p, name, argi);
}

static int ir_DatabaseName (IrExpArg *iea,
            char *p, const char *name, int argi)
{
    return ir_CString (iea, p, name, argi);
}

static int ir_InternationalString (IrExpArg *iea,
            char *p, const char *name, int argi)
{
    return ir_CString (iea, p, name, argi);
}

static int ir_GeneralizedTime (IrExpArg *iea,
            char *p, const char *name, int argi)
{
    return ir_CString (iea, p, name, argi);
}

static int ir_oid (IrExpArg *iea,
            Odr_oid *p, const char *name, int argi)
{
    int first = ' ';
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    while (*p != -1)
    {
        char buf[32];
        
        sprintf (buf, "%c%d", first, *p);
        Tcl_AppendResult (iea->interp, buf, NULL);
        first = '.';
        p++;
    }
    return ir_match_end (name, iea, argi);
}

static int ir_TagTypeMapping (IrExpArg *iea,
            Z_TagTypeMapping **p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    /* missing */
    return ir_match_end (name, iea, argi);
}

static int ir_PrimitiveDataType (IrExpArg *iea,
            int *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    /* missing */
    return ir_match_end (name, iea, argi);
}

static int ir_octet (IrExpArg *iea,
            Odr_oct *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    /* missing */
    return ir_match_end (name, iea, argi);
}

static int ir_choice_nop (IrExpArg *iea,
            void *p, const char *name, int argi)
{
    if (argi >= iea->argc)
        Tcl_AppendResult (iea->interp, name, " ", NULL);
    return TCL_OK;
}

static int ir_bool (IrExpArg *iea,
            bool_t *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    Tcl_AppendResult (iea->interp, *p ? "1" : "0", NULL);
    return ir_match_end (name, iea, argi);
}

static int ir_integer (IrExpArg *iea,
            int *p, const char *name, int argi)
{
    char buf[64];
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    sprintf (buf, "%d", *p);
    Tcl_AppendResult (iea->interp, buf, NULL);
    return ir_match_end (name, iea, argi);
}

static int ir_LanguageCode (IrExpArg *iea,
            char *p, const char *name, int argi)
{
    return ir_CString (iea, p, name, argi);
}

static int ir_External (IrExpArg *iea,
            Z_External *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    /* missing */
    return ir_match_end (name, iea, argi);
}

static int ir_sequence (int (*fh)(), IrExpArg *iea, void *p, int *num,
                 const char *name, int argi)
{
    void **pp = (void **) p;
    int i;

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    for (i = 0; i < *num; i++)
        (*fh)(iea, pp[i], "", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Term (IrExpArg *iea,
            Z_Term *p, const char *name, int argi)
{
    static IrExpChoice arm [] = {
        { "general",            Z_Term_general,
                               ir_octet },
        { "numeric",            Z_Term_numeric,
                               ir_integer },
        { "characterString",    Z_Term_characterString,
                               ir_InternationalString },
        { "oid",                Z_Term_oid,
                               ir_oid },
        { "dateTime",           Z_Term_dateTime,
                               ir_GeneralizedTime },
        { "external",           Z_Term_external,
                               ir_External },
        { "null",               Z_Term_null,
                                ir_null },
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;

    ir_choice (iea, arm, &p->which, p->u.general, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_TargetInfo (IrExpArg *iea,
            Z_TargetInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->recentNews, "recentNews", argi);
    ir_IconObject (iea, p->icon, "icon", argi);
    ir_bool (iea, p->namedResultSets, "namedResultSets", argi);
    ir_bool (iea, p->multipleDBsearch, "multipleDBsearch", argi);
    ir_integer (iea, p->maxResultSets, "maxResultSets", argi);
    ir_integer (iea, p->maxResultSize, "maxResultSize", argi);
    ir_integer (iea, p->maxTerms, "maxTerms", argi);
    ir_IntUnit (iea, p->timeoutInterval, "timeoutInterval", argi);
    ir_HumanString (iea, p->welcomeMessage, "welcomeMessage", argi);
    ir_ContactInfo (iea, p->contactInfo, "contactInfo", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_sequence (ir_InternationalString, iea, p->nicknames,
                 &p->num_nicknames, "nicknames", argi);
    ir_HumanString (iea, p->usageRest, "usageRest", argi);
    ir_HumanString (iea, p->paymentAddr, "paymentAddr", argi);
    ir_HumanString (iea, p->hours, "hours", argi);
    ir_sequence (ir_DatabaseList, iea, p->dbCombinations,
                 &p->num_dbCombinations, "dbCombinations", argi);
    ir_sequence (ir_NetworkAddress, iea, p->addresses,
                 &p->num_addresses, "addresses", argi);
    ir_sequence (ir_InternationalString, iea, p->languages,
		 &p->num_languages, "languages", argi);
    ir_AccessInfo (iea, p->commonAccessInfo, "commonAccessInfo", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_DatabaseInfo (IrExpArg *iea,
            Z_DatabaseInfo *p, const char *name, int argi)
{
    static IrExpChoice arm_recordCount [] = {
        { "actualNumber",       Z_DatabaseInfo_actualNumber,
                               ir_integer },
        { "approxNumber",       Z_DatabaseInfo_approxNumber,
                               ir_integer },
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;

    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_DatabaseName (iea, p->name, "name", argi);
    ir_null (iea, p->explainDatabase, "explainDatabase", argi);
    ir_sequence (ir_DatabaseName, iea, p->nicknames,
                 &p->num_nicknames, "nicknames", argi);
    ir_IconObject (iea, p->icon, "icon", argi);
    ir_bool (iea, p->userFee, "userFee", argi);
    ir_bool (iea, p->available, "available", argi);
    ir_HumanString (iea, p->titleString, "titleString", argi);
    ir_sequence (ir_HumanString, iea, p->keywords,
                 &p->num_keywords, "keywords", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_DatabaseList (iea, p->associatedDbs, "associatedDbs", argi);
    ir_DatabaseList (iea, p->subDbs, "subDbs", argi);
    ir_HumanString (iea, p->disclaimers, "disclaimers", argi);
    ir_HumanString (iea, p->news, "news", argi);

    ir_choice (iea, arm_recordCount, &p->which,
                                     p->u.actualNumber, argi);
    ir_HumanString (iea, p->defaultOrder, "defaultOrder", argi);
    ir_integer (iea, p->avRecordSize, "avRecordSize", argi);
    ir_integer (iea, p->maxRecordSize, "maxRecordSize", argi);
    ir_HumanString (iea, p->hours, "hours", argi);
    ir_HumanString (iea, p->bestTime, "bestTime", argi);
    ir_GeneralizedTime (iea, p->lastUpdate, "lastUpdate", argi);
    ir_IntUnit (iea, p->updateInterval, "updateInterval", argi);
    ir_HumanString (iea, p->coverage, "coverage", argi);
    ir_bool (iea, p->proprietary, "proprietary", argi);
    ir_HumanString (iea, p->copyrightText, "copyrightText", argi);
    ir_HumanString (iea, p->copyrightNotice, "copyrightNotice", argi);
    ir_ContactInfo (iea, p->producerContactInfo, "producerContactInfo", argi);
    ir_ContactInfo (iea, p->supplierContactInfo, "supplierContactInfo", argi);
    ir_ContactInfo (iea, p->submissionContactInfo, "submissionContactInfo", argi);
    ir_AccessInfo (iea, p->accessInfo, "accessInfo", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_SchemaInfo (IrExpArg *iea,
            Z_SchemaInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_oid (iea, p->schema, "schema", argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
 
    ir_sequence (ir_TagTypeMapping, iea, p->tagTypeMapping,
                 &p->num_tagTypeMapping, "tagTypeMapping", argi);
    ir_sequence (ir_ElementInfo, iea, p->recordStructure,
                 &p->num_recordStructure, "recordStructure", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ElementDataTypePrimitive (IrExpArg *iea,
            int *p, const char *name, int argi)
{
    static IrExpChoice arm[] = {
        {"octetString",  Z_PrimitiveDataType_octetString, ir_choice_nop },
        {"numeric",      Z_PrimitiveDataType_numeric,     ir_choice_nop },
        {NULL, 0, NULL}};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, p, odr_nullval(), argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ElementInfo (IrExpArg *iea,
            Z_ElementInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->elementName, "elementName", argi);
    ir_Path (iea, p->elementTagPath, "elementTagPath", argi);
    ir_ElementDataType (iea, p->dataType, "dataType", argi);
    ir_bool (iea, p->required, "required", argi);
    ir_bool (iea, p->repeatable, "repeatable", argi);
    ir_HumanString (iea, p->description, "description", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ElementInfoList (IrExpArg *iea,
            Z_ElementInfoList *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
#if ASN_COMPILED
    ir_sequence (ir_ElementInfo, iea, p->elements,
                 &p->num, "elements", argi);
#else
    ir_sequence (ir_ElementInfo, iea, p->list,
                 &p->num, "elements", argi);
#endif
    return ir_match_end (name, iea, argi);
}

static int ir_ElementDataType (IrExpArg *iea,
            Z_ElementDataType *p, const char *name, int argi)
{
    static IrExpChoice arm[] = {
        { "primitive",  Z_ElementDataType_primitive,
                        ir_ElementDataTypePrimitive },
        { "structured", Z_ElementDataType_structured,
                        ir_ElementInfoList },
        { NULL, 0, NULL }};
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.primitive, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_PathUnit (IrExpArg *iea,
            Z_PathUnit *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_integer (iea, p->tagType, "tagType", argi);
    ir_StringOrNumeric (iea, p->tagValue, "tagValue", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Path (IrExpArg *iea,
            Z_Path *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
#if ASN_COMPILED
    ir_sequence (ir_PathUnit, iea, p->elements, 
                 &p->num, "elements", argi);
#else
    ir_sequence (ir_PathUnit, iea, p->list, 
                 &p->num, "elements", argi);
#endif
    return ir_match_end (name, iea, argi);
}

#if ASN_COMPILED
static int ir_TagSetElements (IrExpArg *iea,
            Z_TagSetElements *p, const char *name, int argi)
#else
static int ir_TagSetElements (IrExpArg *iea,
            Z_TagSetInfoElements *p, const char *name, int argi)
#endif
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->elementname, "elementname", argi);
    ir_sequence (ir_InternationalString, iea, p->nicknames,
                 &p->num_nicknames, "nicknames", argi);
    ir_StringOrNumeric (iea, p->elementTag, "elementTag", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_PrimitiveDataType (iea, p->dataType, "dataType", argi);
    ir_OtherInformation (iea, p->otherTagInfo, "otherTagInfo", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_TagSetInfo (IrExpArg *iea,
            Z_TagSetInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_oid (iea, p->tagSet, "tagSet", argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_sequence (ir_TagSetElements, iea, p->elements,
                 &p->num_elements, "elements", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_RecordSyntaxInfo (IrExpArg *iea,
            Z_RecordSyntaxInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_oid (iea, p->recordSyntax, "recordSyntax", argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_sequence (ir_oid, iea, p->transferSyntaxes,
                 &p->num_transferSyntaxes, "transferSyntaxes", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_InternationalString (iea, p->asn1Module, "asn1Module", argi);
    ir_sequence (ir_ElementInfo, iea, p->abstractStructure,
                 &p->num_abstractStructure, "abstractStructure", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeType (IrExpArg *iea,
            Z_AttributeType *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_integer (iea, p->attributeType, "attributeType", argi);
    ir_sequence (ir_AttributeDescription, iea, p->attributeValues,
                 &p->num_attributeValues, "attributeValues", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeSetInfo (IrExpArg *iea,
            Z_AttributeSetInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_oid (iea, p->attributeSet, "attributeSet", argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_sequence (ir_AttributeType, iea, p->attributes,
                 &p->num_attributes, "attributes", argi);
    ir_HumanString (iea, p->description, "description", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeDescription (IrExpArg *iea,
            Z_AttributeDescription *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_StringOrNumeric (iea, p->attributeValue, "attributeValue", argi);
    ir_sequence (ir_StringOrNumeric,iea, p->equivalentAttributes,
                 &p->num_equivalentAttributes, "equivalentAttributes", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_TermListElement (IrExpArg *iea,
            Z_TermListElement *p, const char *name, int argi)
{
    static IrExpChoice searchCostArm [] = {
        { "optimized",  Z_TermListElement_optimized, ir_choice_nop },
        { "normal",     Z_TermListElement_normal,    ir_choice_nop },
        { "expensive",  Z_TermListElement_expensive, ir_choice_nop },
        { "filter",     Z_TermListElement_filter,    ir_choice_nop },
        { NULL, 0, NULL }};
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->title, "title", argi);
    if (p->searchCost)
        ir_choice (iea, searchCostArm, p->searchCost, odr_nullval(), argi);

    ir_bool (iea, p->scanable, "scanable", argi);
    ir_sequence (ir_InternationalString, iea, p->broader,
                 &p->num_broader, "broader", argi);
    ir_sequence (ir_InternationalString, iea, p->narrower,
                 &p->num_narrower, "narrower", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_TermListInfo (IrExpArg *iea,
            Z_TermListInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_DatabaseName (iea, p->databaseName, "databaseName", argi);
    ir_sequence (ir_TermListElement, iea, p->termLists,
                 &p->num_termLists, "termLists", argi);    
    return ir_match_end (name, iea, argi);
}

static int ir_ExtendedServicesInfo (IrExpArg *iea,
            Z_ExtendedServicesInfo *p, const char *name, int argi)
{
    static IrExpChoice waitActionArm [] = {
        { "waitSupported",     Z_ExtendedServicesInfo_waitSupported,
                              ir_choice_nop },
        { "waitAlways",        Z_ExtendedServicesInfo_waitAlways,
                              ir_choice_nop },
        { "waitNotSupported",  Z_ExtendedServicesInfo_waitNotSupported,
                              ir_choice_nop },
        { "depends",           Z_ExtendedServicesInfo_depends,
                              ir_choice_nop },
        { "notSaying",         Z_ExtendedServicesInfo_notSaying,
                              ir_choice_nop },
        { NULL, 0, NULL }};

    ir_InternationalString (iea, p->name, "name", argi);
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_oid (iea, p->type, "type", argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_bool (iea, p->privateType, "privateType", argi);
    ir_bool (iea, p->restrictionsApply, "restrictionsApply", argi);
    ir_bool (iea, p->feeApply, "feeApply", argi);
    ir_bool (iea, p->available, "available", argi);
    ir_bool (iea, p->retentionSupported, "retentionSupported", argi);

    ir_choice (iea, waitActionArm, p->waitAction, odr_nullval(), argi);

    ir_HumanString (iea, p->description, "description", argi);
    ir_External (iea, p->specificExplain, "specificExplain", argi);
    ir_InternationalString (iea, p->esASN, "esASN", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeDetails (IrExpArg *iea,
            Z_AttributeDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_DatabaseName (iea, p->databaseName, "databaseName", argi);
    ir_sequence (ir_AttributeSetDetails, iea, p->attributesBySet,
                 &p->num_attributesBySet, "attributesBySet", argi);
    ir_AttributeCombinations (iea, p->attributeCombinations,
                                     "attributeCombinations", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeSetDetails (IrExpArg *iea,
            Z_AttributeSetDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_oid (iea, p->attributeSet, "attributeSet", argi);
    ir_sequence (ir_AttributeTypeDetails, iea, p->attributesByType, 
                 &p->num_attributesByType, "attributesByType", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeTypeDetails (IrExpArg *iea,
            Z_AttributeTypeDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_integer (iea, p->attributeType, "attributeType", argi);
    ir_OmittedAttributeInterpretation (iea, p->defaultIfOmitted,
                                       "defaultIfOmitted", argi);
    ir_sequence (ir_AttributeValue, iea, p->attributeValues,
                 &p->num_attributeValues, "attributeValues", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_OmittedAttributeInterpretation (IrExpArg *iea,
            Z_OmittedAttributeInterpretation *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_StringOrNumeric (iea, p->defaultValue, "defaultValue", argi);
    ir_HumanString (iea, p->defaultDescription, "defaultDescription", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_EScanInfo (IrExpArg *iea,
            Z_EScanInfo *p, const char *name, int argi)
{ 
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_integer (iea, p->maxStepSize, "maxStepSize", argi);
    ir_HumanString (iea, p->collatingSequence, "collatingSequence", argi);
    ir_bool (iea, p->increasing, "increasing", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_TermListDetails (IrExpArg *iea,
            Z_TermListDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_InternationalString (iea, p->termListName, "termListName", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_AttributeCombinations (iea, p->attributes, "attributes", argi);

    ir_EScanInfo (iea, p->scanInfo, "scanInfo", argi);

    ir_integer (iea, p->estNumberTerms, "estNumberTerms", argi);
    ir_sequence (ir_Term, iea, p->sampleTerms,
                &p->num_sampleTerms, "sampleTerms", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ElementSetDetails (IrExpArg *iea,
            Z_ElementSetDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_DatabaseName (iea, p->databaseName, "databaseName", argi);
    ir_ElementSetName (iea, p->elementSetName, "elementSetName", argi);
    ir_oid (iea, p->recordSyntax, "recordSyntax", argi);
    ir_oid (iea, p->schema, "schema", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_sequence (ir_PerElementDetails, iea, p->detailsPerElement,
                 &p->num_detailsPerElement, "detailsPerElement", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_RetrievalRecordDetails (IrExpArg *iea,
            Z_RetrievalRecordDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_DatabaseName (iea, p->databaseName, "databaseName", argi);
    ir_oid (iea, p->schema, "schema", argi);
    ir_oid (iea, p->recordSyntax, "recordSyntax", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_sequence (ir_PerElementDetails, iea, p->detailsPerElement,
                 &p->num_detailsPerElement, "detailsPerElement", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_PerElementDetails (IrExpArg *iea,
            Z_PerElementDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_RecordTag (iea, p->recordTag, "recordTag", argi);
    ir_sequence (ir_Path, iea, p->schemaTags,
                 &p->num_schemaTags, "schemaTags", argi);
    ir_integer (iea, p->maxSize, "maxSize", argi);
    ir_integer (iea, p->minSize, "minSize", argi);
    ir_integer (iea, p->avgSize, "avgSize", argi);
    ir_integer (iea, p->fixedSize, "fixedSize", argi);
    ir_bool (iea, p->repeatable, "repeatable", argi);
    ir_bool (iea, p->required, "required", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_HumanString (iea, p->contents, "contents", argi);
    ir_HumanString (iea, p->billingInfo, "billingInfo", argi);
    ir_HumanString (iea, p->restrictions, "restrictions", argi);
    ir_sequence (ir_InternationalString, iea, p->alternateNames,
                 &p->num_alternateNames, "alternateNames", argi);
    ir_sequence (ir_InternationalString, iea, p->genericNames,
                 &p->num_genericNames, "genericNames", argi);
    ir_AttributeCombinations (iea, p->searchAccess, "searchAccess", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_RecordTag (IrExpArg *iea,
            Z_RecordTag *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_StringOrNumeric (iea, p->qualifier, "qualifier", argi);
    ir_StringOrNumeric (iea, p->tagValue, "tagValue", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_SortDetails (IrExpArg *iea,
            Z_SortDetails *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_DatabaseName (iea, p->databaseName, "databaseName", argi);
    ir_sequence (ir_SortKeyDetails, iea, p->sortKeys,
                 &p->num_sortKeys, "sortKeys", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_SortKeyDetails (IrExpArg *iea,
            Z_SortKeyDetails *p, const char *name, int argi)
{
    static IrExpChoice sortArm [] = {
        { "always",      Z_SortKeyDetails_always, 
                         ir_choice_nop },
        { "never",       Z_SortKeyDetails_never, 
                         ir_choice_nop },
        { "defaultYes",  Z_SortKeyDetails_default_yes, 
                         ir_choice_nop },
        { "defaultNo",   Z_SortKeyDetails_default_no, 
                         ir_choice_nop },
        { NULL, 0, NULL }};

    static IrExpChoice sortArm2 [] = {
        { "character",   Z_SortKeyDetails_character, 
                         ir_null },
        { "numeric",     Z_SortKeyDetails_numeric, 
                         ir_null },
        { "structured",  Z_SortKeyDetails_structured, 
                         ir_HumanString },
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_HumanString (iea, p->description, "description", argi);
    ir_sequence (ir_Specification, iea, p->elementSpecifications,
                 &p->num_elementSpecifications, "elementSpecifications", argi);
    ir_AttributeCombinations (iea, p->attributeSpecifications,
                                     "attributeSpecifications", argi);
    ir_choice (iea, sortArm2, &p->which, p->u.character, argi); 

    if (p->caseSensitivity) 
        ir_choice (iea, sortArm, p->caseSensitivity, odr_nullval(), argi); 

    return ir_match_end (name, iea, argi);
}

static int ir_ProcessingInformation (IrExpArg *iea,
            Z_ProcessingInformation *p, const char *name, int argi)
{
    IrExpChoice arm[] = {
        { "access",             Z_ProcessingInformation_access,
                                ir_choice_nop },
        { "search",             Z_ProcessingInformation_search,
                                ir_choice_nop },
        { "retrieval",          Z_ProcessingInformation_retrieval,
                                ir_choice_nop },
        { "recordPresentation", Z_ProcessingInformation_record_presentation,
                                ir_choice_nop },
        { "recordHandling",     Z_ProcessingInformation_record_handling,
                                ir_choice_nop },
        { NULL, 0, NULL }};
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_DatabaseName (iea, p->databaseName, "databaseName", argi);

    ir_choice (iea, arm, p->processingContext, odr_nullval(), argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_oid (iea, p->oid, "oid", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_External (iea, p->instructions, "instructions", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_VariantSetInfo (IrExpArg *iea,
            Z_VariantSetInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_oid (iea, p->variantSet, "variantSet", argi);
    ir_InternationalString (iea, p->name, "name", argi);
    ir_sequence (ir_VariantClass, iea, p->variants,
                 &p->num_variants, "variants", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_VariantClass (IrExpArg *iea,
            Z_VariantClass *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_integer (iea, p->variantClass, "variantClass", argi);
    ir_sequence (ir_VariantType, iea, p->variantTypes,
                 &p->num_variantTypes, "variantTypes", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_VariantType (IrExpArg *iea,
            Z_VariantType *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_integer (iea, p->variantType, "variantType", argi);
    ir_VariantValue (iea, p->variantValue, "variantValue", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ValueSetEnumerated (IrExpArg *iea,
            Z_ValueSetEnumerated *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_ValueDescription, iea, p->elements,
                 &p->num, "enumerated", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ValueSet (IrExpArg *iea,
            Z_ValueSet *p, const char *name, int argi)
{
    IrExpChoice arm [] = {
        { "range",      Z_ValueSet_range,      ir_ValueRange },
        { "enumerated", Z_ValueSet_enumerated, ir_ValueSetEnumerated },
        { NULL, 0, NULL }};
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.range, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_VariantValue (IrExpArg *iea,
            Z_VariantValue *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_PrimitiveDataType (iea, p->dataType, "dataType", argi);
    if (p->values)
        ir_ValueSet (iea, p->values, "values", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ValueRange (IrExpArg *iea,
            Z_ValueRange *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_ValueDescription (iea, p->lower, "lower", argi);
    ir_ValueDescription (iea, p->upper, "upper", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ValueDescription (IrExpArg *iea,
            Z_ValueDescription *p, const char *name, int argi)
{
    static IrExpChoice arm [] = {
        { "integer",      Z_ValueDescription_integer, ir_integer },
        { "string",       Z_ValueDescription_string,  ir_InternationalString},
        { "octets",       Z_ValueDescription_octets,  ir_octet},
        { "oid",          Z_ValueDescription_oid,     ir_oid},
        { "unit",         Z_ValueDescription_unit,    ir_Unit},
        { "valueAndUnit", Z_ValueDescription_valueAndUnit, ir_IntUnit},
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.integer, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_UnitInfo (IrExpArg *iea,
            Z_UnitInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_InternationalString (iea, p->unitSystem, "unitSystem", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_sequence (ir_UnitType, iea, p->units,
                 &p->num_units, "units", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_UnitType (IrExpArg *iea,
            Z_UnitType *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_StringOrNumeric (iea, p->unitType, "unitType", argi);
    ir_sequence (ir_Units, iea, p->units, &p->num_units, "units", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Units (IrExpArg *iea,
            Z_Units *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_StringOrNumeric (iea, p->unit, "unit", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_CategoryList (IrExpArg *iea,
            Z_CategoryList *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_CommonInfo (iea, p->commonInfo, "commonInfo", argi);
    ir_sequence (ir_CategoryInfo, iea, p->categories,
                 &p->num_categories, "categories", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_CategoryInfo (IrExpArg *iea,
            Z_CategoryInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->category, "category", argi);
    ir_InternationalString (iea, p->originalCategory, "originalCategory", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_InternationalString (iea, p->asn1Module, "asn1Module", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_CommonInfo (IrExpArg *iea,
            Z_CommonInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_GeneralizedTime (iea, p->dateAdded, "dateAdded", argi);
    ir_GeneralizedTime (iea, p->dateChanged, "dateChanged", argi);
    ir_GeneralizedTime (iea, p->expiry, "expiry", argi);
    ir_LanguageCode (iea, p->humanStringLanguage, "humanString-Language",
                     argi);
    ir_OtherInformation (iea, p->otherInfo, "otherInfo", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_HumanStringUnit (IrExpArg *iea,
            Z_HumanStringUnit *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_LanguageCode (iea, p->language, "language", argi);
    ir_InternationalString (iea, p->text, "text", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_HumanString (IrExpArg *iea,
            Z_HumanString *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_HumanStringUnit, iea, p->strings,
                 &p->num_strings, "strings", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_IconObjectUnit (IrExpArg *iea,
            Z_IconObjectUnit *p, const char *name, int argi)
{
    static IrExpChoice arm [] = {
        { "ianaType",     Z_IconObjectUnit_ianaType,  ir_InternationalString },
        { "z3950type",    Z_IconObjectUnit_z3950type, ir_InternationalString },
        { "otherType",    Z_IconObjectUnit_otherType, ir_InternationalString },
        { NULL, 0, NULL }};
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, odr_nullval(), argi);
    ir_octet (iea, p->content, "content", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_IconObject (IrExpArg *iea,
            Z_IconObject *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_IconObjectUnit, iea, p->elements,
                 &p->num, "iconUnits", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ContactInfo (IrExpArg *iea,
            Z_ContactInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->name, "name", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_HumanString (iea, p->address, "address", argi);
    ir_InternationalString (iea, p->email, "email", argi);
    ir_InternationalString (iea, p->phone, "phone", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_NetworkAddressIA (IrExpArg *iea,
            Z_NetworkAddressIA *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->hostAddress, "hostAddress", argi);
    ir_integer (iea, p->port, "port", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_NetworkAddressOPA (IrExpArg *iea,
            Z_NetworkAddressOPA *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->pSel, "pSel", argi);
    ir_InternationalString (iea, p->sSel, "sSel", argi);
    ir_InternationalString (iea, p->tSel, "tSel", argi);
    ir_InternationalString (iea, p->nSap, "nSap", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_NetworkAddressOther (IrExpArg *iea,
            Z_NetworkAddressOther *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->type, "type", argi);
    ir_InternationalString (iea, p->address, "address", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_NetworkAddress (IrExpArg *iea,
            Z_NetworkAddress *p, const char *name, int argi)
{
    IrExpChoice arm [] = {
        { "iA",    Z_NetworkAddress_iA,    ir_NetworkAddressIA },
        { "oPA",   Z_NetworkAddress_oPA,   ir_NetworkAddressOPA },
        { "other", Z_NetworkAddress_other, ir_NetworkAddressOther },
        { NULL, 0, NULL }};
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.internetAddress, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AccessInfo (IrExpArg *iea,
            Z_AccessInfo *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_QueryTypeDetails, iea, p->queryTypesSupported,
                 &p->num_queryTypesSupported, "queryTypesSupported", argi);
    ir_sequence (ir_oid, iea, p->diagnosticsSets,
                 &p->num_diagnosticsSets, "diagnosticsSets", argi);
    ir_sequence (ir_oid, iea, p->attributeSetIds,
                 &p->num_attributeSetIds, "attributeSetIds", argi);
    ir_sequence (ir_oid, iea, p->schemas,
                 &p->num_schemas, "schemas", argi);
    ir_sequence (ir_oid, iea, p->recordSyntaxes,
                 &p->num_recordSyntaxes, "recordSyntaxes", argi);
    ir_sequence (ir_oid, iea, p->resourceChallenges,
                 &p->num_resourceChallenges, "resourceChallenges", argi);
    ir_AccessRestrictions (iea, p->restrictedAccess, "restrictedAccess", argi);
    ir_Costs (iea, p->costInfo, "costInfo", argi);
    ir_sequence (ir_oid, iea, p->variantSets,
                 &p->num_variantSets, "variantSets", argi);
    ir_sequence (ir_ElementSetName, iea, p->elementSetNames,
                 &p->num_elementSetNames, "elementSetNames", argi);
    ir_sequence (ir_InternationalString, iea, p->unitSystems,
                 &p->num_unitSystems, "unitSystems", argi);
    return ir_match_end (name, iea, argi);
}


static int ir_QueryTypeDetails (IrExpArg *iea,
            Z_QueryTypeDetails *p, const char *name, int argi)
{
    static IrExpChoice arm[] = {
        { "private",    Z_QueryTypeDetails_private,
                        ir_PrivateCapabilities },
        { "rpn",        Z_QueryTypeDetails_rpn,
                        ir_RpnCapabilities },
        { "iso8777",    Z_QueryTypeDetails_iso8777,
                        ir_Iso8777Capabilities },
        { "z39_58",      Z_QueryTypeDetails_z39_58,
                        ir_HumanString },
        { "erpn",       Z_QueryTypeDetails_erpn,
                        ir_RpnCapabilities },
        { "rankedList", Z_QueryTypeDetails_rankedList,
                        ir_HumanString },
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.zprivate, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_PrivateCapOperator (IrExpArg *iea,
            Z_PrivateCapOperator *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->roperator, "operator", argi);
    ir_HumanString (iea, p->description, "description", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_PrivateCapabilities (IrExpArg *iea,
            Z_PrivateCapabilities *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;

    ir_sequence (ir_PrivateCapOperator, iea, p->operators,
                 &p->num_operators, "operators", argi);
    ir_sequence (ir_SearchKey, iea, p->searchKeys,
                 &p->num_searchKeys, "searchKeys", argi);
    ir_sequence (ir_HumanString, iea, p->description,
                 &p->num_description, "description", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_RpnCapabilities (IrExpArg *iea,
            Z_RpnCapabilities *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_integer, iea, p->operators, 
                 &p->num_operators, "operators", argi);
    ir_bool (iea, p->resultSetAsOperandSupported,
             "resultSetAsOperandSupported", argi);
    ir_bool (iea, p->restrictionOperandSupported,
             "restrictionOperandSupported", argi);
    ir_ProximitySupport (iea, p->proximity, "proximity", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Iso8777Capabilities (IrExpArg *iea,
            Z_Iso8777Capabilities *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_SearchKey, iea, p->searchKeys,
                 &p->num_searchKeys, "searchKeys", argi);
    ir_HumanString (iea, p->restrictions, "restrictions", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ProxSupportPrivate (IrExpArg *iea,
            Z_ProxSupportPrivate *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_integer (iea, p->unit, "unit", argi);
    ir_HumanString (iea, p->description, "description", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ProxSupportUnit (IrExpArg *iea,
            Z_ProxSupportUnit *p, const char *name, int argi)
{
    static IrExpChoice arm [] = {
        { "known",   Z_ProxSupportUnit_known,   ir_integer },
        { "private", Z_ProxSupportUnit_private, ir_ProxSupportPrivate },
        { NULL, 0, NULL }};
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.zprivate, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ProximitySupport (IrExpArg *iea,
            Z_ProximitySupport *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_bool (iea, p->anySupport, "anySupport", argi);
    ir_sequence (ir_ProxSupportUnit, iea, p->unitsSupported,
                 &p->num_unitsSupported, "unitsSupported", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_SearchKey (IrExpArg *iea,
            Z_SearchKey *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->searchKey, "searchKey", argi);
    ir_HumanString (iea, p->description, "description", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AccessRestrictionsUnit (IrExpArg *iea,
            Z_AccessRestrictionsUnit *p, const char *name, int argi)
{
    static IrExpChoice arm[] = {
        { "any",               Z_AccessRestrictionsUnit_any,
                               ir_choice_nop },
        { "search",            Z_AccessRestrictionsUnit_search,
                               ir_choice_nop },
        { "present",           Z_AccessRestrictionsUnit_present,
                               ir_choice_nop },
        { "specificElements",  Z_AccessRestrictionsUnit_specific_elements,
                               ir_choice_nop },
        { "extendedServices",  Z_AccessRestrictionsUnit_extended_services,
                               ir_choice_nop },
        { "byDatabase",        Z_AccessRestrictionsUnit_by_database,
                               ir_choice_nop },
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, p->accessType, odr_nullval(), argi);
    ir_HumanString (iea, p->accessText, "accessText", argi);
    ir_sequence (ir_oid, iea, p->accessChallenges,
                 &p->num_accessChallenges, "accessChallenges", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AccessRestrictions (IrExpArg *iea,
            Z_AccessRestrictions *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_AccessRestrictionsUnit, iea, p->elements,
                 &p->num, "restrictions", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_CostsOtherCharge (IrExpArg *iea,
            Z_CostsOtherCharge *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_HumanString (iea, p->forWhat, "forWhat", argi);
    ir_Charge (iea, p->charge, "charge", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Costs (IrExpArg *iea,
            Z_Costs *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_Charge (iea, p->connectCharge, "connectCharge", argi);
    ir_Charge (iea, p->connectTime, "connectTime", argi);
    ir_Charge (iea, p->displayCharge, "displayCharge", argi);
    ir_Charge (iea, p->searchCharge, "searchCharge", argi);
    ir_Charge (iea, p->subscriptCharge, "subscriptCharge", argi);

    ir_sequence (ir_CostsOtherCharge, iea, p->otherCharges,
                 &p->num_otherCharges, "otherCharges", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Charge (IrExpArg *iea,
            Z_Charge *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_IntUnit (iea, p->cost, "cost", argi);
    ir_Unit (iea, p->perWhat, "perWhat", argi);
    ir_HumanString (iea, p->text, "text", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_DatabaseList (IrExpArg *iea,
            Z_DatabaseList *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_DatabaseName, iea, p->databases,
                 &p->num_databases, "databases", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeCombinations (IrExpArg *iea,
            Z_AttributeCombinations *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_oid (iea, p->defaultAttributeSet, "defaultAttributeSet", argi);
    ir_sequence (ir_AttributeCombination, iea, p->legalCombinations,
                 &p->num_legalCombinations, "legalCombinations", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeCombination (IrExpArg *iea,
            Z_AttributeCombination *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_AttributeOccurrence, iea, p->occurrences,
                 &p->num_occurrences, "occurrences", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeValueList (IrExpArg *iea,
            Z_AttributeValueList *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_StringOrNumeric, iea, p->attributes,
                 &p->num_attributes, "attributes", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeOccurrence (IrExpArg *iea,
            Z_AttributeOccurrence *p, const char *name, int argi)
{
    static IrExpChoice arm [] = {
        { "anyOrNone",   Z_AttributeOcc_any_or_none, ir_null },
        { "specific",    Z_AttributeOcc_specific, 
	  ir_AttributeValueList },
        { NULL, 0, NULL } };
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_oid (iea, p->attributeSet, "attributeSet", argi);
    ir_integer (iea, p->attributeType, "attributeType", argi);
    ir_null (iea, p->mustBeSupplied, "mustBeSupplied", argi);
    ir_choice (iea, arm, &p->which, p->attributeValues.any_or_none, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_AttributeValue (IrExpArg *iea,
            Z_AttributeValue *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_StringOrNumeric (iea, p->value, "value", argi);
    ir_HumanString (iea, p->description, "description", argi);
    ir_sequence (ir_StringOrNumeric, iea, p->subAttributes,
                 &p->num_subAttributes, "subAttributes", argi);
    ir_sequence (ir_StringOrNumeric, iea, p->superAttributes,
                 &p->num_superAttributes, "superAttributes", argi);
    ir_null (iea, p->partialSupport, "partialSupport", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_StringOrNumeric (IrExpArg *iea,
            Z_StringOrNumeric *p, const char *name, int argi)
{
    IrExpChoice arm[] = {
        { "string",  Z_StringOrNumeric_string,  ir_InternationalString },
        { "numeric", Z_StringOrNumeric_numeric, ir_integer },
        { NULL, 0, NULL }};
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.string, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_ElementSpec (IrExpArg *iea,
            Z_ElementSpec *p, const char *name, int argi)
{
    static IrExpChoice arm[] = {
        { "elementSetName", Z_ElementSpec_elementSetName,
                            ir_InternationalString },
        { "externalSpec",   Z_ElementSpec_externalSpec,
                            ir_External },
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_choice (iea, arm, &p->which, p->u.elementSetName, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Specification (IrExpArg *iea,
            Z_Specification *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
#if YAZ_VERSIONL >= 0x010903L
    if (p->which == Z_Schema_oid)
       ir_oid (iea, p->schema.oid, "schema", argi);
#else
    ir_oid (iea, p->schema, "schema", argi);
#endif
    ir_ElementSpec (iea, p->elementSpec, "elementSpec", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_InfoCategory (IrExpArg *iea,
            Z_InfoCategory *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_oid (iea, p->categoryTypeId, "categoryTypeId", argi);
    ir_integer (iea, p->categoryValue, "categoryValue", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_OtherInformationUnit (IrExpArg *iea,
            Z_OtherInformationUnit *p, const char *name, int argi)
{
    static IrExpChoice arm[] = {
        { "characterInfo",
	  Z_OtherInfo_characterInfo, ir_InternationalString },
        { "binaryInfo",
	  Z_OtherInfo_binaryInfo, ir_octet},
        { "externallyDefinedInfo",
	  Z_OtherInfo_externallyDefinedInfo,
	  ir_External},
        { "oid",
	  Z_OtherInfo_oid, ir_oid},
        { NULL, 0, NULL }};

    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InfoCategory (iea, p->category, "category", argi);
    ir_choice (iea, arm, &p->which, p->information.characterInfo, argi);
    return ir_match_end (name, iea, argi);
}

static int ir_OtherInformation (IrExpArg *iea,
            Z_OtherInformation *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_sequence (ir_OtherInformationUnit, iea, p->list,
                 &p->num_elements, "list", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_IntUnit (IrExpArg *iea,
            Z_IntUnit *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_integer (iea, p->value, "value", argi);
    ir_Unit (iea, p->unitUsed, "unitUsed", argi);
    return ir_match_end (name, iea, argi);
}

static int ir_Unit (IrExpArg *iea,
            Z_Unit *p, const char *name, int argi)
{
    if (!ir_match_start (name, p, iea, ++argi))
        return TCL_OK;
    ir_InternationalString (iea, p->unitSystem, "unitSystem", argi);
    ir_StringOrNumeric (iea, p->unitType, "unitType", argi);
    ir_StringOrNumeric (iea, p->unit, "unit", argi);
    ir_integer (iea, p->scaleFactor, "scaleFactor", argi);
    return ir_match_end (name, iea, argi);
}

int ir_ExplainRecord (IrExpArg *iea, Z_ExplainRecord *p, int argi)
{
    static IrExpChoice arm[] = {
        {"targetInfo",              Z_Explain_targetInfo,
                                           ir_TargetInfo},
        {"databaseInfo",            Z_Explain_databaseInfo,
                                           ir_DatabaseInfo},
        {"schemaInfo",              Z_Explain_schemaInfo,
                                           ir_SchemaInfo},
        {"tagSetInfo",              Z_Explain_tagSetInfo,
                                           ir_TagSetInfo},
        {"recordSyntaxInfo",        Z_Explain_recordSyntaxInfo,
                                           ir_RecordSyntaxInfo},
        {"attributeSetInfo",        Z_Explain_attributeSetInfo,
                                           ir_AttributeSetInfo},
        {"termListInfo",            Z_Explain_termListInfo,
                                           ir_TermListInfo},
        {"extendedServicesInfo",    Z_Explain_extendedServicesInfo,
                                           ir_ExtendedServicesInfo},
        {"attributeDetails",        Z_Explain_attributeDetails,
                                           ir_AttributeDetails},
        {"termListDetails",         Z_Explain_termListDetails,
                                           ir_TermListDetails},
        {"elementSetDetails",       Z_Explain_elementSetDetails,
                                           ir_ElementSetDetails},
        {"retrievalRecordDetails",  Z_Explain_retrievalRecordDetails,
                                           ir_RetrievalRecordDetails},
        {"sortDetails",             Z_Explain_sortDetails,
                                           ir_SortDetails},
        {"processing",              Z_Explain_processing,
                                           ir_ProcessingInformation},
        {"variants",                Z_Explain_variants,
                                           ir_VariantSetInfo},
        {"units",                   Z_Explain_units,
                                           ir_UnitInfo},
        {"categoryList",            Z_Explain_categoryList,
                                           ir_CategoryList},
        {NULL,                         0,   NULL }};
        

    return ir_choice (iea, arm, &p->which, p->u.targetInfo, argi);
}

int ir_tcl_get_explain (Tcl_Interp *interp, Z_ExplainRecord *rec,
                        int argc, char **argv)
{
    IrExpArg iea;

    iea.argv = argv;
    iea.argc = argc;
    iea.interp = interp;

    return ir_ExplainRecord (&iea, rec, 2);
}
