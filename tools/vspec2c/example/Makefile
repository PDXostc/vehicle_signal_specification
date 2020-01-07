#
# Generate a demo that dumps the standard vss spec, encoded in C,
# on stdout.
#
# We use vspec2c to translate the YAML spec files into two header files,
# vss_generated.h and vss_generated_macro.h, and then compile those
# with ../libvss.so to get the full spec in C.
# vss_dump itself traverses the spec and prints it to stdout.
#

CFLAGS ?=-O2 -Wall  -I. -I../
DESTDIR?=/usr/local
VSS_VERSION=$(shell cat ../../VERSION)
TARGET=vss_dump
OBJ=vss_dump.o
GEN_HDR=signal_spec.h
GEN_HDR_MACRO=signal_macro.h

#
# Link statically to libvss.a so that we don't have to worry about
# ldconfig or LD_LIBRARY_PATH
#
${TARGET}: ${GEN_HDR} ${GEN_HDR_MACRO} ${OBJ}
	${CC} -o ${TARGET} ${OBJ} -static -L.. -lvss

#
# Invoke vspec2c.py to translate spec/VehicleSignalSpecification.vspec to
# header files that can be linked with ../libvss.so
#
${GEN_HDR} ${GEN_HDR_MACRO}:
	../../vspec2c.py \
		-i:../../../spec/VehicleSignalSpecification.id \
		-I ../../../spec \
		../../../spec/VehicleSignalSpecification.vspec \
		${GEN_HDR} ${GEN_HDR_MACRO}

install:  ${TARGET}
	install -d ${DESTDIR}/lib;
	install -m 0755 ${TARGET} ${DESTDIR}/bin;

uninstall:
	rm -f ${DESTDIR}/bin/${TARGET};

clean:
	rm -f ${OBJ} ${TARGET} ${GEN_HDR} ${GEN_HDR_MACRO} *~
