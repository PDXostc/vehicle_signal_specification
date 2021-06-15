#
# Makefile to generate specifications
#

.PHONY: clean all travis_targets json franca csv tests binary protobuf ttl ocf c install deploy

all: clean json franca csv binary tests protobuf

# All mandatory targets that shall be built and pass on each pull request for
# vehicle-signal-specification or vss-tools
travis_targets: clean json franca binary csv tests deploy


# Additional targets that shall be built by travis, but where it is not mandatory
# that the builds shall pass.
# This is typically intended for less maintainted tools that are allowed to break
# from time to time
# Can be run from e.g. travis with "make -k travis_optional || true" to continue
# even if errors occur and not do not halt travis build if errors occur
travis_optional: clean c ocf protobuf ttl

DESTDIR?=/usr/local
TOOLSDIR?=./vss-tools
DEPLOYDIR?=./docs-gen/static/releases/nightly


json:
	${TOOLSDIR}/vspec2json.py -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).json

franca:
	${TOOLSDIR}/vspec2franca.py -v $$(cat VERSION)  -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).fidl


csv:
	${TOOLSDIR}/vspec2csv.py -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).csv

tests:
	PYTHONPATH=${TOOLSDIR} pytest

binary:
	gcc -shared -o ${TOOLSDIR}/binary/binarytool.so -fPIC ${TOOLSDIR}/binary/binarytool.c
	${TOOLSDIR}/vspec2binary.py ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).binary

protobuf:
	${TOOLSDIR}/contrib/vspec2protobuf.py  -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).proto

ocf:
	${TOOLSDIR}/contrib/ocf/vspec2ocf.py -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).ocf.json

ttl:
	${TOOLSDIR}/contrib/vspec2ttl/vspec2ttl.py -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).ttl

c:
	(cd ${TOOLSDIR}/contrib/vspec2c/; make )
	PYTHONPATH=${TOOLSDIR} ${TOOLSDIR}/contrib/vspec2c.py -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).h vss_rel_$$(cat VERSION)_macro.h

clean:
	rm -f vss_rel_*
	(cd ${TOOLSDIR}/contrib/vspec2c/; make clean)

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
