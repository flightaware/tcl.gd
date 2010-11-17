/*
 * tcl.gd
 *
 * Copyright (C) 2005 by Karl Lehenbauer, All Rights Reserved
 *
 * $Id: tclgd.c,v 1.29 2008-06-06 21:51:16 karl Exp $
 */

#include "tclgd.h"
#include <string.h>


/*
 *----------------------------------------------------------------------
 *
 * tclgd_cmdNameObjToIM --
 *
 *    Given a Tcl_Obj containing a name for an existing GD image handling
 *    command object and a pointer to a gd image pointer...
 *
 *    If the command name can be successfully looked up, set the passed
 *    address of a gd image pointer to contain the GD image held within
 *    the image handling object's client data and return TCL_OK, else
 *    return TCL_ERROR.
 *
 * Results:
 *    None.
 *
 *----------------------------------------------------------------------
 */
static int
tclgd_cmdNameObjToIM (Tcl_Interp *interp, Tcl_Obj *commandNameObj, gdImagePtr *srcImPtr)
{
    Tcl_CmdInfo        cmdInfo;

    if (!Tcl_GetCommandInfo ( interp, Tcl_GetString(commandNameObj), &cmdInfo)) {
	return TCL_ERROR;
    }

    if (cmdInfo.objClientData == NULL) {
	Tcl_AppendResult (interp, "Error: '", Tcl_GetString (commandNameObj), "' is not a tclgd object", NULL);
	return TCL_ERROR;
    }

    *srcImPtr = ((tclgd_clientData *)cmdInfo.objClientData)->im;

    return TCL_OK;
}

static double
tclgd_imageCompareRatio (gdImagePtr im1, gdImagePtr im2)
{
    int x, y;
    int p1, p2;
    long matchCount = 0;

    if (im1->sx != im2->sx || im1->sy != im2->sy) {
        return -1;
    }

    for (y = 0; (y < im1->sy); y++) {
        for (x = 0; (x < im1->sx); x++) {
	    p1 = im1->trueColor ? gdImageTrueColorPixel (im1, x, y) : gdImagePalettePixel (im1, x, y);
	    p2 = im2->trueColor ? gdImageTrueColorPixel (im2, x, y) : gdImagePalettePixel (im2, x, y);

	    if (gdImageRed(im1, p1) == gdImageRed(im2, p2) && gdImageGreen(im1, p1) == gdImageGreen(im2, p2) && gdImageBlue(im1, p1) == gdImageBlue(im2, p2)) {
	        matchCount++;
	    }
	}
    }

    return (double)(matchCount) / (double)(im1->sx * im1->sy);
}

/* tclgd_complain routines -- these get called in a lot of places after
 * integer and double-precision floating point conversion failures to
 * help the user understand which function argument contained bad
 * data
 */

static int
tclgd_complain(Tcl_Interp *interp, char *element) {
    Tcl_AppendResult (interp, " while converting ", element, " component", NULL);
    return TCL_ERROR;
}

static int
tclgd_complainX(Tcl_Interp *interp) {
    return tclgd_complain (interp, "x");
}

static int
tclgd_complainY(Tcl_Interp *interp) {
    return tclgd_complain (interp, "y");
}

static int
tclgd_complainX1(Tcl_Interp *interp) {
    return tclgd_complain (interp, "x1");
}

static int
tclgd_complainY1(Tcl_Interp *interp) {
    return tclgd_complain (interp, "y1");
}

static int
tclgd_complainX2(Tcl_Interp *interp) {
    return tclgd_complain (interp, "x2");
}

static int
tclgd_complainY2(Tcl_Interp *interp) {
    return tclgd_complain (interp, "y2");
}

static int
tclgd_complainDestX(Tcl_Interp *interp) {
    return tclgd_complain (interp, "destX");
}

static int
tclgd_complainDestY(Tcl_Interp *interp) {
    return tclgd_complain (interp, "destY");
}

static int
tclgd_complainSrcX(Tcl_Interp *interp) {
    return tclgd_complain (interp, "srcX");
}

static int
tclgd_complainSrcY(Tcl_Interp *interp) {
    return tclgd_complain (interp, "srcY");
}

static int
tclgd_complainWidth(Tcl_Interp *interp) {
    return tclgd_complain (interp, "width");
}

static int
tclgd_complainHeight(Tcl_Interp *interp) {
    return tclgd_complain (interp, "height");
}

static int
tclgd_complainDestWidth(Tcl_Interp *interp) {
    return tclgd_complain (interp, "destWidth");
}

static int
tclgd_complainDestHeight(Tcl_Interp *interp) {
    return tclgd_complain (interp, "destHeight");
}

static int
tclgd_complainSrcWidth(Tcl_Interp *interp) {
    return tclgd_complain (interp, "srcWidth");
}

static int
tclgd_complainSrcHeight(Tcl_Interp *interp) {
    return tclgd_complain (interp, "srctHeight");
}

static int
tclgd_complainStartDegrees(Tcl_Interp *interp) {
    return tclgd_complain (interp, "startDegrees");
}

static int
tclgd_complainEndDegrees(Tcl_Interp *interp) {
    return tclgd_complain (interp, "endDegrees");
}

static int
tclgd_complainCX(Tcl_Interp *interp) {
    return tclgd_complain (interp, "cx");
}

static int
tclgd_complainCY(Tcl_Interp *interp) {
    return tclgd_complain (interp, "cy");
}

static int
tclgd_complainRed(Tcl_Interp *interp) {
    return tclgd_complain (interp, "red");
}

static int
tclgd_complainGreen(Tcl_Interp *interp) {
    return tclgd_complain (interp, "green");
}

static int
tclgd_complainBlue(Tcl_Interp *interp) {
    return tclgd_complain (interp, "blue");
}

static int
tclgd_complainAlpha(Tcl_Interp *interp) {
    return tclgd_complain (interp, "alpha");
}

static int
tclgd_complainColor(Tcl_Interp *interp) {
    return tclgd_complain (interp, "color");
}

static int
tclgd_complainAngle(Tcl_Interp *interp) {
    return tclgd_complain (interp, "angle");
}

static int
tclgd_complainFgColor(Tcl_Interp *interp) {
    return tclgd_complain (interp, "fgcolor");
}

static int
tclgd_complainPercent(Tcl_Interp *interp) {
    return tclgd_complain (interp, "percent");
}

static int
tclgd_complainCompression(Tcl_Interp *interp) {
    return tclgd_complain (interp, "compression");
}

static int
tclgd_complainQuality(Tcl_Interp *interp) {
    return tclgd_complain (interp, "quality");
}

static int
tclgd_complainCorrupt(Tcl_Interp *interp) {
    Tcl_AppendResult (interp, "image is corrupt or incorrect image type", NULL);
    return TCL_ERROR;
}

#ifndef GD_GIF
static int
tclgd_complainNoGIFSupport(Tcl_Interp *interp) {
    Tcl_AppendResult (interp, "GD library not built with GIF support", NULL);
    return TCL_ERROR;
}
#endif

#ifndef GD_XPM
static int
tclgd_complainNoXPMSupport(Tcl_Interp *interp) {
    Tcl_AppendResult (interp, "GD library not built with XPM support", NULL);
    return TCL_ERROR;
}
#endif

#ifndef GD_PNG
static int
tclgd_complainNoPNGSupport(Tcl_Interp *interp) {
    Tcl_AppendResult (interp, "GD library not built with PNG support", NULL);
    return TCL_ERROR;
}
#endif

#ifndef GD_JPEG
static int
tclgd_complainNoJPEGSupport(Tcl_Interp *interp) {
    Tcl_AppendResult (interp, "GD library not built with JPEG support", NULL);
    return TCL_ERROR;
}
#endif

/*
 *----------------------------------------------------------------------
 *
 * tclgd_GDdeleteProc --
 *
 *    callback proc that deletes the corresponding gd image when one of our
 *    self-generated gd commands gets deleted
 *
 * Results:
 *    destroys the gd image we're carrying around with us
 *
 *----------------------------------------------------------------------
 */
void tclgd_GDdeleteProc (ClientData clientData) {
    tclgd_clientData *tclgdClientData = (tclgd_clientData *)clientData;

    if (tclgdClientData->destroyOnDelete) {
	gdImageDestroy(tclgdClientData->im);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * tclgd_GetColor --
 *
 *    Get a color integer or one of the special pseudocolors like
 *    antialised, brushed, styled, styled_brushed, tiled, transparent.
 *
 * Results:
 *    the integer or the constant corresponding to the pseudocolor
 *
 *----------------------------------------------------------------------
 */
 int tclgd_GetColor (Tcl_Interp *interp, Tcl_Obj *obj, int *color)
 {
     int optIndex;

     static CONST char *options[] = {
	"antialiased",
	"brushed",
	"styled",
	"styled_brushed",
	"tiled",
	"transparent",
	NULL
     };

    enum options {
	OPT_ANTIALIASED,
	OPT_BRUSHED,
	OPT_STYLED,
	OPT_STYLED_BRUSHED,
	OPT_TILED,
	OPT_TRANSPARENT
    };

    /* if color is an integer, we're done */
    if (Tcl_GetIntFromObj (interp, obj, color) == TCL_OK) {
	/* printf("tclgd_GetColor returns normal color %d for %s\n", *color, Tcl_GetString (obj)); */
       return TCL_OK;
    }

    Tcl_ResetResult (interp);

    if (Tcl_GetIndexFromObj(interp, obj, options, "color", 
	TCL_EXACT, &optIndex) != TCL_OK) {
	Tcl_AppendResult (interp, ", or a color index", NULL);
	return TCL_ERROR;
    }

    switch ((enum options) optIndex) {
      case OPT_ANTIALIASED:
          *color = gdAntiAliased;
	  break;

      case OPT_BRUSHED:
          *color = gdBrushed;
	  break;

      case OPT_STYLED:
          *color = gdStyled;
	  break;

      case OPT_STYLED_BRUSHED:
          *color = gdStyledBrushed;

      case OPT_TILED:
          *color = gdTiled;
	  break;

      case OPT_TRANSPARENT:
          *color = gdTransparent;
	  break;
    }
    /* printf("tclgd_GetColor returns custom color %d\n", *color); */
    return TCL_OK;
 }


/*
 *----------------------------------------------------------------------
 *
 * tclgd_gdObjectObjCmd --
 *
 *    dispatches the subcommands of a gd object command
 *
 * Results:
 *    stuff
 *
 *----------------------------------------------------------------------
 */
int
tclgd_gdObjectObjCmd(ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    gdImagePtr im = ((tclgd_clientData *)cData)->im;
    int         optIndex;

    static CONST char *options[] = {
	"pixel",
	"pixelrgb",
	"line",
	"polygon",
	"rectangle",
	"filled_rectangle",
	"arc",
	"filled_arc",
	"filled_ellipse",
	"fill_to_border",
	"fill",
	"text",
	"text_bounds",
	"allocate_color",
	"closest_color",
	"closest_color_hwb",
	"exact_color",
	"resolve_color",
	"total_colors",
	"deallocate_color",
	"true_color",
	"interlace",
	"transparent",
	"set_anti_aliased",
	"set_anti_aliased_dont_blend",
	"set_brush",
	"set_tile",
	"set_style",
	"set_thickness",
	"alpha_blending",
	"save_alpha",
	"clip",
	"get_alpha",
	"bounds_safe",
	"green_component",
	"red_component",
	"blue_component",
	"rgb_components",
	"width",
	"height",
	"compare_ratio",
	"copy",
	"copy_resized",
	"copy_resampled",
	"copy_rotated",
	"copy_merge",
	"copy_merge_grey",
	"copy_palette",
	"sharpen",
	"compare",
	"square_to_circle",
	"write_jpeg",
	"jpeg_data",
	"write_gif",
	"gif_data",
	"gif_anim_begin",
	"gif_anim_add",
	"gif_anim_end",
	"write_png",
	"png_data",
	"write_wbmp",
	"wbmp_data",
	"write_gd",
	"gd_data",
	"write_gd2",
	"gd2_data",
	(char *)NULL
    };

    enum options {
	OPT_PIXEL,
	OPT_PIXELRGB,
	OPT_LINE,
	OPT_POLYGON,
	OPT_RECTANGLE,
	OPT_FILLED_RECTANGLE,
	OPT_ARC,
	OPT_FILLED_ARC,
	OPT_FILLED_ELLIPSE,
	OPT_FILL_TO_BORDER,
	OPT_FILL,
	OPT_TEXT,
	OPT_TEXT_BOUNDS,
	OPT_ALLOCATE_COLOR,
	OPT_CLOSEST_COLOR,
	OPT_CLOSEST_COLOR_HWB,
	OPT_EXACT_COLOR,
	OPT_RESOLVE_COLOR,
	OPT_TOTAL_COLORS,
	OPT_DEALLOCATE_COLOR,
	OPT_TRUE_COLOR,
	OPT_INTERLACE,
	OPT_TRANSPARENT,
	OPT_SET_ANTI_ALIASED,
	OPT_SET_ANTI_ALIASED_DONT_BLEND,
	OPT_SET_BRUSH,
	OPT_SET_TILE,
	OPT_STYLE,
	OPT_SET_THICKNESS,
	OPT_ALPHA_BLENDING,
	OPT_SAVE_ALPHA,
	OPT_CLIP,
	OPT_GET_ALPHA,
	OPT_BOUNDS_SAFE,
	OPT_GREEN_COMPONENT,
	OPT_RED_COMPONENT,
	OPT_BLUE_COMPONENT,
	OPT_RGB_COMPONENTS,
	OPT_WIDTH,
	OPT_HEIGHT,
	OPT_COMPARE_RATIO,
	OPT_COPY,
	OPT_COPY_RESIZED,
	OPT_COPY_RESAMPLED,
	OPT_COPY_ROTATED,
	OPT_COPY_MERGE,
	OPT_COPY_MERGE_GREY,
	OPT_COPY_PALETTE,
	OPT_SHARPEN,
	OPT_COMPARE,
	OPT_SQUARE_TO_CIRCLE,
	OPT_WRITE_JPEG,
	OPT_JPEG_DATA,
	OPT_WRITE_GIF,
	OPT_GIF_DATA,
	OPT_GIF_ANIM_BEGIN,
	OPT_GIF_ANIM_ADD,
	OPT_GIF_ANIM_END,
	OPT_WRITE_PNG,
	OPT_PNG_DATA,
	OPT_WRITE_WBMP,
	OPT_WBMP_DATA,
	OPT_WRITE_GD,
	OPT_GD_DATA,
	OPT_WRITE_GD2,
	OPT_GD2_DATA
    };

    if (objc == 1) {
	Tcl_WrongNumArgs (interp, 1, objv, "option ?args?");
	return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objv[1], options, "option", 
	TCL_EXACT, &optIndex) != TCL_OK) {
	return TCL_ERROR;
    }

    switch ((enum options) optIndex) {
      case OPT_PIXEL: {
	int x, y, color;

	if ((objc < 4) || (objc > 5)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x y ?color?");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

       if (objc == 4) {
	   Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageGetPixel (im, x, y)));
	   return TCL_OK;
	}

	if (tclgd_GetColor (interp, objv[4], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageSetPixel (im, x, y, color);
	break;
      }

      case OPT_PIXELRGB: {
	int x, y, color;

	if ((objc < 4) || (objc > 5)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x y ?rgbColorList?");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

       /* if they're querying the color, get the color from the pixel
        * and return its red, green and blue components as a list.
	*/
       if (objc == 4) {
	   Tcl_Obj *listObjv[3];

           color = gdImageGetPixel (im, x, y);

	    listObjv[0] = Tcl_NewIntObj gdImageRed (im, color);
	    listObjv[1] = Tcl_NewIntObj gdImageGreen (im, color);
	    listObjv[2] = Tcl_NewIntObj gdImageBlue (im, color);

	    Tcl_SetObjResult (interp, Tcl_NewListObj (3, listObjv));
	    return TCL_OK;
	}

	if (tclgd_GetColor (interp, objv[4], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageSetPixel (im, x, y, color);
	break;
      }

      case OPT_LINE: {
	int x1, y1, x2, y2, color;

	if (objc != 7) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x1 y1 x2 y2 color");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x1) == TCL_ERROR) {
	   return tclgd_complainX1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y1) == TCL_ERROR) {
	   return tclgd_complainY1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &x2) == TCL_ERROR) {
	   return tclgd_complainX2 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &y2) == TCL_ERROR) {
	   return tclgd_complainY2 (interp);
       }

	if (tclgd_GetColor (interp, objv[6], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	/* printf("gdImageLine %d %d %d %d %d\n", x1, y1, x2, y2, color); */
	gdImageLine (im, x1, y1, x2, y2, color);
	break;
      }

      case OPT_POLYGON: {
        int         suboptIndex;
	Tcl_Obj   **pointObjList;
	int         nElements;
	int         objOffset;
	gdPoint     *points;
	int          i;
	int          x;
	int          y;
	int          color;

	static CONST char *subOptions[] = {
	    "filled",
	    "open",
	    "basic",
	    NULL
	};

	enum suboptions {
	    SUBOPT_FILLED,
	    SUBOPT_OPEN,
	    SUBOPT_BASIC
	};

        if ((objc < 4) || (objc > 5)) {
	    Tcl_WrongNumArgs (interp, 2, objv,"?filled|open? pointList color");
	    return TCL_ERROR;
	}

	if (objc == 4) {
	    suboptIndex = SUBOPT_BASIC;
	    objOffset = 2;
	} else {
	    objOffset = 3;
	    if (Tcl_GetIndexFromObj(interp, objv[2], subOptions, "suboption", 
		TCL_EXACT, &suboptIndex) != TCL_OK) {
		return TCL_ERROR;
	    }
	}

	if (Tcl_ListObjGetElements (interp, objv[objOffset++], &nElements, &pointObjList) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	if ((nElements % 2 != 0) || (nElements < 6)) {
	    Tcl_WrongNumArgs (interp, 2, objv,"point list must at least three pairs of points");
	    return TCL_ERROR;
	}

	points = (gdPoint *)ckalloc (sizeof (gdPoint) * (nElements / 2));

	for (i = 0; i < nElements; i += 2) {
	    if (Tcl_GetIntFromObj (interp, pointObjList[i], &x) == TCL_ERROR) {
	        return TCL_ERROR;
	    }

	    if (Tcl_GetIntFromObj (interp, pointObjList[i+1], &y) == TCL_ERROR) {
	        return TCL_ERROR;
	    }

	    points[i/2].x = x;
	    points[i/2].y = y;
	}

	if (tclgd_GetColor (interp, objv[objOffset], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}


	switch ((enum suboptions) suboptIndex) {
	  case SUBOPT_FILLED: {
	      gdImageFilledPolygon (im, points, nElements/2, color);
	  }

	  case SUBOPT_OPEN: {
	      gdImageOpenPolygon (im, points, nElements/2, color);
	  }

	  case SUBOPT_BASIC: {
	      gdImagePolygon (im, points, nElements/2, color);
	  }
	}

	return TCL_OK;
      }

      case OPT_RECTANGLE: {
	int x1, y1, x2, y2, color;

	if (objc != 7) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x1 y1 x2 y2 color");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x1) == TCL_ERROR) {
	   return tclgd_complainX1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y1) == TCL_ERROR) {
	   return tclgd_complainY1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &x2) == TCL_ERROR) {
	   return tclgd_complainX2 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &y2) == TCL_ERROR) {
	   return tclgd_complainY2 (interp);
       }

	if (tclgd_GetColor (interp, objv[6], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageFilledRectangle (im, x1, y1, x2, y2, color);
	break;
      }

      case OPT_FILLED_RECTANGLE: {
	int x1, y1, x2, y2, color;

	if (objc != 7) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x1 y1 x2 y2 color");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x1) == TCL_ERROR) {
	   return tclgd_complainX1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y1) == TCL_ERROR) {
	   return tclgd_complainY1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &x2) == TCL_ERROR) {
	   return tclgd_complainX2 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &y2) == TCL_ERROR) {
	   return tclgd_complainY2 (interp);
       }

	if (tclgd_GetColor (interp, objv[6], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageFilledRectangle (im, x1, y1, x2, y2, color);
	break;
      }

      case OPT_ARC: {
	int cx, cy, w, h, s, e, color;

	if (objc != 9) {
	    Tcl_WrongNumArgs (interp, 2, objv, "cx cy width height startDegrees endDegrees color");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &cx) == TCL_ERROR) {
	   return tclgd_complainCX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &cy) == TCL_ERROR) {
	   return tclgd_complainCY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &w) == TCL_ERROR) {
	   return tclgd_complainWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &h) == TCL_ERROR) {
	   return tclgd_complainHeight (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &s) == TCL_ERROR) {
	   return tclgd_complainStartDegrees (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &e) == TCL_ERROR) {
	   return tclgd_complainEndDegrees (interp);
       }

	if (tclgd_GetColor (interp, objv[8], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageArc (im, cx, cy, w, h, s, e, color);
	break;
      }

      case OPT_FILLED_ARC: {
	int     i;
	int     cx;
	int     cy;
	int     w;
	int     h;
	int     s;
	int     e;
	int     color;
	int     style = 0;
	int     styleIndex;

	static CONST char *styles[] = {
	    "arc",
	    "chord",
	    "pie",
	    "nofill",
	    "edged",
	    NULL
	};

	enum styles {
	    STYLE_ARC,
	    STYLE_CHORD,
	    STYLE_PIE,
	    STYLE_NOFILL,
	    STYLE_EDGED
	};

	if (objc < 9) {
	    Tcl_WrongNumArgs (interp, 2, objv, "cx cy width height startDegrees endDegrees color ?arc? ?chord? ?pie? ?nofill? ?edged?");
	    return TCL_ERROR;
	}
       if (Tcl_GetIntFromObj (interp, objv[2], &cx) == TCL_ERROR) {
	   return tclgd_complainCX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &cy) == TCL_ERROR) {
	   return tclgd_complainCY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &w) == TCL_ERROR) {
	   return tclgd_complainWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &h) == TCL_ERROR) {
	   return tclgd_complainHeight (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &s) == TCL_ERROR) {
	   return tclgd_complainStartDegrees (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &e) == TCL_ERROR) {
	   return tclgd_complainEndDegrees (interp);
       }

	if (tclgd_GetColor (interp, objv[8], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	for (i = 9; i < objc; i++) {
	    if (Tcl_GetIndexFromObj(interp, objv[i], styles, "style", 
		TCL_EXACT, &styleIndex) != TCL_OK) {
		return TCL_ERROR;
	    }

	    switch ((enum styles) styleIndex) {
	      case STYLE_ARC:
		style |= gdArc;
		break;

	      case STYLE_CHORD:
		style |= gdChord;
		break;

	      case STYLE_PIE:
		style |= gdPie;
		break;

	      case STYLE_NOFILL:
		style |= gdNoFill;
		break;

	      case STYLE_EDGED:
		style |= gdEdged;
		break;
	    }
	}

	gdImageFilledArc (im, cx, cy, w, h, s, e, color, style);
	break;
      }

      case OPT_FILLED_ELLIPSE: {
	int cx, cy, w, h, color;

	if (objc != 7) {
	    Tcl_WrongNumArgs (interp, 2, objv, "cx cy width height color");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &cx) == TCL_ERROR) {
	   return tclgd_complainCX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &cy) == TCL_ERROR) {
	   return tclgd_complainCY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &w) == TCL_ERROR) {
	   return tclgd_complainWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &h) == TCL_ERROR) {
	   return tclgd_complainHeight (interp);
       }

	if (tclgd_GetColor (interp, objv[6], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageFilledEllipse (im, cx, cy, w, h, color);
	break;
      }

      case OPT_FILL_TO_BORDER: {
	int x, y, border, color;

	if (objc != 6) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x y borderColor color");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

	if (tclgd_GetColor (interp, objv[4], &border) == TCL_ERROR) {
	   return tclgd_complain (interp, "borderColor");
	}

	if (tclgd_GetColor (interp, objv[5], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	gdImageFillToBorder (im, x, y, border, color);
	break;
      }

      case OPT_FILL: {
	int x, y, color;

	if (objc != 5) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x y color");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x) == TCL_ERROR) {
	    return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y) == TCL_ERROR) {
	    return tclgd_complainY (interp);
       }

	if (tclgd_GetColor (interp, objv[4], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageFill (im, x, y, color);
	break;
      }

      case OPT_TEXT_BOUNDS:
      case OPT_TEXT: {
	gdImagePtr textIm = NULL;
	int        color;
	char      *fontName;
	double     pointSize;
	double     angle;
	int        x;
	int        y;
	char      *text;
	char      *resultString;

	int        brect[8];
	Tcl_Obj    *brectObjv[8];

	if (optIndex == OPT_TEXT) {
	    textIm = im;
	}

	if (objc != 9) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color font pointSize angle x y text");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	fontName = Tcl_GetString (objv[3]);

       if (Tcl_GetDoubleFromObj (interp, objv[4], &pointSize) == TCL_ERROR) {
	    return tclgd_complain (interp, "pointSize");
       }

       if (Tcl_GetDoubleFromObj (interp, objv[5], &angle) == TCL_ERROR) {
	    return tclgd_complain (interp, "angle");
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &x) == TCL_ERROR) {
	    return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &y) == TCL_ERROR) {
	    return tclgd_complainY (interp);
       }

       text = Tcl_GetString (objv[8]);

	resultString = gdImageStringFT (textIm, &brect[0], color, fontName, pointSize, angle, x, y, text);
	if (resultString != NULL) {
	    Tcl_SetObjResult (interp, Tcl_NewStringObj (resultString, -1));
	    Tcl_AppendResult (interp, " '", fontName, "'", NULL);
	    return TCL_ERROR;
	}

	if (optIndex == OPT_TEXT_BOUNDS) {
	    int i;

	    for (i = 0; i < 8; i++) {
		brectObjv[i] = Tcl_NewIntObj(brect[i]);
	    }

	    Tcl_SetObjResult (interp, Tcl_NewListObj (8, brectObjv));
	    return TCL_OK;
	}
	break;
      }

      case OPT_ALLOCATE_COLOR: {
	int r, g, b, alpha;

	if ((objc < 5) || (objc > 6)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "r g b ?alpha?");
	    return TCL_ERROR;
	}

        if (Tcl_GetIntFromObj (interp, objv[2], &r) == TCL_ERROR) {
	    return tclgd_complainRed (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[3], &g) == TCL_ERROR) {
	    return tclgd_complainGreen (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[4], &b) == TCL_ERROR) {
	    return tclgd_complainBlue (interp);
        }

	if (objc == 5) {
	    Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorAllocate (im, r, g, b)));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorAllocateAlpha (im, r, g, b, alpha)));
	return TCL_OK;
      }

      case OPT_CLOSEST_COLOR: {
	int r, g, b, alpha;

	if ((objc < 5) || (objc > 6)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "r g b ?alpha?");
	    return TCL_ERROR;
	}

        if (Tcl_GetIntFromObj (interp, objv[2], &r) == TCL_ERROR) {
	   return tclgd_complainRed (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[3], &g) == TCL_ERROR) {
	   return tclgd_complainGreen (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[4], &b) == TCL_ERROR) {
	   return tclgd_complainBlue (interp);
        }

	if (objc == 5) {
	    Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorClosest (im, r, g, b)));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorClosestAlpha (im, r, g, b, alpha)));
	return TCL_OK;
      }

      case OPT_CLOSEST_COLOR_HWB: {
	int r, g, b;

	if (objc != 5) {
	    Tcl_WrongNumArgs (interp, 2, objv, "r g b");
	    return TCL_ERROR;
	}

        if (Tcl_GetIntFromObj (interp, objv[2], &r) == TCL_ERROR) {
	   return tclgd_complainRed (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[3], &g) == TCL_ERROR) {
	   return tclgd_complainGreen (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[4], &b) == TCL_ERROR) {
	   return tclgd_complainBlue (interp);
        }

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorClosestHWB (im, r, g, b)));
	return TCL_OK;
      }

      case OPT_EXACT_COLOR: {
	int r, g, b, alpha;

	if ((objc < 5) || (objc > 6)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "r g b ?alpha?");
	    return TCL_ERROR;
	}

        if (Tcl_GetIntFromObj (interp, objv[2], &r) == TCL_ERROR) {
	   return tclgd_complainRed (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[3], &g) == TCL_ERROR) {
	   return tclgd_complainGreen (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[4], &b) == TCL_ERROR) {
	   return tclgd_complainBlue (interp);
        }

	if (objc == 5) {
	    Tcl_SetObjResult (interp, Tcl_NewIntObj(gdImageColorExact (im, r, g, b)));
	    return TCL_OK;
	}

        if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
        }

	Tcl_SetObjResult (interp, Tcl_NewIntObj(gdImageColorExactAlpha (im, r, g, b, alpha)));
	return TCL_OK;
      }

      case OPT_RESOLVE_COLOR: {
	int r, g, b, alpha;

	if ((objc < 5) || (objc > 6)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "r g b ?alpha?");
	    return TCL_ERROR;
	}

        if (Tcl_GetIntFromObj (interp, objv[2], &r) == TCL_ERROR) {
	   return tclgd_complainRed (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[3], &g) == TCL_ERROR) {
	   return tclgd_complainGreen (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[4], &b) == TCL_ERROR) {
	   return tclgd_complainBlue (interp);
        }

	if (objc == 5) {
	    Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorResolve (im, r, g, b)));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorResolveAlpha (im, r, g, b, alpha)));
	break;
      }

      case OPT_TOTAL_COLORS:
	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}
	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageColorsTotal (im)));
	break;

      case OPT_DEALLOCATE_COLOR: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (tclgd_GetColor (interp, objv[2], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageColorDeallocate (im, color);
	break;
      }

      case OPT_TRUE_COLOR: {
	int r, g, b, alpha;

	if ((objc < 5) || (objc > 6)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "r g b ?alpha?");
	    return TCL_ERROR;
	}

        if (Tcl_GetIntFromObj (interp, objv[2], &r) == TCL_ERROR) {
	   return tclgd_complainRed (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[3], &g) == TCL_ERROR) {
	   return tclgd_complainGreen (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[4], &b) == TCL_ERROR) {
	   return tclgd_complainBlue (interp);
        }

	if (objc == 5) {
	    Tcl_SetObjResult (interp, Tcl_NewIntObj (gdTrueColor (r, g, b)));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdTrueColorAlpha (r, g, b, alpha)));
	return TCL_OK;
      }

      case OPT_INTERLACE: {
	int interlaced;

	if (objc == 2) {
	  Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageGetInterlaced (im)));
	  return TCL_OK;
	}

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "?boolean?");
	}

	if (Tcl_GetBooleanFromObj (interp, objv[2], &interlaced) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageInterlace (im, interlaced);
	return TCL_OK;
      }

      case OPT_TRANSPARENT: {
	int color;

	if ((objc < 2) || (objc > 3)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "?color?");
	    return TCL_ERROR;
	}

	if (objc == 2) {
	   Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageGetTransparent (im)));
	   return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	gdImageColorTransparent (im, color);
	break;
      }

      case OPT_SET_ANTI_ALIASED: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	/* printf("gdImageSetAntiAliased %d\n", color); */
	gdImageSetAntiAliased (im, color);
	return TCL_OK;
      }

      case OPT_SET_ANTI_ALIASED_DONT_BLEND: {
	int color, dontBlend;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (tclgd_GetColor (interp, objv[2], &color) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &dontBlend) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageSetAntiAliasedDontBlend (im, color, dontBlend);
	return TCL_OK;
      }

      case OPT_SET_BRUSH: {
	gdImagePtr   brushIm;

	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "brushImageCommand");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &brushIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageSetBrush (im, brushIm);
	break;
      }

      case OPT_SET_TILE: {
	gdImagePtr   tileIm;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "tileImageCommand");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &tileIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageSetBrush (im, tileIm);
	break;
      }

      case OPT_STYLE: {
	Tcl_Obj   **colorObjList;
	int         nColors;
	int         i;
	int        *styleInts;

        if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "colorList");
	    return TCL_ERROR;
	}

	if (Tcl_ListObjGetElements (interp, objv[2], &nColors, &colorObjList) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	styleInts = (int *)ckalloc (nColors * sizeof (int));

	for (i = 0; i < nColors; i++) {
	    if (tclgd_GetColor (interp, colorObjList[i], &styleInts[i]) == TCL_ERROR) {
		return TCL_ERROR;
	    }
	}

	gdImageSetStyle (im, styleInts, nColors);
	ckfree ((char *)styleInts);
	break;
      }

      case OPT_SET_THICKNESS: {
	int thickness;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "thickness");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &thickness) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	/* printf("gdImageSetThickness %d\n", thickness); */
	gdImageSetThickness (im, thickness);
	return TCL_OK;
      }

      case OPT_ALPHA_BLENDING: {
	int blending;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "blending");
	    return TCL_ERROR;
	}

	if (Tcl_GetBooleanFromObj (interp, objv[2], &blending) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageAlphaBlending (im, blending);
	return TCL_OK;
      }

      case OPT_SAVE_ALPHA: {
	int saveAlpha;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "boolean");
	    return TCL_ERROR;
	}

	if (Tcl_GetBooleanFromObj (interp, objv[2], &saveAlpha) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageSaveAlpha (im, saveAlpha);
	return TCL_OK;
      }

      case OPT_CLIP: {
	int x1, y1, x2, y2;

	if (objc != 2 && objc != 6) {
	    Tcl_WrongNumArgs (interp, 2, objv, "?x1 y1 x2 y2?");
	    return TCL_ERROR;
	}

	if (objc == 2) {
	    Tcl_Obj *clipObjv[4];

	    gdImageGetClip (im, &x1, &y1, &x2, &y2);
	    clipObjv[0] = Tcl_NewIntObj(x1);
	    clipObjv[1] = Tcl_NewIntObj(y1);
	    clipObjv[2] = Tcl_NewIntObj(x2);
	    clipObjv[3] = Tcl_NewIntObj(y2);
	    Tcl_SetObjResult (interp, Tcl_NewListObj (4, clipObjv));
	    return TCL_OK;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x1) == TCL_ERROR) {
	   return tclgd_complainX1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y1) == TCL_ERROR) {
	   return tclgd_complainY1 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &x2) == TCL_ERROR) {
	   return tclgd_complainX2 (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &y2) == TCL_ERROR) {
	   return tclgd_complainY2 (interp);
       }

	gdImageSetClip (im, x1, y1, x2, y2);
	break;
      }

      case OPT_BOUNDS_SAFE: {
	int x, y;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "x y");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[2], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[3], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

	Tcl_SetObjResult (interp, Tcl_NewBooleanObj (gdImageBoundsSafe (im, x, y)));
	break;
      }

      case OPT_GET_ALPHA: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "colorIndex");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	   return tclgd_complainColor (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageAlpha (im, color)));
	return TCL_OK;
      }


      case OPT_GREEN_COMPONENT: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "colorIndex");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	   return tclgd_complainColor (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageGreen (im, color)));
	return TCL_OK;
      }

      case OPT_RED_COMPONENT: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "colorIndex");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageRed (im, color)));
	return TCL_OK;
      }

      case OPT_BLUE_COMPONENT: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "colorIndex");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	Tcl_SetObjResult (interp, Tcl_NewIntObj gdImageBlue (im, color));
	return TCL_OK;
      }

      case OPT_RGB_COMPONENTS: {
	int color;
	Tcl_Obj *listObjv[3];

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	listObjv[0] = Tcl_NewIntObj gdImageRed (im, color);
	listObjv[1] = Tcl_NewIntObj gdImageGreen (im, color);
	listObjv[2] = Tcl_NewIntObj gdImageBlue (im, color);

	Tcl_SetObjResult (interp, Tcl_NewListObj (3, listObjv));
	return TCL_OK;
      }

      case OPT_WIDTH:
	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageSX(im)));
	break;

      case OPT_HEIGHT:
	Tcl_SetObjResult (interp, Tcl_NewIntObj (gdImageSY(im)));
	break;

      case OPT_COMPARE_RATIO: {
	gdImagePtr   srcIm;
	double ratio;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &srcIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	ratio = tclgd_imageCompareRatio (im, srcIm);
	if (ratio < 0) {
	    Tcl_AppendResult (interp, "compare_ratio images must have identical dimensions", NULL);
	    return TCL_ERROR;
	}
	Tcl_SetObjResult (interp, Tcl_NewDoubleObj (ratio));
	break;
      }

      case OPT_COPY: {
        int          destX;
	int          destY;
	int          srcX;
	int          srcY;
	int          w;
	int          h;
	gdImagePtr   srcIm;

	if (objc != 9) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY w h");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &srcIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[3], &destX) == TCL_ERROR) {
	   return tclgd_complainDestX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &destY) == TCL_ERROR) {
	   return tclgd_complainDestY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &srcX) == TCL_ERROR) {
	   return tclgd_complainSrcX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &srcY) == TCL_ERROR) {
	   return tclgd_complainSrcY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &w) == TCL_ERROR) {
	   return tclgd_complainWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[8], &h) == TCL_ERROR) {
	   return tclgd_complainHeight (interp);
       }

	gdImageCopy (im, srcIm, destX, destY, srcX, srcY, w, h);
	break;
      }

      case OPT_COPY_RESAMPLED:
      case OPT_COPY_RESIZED: {
        int          destX;
	int          destY;
	int          srcX;
	int          srcY;
	int          destW;
	int          destH;
	int          srcW;
	int          srcH;
	gdImagePtr   srcIm;

	if (objc != 11) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY destW destH srcW srcH");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &srcIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[3], &destX) == TCL_ERROR) {
	   return tclgd_complainDestX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &destY) == TCL_ERROR) {
	   return tclgd_complainDestY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &srcX) == TCL_ERROR) {
	   return tclgd_complainSrcY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &srcY) == TCL_ERROR) {
	   return tclgd_complainSrcY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &destW) == TCL_ERROR) {
	   return tclgd_complainDestWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[8], &destH) == TCL_ERROR) {
	   return tclgd_complainDestHeight (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[9], &srcW) == TCL_ERROR) {
	   return tclgd_complainSrcWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[10], &srcH) == TCL_ERROR) {
	   return tclgd_complainSrcHeight (interp);
       }

	if (optIndex == OPT_COPY_RESIZED) {
	    gdImageCopyResized (im, srcIm, destX, destY, srcX, srcY, destW, destH, srcW, srcH);
	} else {
	    gdImageCopyResampled (im, srcIm, destX, destY, srcX, srcY, destW, destH, srcW, srcH);
	}
	break;
      }


      case OPT_COPY_ROTATED: {
        double       destX;
	double       destY;
	int          srcX;
	int          srcY;
	int          srcW;
	int          srcH;
	int          angle;
	gdImagePtr   srcIm;

	if (objc != 10) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY destW destH srcW srcH angle");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &srcIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

       if (Tcl_GetDoubleFromObj (interp, objv[3], &destX) == TCL_ERROR) {
	   return tclgd_complainDestX (interp);
       }

       if (Tcl_GetDoubleFromObj (interp, objv[4], &destY) == TCL_ERROR) {
	   return tclgd_complainDestY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &srcX) == TCL_ERROR) {
	   return tclgd_complainSrcX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &srcY) == TCL_ERROR) {
	   return tclgd_complainSrcY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &srcW) == TCL_ERROR) {
	   return tclgd_complainSrcWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[8], &srcH) == TCL_ERROR) {
	   return tclgd_complainSrcHeight (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[9], &angle) == TCL_ERROR) {
	   return tclgd_complainAngle (interp);
       }

	gdImageCopyRotated (im, srcIm, destX, destY, srcX, srcY, srcW, srcH, angle);
	break;
     }

      case OPT_COPY_MERGE_GREY:
      case OPT_COPY_MERGE: {
        int          destX;
	int          destY;
	int          srcX;
	int          srcY;
	int          w;
	int          h;
	int          pct;
	gdImagePtr   srcIm;

	if (objc != 10) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY width height percent");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &srcIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

        if (Tcl_GetIntFromObj (interp, objv[3], &destX) == TCL_ERROR) {
	   return tclgd_complainDestX (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[4], &destY) == TCL_ERROR) {
	   return tclgd_complainDestY (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[5], &srcX) == TCL_ERROR) {
	   return tclgd_complainSrcX (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[6], &srcY) == TCL_ERROR) {
	   return tclgd_complainSrcY (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[7], &w) == TCL_ERROR) {
	   return tclgd_complainWidth (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[8], &h) == TCL_ERROR) {
	   return tclgd_complainHeight (interp);
        }

        if (Tcl_GetIntFromObj (interp, objv[9], &pct) == TCL_ERROR) {
	   return tclgd_complainPercent (interp);
        }

        if (optIndex == OPT_COPY_MERGE) {
	    gdImageCopyMerge (im, srcIm, destX, destY, srcX, srcY, w, h, pct);
        } else {
	    gdImageCopyMergeGray (im, srcIm, destX, destY, srcX, srcY, w, h, pct);
        }
        break;
      }

      case OPT_COPY_PALETTE: {
	gdImagePtr   srcIm;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &srcIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImagePaletteCopy (im, srcIm);
	break;
      }

      case OPT_SHARPEN: {
	int pct;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "pct");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &pct) == TCL_ERROR) {
	   return tclgd_complainPercent (interp);
	}

#if 0
	gdImageSharpen (im, pct);
#endif
	return TCL_OK;
      }

      case OPT_COMPARE: {
	gdImagePtr   compIm;
	int          cmpMask;
	Tcl_Obj     *listObj;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "compImageCommand");
	    return TCL_ERROR;
	}

	if (tclgd_cmdNameObjToIM (interp, objv[2], &compIm) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	cmpMask = gdImageCompare (im, compIm);

	listObj = Tcl_NewObj();

	if (cmpMask & GD_CMP_IMAGE) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("image", -1));
	}

	if (cmpMask & GD_CMP_NUM_COLORS) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("num_colors", -1));
	}

	if (cmpMask & GD_CMP_COLOR) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("colors", -1));
	}

	if (cmpMask & GD_CMP_SIZE_X) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("width", -1));
	}

	if (cmpMask & GD_CMP_SIZE_Y) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("height", -1));
	}

	if (cmpMask & GD_CMP_TRANSPARENT) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("transparent", -1));
	}

	if (cmpMask & GD_CMP_BACKGROUND) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("background", -1));
	}

	if (cmpMask & GD_CMP_INTERLACE) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("interlace", -1));
	}

	if (cmpMask & GD_CMP_TRUECOLOR) {
	    Tcl_ListObjAppendElement (interp, listObj, Tcl_NewStringObj ("truecolor", -1));
	}

	Tcl_SetObjResult (interp, listObj);
	break;
      }

      case OPT_SQUARE_TO_CIRCLE: {
        int radius;
	gdImagePtr newIm = NULL;

	if (gdImageSX(im) != gdImageSY(im)) {
	    Tcl_AppendResult (interp, "image must be square for square_to_circle to work", NULL);
	    return TCL_ERROR;
	}

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name radius");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &radius) == TCL_ERROR) {
	   return tclgd_complain (interp, "radius");
	}

#if 0
	newIm = gdImageSquareToCircle (im, radius);
#endif
	Tcl_CreateObjCommand (interp, Tcl_GetString(objv[2]), tclgd_gdObjectObjCmd, newIm, tclgd_GDdeleteProc);
	break;
      }

#ifndef GD_JPEG
      case OPT_WRITE_JPEG:
      case OPT_JPEG_DATA:
	return tclgd_complainNoJPEGSupport(interp);
#else
      case OPT_WRITE_JPEG: {
	gdIOCtx     *outctx;
	int          quality;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel quality");
	    return TCL_ERROR;
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &quality) == TCL_ERROR) {
	   return tclgd_complainQuality (interp);
	}

	gdImageJpegCtx (im, outctx, quality);
	gdFree (outctx);
	break;
      }

      case OPT_JPEG_DATA: {
	int      quality;
	int      size;
	void    *memPtr;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "quality");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &quality) == TCL_ERROR) {
	   return tclgd_complainQuality (interp);
	}

	memPtr = gdImageJpegPtr (im, &size, quality);
	Tcl_SetObjResult (interp, Tcl_NewByteArrayObj (memPtr, size));
	break;
      }
#endif /* GD_JPEG */

#ifndef GD_GIF
      case OPT_WRITE_GIF:
      case OPT_GIF_DATA:
      case OPT_GIF_ANIM_BEGIN:
      case OPT_GIF_ANIM_ADD:
      case OPT_GIF_ANIM_END:
	return tclgd_complainNoGIFSupport(interp);
#else
      case OPT_WRITE_GIF: {
	gdIOCtx     *outctx;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel");
	    return TCL_ERROR;
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

	gdImageGifCtx (im, outctx);
	gdFree (outctx);
	break;
      }

      case OPT_GIF_DATA: {
	int      size;
	void    *memPtr;

	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}

	memPtr = gdImageGifPtr (im, &size);
	Tcl_SetObjResult (interp, Tcl_NewByteArrayObj (memPtr, size));
	break;
      }

      case OPT_GIF_ANIM_BEGIN: {
	gdIOCtx     *outctx;
	int          globalCM;
	int          loops;

	if (objc != 5) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel global_color_map loops");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &globalCM) == TCL_ERROR) {
	    return tclgd_complain (interp, "global_color_map");
	}

	if (Tcl_GetIntFromObj (interp, objv[4], &loops) == TCL_ERROR) {
	    return tclgd_complain (interp, "loops");
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

	gdImageGifAnimBeginCtx (im, outctx, globalCM, loops);
	gdFree (outctx);
	break;
      }

      case OPT_GIF_ANIM_ADD: {
	gdIOCtx     *outctx;
	int          localCM;
	int          leftOffset;
	int          topOffset;
	int          delay;
	int          disposal;
	gdImagePtr   previousIm = NULL;

	if ((objc < 8) || (objc > 9)) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel local_color_map left_offset top_offset delay disposal ?previous_image?");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &localCM) == TCL_ERROR) {
	    return tclgd_complain (interp, "local_color_map");
	}

	if (Tcl_GetIntFromObj (interp, objv[4], &leftOffset) == TCL_ERROR) {
	    return tclgd_complain (interp, "left_offset");
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &topOffset) == TCL_ERROR) {
	    return tclgd_complain (interp, "top_offset");
	}

	if (Tcl_GetIntFromObj (interp, objv[6], &delay) == TCL_ERROR) {
	    return tclgd_complain (interp, "delay");
	}

	if (Tcl_GetIntFromObj (interp, objv[7], &disposal) == TCL_ERROR) {
	    return tclgd_complain (interp, "disposal");
	}

	if (objc == 9) {
	    if (tclgd_cmdNameObjToIM (interp, objv[8], &previousIm) == TCL_ERROR) {
		return TCL_ERROR;
	    }
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

	gdImageGifAnimAddCtx (im, outctx, localCM, leftOffset, topOffset, delay, disposal, previousIm);
	gdFree (outctx);
	break;
      }

      case OPT_GIF_ANIM_END: {
	gdIOCtx     *outctx;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel");
	    return TCL_ERROR;
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

	gdImageGifAnimEndCtx (outctx);
	gdFree (outctx);
	break;
      }
#endif /* GD_GIF */

#ifndef GD_PNG
      case OPT_WRITE_PNG:
      case OPT_PNG_DATA:
	return tclgd_complainNoPNGSupport(interp);
#else
      case OPT_WRITE_PNG: {
	gdIOCtx     *outctx;
	int          compression;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel compressionLevel");
	    return TCL_ERROR;
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &compression) == TCL_ERROR) {
	   return tclgd_complainCompression (interp);
	}

	if ((compression < -1) || (compression > 9)) {
	    Tcl_AppendResult (interp, "compression must be between -1 (default), 0 (no compression), 1 (quick compress) - 9 (best possible)", NULL);
	    return TCL_ERROR;
	}

	gdImagePngCtxEx (im, outctx, compression);
	gdFree (outctx);
	break;
      }

      case OPT_PNG_DATA: {
	int      compression;
	int      size;
	void    *memPtr;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "compressionLevel");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &compression) == TCL_ERROR) {
	   return tclgd_complainCompression (interp);
	}

	if ((compression < -1) || (compression > 9)) {
	    Tcl_AppendResult (interp, "compression must be between -1 (default), 0 (no compression), 1 (quick compress) - 9 (best possible)", NULL);
	    return TCL_ERROR;
	}

	memPtr = gdImagePngPtrEx (im, &size, compression);
	Tcl_SetObjResult (interp, Tcl_NewByteArrayObj (memPtr, size));
	break;
      }
#endif /* GD_PNG */

      case OPT_WRITE_WBMP: {
	gdIOCtx     *outctx;
	int          fgcolor;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel fgcolor");
	    return TCL_ERROR;
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &fgcolor) == TCL_ERROR) {
	   return tclgd_complainFgColor (interp);
	}

	gdImageWBMPCtx (im, fgcolor, outctx);
	gdFree (outctx);
	return TCL_OK;
      }

      case OPT_WBMP_DATA: {
	int   fgcolor;
	int      size;
	void    *memPtr;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "fgcolor");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &fgcolor) == TCL_ERROR) {
	   return tclgd_complainFgColor (interp);
	}

	memPtr = gdImageWBMPPtr (im, &size, fgcolor);
	Tcl_SetObjResult (interp, Tcl_NewByteArrayObj (memPtr, size));
	break;
      }

      case OPT_WRITE_GD: {
	FILE *file;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[2]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	gdImageGd (im, file);
	fflush (file);
	break;
      }

      case OPT_GD_DATA: {
	int      size;
	void    *memPtr;

	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}

	memPtr = gdImageGdPtr (im, &size);
	Tcl_SetObjResult (interp, Tcl_NewByteArrayObj (memPtr, size));
	break;
      }

      case OPT_WRITE_GD2: {
	gdIOCtx     *outctx;
	int          chunkSize;
	int          formatIndex;
	int          format;

	static CONST char *formatOptions[] = {
	    "compressed",
	    "raw",
	    NULL
	};

	enum formatOptions {
	    FORMAT_COMPRESSED,
	    FORMAT_RAW
	};


	if (objc != 5) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel chunkSize format");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &chunkSize) == TCL_ERROR) {
	   return tclgd_complain (interp, "chunk size");
	}

	if (Tcl_GetIndexFromObj (interp, objv[4], formatOptions, "format", 
	    TCL_EXACT, &formatIndex) != TCL_OK) {
	    return TCL_ERROR;
	}

	switch ((enum formatOptions) formatIndex) {
	  case FORMAT_COMPRESSED:
	    format = GD2_FMT_COMPRESSED;
	    break;

	  case FORMAT_RAW:
	    format = GD2_FMT_RAW;
	    break;
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]), TCL_WRITABLE)) == NULL) {
	    return TCL_ERROR;
	}

#if 0
	gdImageGd2Ctx (im, outctx, chunkSize, format);
#else
	Tcl_AppendResult (interp, "gdImageGd2Ctx is documented but doesn'ta ctually exist", NULL);
	return TCL_ERROR;

#endif
	gdFree (outctx);
	break;
      }

      case OPT_GD2_DATA: {
	int          chunkSize;
	int          formatIndex;
	int          format = GD2_FMT_COMPRESSED;
	int          size;
	void        *memPtr;

	static CONST char *formatOptions[] = {
	    "compressed",
	    "raw",
	    NULL
	};

	enum formatOptions {
	    FORMAT_COMPRESSED,
	    FORMAT_RAW
	};


	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "chunkSize format");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &chunkSize) == TCL_ERROR) {
	   return tclgd_complain (interp, "chunk size");
	}

	if (Tcl_GetIndexFromObj (interp, objv[3], formatOptions, "format", 
	    TCL_EXACT, &formatIndex) != TCL_OK) {
	    return TCL_ERROR;
	}

	switch ((enum formatOptions) formatIndex) {
	  case FORMAT_COMPRESSED:
	    format = GD2_FMT_COMPRESSED;
	    break;

	  case FORMAT_RAW:
	    format = GD2_FMT_RAW;
	    break;
	}

	memPtr = gdImageGd2Ptr (im, chunkSize, format, &size);
	Tcl_SetObjResult (interp, Tcl_NewByteArrayObj (memPtr, size));
	break;
      }
    }

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * tclgd_newGDObject --
 *
 *	This procedure is invoked to create a new Tcl command
 *      that invokes the tclgd_gdObjectobjCmd command, that
 *      attaches the specified gd image pointer to the new
 *      command's client data structure.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	A new Tcl command is created.
 *
 *----------------------------------------------------------------------
 */
int
tclgd_newGDObject (Tcl_Interp *interp, Tcl_Obj *nameObj, gdImagePtr im, int destroyOnDelete)
{
    char              *newName = Tcl_GetString (nameObj);;
    char               autoObjName[64];
    tclgd_clientData  *tclgdClientData;

    tclgdClientData = (tclgd_clientData *)ckalloc (sizeof (tclgd_clientData));
    tclgdClientData->im = im;
    tclgdClientData->destroyOnDelete = destroyOnDelete;

    static unsigned long nextObjNumber = 0;

    /* if new name is "#auto", generate a unique object name */
    if (strcmp (newName, "#auto") == 0) {
	while (1) {
	    Tcl_CmdInfo dummy;

	    snprintf (autoObjName, sizeof (autoObjName), "gd%lu", nextObjNumber++);
	    /* if autoObjName doesn't exist in the interpreter,
	     * we've got a good name
	     */
	    if (Tcl_GetCommandInfo (interp, autoObjName, &dummy) == 0) {
		newName = autoObjName;
		break;
	    }
	}
    }

    Tcl_CreateObjCommand (interp, newName, tclgd_gdObjectObjCmd, tclgdClientData, tclgd_GDdeleteProc);
    Tcl_SetObjResult (interp, Tcl_NewStringObj (newName, -1));
    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * tclgd_newGDObjectAttach --
 *
 *	This procedure is invoked to create a new Tcl command
 *      that invokes the tclgd_gdObjectobjCmd command, that
 *      attaches the specified gd image pointer to the new
 *      command's client data structure.
 *
 *      The differences between this and tclgd_newGDObject are:
 *
 *          1. The new command name is passed as a char * instead of a
 *             Tcl_Object.
 *
 *          2. When the created command is deleted, the gd buffer is not
 *             destroyed via gdImageDestroy
 *             .
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	A new Tcl command is created.
 *
 *----------------------------------------------------------------------
 */
int
tclgd_newGDObjectAttach (Tcl_Interp *interp, char *name, gdImagePtr im)
{
    return tclgd_newGDObject (interp, Tcl_NewStringObj(name, -1), im, 0);
}


/*
 *----------------------------------------------------------------------
 *
 * tclgd_GDObjCmd --
 *
 *	This procedure is invoked to process the "GD" command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
int
tclgd_GDObjCmd(clientData, interp, objc, objv)
    ClientData clientData;		/* registered proc hashtable ptr. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int objc;				/* Number of arguments. */
    Tcl_Obj   *CONST objv[];
{
    int          optIndex;
    gdImagePtr   im = NULL;
    gdIOCtx     *inctx = NULL;

    static CONST char *options[] = {
        "create",
	"create_truecolor",
	"create_from_jpeg",
	"create_from_jpeg_data",
	"create_from_png",
	"create_from_png_data",
	"create_from_gif",
	"create_from_gif_data",
	"create_from_gd",
	"create_from_gd_data",
	"create_from_gd2",
	"create_from_gd2_data",
	"create_from_gd2_part",
	"create_from_gd2_part_data",
	"create_from_wbmp",
	"create_from_wbmp_data",
	"create_from_xbm",
	"create_from_xpm",
	"version",
	"features",
        (char *)NULL
    };

    enum options
    {
        OPT_CREATE, 
	OPT_CREATE_TRUECOLOR,
	OPT_CREATE_FROM_JPEG,
	OPT_CREATE_FROM_JPEG_DATA,
	OPT_CREATE_FROM_PNG,
	OPT_CREATE_FROM_PNG_DATA,
	OPT_CREATE_FROM_GIF,
	OPT_CREATE_FROM_GIF_DATA,
	OPT_CREATE_FROM_GD,
	OPT_CREATE_FROM_GD_DATA,
	OPT_CREATE_FROM_GD2,
	OPT_CREATE_FROM_GD2_DATA,
	OPT_CREATE_FROM_GD2_PART,
	OPT_CREATE_FROM_GD2_PART_DATA,
	OPT_CREATE_FROM_WBMP,
	OPT_CREATE_FROM_WBMP_DATA,
	OPT_CREATE_FROM_XBM,
	OPT_CREATE_FROM_XPM,
	OPT_VERSION,
	OPT_FEATURES
    };

    if (objc < 2) {
	Tcl_WrongNumArgs (interp, 1, objv, "subcommand ?args?");
	return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objv[1], options, "option", TCL_EXACT,
        &optIndex) != TCL_OK)
    {
	    return TCL_ERROR;
    }

    switch ((enum options) optIndex) {
      case OPT_CREATE: {
	int x, y;

	if (objc != 5) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name x y");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[3], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

       im = gdImageCreate (x, y);
       break;
      }

      case OPT_CREATE_TRUECOLOR: {
	int x, y;

	if (objc != 5) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name x y");
	    return TCL_ERROR;
	}

       if (Tcl_GetIntFromObj (interp, objv[3], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[4], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

       im = gdImageCreateTrueColor (x, y);
       break;
      }

#ifndef GD_JPEG
      case OPT_CREATE_FROM_JPEG:
      case OPT_CREATE_FROM_JPEG_DATA:
	return tclgd_complainNoJPEGSupport(interp);
#else
      case OPT_CREATE_FROM_JPEG: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name channel");
	    return TCL_ERROR;
	}

	if ((inctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[3]), TCL_READABLE)) == NULL) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromJpegCtx (inctx);
	break;
      }

      case OPT_CREATE_FROM_JPEG_DATA: {
        int            size;
	unsigned char *memPtr;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[3], &size);
	im = gdImageCreateFromJpegPtr (size, memPtr);
	break;
      }
#endif /* GD_JPEG */

#ifndef GD_PNG
      case OPT_CREATE_FROM_PNG:
      case OPT_CREATE_FROM_PNG_DATA:
	return tclgd_complainNoPNGSupport(interp);
#else
      case OPT_CREATE_FROM_PNG: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name channel");
	    return TCL_ERROR;
	}

	if ((inctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[3]), TCL_READABLE)) == NULL) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromPngCtx (inctx);
	break;
      }

      case OPT_CREATE_FROM_PNG_DATA: {
        int            size;
	unsigned char *memPtr;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[3], &size);
	im = gdImageCreateFromPngPtr (size, memPtr);
	break;
      }
#endif /* GD_PNG */

#ifndef GD_GIF
      case OPT_CREATE_FROM_GIF:
      case OPT_CREATE_FROM_GIF_DATA:
	return tclgd_complainNoGIFSupport(interp);
#else
      case OPT_CREATE_FROM_GIF: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name channel");
	    return TCL_ERROR;
	}

	if ((inctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[3]), TCL_READABLE)) == NULL) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromGifCtx (inctx);
	break;
      }

      case OPT_CREATE_FROM_GIF_DATA: {
        int            size;
	unsigned char *memPtr;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[3], &size);
	im = gdImageCreateFromGifPtr (size, memPtr);
	break;
      }
#endif /* GD_GIF */

      case OPT_CREATE_FROM_GD: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name channel");
	    return TCL_ERROR;
	}

	if ((inctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[3]), TCL_READABLE)) == NULL) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromGdCtx (inctx);
	break;
      }

      case OPT_CREATE_FROM_GD_DATA: {
        int            size;
	unsigned char *memPtr;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[3], &size);
	im = gdImageCreateFromGdPtr (size, memPtr);
	break;
      }

      case OPT_CREATE_FROM_GD2: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name channel");
	    return TCL_ERROR;
	}

	if ((inctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[3]), TCL_READABLE)) == NULL) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromGd2Ctx (inctx);
	break;
      }

      case OPT_CREATE_FROM_GD2_DATA: {
        int            size;
	unsigned char *memPtr;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[3], &size);
	im = gdImageCreateFromGd2Ptr (size, memPtr);
	break;
      }

      case OPT_CREATE_FROM_GD2_PART: {
	int          x;
	int          y;
	int          w;
	int          h;

	if (objc != 8) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name channel x y w h");
	    return TCL_ERROR;
	}

	/* check the integer conversions first so we don't leave an
	 * unfreed inctx if one of them fails
	 */

        if (Tcl_GetIntFromObj (interp, objv[4], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
        }

       if (Tcl_GetIntFromObj (interp, objv[5], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &w) == TCL_ERROR) {
	   return tclgd_complainWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &h) == TCL_ERROR) {
	   return tclgd_complainHeight (interp);
       }

	if ((inctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[3]), TCL_READABLE)) == NULL) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromGd2PartCtx (inctx, x, y, w, h);
	break;
      }

      case OPT_CREATE_FROM_GD2_PART_DATA: {
        int            size;
	unsigned char *memPtr;

	int            x;
	int            y;
	int            w;
	int            h;

	if (objc != 8) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data x y w h");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[3], &size);

       if (Tcl_GetIntFromObj (interp, objv[4], &x) == TCL_ERROR) {
	   return tclgd_complainX (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[5], &y) == TCL_ERROR) {
	   return tclgd_complainY (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[6], &w) == TCL_ERROR) {
	   return tclgd_complainWidth (interp);
       }

       if (Tcl_GetIntFromObj (interp, objv[7], &h) == TCL_ERROR) {
	   return tclgd_complainHeight (interp);
       }

	im = gdImageCreateFromGd2PartPtr (size, memPtr, x, y, w, h);
	break;
      }

      case OPT_CREATE_FROM_WBMP: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name channel");
	    return TCL_ERROR;
	}

	if ((inctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[3]), TCL_READABLE)) == NULL) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromWBMPCtx (inctx);
	break;
      }

      case OPT_CREATE_FROM_WBMP_DATA: {
        int            size;
	unsigned char *memPtr;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[3], &size);
	im = gdImageCreateFromWBMPPtr (size, memPtr);
	break;
      }

      case OPT_CREATE_FROM_XBM: {
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromXbm (file);
	break;
      }

#ifndef GD_XPM
      case OPT_CREATE_FROM_XPM:
	return tclgd_complainNoXPMSupport(interp);
#else
      case OPT_CREATE_FROM_XPM: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileName");
	    return TCL_ERROR;
	}

	im = gdImageCreateFromXpm (Tcl_GetString(objv[2]));
	break;
      }
#endif /* GD_XPM */

      case OPT_VERSION: {
	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}

	Tcl_SetObjResult (interp, Tcl_NewStringObj (GD_VERSION, -1));
	return TCL_OK;
      }

      case OPT_FEATURES: {
	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}

	Tcl_SetObjResult (interp, Tcl_NewStringObj (GD_FEATURES, -1));
	return TCL_OK;
      }
    }

    /* if we got here we completed one of the create routines...
     *
     * if inctx was allocated, we need to free it
     */
    if (inctx == NULL) {
	gdFree (inctx);
    }

    /*
     * if the image pointer is null, something is wrong, complain and bail
     */
    if (im == NULL) {
	return tclgd_complainCorrupt(interp);
    }

    return tclgd_newGDObject (interp, objv[2], im, 1);
}

