/*
 * CDATA
 *
 * $Id: tclgd.h,v 1.6 2005-11-25 09:15:32 karl Exp $
 *
 * Copyright (C) 2005 by Karl Lehenbauer, All Rights Reserved
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
} tclgd_clientData;

gdIOCtx *tclgd_newChannelCtx (Tcl_Channel channel);

gdIOCtx *
tclgd_channelNameToIOCtx (Tcl_Interp *interp, char *channelName, int modeFlag);

