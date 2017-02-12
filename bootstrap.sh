#!/bin/sh
autoreconf -i
# Bug in gettext package > 0.18.3, `make dist` needs ChangeLog file:
touch intl/ChangeLog
