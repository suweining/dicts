CPP		= 	/opt/gcc-4.7.2/bin/g++
DICT_VERSION	=	v.1.0.0
PACKAGE_NAME	=	match_engine_$(DICT_VERSION)

all:
	/usr/bin/install -c -d libdicts/lib
	/usr/bin/install -c -d libdicts/include
	$(MAKE) -C src
clean:
	$(MAKE) clean -C src
	rm -rf packages $(PACKAGE_NAME).tgz
	rm -rf libdicts

install:

	/usr/bin/install -c -d libdicts/lib
	/usr/bin/install -c -d libdicts/include

	$(MAKE) -C src

	rm -rf packages/$(PACKAGE_NAME)
	rm -rf packages/$(PACKAGE_NAME).tgz

	# gen run env
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/bin
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/lib
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/config
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/data
	/usr/bin/install -c -d packages/$(PACKAGE_NAME)/logs

	cp -r bin/*  packages/$(PACKAGE_NAME)/bin
	cp -r config/* packages/$(PACKAGE_NAME)/config
	cp -r data/* packages/$(PACKAGE_NAME)/data
	cp -r lib/* packages/$(PACKAGE_NAME)/lib

	find packages/$(PACKAGE_NAME) -name ".git" | xargs rm -rf
	find packages/$(PACKAGE_NAME) -name ".svn" | xargs rm -rf
	find packages/$(PACKAGE_NAME) -type l | xargs rm -rf

	# gen lib env
	/usr/bin/install -c -d packages/lib_$(PACKAGE_NAME)
	/usr/bin/install -c -d packages/lib_$(PACKAGE_NAME)/include
	/usr/bin/install -c -d packages/lib_$(PACKAGE_NAME)/lib

	cp bin/*.a bin/*.so packages/lib_$(PACKAGE_NAME)/lib
	cp include/*.h packages/lib_$(PACKAGE_NAME)/include

package: clean all install
	tar zcf packages/$(PACKAGE_NAME).tgz -C packages $(PACKAGE_NAME)

.PHONY: install clean
