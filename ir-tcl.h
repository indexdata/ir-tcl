
int ir_tcl_init (Tcl_Interp *interp);

void ir_select_add      (int fd, void *obj);
void ir_select_remove   (int fd, void *obj);
void ir_select_proc     (ClientData clientData);
