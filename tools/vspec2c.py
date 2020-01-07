#!/usr/bin/python3

#
# (C) 2019 Jaguar Land Rover
#
# All files and artifacts in this repository are licensed under the
# provisions of the license provided by the LICENSE file in this repository.
#
#
# Concert vspec file to C header and source file.
#

import sys
import os
import vspec
import getopt
import hashlib
import json
def usage():
    print("Usage:", sys.argv[0], "[-I include-dir] ... [-i prefix:id-file] vspec-file output-header-file output-macro-file")
    print("  -I include-dir       Add include directory to search for included vspec")
    print("                       files. Can be used multiple timees.")
    print()
    print("  -i prefix:uuid-file  File to use for storing generated UUIDs for signals with")
    print("                       a given path prefix. Can be used multiple times to store")
    print("                       UUIDs for signal sub-trees in different files.")
    print()
    print(" output-header-file    The file to output the signal spec-encoding header info to.")
    sys.exit(255)


type_map =  {
    "int8": "VSS_INT8",
    "uint8": "VSS_UINT8",
    "int16": "VSS_INT16",
    "uint16": "VSS_UINT16",
    "int32": "VSS_INT32",
    "uint32": "VSS_UINT32",
    "int64": "VSS_INT64",
    "uint64": "VSS_UINT64",
    "float": "VSS_FLOAT",
    "double": "VSS_DOUBLE",
    "bool": "VSS_BOOLEAN",
    "boolean": "VSS_BOOLEAN",
    "string": "VSS_STRING",
    "stream": "VSS_STREAM",
    "na": "VSS_NA"
}

element_map = {
    "attribute": "VSS_ATTRIBUTE",
    "branch": "VSS_BRANCH",
    "sensor": "VSS_SENSOR",
    "actuator": "VSS_ACTUATOR",
    "rbranch": "VSS_RBRANCH",
    "element": "VSS_ELEMENT"
}

signal_count = 0

def emit_signal(signal_name, vspec_data):
    try:
        index = vspec_data['_index_'];
        parent_index = vspec_data['_parent_index_'];
        uuid = vspec_data['uuid'];
        signature = vspec_data['signature'];
        elem_type = element_map[vspec_data['type'].lower()];
    except KeyError as e:
        print("Missing in vspec element key: {}".format(e))
        print("Path: {}".format(vspec_data['_signal_path_']))
        exit(255)

    data_type = 'VSS_NA'
    unit = ''
    min = '{ .i = VSS_LIMIT_UNDEFINED }' # not defined.
    max = '{ .i = VSS_LIMIT_UNDEFINED }' # not defined.
    desc = ''
    enum = '{ 0 }'
    sensor = ''
    actuator = ''
    children = '{ 0 }'

    if 'datatype' in vspec_data:
        lcase_datatype = vspec_data['datatype'].lower()
        try:
            data_type = type_map[lcase_datatype];
        except KeyError as e:
            print("Illegal data type: {}".format(e))
            print("Signal: {}".format(vspec_data['_signal_path_']))
            print("Try: int8 uint8 int16 uint16 int32 uint32 int64 uint64")
            print("     float double string boolean stream")
            exit(255)

        if 'min' in vspec_data:
            if lcase_datatype in [ "int8", "uint8", "int16", "uint16", "int32" , "uint32"]:
                min = "{{ .i = {} }}".format(vspec_data['min'])
            elif lcase_datatype in [ "double", "float"]:
                min = "{{ .d = {} }}".format(vspec_data['min'])
            else:
                print("Signal {}: Ignoring specified min value for type {}".format(vspec_data['_signal_path_'], data_type))

        if 'max' in vspec_data:
            if lcase_datatype in [ "int8", "uint8", "int16", "uint16", "int32" , "uint32"]:
                max = "{{ .i = {} }}".format(vspec_data['max'])
            elif lcase_datatype in [ "double", "float"]:
                max = "{{ .d = {} }}".format(vspec_data['max'])
            else:
                print("Signal {}: Ignoring specified max value for type {}".format(vspec_data['_signal_path_'], data_type))



    if 'unit' in vspec_data:
        unit = vspec_data['unit']


    if 'description' in vspec_data:
        desc = vspec_data['description']

    children = '{ '
    if 'children' in vspec_data and len(vspec_data['children']):
        for k, v in sorted(vspec_data['children'].items(), key=lambda item: item[0]):
            children += '&vss_signal[{}], '.format(v['_index_'])

    children += ' 0 }'


    enum = '{ '
    if 'enum' in vspec_data:
        for enum_elem in vspec_data['enum']:
            enum += '"{}", '.format(enum_elem)
    enum += ' 0 }'

    if 'sensor' in vspec_data:
        sensor = vspec_data['sensor']

    actuator = ''
    if 'actuator' in vspec_data:
        actuator = vspec_data['actuator']

    if parent_index == -1:
        parent = "0"
    else:
        parent = "&vss_signal[{}]".format(vspec_data['_parent_index_'])

    return f'    {{ {index}, {parent}, (vss_signal_t*[]) {children}, "{signal_name}", "{uuid}", {hex(signature)}, {elem_type}, {data_type}, "{unit}", {min}, {max}, "{desc}", (const char*[]) {enum}, "{sensor}", "{actuator}", (void*) 0 }},\n'


#
# Put together a string with all relevant data for a signal and
# upodate the provided sha256 with it.
#
def update_sha256(vspec_data, sha256hash):
    sha256hash.update(vspec_data['uuid'].encode("utf-8"))
    sha256hash.update(vspec_data['type'].encode("utf-8"))
    if 'name' in vspec_data:
        sha256hash.update(vspec_data['name'].encode("utf-8"))

    if 'enum' in vspec_data:
        for enum_elem in vspec_data['enum']:
            sha256hash.update(enum_elem.encode("utf-8") )

    if 'datatype' in vspec_data:
        sha256hash.update(vspec_data['datatype'].encode("utf-8"))

    if 'elem_type' in vspec_data:
        sha256hash.update(vspec_data['elem_type'].encode("utf-8"))

    if 'max' in vspec_data:
        sha256hash.update("{}".format(vspec_data['max']).encode("utf-8"))

    if 'min' in vspec_data:
        sha256hash.update("{}".format(vspec_data['min']).encode("utf-8"))

    return sha256hash

#
# Create a hash for self's and all children's uuid
# and add it to vspec_data as signature
#
def add_signal_signature(name, vspec_data, sha256hash = None):
    # If sha256hash is not set, then this is a root call.
    if not sha256hash:
        local_sha = hashlib.sha256()
        store_signature = True
    else:
        local_sha = sha256hash
        store_signature = False

    # Bug in vspec.py: All elements seem to have 'type' == 'branch'
    # and 'children' == {}, even if they are signals and not branches.
    if 'children' in vspec_data and len(vspec_data['children']):
        for k, v in sorted(vspec_data['children'].items(), key=lambda item: item[0]):
            local_sha = add_signal_signature(k, v, local_sha)
            # Recurse to children and add signatures to each of them
            add_signal_signature(k, v)

        local_sha.update(name.encode("utf-8"))
        local_sha = update_sha256(vspec_data, local_sha)
        if store_signature:
            if 'signature' in vspec_data:
                return None
            vspec_data['signature'] = int(local_sha.hexdigest()[0:7], 16)
            return None

        return local_sha

    local_sha.update(name.encode("utf-8"))
    local_sha = update_sha256(vspec_data, local_sha)

    # We are a part of a recursive call, return updated sha
    if not store_signature:
        return local_sha

    # We are the top-level call to add_signal_signature()
    # Update signature member and return None
    if 'signature' in vspec_data:
        return None

    vspec_data['signature'] = int(local_sha.hexdigest()[0:7], 16)
    return None




def add_signal_index(vspec_data,  index = 0, parent_index = -1):
    for k, v in sorted(vspec_data.items(), key=lambda item: item[0]):
        v['_index_'] = index;
        index += 1
        v['_parent_index_'] = parent_index;

        # Bug in vspec.py: All elements seem to have 'type' == 'branch'
        # and 'children' == {}, even if they are signals and not branches.
        if 'children' in v and len(v['children']):
            index = add_signal_index(v['children'], index, v['_index_'])

    return index

def add_signal_path(vspec_data, parent_signal = ""):
    for k, v in sorted(vspec_data.items(), key=lambda item: item[0]):
        if len(parent_signal) > 0:
            signal_path = parent_signal + "_" + k
        else:
            signal_path = k

        v['_signal_path_'] = signal_path

        # Bug in vspec.py: All elements seem to have 'type' == 'branch'
        # and 'children' == {}, even if they are signals and not branches.
        if 'children' in v:
            add_signal_path(v['children'], signal_path)



def generate_source(vspec_data):
    global signal_count
    sig_decl = ''
    for k, v in sorted(vspec_data.items(), key=lambda item: item[0]):
        sig_decl += emit_signal(k, v)
        signal_count += 1

        # Bug in vspec.py: All elements seem to have 'type' == 'branch'
        # and 'children' == {}, even if they are signals and not branches.
        if 'children' in v and len(v['children']):
            sig_decl += generate_source(v['children'])

    return sig_decl


def generate_header(vspec_data):
    macro = ''
    for k, v in sorted(vspec_data.items(), key=lambda item: item[0]):
        macro += '#define VSS_{}() vss_get_signal_by_index({})\n'.format(v['_signal_path_'],v['_index_'])

        # Bug in vspec.py: All elements seem to have 'type' == 'branch'
        # and 'children' == {}, even if they are signals and not branches.
        if 'children' in v and len(v['children']):
            macro += generate_header(v['children'])

    return macro


if __name__ == "__main__":
    #
    # Check that we have the correct arguments
    #
    opts, args= getopt.getopt(sys.argv[1:], "I:i:")

    # Always search current directory for include_file
    include_dirs = ["."]
    for o, a in opts:
        if o == "-I":
            include_dirs.append(a)
        elif o == "-i":
            id_spec = a.split(":")
            if len(id_spec) != 2:
                print("ERROR: -i needs a 'prefix:id_file' argument.")
                usage()

            [prefix, file_name] = id_spec
            vspec.db_mgr.create_signal_uuid_db(prefix, file_name)
        else:
            usage()

    if len(args) != 3:
        usage()

    try:
        tree = vspec.load(args[0], include_dirs)
    except vspec.VSpecError as e:
        print("Error: {}".format(e.__str__()))
        exit(255)

    add_signal_index(tree)
    add_signal_path(tree)

    # Generate a hash for every branch and node in the tree

    for k, v in sorted(tree.items(), key=lambda item: item[0]):
        add_signal_signature(k,v)



    # Generate header file

    with open (args[1], "w") as hdr_out:
        hdr_out.write("// Automatically generated by vspec2c.py (https://github.com/GENIVI/vehicle_signal_specification)\n")
        hdr_out.write("// Do not edit\n\n")
        hdr_out.write("// NOTE: Only include this file once from a single C(++) file in your project.\n\n")
        hdr_out.write("#ifndef __VSPEC_GEN__\n")
        hdr_out.write("#define __VSPEC_GEN__\n\n")
        hdr_out.write("#ifdef __cplusplus\n")
        hdr_out.write("""extern "C" {\n""")
        hdr_out.write("#endif\n")
        hdr_out.write("\n");
        hdr_out.write("// Include vehicle signal spec library header defined in:\n")
        hdr_out.write("// github.com/GENIVI/vehicle_signal_specification/tree/master/tools/vspec2c\n")
        hdr_out.write("#include <vehicle_signal_specification.h>\n\n")
        hdr_out.write("// SHA256 hash of vehicle spec\n")


        hdr_out.write("\n\n// VSS Signal Array\n")
        hdr_out.write("vss_signal_t vss_signal[] = {\n")
        hdr_out.write(generate_source(tree))
        hdr_out.write("};\n")
        hdr_out.write("\n\n// VSS Signal Array size\n")
        hdr_out.write("const int vss_signal_count = {};\n\n".format(signal_count));

        hdr_out.write("#ifdef __cplusplus\n")
        hdr_out.write("}\n")
        hdr_out.write("#endif\n")
        hdr_out.write("#endif // __VSPEC_GEN__\n");


    with open (args[2], "w") as macro_out:
        macro_out.write("// Automatically generated by vspec2c.py (https://github.com/GENIVI/vehicle_signal_specification)\n")
        macro_out.write("// Do not edit\n\n")
        macro_out.write("// Macro definitions\n")
        macro_out.write("#ifndef __VSPEC_GEN_MACRO__\n");
        macro_out.write("#define __VSPEC_GEN_MACRO__\n");
        macro_out.write(generate_header(tree))
        macro_out.write("#endif // __VSPEC_GEN_MACRO__\n");
