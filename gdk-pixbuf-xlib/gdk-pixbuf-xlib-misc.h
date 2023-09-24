/* GdkPixbuf library - Xlib-MISC header file
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
#ifndef GDK_PIXBUF_XLIB_MISC_H
#define GDK_PIXBUF_XLIB_MISC_H

G_BEGIN_DECLS

#include <X11/Xlib.h>

/**
 * XlibPropMode:
 * @XLIB_PROP_MODE_REPLACE: the new data replaces the existing data.
 * @XLIB_PROP_MODE_PREPEND: the new data is prepended to the existing data.
 * @XLIB_PROP_MODE_APPEND: the new data is appended to the existing data.
 *
 * Describes how existing data is combined with new data when
 * using xlib_property_change().
 */
typedef enum
{
  XLIB_PROP_MODE_REPLACE,
  XLIB_PROP_MODE_PREPEND,
  XLIB_PROP_MODE_APPEND
} XlibPropMode;

_GDK_PIXBUF_EXTERN
int xlib_clear_window(Window xid);

_GDK_PIXBUF_EXTERN
gboolean xlib_property_get (Window   window,
                            Atom     property,
                            Atom     type,
                            gulong   offset,
                            gulong   length,
                            gint     pdelete,
                            Atom    *actual_property_type,
                            gint    *actual_format_type,
                            gint    *actual_length,
                            guchar **data);

_GDK_PIXBUF_EXTERN
void xlib_property_change (Window        window,
                           Atom          property,
                           Atom          type,
                           gint          format,
                           XlibPropMode  mode,
                           const guchar *data,
                           gint          nelements);

_GDK_PIXBUF_EXTERN
Atom xlib_atom_intern (const gchar *atom_name,
                      gboolean     only_if_exists);

_GDK_PIXBUF_EXTERN
gchar * xlib_atom_name (Atom atom);

G_END_DECLS

#endif /* GDK_PIXBUF_XLIB_MISC_H */
