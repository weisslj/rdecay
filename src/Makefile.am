bin_PROGRAMS = rdecay
rdecay_SOURCES = \
    main.c        \
    gettext.h     \
    topwin.c      topwin.h    \
    afield.c      afield.h    \
    coord.c       coord.h     \
    graph.c       graph.h     \
    color.c       color.h     \
    ui_afield.c   ui_afield.h \
    ui_graph.c    ui_graph.h  \
    darea.c       darea.h     \
    input.c       input.h     \
    status.c      status.h    \
    timer.c       timer.h     \
    random.c      random.h    \
    sim.c         sim.h

#    sound.c       sound.h

localedir = $(datadir)/locale
#AM_CFLAGS = -ansi -pedantic -W -Wall -DLOCALEDIR=\"$(localedir)\" @GTK_CFLAGS@ @GSL_CFLAGS@ @SDL_CFLAGS@
#LIBS = @LIBINTL@ @LIBS@ @GTK_LIBS@ @GSL_LIBS@ @SDL_LIBS@
AM_CFLAGS = -DLOCALEDIR=\"$(localedir)\" @WARN_FLAGS@ @GTK_CFLAGS@ @GSL_CFLAGS@
LIBS = @LIBINTL@ @LIBS@ @GTK_LIBS@ @GSL_LIBS@
