/*
 * Copyright (c) 1995-1996, Index Data.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation, in whole or in part, for any purpose, is hereby granted,
 * provided that:
 *
 * 1. This copyright and permission notice appear in all copies of the
 * software and its documentation. Notices of copyright or attribution
 * which appear at the beginning of any file must remain unchanged.
 *
 * 2. The names of Index Data or the individual authors may not be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL INDEX DATA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR
 * NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * $Log: ir-tcl.h,v $
 * Revision 1.17  1999-02-11 11:30:11  adam
 * Updated for WIN32.
 *
 * Revision 1.16  1997/04/13 18:57:28  adam
 * Better error reporting and aligned with Tcl/Tk style.
 * Rework of notifier code with Tcl_File handles.
 *
 * Revision 1.15  1996/07/26 09:15:09  adam
 * IrTcl version 1.2 patch level 1.
 *
 * Revision 1.14  1996/02/21  10:16:19  adam
 * Simplified select handling. Only one function ir_tcl_select_set has
 * to be externally defined.
 *
 * Revision 1.13  1996/02/19  15:41:54  adam
 * Better log messages.
 * Minor improvement of connect method.
 *
 * Revision 1.12  1996/02/05  17:58:04  adam
 * Ported ir-tcl to use the beta releases of tcl7.5/tk4.1.
 *
 * Revision 1.11  1995/09/21  13:11:52  adam
 * Support of dynamic loading.
 * Test script uses load command if necessary.
 *
 * Revision 1.10  1995/06/22  07:15:59  adam
 * Version number moved to Makefile.in
 * Some work on autoconf.
 *
 * Revision 1.9  1995/06/19  13:54:21  adam
 * New version.
 *
 * Revision 1.8  1995/06/19  13:06:08  adam
 * New define: IR_TCL_VERSION.
 *
 * Revision 1.7  1995/05/29  08:44:23  adam
 * Work on delete of objects.
 *
 * Revision 1.6  1995/05/23  15:34:49  adam
 * Many new settings, userInformationField, smallSetUpperBound, etc.
 * A number of settings are inherited when ir-set is executed.
 * This version is incompatible with the graphical test client (client.tcl).
 *
 * Revision 1.5  1995/03/20  08:53:27  adam
 * Event loop in tclmain.c rewritten. New method searchStatus.
 *
 * Revision 1.4  1995/03/17  18:26:18  adam
 * Non-blocking i/o used now. Database names popup as cascade items.
 *
 * Revision 1.3  1995/03/17  07:50:28  adam
 * Headers have changed a little.
 *
 */

#ifndef IR_TCL_H
#define IR_TCL_H

#ifndef WIN32
int Irtcl_Init (Tcl_Interp *interp);
#endif

void *ir_tcl_malloc (size_t size);
void ir_tcl_select_set (void (*f)(ClientData clientData, int r, int w, int e),
                        int fd, ClientData clientData, int r, int w, int e);
#endif
