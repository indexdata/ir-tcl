/*
 * IR toolkit for tcl/tk
 * (c) Index Data 1995
 *
 * $Log: ir-tcl.h,v $
 * Revision 1.3  1995-03-17 07:50:28  adam
 * Headers have changed a little.
 *
 */

int ir_tcl_init (Tcl_Interp *interp);

void ir_select_add      (int fd, void *obj);
void ir_select_remove   (int fd, void *obj);
void ir_select_proc     (ClientData clientData);
