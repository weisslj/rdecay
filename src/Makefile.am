bin_PROGRAMS = rdecay
rdecay_SOURCES = \
    main.c        \
    arg.c         arg.h       \
    opt.c         opt.h       \
    atoms.c       atoms.h     \
    gui.c         gui.h       \
    afield.c      afield.h    \
    coord.c       coord.h     \
    graph.c       graph.h     \
    color.c       color.h     \
    ui_afield.c   ui_afield.h \
    ui_graph.c    ui_graph.h  \
    darea.c       darea.h     \
    status.c      status.h    \
    timer.c       timer.h     \
    util.c        util.h      \
    random.c      random.h    \
    sim.c         sim.h

localedir = $(datadir)/locale
AM_CFLAGS = -DLOCALEDIR=\"$(localedir)\" @WARN_FLAGS@ @GTK_CFLAGS@ @GSL_CFLAGS@
INCLUDES = -I../lib
LIBS = @LIBINTL@ @LIBS@ @GTK_LIBS@ @GSL_LIBS@
