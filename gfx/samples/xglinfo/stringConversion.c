/*
 * 
 *           Copyright (c) Digital Equipment Corporation, 1995
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute  this software and its
 * documentation for any  purpose   and without fee  is  hereby  granted,
 * provided that the above copyright notice appear in all copies and that
 * both  that  copyright  notice  and  this  permission notice appear  in
 * supporting documentation, and that the name of Digital  not be used in
 * advertising or publicity  pertaining to distribution  of the  software
 * without specific, written prior permission.
 * 
 * DIGITAL DISCLAIMS   ALL  WARRANTIES WITH   REGARD   TO  THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT   SHALL  DIGITAL  BE   LIABLE  FOR  ANY SPECIAL,   INDIRECT   OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION  OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/*
 * HISTORY
 */
/*****************************************************************************/
/******************************** Documentation ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Include Files *******************************/
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "global.h"

/*****************************************************************************/
/****************************** Internal Defines *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Internal Type Definitions ************************/
/*****************************************************************************/

typedef struct {
  long Key;
  char *String;
} NumberedStrings;


/*****************************************************************************/
/**********************  External Function Declarations  *********************/
/*****************************************************************************/

/*****************************************************************************/
/**********************  Internal Function Declarations  *********************/
/*****************************************************************************/

/*****************************************************************************/
/*************************  External Global Variables  ***********************/
/*****************************************************************************/

/*****************************************************************************/
/*************************  Internal Global Variables  ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************  Internal Functions  ****************************/
/*****************************************************************************/

static void
HeapSort(
long     n,
NumberedStrings x[]
)
{
  long i;			/* array index */
  long j;			/* array index */
  long k;			/* array index */
  NumberedStrings y;		/* temp element for swapping */

  /*
   * heap sort only works if n is greater than or equal to 3, so if n < 3
   * sort trivally
   */
  if (n == 1)
    return;
  if (n == 2) {
    if (x[0].Key > x[1].Key) {
      y = x[0];
      x[0] = x[1];
      x[1] = y;
    }
    return;
  }
  /* create initial heap */
  for (k = 1; k < n; k++) {
    /* insert x[k] into existing heap of size k-1 */
    i = k;
    y = x[k];		/* y is node to insert */
    j = (i - 1) / 2;		/* j is the father of i */
    while (i >= 1) {
      if (y.Key <= x[j].Key)
	break;
      x[i] = x[j];
      i = j;
      j = (i - 1) / 2;
    }
    x[i] = y;
  }

  /*
   * We remove x[0] and place it in its proper position in the array. We then
   * adjust the heap
   */
  for (k = n - 1; k > 0; k--) {
    y = x[k];
    x[k] = x[0];
    /*
     * Readjust the heap of order k-1. Move y down the heap for proper
     * position
     */
    i = 0;
    j = 1;
    if ((k - 1 >= 2) && (x[2].Key > x[1].Key))
      j = 2;
    /* j is the larger son of i in the heap of size k-1 */
    while (j <= k - 1) {
      if (x[j].Key <= y.Key)
	break;
      x[i] = x[j];
      i = j;
      j = (2 * i) + 1;
      if (j + 1 <= k - 1)
	if (x[j + 1].Key > x[j].Key)
	  j++;
    }
    x[i] = y;
  }
}


static char *BinarySearch (
  long Key,
  NumberedStrings *Table,
  long TableSize
)
{
  long Low = 0;
  long High = TableSize - 1;
  long Mid;
  static char ErrString[80];

  while (Low <= High) {
    Mid = (Low + High) / 2;
    if (Key == Table[Mid].Key) return (Table[Mid].String);
    if (Key < Table[Mid].Key)
      High = Mid - 1;
    else
      Low = Mid + 1;
  }
  sprintf(ErrString, "Key Unknown %d", Key);
  return (ErrString);
}

/*****************************************************************************/
/****************************  Exported Functions  ***************************/
/*****************************************************************************/

char *VisualClassName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {StaticGray,	"StaticGray"},
    {GrayScale,		"GrayScale"},
    {StaticColor,	"StaticColor"},
    {PseudoColor,	"PseudoColor"},
    {TrueColor,		"TrueColor"},
    {DirectColor,	"DirectColor"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *EventMaskName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {NoEventMask,		"NoEvent"},
    {KeyPressMask,		"KeyPress"},
    {KeyReleaseMask,		"KeyRelease"},
    {ButtonPressMask,		"ButtonPress"},
    {ButtonReleaseMask,		"ButtonRelease"},
    {EnterWindowMask,		"EnterWindow"},
    {LeaveWindowMask,		"LeaveWindow"},
    {PointerMotionMask,		"PointerMotion"},
    {PointerMotionHintMask,	"PointerMotionHint"},
    {Button1MotionMask,		"Button1Motion"},
    {Button2MotionMask,		"Button2Motion"},
    {Button3MotionMask,		"Button3Motion"},
    {Button4MotionMask,		"Button4Motion"},
    {Button5MotionMask,		"Button5Motion"},
    {ButtonMotionMask,		"ButtonMotion"},
    {KeymapStateMask,		"KeymapState"},
    {ExposureMask,		"Exposure"},
    {VisibilityChangeMask,	"VisibilityChange"},
    {StructureNotifyMask,	"StructureNotify"},
    {ResizeRedirectMask,	"ResizeRedirect"},
    {SubstructureNotifyMask,	"SubstructureNotify"},
    {SubstructureRedirectMask,	"SubstructureRedirect"},
    {FocusChangeMask,		"FocusChange"},
    {PropertyChangeMask,	"PropertyChange"},
    {ColormapChangeMask,	"ColormapChange"},
    {OwnerGrabButtonMask,	"OwnerGrabButton"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *ByteOrderName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {LSBFirst,	"LSBFirst"},
    {MSBFirst,	"MSBFirst"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *WindowFocusName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {None,		"None"},
    {PointerRoot,	"PointerRoot"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

char *WindowFocusRevertName(
  long Key
)
{
  static NumberedStrings Names[] = {
    {RevertToNone,		"RevertToNone"},
    {RevertToPointerRoot,	"RevertToPointerRoot"},
    {RevertToParent,		"RevertToParent"},
  };
  static int Sorted = 0;	/* not sorted yet */
  static const long N = sizeof(Names) / sizeof(NumberedStrings);

  if (!Sorted) {
    Sorted = 1;
    HeapSort(N, Names);
  }
  return(BinarySearch(Key, Names, N));
}

