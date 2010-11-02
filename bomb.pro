TEMPLATE = subdirs
SUBDIRS = common client gui serverd
client.depends = common
gui.depends = common client
serverd.depends = common

