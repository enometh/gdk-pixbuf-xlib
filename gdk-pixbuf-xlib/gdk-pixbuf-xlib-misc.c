/* GdkPixbuf library - Xlib-MISC functions
 *
 * Authors: GTK+ authors
 * SPDX-License-Identifier: GPL-2.1-or-later
 *
 *   Time-stamp: <>
 *   Touched: Fri Sep 22 14:44:24 2023 +0530 <enometh@net.meer>
 *   Bugs-To: enometh@net.meer
 *   Status: Experimental.  Do not redistribute
 *   Copyright (C) 2023 Madhu.  All Rights Reserved.
 */

#include "config.h"
#include <X11/Xatom.h>
#include "gdk-pixbuf-xlib-private.h"

/*
 * ----------------------------------------------------------------------
 * ;madhu 230922 misc
 *
 * - xlib_clear_window: to force a refresh
 *
 * - xlib_property_get: copies gdk_property_get from gdk-3.0 which
 *   never worked with pygi or gjs.
 *   <https://bugzilla.gnome.org/attachment.cgi?id=208093>,
 *   <https://gitlab.gnome.org/GNOME/gtk/-/issues/383>.  gjs-1.76.2
 *   just segfaults on Gdk.init. Doesn't use gdk types.
 *
 * - xlib_property_change: copies gdk_property_change from gdk-3.0
 *   which was skipped by introspection, without using gdk types.
 *
 * - xlib_atom_intern: thin wrapper around XInternAtom without any of
 *   the GdkAtom optimizations.
 *
 * - xlilb_atom_name: thin wrapper around XGetAtomName without any of
 *   the GdkAtom optimizations.
 */


/**
 * xlib_clear_window:
 * @xid: (type xlib.Window): Window xid
 *
 * Calls XClearWindow on Window @xid.
 *
 * Returns: FIXME
 **/
int
xlib_clear_window(Window xid)
{
	return XClearWindow (gdk_pixbuf_dpy, xid);
}

/**
 * xlib_property_get:
 * @window: an #xlib.Window
 * @property: the property to retrieve
 * @type: the desired property type, or %NULL, if any type of data
 *   is acceptable. If this does not match the actual
 *   type, then @actual_format and @actual_length will
 *   be filled in, a warning will be printed to stderr
 *   and no data will be returned.
 * @offset: the offset into the property at which to begin
 *   retrieving data, in 4 byte units.
 * @length: the length of the data to retrieve in bytes.  Data is
 *   considered to be retrieved in 4 byte chunks, so @length
 *   will be rounded up to the next highest 4 byte boundary
 *   (so be careful not to pass a value that might overflow
 *   when rounded up).
 * @pdelete: if %TRUE, delete the property after retrieving the
 *   data.
 * @actual_property_type: (out) (transfer none): location to store the
 *   actual type of the property.
 * @actual_format_type: (out): location to store the actual return
 *   format of the data; either 8, 16 or 32 bits.
 * @actual_length: location to store the length of the retrieved data,
 *   bytes.  Data returned in the 32 bit format is stored
 *   in a long variable, so the actual number of 32 bit
 *   elements should be be calculated via
 *   @actual_length / sizeof(glong) to ensure portability to
 *   64 bit systems.
 * @data: (out) (array length=actual_length) (transfer full): location
 *   to store a pointer to the data. The retrieved data should be
 *   freed with g_free() when you are finished using it.
 *
 * Retrieves a portion of the contents of a property. If the
 * property does not exist, then the function returns %FALSE,
 * and %GDK_NONE will be stored in @actual_property_type.
 *
 * Calls XGetWindowProperty(). This function and this documentation is
 * a cutpaste job on gdk_property_get from gtk+-3.24.
 *
 * The XGetWindowProperty() function that xlib_property_set()
 * uses has a very confusing and complicated set of semantics.
 * Unfortunately, xlib_property_get() makes the situation
 * worse instead of better (the semantics should be considered
 * undefined), and also prints warnings to stderr in cases where it
 * should return a useful error to the program. You are advised to use
 * XGetWindowProperty() directly until a replacement function for
 * xlib_property_get() is provided.
 *
 *
 * Returns: %TRUE if data was successfully received and stored
 *   in @data, otherwise %FALSE.
 */
gboolean
xlib_property_get (Window   window,
                   Atom     property,
                   Atom     type,
                   gulong   offset,
                   gulong   length,
                   gint     pdelete,
                   Atom    *actual_property_type,
                   gint    *actual_format_type,
                   gint    *actual_length,
                   guchar **data)
{
  Atom ret_prop_type;
  gint ret_format;
  gulong ret_nitems;
  gulong ret_bytes_after;
  gulong get_length;
  gulong ret_length;
  guchar *ret_data;
  int res;

  ret_data = NULL;

  /*
   * Round up length to next 4 byte value.  Some code is in the (bad?)
   * habit of passing G_MAXLONG as the length argument, causing an
   * overflow to negative on the add.  In this case, we clamp the
   * value to G_MAXLONG.
   */
  get_length = length + 3;
  if (get_length > G_MAXLONG)
    get_length = G_MAXLONG;

  /* To fail, either the user passed 0 or G_MAXULONG */
  get_length = get_length / 4;
  if (get_length == 0)
    {
      g_warning ("gdk_propery-get(): invalid length 0");
      return FALSE;
    }

  res = XGetWindowProperty (gdk_pixbuf_dpy,
			    window,
			    property,
			    offset, get_length, pdelete,
			    type, &ret_prop_type, &ret_format,
			    &ret_nitems, &ret_bytes_after,
			    &ret_data);

  if (res != Success || (ret_prop_type == None && ret_format == 0))
    {
      return FALSE;
    }

  if (actual_property_type)
    *actual_property_type = ret_prop_type;
  if (actual_format_type)
    *actual_format_type = ret_format;

  if ((type != AnyPropertyType) && (ret_prop_type != type))
    {
      XFree (ret_data);
      g_warning ("Couldn't match property type %s to %s\n",
		 XGetAtomName (gdk_pixbuf_dpy, ret_prop_type),
		 XGetAtomName (gdk_pixbuf_dpy, type));
      return FALSE;
    }

  /* FIXME: ignoring bytes_after could have very bad effects */
  if (ret_bytes_after)
    g_warning ("Ignoring non-zero bytes_after %lu. The results will be wrong.\n",
	       ret_bytes_after);

  if (data)
    {
      if (ret_prop_type == XA_ATOM ||
	  ret_prop_type == XInternAtom (gdk_pixbuf_dpy, "ATOM_PAIR", TRUE))
	{
	  /*
	   * data is an array of X atoms, we pointlessly copy it into
	   * a new array of Atoms.
	   */
	  Atom *ret_atoms = g_new (Atom, ret_nitems);
	  Atom *xatoms = (Atom *)ret_data;
	  int i;

	  *data = (guchar *)ret_atoms;

	  for (i = 0; i < ret_nitems; i++)
	    ret_atoms[i] =  xatoms[i];

	  if (actual_length)
	    *actual_length = ret_nitems * sizeof (Atom);
	}
      else
	{
	  switch (ret_format)
	    {
	    case 8:
	      ret_length = ret_nitems;
	      break;
	    case 16:
	      ret_length = sizeof(short) * ret_nitems;
	      break;
	    case 32:
	      ret_length = sizeof(long) * ret_nitems;
	      break;
	    default:
	      g_warning ("unknown property return format: %d", ret_format);
	      XFree (ret_data);
	      return FALSE;
	    }

	  *data = g_new (guchar, ret_length);
	  memcpy (*data, ret_data, ret_length);
	  if (actual_length)
	    *actual_length = ret_length;
	}
    }

  XFree (ret_data);

  return TRUE;

}

/**
 * xlib_property_change:
 * @window: (type xlib.Window): a Window
 * @property: the property to change
 * @type: the new type for the property. If @mode is
 *   %XLIB_PROP_MODE_PREPEND or %XLIB_PROP_MODE_APPEND, then this
 *   must match the existing type or an error will occur.
 * @format: the new format for the property. If @mode is
 *   %XLIB_PROP_MODE_PREPEND or %XLIB_PROP_MODE_APPEND, then this
 *   must match the existing format or an error will occur.
 * @mode: a value describing how the new data is to be combined
 *   with the current data.
 * @data: the data (a `guchar *`
 *   `gushort *`, or `gulong *`,
 *   depending on @format), cast to a `guchar *`.
 * @nelements: the number of elements of size determined by the format,
 *   contained in @data.
 *
 * Changes the contents of a property on a window.
 *
 * Calls XChangeProperty().  This function and documentation is a
 * cut-paste job from gdk_property_change which is not in g-i.
 *
 *
 */
void
xlib_property_change (Window        window,
                      Atom          property,
                      Atom          type,
                      gint          format,
                      XlibPropMode  mode,
                      const guchar *data,
                      gint          nelements)
{
  if (type == XA_ATOM ||
      type == XInternAtom (gdk_pixbuf_dpy, "ATOM_PAIR", TRUE))
    {
      /*
       * data is an array of Atoms, we needlessly to convert it
       * to an array of X Atoms
       */
      gint i;
      Atom *atoms = (Atom*) data;
      Atom *xatoms;

      xatoms = g_new (Atom, nelements);
      for (i = 0; i < nelements; i++)
	xatoms[i] = atoms[i];

      XChangeProperty (gdk_pixbuf_dpy, window,
		       property, type,
		       format, mode, (guchar *)atoms, nelements);
      g_free (xatoms);
    }
  else
    XChangeProperty (gdk_pixbuf_dpy, window, property,
		     type, format, mode, (guchar *)data, nelements);
}

/**
 * xlib_atom_intern:
 * @atom_name: a string.
 * @only_if_exists :return %NULL if the requested atom doesn’t already
 *   exists.
 * Finds or creates an atom corresponding to a given string.
 *
 * Returns: the #xlib.Atom
 * corresponding to @atom_name.
 */
Atom
xlib_atom_intern (const gchar *atom_name,
                  gboolean     only_if_exists)
{
  g_return_val_if_fail (atom_name != NULL, NULL);

  return XInternAtom (gdk_pixbuf_dpy, atom_name, only_if_exists);
}

static char         _x_err = 0;

static int
Tmp_HandleXError (Display * d, XErrorEvent * ev)
{
  g_return_val_if_fail (ev->error_code != 0, 0);
  _x_err = ev->error_code;
  return 0;
}

/**
 * xlib_atom_name:
 * @atom: a #xlib.Atom.
 *
 * Determines the string corresponding to an atom.
 *
 * Returns: a newly-allocated string containing the string
 *   corresponding to @atom. When you are done with the
 *   return value, you should free it using g_free().
 */
gchar *
xlib_atom_name (Atom atom)
{
  XErrorHandler       prev_erh;
  gchar *str = 0;

  XSync (gdk_pixbuf_dpy, False);
  prev_erh = XSetErrorHandler (Tmp_HandleXError);
  _x_err = 0;
  str = XGetAtomName (gdk_pixbuf_dpy, atom);
  XSync (gdk_pixbuf_dpy, False);
  XSetErrorHandler (prev_erh);

  if (_x_err != 0)
    g_warning ("error: badatom (5): code=%d", _x_err);

  return str;
}
