#!/usr/bin/python

#
# (C) 2016 Jaguar Land Rover
#
# All files and artifacts in this repository are licensed under the
# provisions of the license provided by the LICENSE file in this repository.
#
#
# Convert vspec file to FrancaIDL spec.
#

import sys
import vspec
import json
import getopt

def usage():
    print("Usage:", sys.argv[0], "[-I include_dir] ... [-i prefix:id_file:start_id] vspec_file franca_file")
    print("  -I include_dir              Add include directory to search for included vspec")
    print("                              files. Can be used multiple timees.")
    print()
    print("  -i prefix:id_file:start_id  Add include directory to search for included vspec")
    print("                              files. Can be used multiple timees.")
    print()
    print(" vspec_file                   The vehicle specification file to parse.")
    print(" franca_file                  The file to output the Franca IDL spec to.")
    sys.exit(255)


def traverse_tree(tree, outf, prefix_arr, is_first_element):
    # Convert a prefix array of path elements to a string
    def prefix_to_string(prefix_arr):
        if len(prefix_arr) == 0:
            return ""

        res = prefix_arr[0]
        for elem in prefix_arr[1:]:
            res = "{}.{}".format(res, elem)

        return res


    # Traverse all elemnts in tree.
    for key, val in tree.items():
        # Is this a branch?
        if "children" in val:
            # Yes. Recurse
            traverse_tree(val['children'], outf, prefix_arr + [ key ], is_first_element)
            continue

        # Drop a comma in before the next element.
        if not is_first_element:
            outf.write(",\n")

        outf.write("""{{
    name: "{}"
    type: "{}"
    description: "{}"
""".format(prefix_to_string(prefix_arr + [ key ]),
           val['type'],
           val['description']))

        if "datatype" in val:
            outf.write("    datatype: {}\n".format(val["datatype"]))

        if "id" in val:
            outf.write("    id: {}\n".format(val["id"]))

        if "min" in val:
            outf.write("    min: {}\n".format(val["min"]))

        if "max" in val:
            outf.write("    max: {}\n".format(val["max"]))

        if "unit" in val:
            outf.write("    unit: {}\n".format(val["unit"]))

        if "enum" in val:
            outf.write("    enum: {}\n".format(val["enum"]))

        if "sensor" in val:
            outf.write("    sensor: {}\n".format(val["sensor"]))

        if "actuator" in val:
            outf.write("    actuator: {}\n".format(val["actuator"]))

        outf.write("}\n")
        is_first_element = False


if __name__ == "__main__":
    #
    # Check that we have the correct arguments
    #
    opts, args= getopt.getopt(sys.argv[1:], "I:i:v:")

    # Always search current directory for include_file
    vss_version = "unspecified version"
    include_dirs = ["."]
    for o, a in opts:
        if o == "-I":
            include_dirs.append(a)
        elif o == "-v":
            vss_version = a
        elif o == "-i":
            id_spec = a.split(":")
            if len(id_spec) != 3:
                print("ERROR: -i needs a 'prefix:id_file:start_id' argument.")
                usage()

            [prefix, file_name, start_id] = id_spec
            vspec.db_mgr.create_signal_db(prefix, file_name, int(start_id))
        else:
            usage()

    if len(args) != 2:
        usage()

    franca_out = open (args[1], "w")
    try:
        tree = vspec.load(args[0], include_dirs)
    except vspec.VSpecError as e:
        print("Error: {}".format(e))
        exit(255)

    franca_out.write(
"""// Copyright (C) 2016, GENIVI Alliance
//
// This program is licensed under the terms and conditions of the
// Mozilla Public License, version 2.0.  The full text of the
// Mozilla Public License is at https://www.mozilla.org/MPL/2.0/

const UTF8String VSS_VERSION = "{}"

struct SignalSpec {{
    UInt32 id
    String name
    String type
    String description
}}

const SignalSpec[] signal_spec = [
""".format(vss_version))

    traverse_tree(tree, franca_out, [], True)

    franca_out.write(
"""
]
""")

    franca_out.close()
