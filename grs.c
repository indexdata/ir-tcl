/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: grs.c,v $
 * Revision 1.7  1996-06-05 09:26:20  adam
 * Bug fix: the change above introduced an error.
 *
 * Revision 1.6  1996/06/05  08:59:23  adam
 * Changed syntax of element specs in GRS-1 retrieval.
 *
 * Revision 1.5  1996/05/29  20:28:08  adam
 * Bug fix: Function ir_tcl_grs_del sometimes free'd bad memory.
 *
 * Revision 1.4  1996/05/29  06:37:42  adam
 * Function ir_tcl_get_grs_r enhanced so that specific elements can be
 * extracted.
 *
 * Revision 1.3  1996/03/05 09:21:01  adam
 * Bug fix: memory used by GRS records wasn't freed.
 * Rewrote some of the error handling code - the connection is always
 * closed before failback is called.
 * If failback is defined the send APDU methods (init, search, ...) will
 * return OK but invoke failback (as is the case if the write operation
 * fails).
 * Bug fix: ref_count in assoc object could grow if fraction of PDU was
 * read.
 *
 * Revision 1.2  1995/09/20  11:37:01  adam
 * Configure searches for tk4.1 and tk7.5.
 * Work on GRS.
 *
 * Revision 1.1  1995/08/29  15:38:34  adam
 * Added grs.c. new version.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "ir-tclp.h"

void ir_tcl_grs_del (IrTcl_GRS_Record **grs_record)
{
    struct GRS_Record_entry *e;
    int i;

    while (!*grs_record)
        return;
    e = (*grs_record)->entries;
    for (i = 0; i < (*grs_record)->noTags; i++, e++)
    {
        switch (e->tagWhich)
        {
        case Z_StringOrNumeric_numeric:
            break;
        default:
            free (e->tagVal.str);
        }
        switch (e->dataWhich)
        {
        case Z_ElementData_octets:
            free (e->tagData.octets.buf);
            break;
        case Z_ElementData_numeric:
            break;
        case Z_ElementData_date:
            free (e->tagData.str);
            break;            
        case Z_ElementData_ext:
            break;
        case Z_ElementData_string:
	    free (e->tagData.str);
            break;
        case Z_ElementData_trueOrFalse:
        case Z_ElementData_oid:
        case Z_ElementData_intUnit:
        case Z_ElementData_elementNotThere:
        case Z_ElementData_elementEmpty:
        case Z_ElementData_noDataRequested:
        case Z_ElementData_diagnostic:
            break;
        case Z_ElementData_subtree:
            ir_tcl_grs_del (&e->tagData.sub);
            break;
        }
    }
    free ((*grs_record)->entries);
    free (*grs_record);
    *grs_record = NULL;
}

void ir_tcl_grs_mk (Z_GenericRecord *r, IrTcl_GRS_Record **grs_record)
{
    int i;
    struct GRS_Record_entry *e;

    *grs_record = NULL;
    if (!r)
        return;
    *grs_record = ir_tcl_malloc (sizeof(**grs_record));
    if (!((*grs_record)->noTags = r->num_elements))
    {
        (*grs_record)->entries = NULL;
        return;
    }
    e = (*grs_record)->entries = ir_tcl_malloc (r->num_elements *
                                                sizeof(*e));
    for (i = 0; i < r->num_elements; i++, e++)
    {
        Z_TaggedElement *t;

        t = r->elements[i];
        if (t->tagType)
            e->tagType = *t->tagType;
        else
            e->tagType = 0;
        e->tagWhich = t->tagValue->which;
        if (t->tagValue->which == Z_StringOrNumeric_numeric)
            e->tagVal.num = *t->tagValue->u.numeric;
        else
            ir_tcl_strdup (NULL, &e->tagVal.str, t->tagValue->u.string);
        e->dataWhich = t->content->which;

        switch (t->content->which)
        {
        case Z_ElementData_octets:
            e->tagData.octets.len = t->content->u.octets->len;
            e->tagData.octets.buf = ir_tcl_malloc (t->content->u.octets->len);
            memcpy (e->tagData.octets.buf, t->content->u.octets->buf, 
                    t->content->u.octets->len);
            break;
        case Z_ElementData_numeric:
            e->tagData.num = *t->content->u.numeric;
            break;
        case Z_ElementData_date:
            ir_tcl_strdup (NULL, &e->tagData.str, t->content->u.string);
            break;            
        case Z_ElementData_ext:
            break;
        case Z_ElementData_string:
            ir_tcl_strdup (NULL, &e->tagData.str, t->content->u.string);
            break;
        case Z_ElementData_trueOrFalse:
            e->tagData.bool = *t->content->u.trueOrFalse;
            break;
        case Z_ElementData_oid:
            break;
        case Z_ElementData_intUnit:
            break;
        case Z_ElementData_elementNotThere:
        case Z_ElementData_elementEmpty:
        case Z_ElementData_noDataRequested:
            break;
        case Z_ElementData_diagnostic:
            break;
        case Z_ElementData_subtree:
            ir_tcl_grs_mk (t->content->u.subtree, &e->tagData.sub);
            break;
        }
    }
}

static int ir_tcl_get_grs_r (Tcl_Interp *interp, IrTcl_GRS_Record *grs_record,
                             int argc, char **argv, int argno)
{
    static char tmpbuf[32];
    int i;
    struct GRS_Record_entry *e = grs_record->entries;

    for (i = 0; i<grs_record->noTags; i++, e++)
    {
        int yes = 0;
        if (argno >= argc)
            yes = 1;
        else
        {
            const char *cp0 = argv[argno];
            const char *cp1 = strchr (cp0, ',');

            if (!cp1 || cp1-cp0 < 1)
                yes = 1;
            else
            {
                if (*cp0 == '(')
                    cp0++;
                if (atoi(cp0) == e->tagType) 
                {
                    if (e->tagWhich == Z_StringOrNumeric_numeric)
                    {
                        if (atoi (cp1+1) == e->tagVal.num)
                            yes = 1;
                    }
                    else
                    {
                        int len = strlen(cp1+1);
                        if (cp1[len] == ')')
                            len--;
                        if (len && strlen(e->tagVal.str) == len &&
                            !memcmp (cp1+1, e->tagVal.str, len))
                            yes = 1;
                    }
                }
            }
        }
        if (!yes)
            continue;
        Tcl_AppendResult (interp, "{ ", NULL);
        sprintf (tmpbuf, "%d", e->tagType);
        Tcl_AppendElement (interp, tmpbuf);

        if (e->tagWhich == Z_StringOrNumeric_numeric)
        {
            Tcl_AppendResult (interp, " numeric ", NULL);
            sprintf (tmpbuf, "%d", e->tagVal.num);
            Tcl_AppendElement (interp, tmpbuf);
        }
        else
        {
            Tcl_AppendResult (interp, " string ", NULL);
            Tcl_AppendElement (interp, e->tagVal.str);
        }
        switch (e->dataWhich)
        {
        case Z_ElementData_octets:
            Tcl_AppendResult (interp, " octets {} ", NULL);
            break;
        case Z_ElementData_numeric:
            Tcl_AppendResult (interp, " numeric ", NULL);
            sprintf (tmpbuf, "%d", e->tagData.num);
            Tcl_AppendElement (interp, tmpbuf);
            break;
        case Z_ElementData_date:
            Tcl_AppendResult (interp, " date {} ", NULL);
            break;
        case Z_ElementData_ext:
            Tcl_AppendResult (interp, " ext {} ", NULL);
            break;
        case Z_ElementData_string:
            Tcl_AppendResult (interp, " string ", NULL);
            Tcl_AppendElement (interp, e->tagData.str);
            break;
        case Z_ElementData_trueOrFalse:
            Tcl_AppendResult (interp, " bool ",
                              e->tagData.bool ? "1" : "0", " ", NULL);
            break;
        case Z_ElementData_oid:
            Tcl_AppendResult (interp, " oid {} ", NULL);
            break;
        case Z_ElementData_intUnit:
            Tcl_AppendResult (interp, " intUnit {} ", NULL);
            break;
        case Z_ElementData_elementNotThere:
            Tcl_AppendResult (interp, " notThere {} ", NULL);
            break;
        case Z_ElementData_elementEmpty:
            Tcl_AppendResult (interp, " empty {} ", NULL);
            break;
        case Z_ElementData_noDataRequested:
            Tcl_AppendResult (interp, " notRequested {} ", NULL);
            break;
        case Z_ElementData_diagnostic:
            Tcl_AppendResult (interp, " diagnostic {} ", NULL);
            break;
        case Z_ElementData_subtree:
            Tcl_AppendResult (interp, " subtree { ", NULL);
            ir_tcl_get_grs_r (interp, e->tagData.sub, argc, argv, argno+1);
            Tcl_AppendResult (interp, " } ", NULL);
            break;
        }
        Tcl_AppendResult (interp, " } ", NULL);
    }
    return TCL_OK;
}

int ir_tcl_get_grs (Tcl_Interp *interp, IrTcl_GRS_Record *grs_record, 
                     int argc, char **argv)
{
    return ir_tcl_get_grs_r (interp, grs_record, argc, argv, 3);
}
    
