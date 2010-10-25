
TOP=$(PWD)/..

ENSYMBLE_PY=C:\sdk\symbian\ensymble\ensymble_python2.5-0.27.py

MAKMAKE:

BLD:

CLEAN:

LIB:

CLEANLIB:

RESOURCE:

FREEZE:

SAVESPACE:

RELEASABLES:

FINAL:
	python $(ENSYMBLE_PY) py2sis --uid=0xE66B8ECD --version=0.0.1 --shortcaption="pyffmpeg" --caption="pyffmpeg" --caps=LocalServices+ReadUserData+WriteUserData+NetworkServices+UserEnvironment $(TOP)/frontend/pyffmpeg.py $(TOP)/sis/pyffmpeg.sis
