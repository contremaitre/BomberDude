TEMPLATE = subdirs
SUBDIRS = src gui serverd
gui.depends = src
serverd.depends = src
