/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 * Sebastian Hammer, Adam Dickmeiss
 *
 * $Log: ir-tcl.h,v $
 * Revision 1.5  1995-03-20 08:53:27  adam
 * Event loop in tclmain.c rewritten. New method searchStatus.
 *
 * Revision 1.4  1995/03/17  18:26:18  adam
 * Non-blocking i/o used now. Database names popup as cascade items.
 *
 * Revision 1.3  1995/03/17  07:50:28  adam
 * Headers have changed a little.
 *
 */

int ir_tcl_init (Tcl_Interp *interp);

void ir_select_add          (int fd, void *obj);
void ir_select_add_write    (int fd, void *obj);
void ir_select_remove       (int fd, void *obj);
void ir_select_remove_write (int fd, void *obj);
void ir_select_read         (ClientData clientData);
void ir_select_write        (ClientData clientData);
