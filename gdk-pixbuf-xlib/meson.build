gdkpixbuf_xlib_sources = [
  'gdk-pixbuf-xlib.c',
  'gdk-pixbuf-xlib-render.c',
  'gdk-pixbuf-xlib-drawable.c',
  'gdk-pixbuf-xlibrgb.c',
]

gdkpixbuf_xlib_headers = [
  'gdk-pixbuf-xlib.h',
  'gdk-pixbuf-xlibrgb.h',
]

gdkpixbuf_xlib_inc = include_directories('.')

gdkpixbuf_xlib_api_path = join_paths(api_name, 'gdk-pixbuf-xlib')

install_headers(gdkpixbuf_xlib_headers, subdir: gdkpixbuf_xlib_api_path)

gdkpixbuf_xlib = library(
  'gdk_pixbuf_xlib-2.0',
  gdkpixbuf_xlib_sources,
  soversion: soversion,
  version: libversion,
  c_args: common_cflags + [
    '-DSTDC_HEADERS',
  ],
  link_args: common_ldflags,
  include_directories: [
    root_inc,
  ],
  dependencies: [ gdk_pixbuf_dep, x11_dep ],
  install: true,
)

gdkpixbuf_xlib_dep = declare_dependency(
  link_with: gdkpixbuf_xlib,
  include_directories: gdkpixbuf_xlib_inc,
)

pkgconfig = import('pkgconfig')

pkgconfig.generate(
  gdkpixbuf_xlib,
  name: 'GdkPixbuf Xlib',
  description: 'GdkPixbuf rendering for Xlib',
  filebase: 'gdk-pixbuf-xlib-2.0',
  requires: [
    gdk_pixbuf_dep,
    x11_dep,
  ],
  subdirs: api_name,
)

gir = find_program('g-ir-scanner', required : get_option('introspection'))
build_gir = gir.found() and (not meson.is_cross_build() or get_option('introspection').enabled())

if build_gir
gir_args = [
  '--warn-all',
  '--accept-unprefixed',
]

gdkpixbuf_xlib_gir = gnome.generate_gir(
    gdkpixbuf_xlib,
    sources: gdkpixbuf_xlib_sources + gdkpixbuf_xlib_headers,
    namespace: 'GdkPixbufXlib',
    nsversion: api_version,
    identifier_prefix: ['Gdk', 'Xlib'],
    symbol_prefix: ['gdk_pixbuf_xlib', 'xlib'],
    export_packages: 'gdk-pixbuf-xlib-2.0',
    includes: [ 'GModule-2.0', 'Gio-2.0', 'GdkPixbuf-2.0', 'xlib-2.0'],
    header: ['gdk-pixbuf/gdk-pixbuf-xlib.h', 'gdk-pixbuf/gdk-pixbuf-xlibrgb.h'],
    install: true,
  )
endif
