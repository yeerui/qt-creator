TEMPLATE = subdirs

SUBDIRS = process unittest

unittest.depends = process
