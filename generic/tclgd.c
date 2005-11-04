/*
 * tcl.gd
 *
 * Copyright (C) 2005 by Karl Lehenbauer, All Rights Reserved
 *
 * $Id: tclgd.c,v 1.12 2005-11-04 02:58:09 karl Exp $
 */

#include "tclgd.h"
#include <string.h>

static char *
tclgd_newObjName (Tcl_Obj *nameObj)
{
    char *name = Tcl_GetString (nameObj);
    static int nextObjNumber = 0;
    static char nextObjName[16];

    if (strcmp (name, "#auto") == 0) {
	sprintf (nextObjName, "gd%d", nextObjNumber++);
	return nextObjName;
    }

    return name;
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
    gdImagePtr im = (gdImagePtr)clientData;

    gdImageDestroy(im);
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
    gdImagePtr im = (gdImagePtr)cData;
    int         optIndex;
    Tcl_Obj    *resultObj = Tcl_GetObjResult(interp);

    static CONST char *options[] = {
	"pixel",
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
	"width",
	"height",
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
	OPT_WIDTH,
	OPT_HEIGHT,
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
	   Tcl_SetIntObj (resultObj, gdImageGetPixel (im, x, y));
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
	    "basic"
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
	    "edged"
	};

	enum styles {
	    STYLE_ARC,
	    STYLE_CHORD,
	    STYLE_PIE,
	    STYLE_NOFILL,
	    STYLE_EDGED
	};

	if (objc < 10) {
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

	if (Tcl_GetIndexFromObj(interp, objv[9], styles, "style", 
	    TCL_EXACT, &styleIndex) != TCL_OK) {
	    return TCL_ERROR;
	}

	for (i = 9; i < objc; i++) {

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

	resultString = gdImageStringFT (im, &brect[0], color, fontName, pointSize, angle, x, y, text);
	if (resultString != NULL) {
	    Tcl_SetStringObj (resultObj, resultString, -1);
	    Tcl_AppendResult (interp, " '", fontName, "'", NULL);
	    return TCL_ERROR;
	}

	if (optIndex == OPT_TEXT_BOUNDS) {
	    int i;

	    for (i = 0; i < 8; i++) {
		brectObjv[i] = Tcl_NewIntObj(brect[i]);
	    }
	    Tcl_SetListObj (resultObj, 8, brectObjv);
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
	    Tcl_SetIntObj (resultObj, gdImageColorAllocate (im, r, g, b));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetIntObj (resultObj, gdImageColorAllocateAlpha (im, r, g, b, alpha));
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
	    Tcl_SetIntObj (resultObj, gdImageColorClosest (im, r, g, b));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetIntObj (resultObj, gdImageColorClosestAlpha (im, r, g, b, alpha));
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

	Tcl_SetIntObj (resultObj, gdImageColorClosestHWB (im, r, g, b));
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
	    Tcl_SetIntObj (resultObj, gdImageColorExact (im, r, g, b));
	    return TCL_OK;
	}

        if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
        }

	Tcl_SetIntObj (resultObj, gdImageColorExactAlpha (im, r, g, b, alpha));
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
	    Tcl_SetIntObj (resultObj, gdImageColorResolve (im, r, g, b));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetIntObj (resultObj, gdImageColorResolveAlpha (im, r, g, b, alpha));
	break;
      }

      case OPT_TOTAL_COLORS:
	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}
	Tcl_SetIntObj (resultObj, gdImageColorsTotal (im));
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
	    Tcl_SetIntObj (resultObj, gdTrueColor (r, g, b));
	    return TCL_OK;
	}

	if (Tcl_GetIntFromObj (interp, objv[5], &alpha) == TCL_ERROR) {
	   return tclgd_complainAlpha (interp);
	}

	Tcl_SetIntObj (resultObj, gdTrueColorAlpha (r, g, b, alpha));
	return TCL_OK;
      }

      case OPT_INTERLACE: {
	int interlaced;

	if (objc == 2) {
	  Tcl_SetIntObj (resultObj, gdImageGetInterlaced (im));
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

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (tclgd_GetColor (interp, objv[2], &color) == TCL_ERROR) {
	    return TCL_ERROR;
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

      case OPT_SET_BRUSH:
	break;

      case OPT_SET_TILE:
	break;

      case OPT_STYLE:
	break;

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
	    Tcl_SetListObj (resultObj, 4, clipObjv);
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

      case OPT_GET_ALPHA:
	break;

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

	Tcl_SetBooleanObj (resultObj, gdImageBoundsSafe (im, x, y));
	break;
      }

      case OPT_GREEN_COMPONENT: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	   return tclgd_complainColor (interp);
	}

	Tcl_SetIntObj (resultObj, gdImageGreen (im, color));
	return TCL_OK;
      }

      case OPT_RED_COMPONENT: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	Tcl_SetIntObj (resultObj, gdImageRed (im, color));
	return TCL_OK;
      }

      case OPT_BLUE_COMPONENT: {
	int color;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "color");
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[2], &color) == TCL_ERROR) {
	    return tclgd_complainColor (interp);
	}

	Tcl_SetIntObj (resultObj, gdImageBlue (im, color));
	return TCL_OK;
      }

      case OPT_WIDTH:
	Tcl_SetIntObj (resultObj, gdImageSX(im));
	break;

      case OPT_HEIGHT:
	Tcl_SetIntObj (resultObj, gdImageSY(im));
	break;

      case OPT_COPY: {
        int          destX;
	int          destY;
	int          srcX;
	int          srcY;
	int          w;
	int          h;
	gdImagePtr   srcIm;
	Tcl_CmdInfo  cmdInfo;

	if (objc != 9) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY w h");
	    return TCL_ERROR;
	}

	if (!Tcl_GetCommandInfo ( interp, Tcl_GetString(objv[2]), &cmdInfo)) {
	    return TCL_ERROR;
	}
	srcIm = (gdImagePtr)cmdInfo.objClientData;


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
	Tcl_CmdInfo  cmdInfo;

	if (objc != 11) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY destW destH srcW srcH");
	    return TCL_ERROR;
	}

	if (!Tcl_GetCommandInfo ( interp, Tcl_GetString(objv[2]), &cmdInfo)) {
	    return TCL_ERROR;
	}
	srcIm = (gdImagePtr)cmdInfo.objClientData;


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
	Tcl_CmdInfo  cmdInfo;

	if (objc != 10) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY destW destH srcW srcH angle");
	    return TCL_ERROR;
	}

	if (!Tcl_GetCommandInfo ( interp, Tcl_GetString(objv[2]), &cmdInfo)) {
	    return TCL_ERROR;
	}
	srcIm = (gdImagePtr)cmdInfo.objClientData;


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
	Tcl_CmdInfo  cmdInfo;

	if (objc != 10) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand destX destY srcX srcY width height percent");
	    return TCL_ERROR;
	}

	if (!Tcl_GetCommandInfo ( interp, Tcl_GetString(objv[2]), &cmdInfo)) {
	    return TCL_ERROR;
	}
	srcIm = (gdImagePtr)cmdInfo.objClientData;


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
	Tcl_CmdInfo  cmdInfo;
	gdImagePtr   srcIm;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "srcImageCommand");
	    return TCL_ERROR;
	}

	if (!Tcl_GetCommandInfo ( interp, Tcl_GetString(objv[2]), &cmdInfo)) {
	    return TCL_ERROR;
	}
	srcIm = (gdImagePtr)cmdInfo.objClientData;

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

      case OPT_COMPARE:
	break;

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

      case OPT_WRITE_JPEG: {
	int   quality;
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "fileHandle quality");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[2]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &quality) == TCL_ERROR) {
	   return tclgd_complainQuality (interp);
	}

	gdImageJpeg (im, file, quality);
	fflush (file);
	return TCL_OK;
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
	Tcl_SetByteArrayObj (resultObj, memPtr, size);
	return TCL_OK;
      }

      case OPT_WRITE_GIF: {
	gdIOCtx     *outctx;

	if (objc != 3) {
	    Tcl_WrongNumArgs (interp, 2, objv, "channel");
	    return TCL_ERROR;
	}

	if ((outctx = tclgd_channelNameToIOCtx (interp, Tcl_GetString(objv[2]))) == NULL) {
	    return TCL_ERROR;
	}

	gdImageGifCtx (im, outctx);
	return TCL_OK;
      }

      case OPT_GIF_DATA: {
	int      size;
	void    *memPtr;

	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}

	memPtr = gdImageGifPtr (im, &size);
	Tcl_SetByteArrayObj (resultObj, memPtr, size);
	return TCL_OK;
      }

      case OPT_WRITE_PNG: {
	int   compression;
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "fileHandle compressionLevel");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[2]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &compression) == TCL_ERROR) {
	   return tclgd_complainCompression (interp);
	}

	if ((compression < -1) || (compression > 9)) {
	    Tcl_AppendResult (interp, "compression must be between -1 (default), 0 (no compression), 1 (quick compress) - 9 (best possible)", NULL);
	    return TCL_ERROR;
	}

	gdImagePngEx (im, file, compression);
	fflush (file);
	return TCL_OK;
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
	Tcl_SetByteArrayObj (resultObj, memPtr, size);
	return TCL_OK;
      }

      case OPT_WRITE_WBMP: {
	int   fgcolor;
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "fileHandle fgcolor");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[2]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	if (Tcl_GetIntFromObj (interp, objv[3], &fgcolor) == TCL_ERROR) {
	   return tclgd_complainFgColor (interp);
	}

	gdImageWBMP (im, fgcolor, file);
	fflush (file);
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
	Tcl_SetByteArrayObj (resultObj, memPtr, size);
	return TCL_OK;
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
	return TCL_OK;
      }

      case OPT_GD_DATA: {
	int      size;
	void    *memPtr;

	if (objc != 2) {
	    Tcl_WrongNumArgs (interp, 2, objv, "");
	    return TCL_ERROR;
	}

	memPtr = gdImageGdPtr (im, &size);
	Tcl_SetByteArrayObj (resultObj, memPtr, size);
	return TCL_OK;
      }

      case OPT_WRITE_GD2: {
      }

      case OPT_GD2_DATA: {
      }

    }

    return TCL_OK;
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
    char        *newName;
    Tcl_Obj     *resultObj = Tcl_GetObjResult(interp);


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
	OPT_CREATE_FROM_XPM
    };

    if (objc < 3) {
	Tcl_WrongNumArgs (interp, 1, objv, "subcommand name ?args?");
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

      case OPT_CREATE_FROM_JPEG: {
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromJpeg (file);
	fflush (file);
	break;
      }

      case OPT_CREATE_FROM_JPEG_DATA: {
        int            size;
	unsigned char *memPtr;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name data");
	    return TCL_ERROR;
	}

	memPtr = Tcl_GetByteArrayFromObj (objv[2], &size);
	im = gdImageCreateFromJpegPtr (size, memPtr);
	break;
      }

      case OPT_CREATE_FROM_PNG: {
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromPng (file);
	fflush (file);
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

      case OPT_CREATE_FROM_GIF: {
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromGif (file);
	fflush (file);
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

      case OPT_CREATE_FROM_GD: {
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromGd (file);
	fflush (file);
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
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromGd2 (file);
	fflush (file);
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
	FILE *file;
	int   x;
	int   y;
	int   w;
	int   h;

	if (objc != 8) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle x y w h");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

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

	im = gdImageCreateFromGd2Part (file, x, y, w, h);
	fflush (file);
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
	FILE *file;

	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileHandle");
	    return TCL_ERROR;
	}

	if (Tcl_GetOpenFile (interp, Tcl_GetString(objv[3]), 1, 1, (ClientData *)&file) == TCL_ERROR) {
	    return TCL_ERROR;
	}

	im = gdImageCreateFromWBMP (file);
	fflush (file);
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
	fflush (file);
	break;
      }

      case OPT_CREATE_FROM_XPM: {
	if (objc != 4) {
	    Tcl_WrongNumArgs (interp, 2, objv, "name fileName");
	    return TCL_ERROR;
	}

	im = gdImageCreateFromXpm (Tcl_GetString(objv[2]));
	break;
      }
    }

    if (im == NULL) {
	Tcl_AppendResult (interp, "image is corrupt or incorrect image type", NULL);
	return TCL_ERROR;
    }

    newName = tclgd_newObjName (objv[2]);
    Tcl_CreateObjCommand (interp, newName, tclgd_gdObjectObjCmd, im, tclgd_GDdeleteProc);
    Tcl_SetStringObj (resultObj, newName, -1);
    return TCL_OK;
}

