/*
 * CDATA
 *
 * $Id: tclgd.h,v 1.4 2005-11-04 03:28:24 karl Exp $
 *
 * Copyright (C) 2005 by Karl Lehenbauer, All Rights Reserved
 */

#include <tcl.h>
#include <gd.h>

extern int
tclgd_GDObjCmd(ClientData dummy, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

typedef struct tclgd_channelIOCtx
{
    gdIOCtx      ctx;
    Tcl_Channel  channel;
} tclgd_channelIOCtx;

gdIOCtx *tclgd_newChannelCtx (Tcl_Channel channel);

gdIOCtx *
tclgd_channelNameToIOCtx (Tcl_Interp *interp, char *channelName, int modeFlag);

