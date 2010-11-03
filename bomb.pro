TEMPLATE = subdirs
SUBDIRS = common client server serverd gui
client.depends = common
server.depends = common
gui.depends = common client server
serverd.depends = common server

