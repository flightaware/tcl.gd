/*
 * tclcdata.c --
 *
 * $Id: tclgdtcl.c,v 1.2 2005-10-30 18:50:50 karl Exp $
 */

#include <tcl.h>
#include "gdtcl.h"

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT


/*
 *----------------------------------------------------------------------
 *
 * gdtcl_Init --
 *
 *	Initialize the gdtcl extension.  The string "gdtcl" 
 *      in the function name must match the PACKAGE declaration at the top of
 *	configure.in.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The tweezer package is created.
 *	One new command "gdtcl" is added to the Tcl interpreter.
 *
 *----------------------------------------------------------------------
 */

EXTERN int
gdtcl_Init(Tcl_Interp *interp)
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

    if (Tcl_PkgProvide(interp, "gdtcl", PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

    /* Create the gdtcl command  */
    Tcl_CreateObjCommand(interp, "GD", (Tcl_ObjCmdProc *) gdtcl_gdtclObjCmd, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * gdtcl_SafeInit --
 *
 *	Initialize the gdtcl in a safe interpreter.
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
gdtcl_SafeInit(Tcl_Interp *interp)
{
    return TCL_OK;
}

