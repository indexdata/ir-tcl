/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: marc.c,v $
 * Revision 1.1  1995-05-26 08:54:19  adam
 * New MARC utilities. Uses prefix query.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "ir-tclp.h"

#define ISO2709_RS 035
#define ISO2709_FS 036
#define ISO2709_IDFS 037

int atoi_n (const char *buf, int len)
{
    int val = 0;

    while (--len >= 0)
    {
        if (isdigit (*buf))
            val = val*10 + (*buf - '0');
	buf++;
    }
    return val;
}

static int marc_compare (const char *f, const char *p)
{
    if (*p == '*')
        return 0;
    if (!f)
        return -*p;
    for (; *f && *p; f++, p++)
    {
        if (*p == '?')
	    continue;
	if (*p != *f)
	    break;
    }
    return *f - *p;
}

FILE *outf = stderr;

char *ir_tcl_fread_marc (FILE *inf, size_t *size)
{
    char length[5];
    char *buf;

    if (fread (length, 1, 5, inf) != 5)
        return NULL;
    *size = atoi_n (length, 5);
    if (*size <= 6)
        return NULL;
    if (!(buf = malloc (*size+1)))
        return NULL;
    if (fread (buf+5, 1, *size-5, inf) != (*size-5))
    {
        free (buf);
	return NULL;
    }
    memcpy (buf, length, 5);
    buf[*size=0] = '\0';
    return buf;
}

int ir_tcl_get_marc (Tcl_Interp *interp, const char *buf, 
                     int argc, char **argv)
{
    int entry_p;
    int record_length;
    int indicator_length;
    int identifier_length;
    int base_address;
    int length_data_entry;
    int length_starting;
    int length_implementation;
    int mode = 0;

    if (!strcmp (argv[3], "field"))
        mode = 'f';
    else if (!strcmp (argv[3], "lines"))
        mode = 'l';
    else
    {
        Tcl_AppendResult (interp, "Unknown MARC extract mode", NULL);
	return TCL_ERROR;
    }
    record_length = atoi_n (buf, 5);
    if (record_length < 25)
    {
        Tcl_AppendResult (interp, "Not a MARC record", NULL);
        return TCL_ERROR;
    }
    indicator_length = atoi_n (buf+10, 1);
    identifier_length = atoi_n (buf+11, 1);
    base_address = atoi_n (buf+12, 4);

    length_data_entry = atoi_n (buf+20, 1);
    length_starting = atoi_n (buf+21, 1);
    length_implementation = atoi_n (buf+22, 1);

    for (entry_p = 24; buf[entry_p] != ISO2709_FS; )
        entry_p += 3+length_data_entry+length_starting;
    base_address = entry_p+1;
    for (entry_p = 24; buf[entry_p] != ISO2709_FS; )
    {
        int data_length;
	int data_offset;
	int end_offset;
	int i, j;
	char tag[4];
	char indicator[128];
	char identifier[128];

        memcpy (tag, buf+entry_p, 3);
	entry_p += 3;
        tag[3] = '\0';
	data_length = atoi_n (buf+entry_p, length_data_entry);
	entry_p += length_data_entry;
	data_offset = atoi_n (buf+entry_p, length_starting);
	entry_p += length_starting;
	i = data_offset + base_address;
	end_offset = i+data_length-1;
	*indicator = '\0';
        if (memcmp (tag, "00", 2) && indicator_length)
	{
            for (j = 0; j<indicator_length; j++)
	        indicator[j] = buf[i++];
	    indicator[j] = '\0';
	}
	if (marc_compare (tag, argv[4]) || marc_compare (indicator, argv[5]))
	    continue;
	while (buf[i] != ISO2709_RS && buf[i] != ISO2709_FS && i < end_offset)
	{
            if (memcmp (tag, "00", 2) && identifier_length)
	    {
	        int i0;
	        i++;
                for (j = 1; j<identifier_length; j++)
		    identifier[j-1] = buf[i++];
		identifier[j-1] = '\0';
	        for (i0 = i; buf[i] != ISO2709_RS && 
		             buf[i] != ISO2709_IDFS &&
	                     buf[i] != ISO2709_FS && i < end_offset; 
			     i++)
		    ;
		if (marc_compare (identifier, argv[6])==0)
		{
                    char *data = malloc (i-i0+1);

		    memcpy (data, buf+i0, i-i0);
		    data[i-i0] = '\0';
                    Tcl_AppendElement (interp, data);
		    free (data);
		}
	    }
	    else
	    {
	        int i0;

	        for (i0 = i; buf[i] != ISO2709_RS && 
	                     buf[i] != ISO2709_FS && i < end_offset; 
			     i++)
		    ;
		if (marc_compare (NULL, argv[6])==0)
		{
                    char *data = malloc (i-i0+1);

		    memcpy (data, buf+i0, i-i0);
		    data[i-i0] = '\0';
                    Tcl_AppendElement (interp, data);
		    free (data);
		}

	    }
	}
	if (i < end_offset)
	    fprintf (outf, "-- separator but not at end of field\n");
	if (buf[i] != ISO2709_RS && buf[i] != ISO2709_FS)
	    fprintf (outf, "-- no separator at end of field\n");
    }
    return TCL_OK;
}

#if 0
int ir_tcl_get_marc_fields(Tcl_Interp *interp, const char *buf,
                           size_t size, int argc, char **argv)
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

int ir_tcl_get_marc_lines(Tcl_Interp *interp, const char *buf,
                          size_t size, int argc, char **argv)
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
#endif
