/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: grs.c,v $
 * Revision 1.1  1995-08-29 15:38:34  adam
 * Added grs.c. new version.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "ir-tclp.h"

void ir_tcl_read_grs (Z_GenericRecord *r, IrTcl_GRS_Record **grs_record)
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

        if (t->content->which == Z_ElementData_subtree)
            ir_tcl_read_grs (t->content->u.subtree, &e->tagData.sub);
        else if (t->content->which == Z_ElementData_string)
            ir_tcl_strdup (NULL, &e->tagData.str, t->content->u.string);
    }
}

static int ir_tcl_get_grs_r (Tcl_Interp *interp, IrTcl_GRS_Record *grs_record,
                             int argc, char **argv, int argno)
{
    static char tmpbuf[32];
    int i;
    struct GRS_Record_entry *e = grs_record->entries;

    if (argno >= argc)
    {
        Tcl_AppendResult (interp, "{ ", NULL);

        for (i = 0; i<grs_record->noTags; i++, e++)
        {

            Tcl_AppendResult (interp, "{ ", NULL);
            sprintf (tmpbuf, "%d", e->tagType);
            Tcl_AppendElement (interp, tmpbuf);

            if (e->tagWhich == Z_StringOrNumeric_numeric)
            {
                Tcl_AppendElement (interp, "N");
                sprintf (tmpbuf, "%d", e->tagVal.num);
                Tcl_AppendElement (interp, tmpbuf);
            }
            else
            {
                Tcl_AppendResult (interp, " S ", NULL);
                Tcl_AppendElement (interp, e->tagVal.str);
            }
            if (e->dataWhich == Z_ElementData_subtree)
            {
                Tcl_AppendResult (interp, " R ", NULL);
                ir_tcl_get_grs_r (interp, e->tagData.sub, argc, argv, argno+1);
            }
            else
            {
                Tcl_AppendElement (interp, "S");
                if (e->tagData.str)
                    Tcl_AppendElement (interp, e->tagData.str );
                else
                    Tcl_AppendResult (interp, " {} ", NULL);
            }
            Tcl_AppendResult (interp, " }", NULL);
        }
    }
    return TCL_OK;
}

int ir_tcl_get_grs (Tcl_Interp *interp, IrTcl_GRS_Record *grs_record, 
                     int argc, char **argv)
{
    return ir_tcl_get_grs_r (interp, grs_record, argc, argv, 4);
}
    
