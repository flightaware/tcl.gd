/*
 * tclgdio.c
 *
 * Implements the tcl.gd I/O interface to Tcl channels.
 *
 * Tcl channel interface written by Karl Lehenbauer,
 * modeled after the gd_io_file.c file in the gd package, which is documented
 * as having been written/modified 1999, Philip Warner.
 *
 */

#include "tclgd.h"

static int tclgd_channelGetbuf (gdIOCtx *, void *, int);
static int tclgd_channelPutbuf (gdIOCtx *, const void *, int);
static void tclgd_channelPutchar (gdIOCtx *, int);
static int  tclgd_channelGetchar (gdIOCtx * ctx);

static int tclgd_channelSeek (struct gdIOCtx *, const int);
static long tclgd_channelTell (struct gdIOCtx *);

static void tclgd_FreeChannelCtx (gdIOCtx * ctx);

/* return data as a dynamic pointer */
BGD_DECLARE(gdIOCtx *) tclgd_newChannelCtx (Tcl_Channel channel)
{
    tclgd_channelIOCtx *ctx;

    ctx = (tclgd_channelIOCtx *) ckalloc (sizeof (tclgd_channelIOCtx));
    if (ctx == NULL) {
	return NULL;
    }

    ctx->channel = channel;

    ctx->ctx.getC = tclgd_channelGetchar;
    ctx->ctx.putC = tclgd_channelPutchar;

    ctx->ctx.getBuf = tclgd_channelGetbuf;
    ctx->ctx.putBuf = tclgd_channelPutbuf;

    ctx->ctx.tell = tclgd_channelTell;
    ctx->ctx.seek = tclgd_channelSeek;

    ctx->ctx.gd_free = tclgd_FreeChannelCtx;

    return (gdIOCtx *) ctx;
}

static void
tclgd_FreeChannelCtx (gdIOCtx * ctx)
{
    /* printf("tclgd_FreeChannelCtx %lx\n", ctx); */
    ckfree ((char *) ctx);
}


static int
tclgd_channelPutbuf (gdIOCtx * ctx, const void *buf, int size)
{
    int writeResult;

    tclgd_channelIOCtx *tctx;

    tctx = (tclgd_channelIOCtx *) ctx;

    writeResult = Tcl_Write (tctx->channel, buf, size);

    if (writeResult != size) {
	Tcl_Panic ("image write of %d chars only did %d, is your channel's translation set to binary?", size, writeResult);
    }

    /* printf("tclgd_channelPutbuf ctx %lx buf %lx size %d result %d\n", ctx, buf, size, writeResult); */

    return writeResult;
}

static int
tclgd_channelGetbuf (gdIOCtx * ctx, void *buf, int size)
{
    tclgd_channelIOCtx *tctx;
    int readResult;

    tctx = (tclgd_channelIOCtx *) ctx;

    readResult = Tcl_Read (tctx->channel, buf, size);

    /* printf("tclgd_channelGetbuf ctx %lx buf %lx size %d result %d\n", ctx, buf, size, readResult); */

    return readResult;
}

static void
tclgd_channelPutchar (gdIOCtx * ctx, int a)
{
    char b;
    tclgd_channelIOCtx *tctx;

    tctx = (tclgd_channelIOCtx *) ctx;

    b = a;
    if (Tcl_Write (tctx->channel, &b, 1) != 1) {
	Tcl_Panic ("image write of 1 char didn't write 1 char, is your channel's translation set to binary?");
    }

    /* printf("tclgd_channelPutchar ctx %lx %d\n", ctx, a); */
}

static int
tclgd_channelGetchar (gdIOCtx * ctx)
{
    unsigned char b[2];
    tclgd_channelIOCtx *tctx;
    tctx = (tclgd_channelIOCtx *) ctx;

    Tcl_Read (tctx->channel, (char *)&b, 1);

    /* printf("tclgd_channelGetchar ctx %lx char %d\n", ctx, b[0]); */

    return b[0];
}

static int
tclgd_channelSeek (struct gdIOCtx *ctx, const int pos)
{
    int seekResult;

    tclgd_channelIOCtx *tctx;
    tctx = (tclgd_channelIOCtx *) ctx;

    seekResult = (Tcl_Seek (tctx->channel, pos, SEEK_SET) != -1);

    /* printf("tclgd_channelSeek ctx %lx pos %d result %d\n", ctx, pos, seekResult); */

    return seekResult;
}

static long
tclgd_channelTell (struct gdIOCtx *ctx)
{
    long tellResult;

    tclgd_channelIOCtx *tctx;
    tctx = (tclgd_channelIOCtx *) ctx;

    tellResult = Tcl_Tell (tctx->channel);

    /* printf("tclgd_channelTell ctx %lx pos %d\n", ctx, tellResult); */

   return tellResult;
}

gdIOCtx *
tclgd_channelNameToIOCtx (Tcl_Interp *interp, char *channelName, int modeFlag)
{
    gdIOCtx     *outctx;
    Tcl_Channel  channel;
    int          mode;

    channel = Tcl_GetChannel (interp, channelName, &mode);
    if (channel == NULL) {
	return NULL;
    }

    if (!(mode & modeFlag)) {
	Tcl_AppendResult (interp, "channel '", channelName, "' not open for ", ((modeFlag & TCL_WRITABLE) ? "writing" : "reading"), NULL);
	return NULL;
    }

    outctx = tclgd_newChannelCtx (channel);

    return outctx;
}

