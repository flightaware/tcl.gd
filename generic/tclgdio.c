
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

#include <tcl.h>
#include "gd.h"

typedef struct tclgd_channelIOCtx
{
  gdIOCtx      ctx;
  Tcl_Channel  channel;
} tclgd_channelIOCtx;

gdIOCtx *tclgd_newChannelCtx (Tcl_Channel channel);

static int tclgd_channelGetbuf (gdIOCtx *, void *, int);
static int tclgd_channelPutbuf (gdIOCtx *, const void *, int);
static void tclgd_channelPutchar (gdIOCtx *, int);
static int  tclgd_channelGetchar (gdIOCtx * ctx);

static int tclgd_Seek (struct gdIOCtx *, const int);
static long tclgd_Tell (struct gdIOCtx *);

static void tclgd_FreeChannelCtx (gdIOCtx * ctx);

/* return data as a dynamic pointer */
BGD_DECLARE(gdIOCtx *) gdNewChannelCtx (Tcl_Channel channel)
{
    tclgd_channelIOCtx *ctx;

    ctx = (tclgd_channelIOCtx *) gdMalloc (sizeof (tclgd_channelIOCtx));
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
    gdFree (ctx);
}


static int
tclgd_channelPutbuf (gdIOCtx * ctx, const void *buf, int size)
{
    tclgd_channelIOCtx *tctx;

    tctx = (tclgd_channelIOCtx *) ctx;

    return Tcl_WriteChars (tctx->channel, buf, size);
}

static int
tclgd_channelGetbuf (gdIOCtx * ctx, void *buf, int size)
{
    tclgd_channelIOCtx *tctx;
    tctx = (tclgd_channelIOCtx *) ctx;

    return Tcl_ReadChars (tctx->channel, buf, size, 0);
}

static void
tclgd_channelPutchar (gdIOCtx * ctx, int a)
{
    unsigned char b;
    tclgd_channelIOCtx *tctx;

    tctx = (tclgd_channelIOCtx *) ctx;

    b = a;
    Tcl_WriteChars (tctx->channel, &b, 1);
}

static int
tclgd_channelGetchar (gdIOCtx * ctx)
{
    unsigned char b;
    tclgd_channelIOCtx *tctx;
    tctx = (tclgd_channelIOCtx *) ctx;

    Tcl_ReadChars (tctx->channel, &b, 1, 0);
    return b;
}


static int
tclgd_channelSeek (struct gdIOCtx *ctx, const int pos)
{
    tclgd_channelIOCtx *tctx;
    tctx = (tclgd_channelIOCtx *) ctx;

    return (Tcl_Seek (tctx->channel, pos, SEEK_SET) != -1);
}

static long
tclgd_channelTell (struct gdIOCtx *ctx)
{
  tclgd_channelIOCtx *tctx;
  tctx = (tclgd_channelIOCtx *) ctx;

  return Tcl_Tell (tctx->channel);
}