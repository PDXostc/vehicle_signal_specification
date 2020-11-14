#
# Makefile to generate specifications
#

.PHONY: clean all json franca c

all: clean json franca csv binary c

DESTDIR?=/usr/local
TOOLSDIR?=./vss-tools
DEPLOYDIR?=./docs-gen/static/releases/nightly


json:
	${TOOLSDIR}/vspec2json.py -i:spec/VehicleSignalSpecification.id -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).json

franca:
	${TOOLSDIR}/vspec2franca.py -v $$(cat VERSION) -i:spec/VehicleSignalSpecification.id -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).fidl


csv:
	${TOOLSDIR}/vspec2csv.py -i:spec/VehicleSignalSpecification.id -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).csv


binary:
	gcc -shared -o ${TOOLSDIR}/binary/binarytool.so -fPIC ${TOOLSDIR}/binary/binarytool.c
	${TOOLSDIR}/vspec2binary.py -i:./spec/VehicleSignalSpecification.id ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).binary

c:
	(cd ${TOOLSDIR}/vspec2c/; make )
	${TOOLSDIR}/vspec2c.py -i:./spec/VehicleSignalSpecification.id -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).h vss_rel_$$(cat VERSION)_macro.h

clean:
	rm -f vss_rel_$$(cat VERSION).json vss_rel_$$(cat VERSION).fidl vss_rel_$$(cat VERSION).binary vss_rel_$$(cat VERSION).csv vss_rel_$$(cat VERSION).h
	(cd ${TOOLSDIR}/vspec2c/; make clean)

install:
	git submodule init
	git submodule update
	(cd ${TOOLSDIR}/; python3 setup.py install --install-scripts=${DESTDIR}/bin)
	$(MAKE) DESTDIR=${DESTDIR} -C ${TOOLSDIR}/vspec2c install
	install -d ${DESTDIR}/share/vss
	(cd spec; cp -r * ${DESTDIR}/share/vss)

deploy:
	if [ -d $(DEPLOYDIR) ]; then \
	  rm -f ${DEPLOYDIR}/vss_rel_*;\
	else \
	  mkdir -p ${DEPLOYDIR}; \
	fi;
	cp  vss_rel_* ${DEPLOYDIR}/
