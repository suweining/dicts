CPP			= /opt/gcc-4.7.2/bin/g++

all:
	$(MAKE) -C src
clean:
	$(MAKE) -C src


DICT_VERSION	=	v.1.0.0
PACKAGE_NAME	=	dict_server-$(DICT_VERSION)
TARGETS		=	./src/server

install:
	rm -rf packages/$(PACKAGE_NAME)
	rm -rf packages/$(PACKAGE_NAME).tgz

	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/bin
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/lib
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/config
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/data
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/proc
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/logs


	mv $(TARGETS)  packages/$(PACKAGE_NAME)/bin
	cp config/config.ini packages/$(PACKAGE_NAME)/config

	find packages/$(PACKAGE_NAME) -name ".git" | xargs rm -rf
	find packages/$(PACKAGE_NAME) -type l|xargs rm -rf

package: clean all install
	tar zcf packages/$(PACKAGE_NAME).tgz -C packages $(PACKAGE_NAME)

.PHONY: install clean
