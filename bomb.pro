TEMPLATE = subdirs
SUBDIRS = common network serverd gui
network.depends = common
gui.depends = common network
serverd.depends = common network
