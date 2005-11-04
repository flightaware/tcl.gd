/*
 * tclcdata.c --
 *
 * $Id: tclgdtcl.c,v 1.5 2005-11-04 02:58:09 karl Exp $
 */

#include "tclgd.h"

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT


/*
 *----------------------------------------------------------------------
 *
 * tclgd_Init --
 *
 *	Initialize the tclgd extension.  The string "tclgd" 
 *      in the function name must match the PACKAGE declaration at the top of
 *	configure.in.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The tweezer package is created.
 *	One new command "tclgd" is added to the Tcl interpreter.
 *
 *----------------------------------------------------------------------
 */

EXTERN int
Tclgd_Init(Tcl_Interp *interp)
{
    /*
     * This may work with 8.0, but we are using strictly stubs here,
     * which requires 8.1.
     */
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
	return TCL_ERROR;
    }

    if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL) {
	return TCL_ERROR;
    }

    if (Tcl_PkgProvide(interp, "tclgd", PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

    /* Create the tclgd command  */
    Tcl_CreateObjCommand(interp, "GD", (Tcl_ObjCmdProc *) tclgd_GDObjCmd, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * tclgd_SafeInit --
 *
 *	Initialize the tclgd in a safe interpreter.
 *
 *      Not safe at this time.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	Very little
 *
 *----------------------------------------------------------------------
 */

EXTERN int
Tclgd_SafeInit(Tcl_Interp *interp)
{
    return TCL_OK;
}

