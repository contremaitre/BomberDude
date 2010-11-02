TEMPLATE = subdirs
SUBDIRS = common gui serverd
gui.depends = common
serverd.depends = common
