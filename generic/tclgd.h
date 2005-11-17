/*
 * CDATA
 *
 * $Id: tclgd.h,v 1.5 2005-11-17 05:48:19 karl Exp $
 *
 * Copyright (C) 2005 by Karl Lehenbauer, All Rights Reserved
 */

#include <tcl.h>
#include <gd.h>

extern int
tclgd_GDObjCmd(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

extern int
tclgd_newGDObject (Tcl_Interp *interp, Tcl_Obj *nameObj, gdImagePtr im);

typedef struct tclgd_channelIOCtx
{
    gdIOCtx      ctx;
    Tcl_Channel  channel;
} tclgd_channelIOCtx;

gdIOCtx *tclgd_newChannelCtx (Tcl_Channel channel);

gdIOCtx *
tclgd_channelNameToIOCtx (Tcl_Interp *interp, char *channelName, int modeFlag);

