/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * See the file LICENSE for details.
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: marc.c,v $
 * Revision 1.9  1996-07-03 13:31:13  adam
 * The xmalloc/xfree functions from YAZ are used to manage memory.
 *
 * Revision 1.8  1995/11/14  16:48:00  adam
 * Bug fix: record extraction in line mode merged lines with same tag.
 *
 * Revision 1.7  1995/11/09  15:24:02  adam
 * Allow charsets [..] in record match.
 *
 * Revision 1.6  1995/08/28  12:21:22  adam
 * Removed lines and list as synonyms of list in MARC extractron.
 * Configure searches also for tk4.0 / tcl7.4.
 *
 * Revision 1.5  1995/06/30  12:39:26  adam
 * Bug fix: loadFile didn't set record type.
 * The MARC routines are a little less strict in the interpretation.
 * Script display.tcl replaces the old marc.tcl.
 * New interactive script: shell.tcl.
 *
 * Revision 1.4  1995/06/22  13:15:09  adam
 * Feature: SUTRS. Setting getSutrs implemented.
 * Work on display formats.
 * Preferred record syntax can be set by the user.
 *
 * Revision 1.3  1995/05/29  08:44:26  adam
 * Work on delete of objects.
 *
 * Revision 1.2  1995/05/26  11:44:11  adam
 * Bugs fixed. More work on MARC utilities and queries. Test
 * client is up-to-date again.
 *
 * Revision 1.1  1995/05/26  08:54:19  adam
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

static int atoi_n (const char *buf, int len)
{
    int val = 0;

    if (!isdigit (buf[len-1]))
        return 0;
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
    int ch;

    if (*p == '*')
        return 0;
    if (!f)
        return -*p;
    for (; (ch = *p) && *f; f++, p++)
        switch (*p)
        {
        case '*':
            return 0;
        case '?':
            ch = *f;
            break;
        case '[':
            while (1)
                if (!*++p)
                    break;
                else if (*p == ']')
                {
                    p++;
                    break;
                }
                else if (*p == *f)
                    ch = *p;
            if (ch != *p)
                return *f - ch;
            break;
        default:
            if (ch != *f)
                return *f - ch;
        }
    return *f - ch;
}

char *ir_tcl_fread_marc (FILE *inf, size_t *size)
{
    char length[5];
    char *buf;

    if (fread (length, 1, 5, inf) != 5)
        return NULL;
    *size = atoi_n (length, 5);
    if (*size <= 6)
        return NULL;
    if (!(buf = xmalloc (*size+1)))
        return NULL;
    if (fread (buf+5, 1, *size-5, inf) != (*size-5))
    {
        xfree (buf);
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
    char ptag[4];
    int mode = 0;

    if (!strcmp (argv[3], "field"))
        mode = 'f';
    else if (!strcmp (argv[3], "line"))
        mode = 'l';
    else
    {
        Tcl_AppendResult (interp, "Unknown MARC extract mode", NULL);
	return TCL_ERROR;
    }
    if (!buf)
    {
        Tcl_AppendResult (interp, "Not a MARC record", NULL);
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

        *ptag = '\0';
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
            int i0;

            if (memcmp (tag, "00", 2) && identifier_length)
	    {
	        i++;
                for (j = 1; j<identifier_length; j++)
		    identifier[j-1] = buf[i++];
		identifier[j-1] = '\0';
	        for (i0 = i; buf[i] != ISO2709_RS && 
		             buf[i] != ISO2709_IDFS &&
	                     buf[i] != ISO2709_FS && i < end_offset; 
			     i++)
		    ;
	    }
	    else
	    {
	        for (i0 = i; buf[i] != ISO2709_RS && 
	                     buf[i] != ISO2709_FS && i < end_offset; 
			     i++)
		    ;
                *identifier = '\0';
	    }
            if (marc_compare (identifier, argv[6])==0)
            {
                char *data = xmalloc (i-i0+1);
             
                memcpy (data, buf+i0, i-i0);
                data[i-i0] = '\0';
                if (mode == 'l')
                {
                    if (strcmp (tag, ptag))
                    {
                        if (*ptag)
                            Tcl_AppendResult (interp, "}} ", NULL);
                        if (!*indicator)
                            Tcl_AppendResult (interp, "{", tag, " {} {", NULL);
                        else
                            Tcl_AppendResult (interp, "{", tag, " {",
                                              indicator, "} {", NULL);
                        strcpy (ptag, tag);
                    }
                    if (!*identifier)
                        Tcl_AppendResult (interp, "{{}", NULL);
                    else
                        Tcl_AppendResult (interp, "{", identifier, NULL);
                    Tcl_AppendElement (interp, data);
                    Tcl_AppendResult (interp, "} ", NULL);
                }
                else
                    Tcl_AppendElement (interp, data);
                xfree (data);
            }
	}
        if (mode == 'l' && *ptag)
            Tcl_AppendResult (interp, "}} ", NULL);
	if (i < end_offset)
            logf (LOG_WARN, "MARC: separator but not at end of field");
	if (buf[i] != ISO2709_RS && buf[i] != ISO2709_FS)
            logf (LOG_WARN, "MARC: no separator at end of field");
    }
    return TCL_OK;
}



