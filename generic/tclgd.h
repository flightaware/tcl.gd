/*
 * tcl.gd include file
 *
 * Copyright (C) 2005 -2013 by Karl Lehenbauer, All Rights Reserved
 *
 * Redistributable under the terms of the Berkeley copyright
 */

#include <tcl.h>
#include <gd.h>

extern int
tclgd_GDObjCmd(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
tclgd_newGDObject (Tcl_Interp *interp, Tcl_Obj *nameObj, gdImagePtr im, int destroyOnDelete);

extern int
tclgd_newGDObjectAttach (Tcl_Interp *interp, char *name, gdImagePtr im);

typedef struct tclgd_channelIOCtx
{
    gdIOCtx      ctx;
    Tcl_Channel  channel;
} tclgd_channelIOCtx;

typedef struct tclgd_clientData
{
    gdImagePtr   im;
    int          destroyOnDelete;
    Tcl_Command  cmdToken;
} tclgd_clientData;

gdIOCtx *tclgd_newChannelCtx (Tcl_Channel channel);

gdIOCtx *
tclgd_channelNameToIOCtx (Tcl_Interp *interp, char *channelName, int modeFlag);

