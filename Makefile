#
# Makefile to generate specifications
#

.PHONY: all json franca vsi

all: clean json franca vsi csv

json:
	./tools/vspec2json.py -i:spec/VehicleSignalSpecification.id:1 -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).json

franca:
	./tools/vspec2franca.py -v $$(cat VERSION) -i:spec/VehicleSignalSpecification.id:1 -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).fidl

vsi:
	./tools/vspec2vsi.py -v $$(cat VERSION) -i:spec/VehicleSignalSpecification.id:1 -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).vsi

csv:
	./tools/vspec2csv.py -i:spec/VehicleSignalSpecification.id:1 -I ./spec ./spec/VehicleSignalSpecification.vspec vss_rel_$$(cat VERSION).csv

clean:
	rm spec/VehicleSignalSpecification.id vss_rel_$$(cat VERSION).json vss_rel_$$(cat VERSION).fidl vss_rel_$$(cat VERSION).vsi vss_rel_$$(cat VERSION).csv
