#
# (C) 2018 Volvo Cars
# (C) 2016 Jaguar Land Rover
#
# All files and artifacts in this repository are licensed under the
# provisions of the license provided by the LICENSE file in this repository.
#
#
# VSpec file parser.
#

import yaml
import json
import os
import uuid

class VSpecError(Exception):
    def __init__(self, *args, **kwargs):
        self.file_name=args[0]
        self.line_nr=args[1]
        self.message = args[2]
        Exception.__init__(self, *args, **kwargs)

    def __str__(self):
        return  "{}: {}: {}".format(self.file_name, self.line_nr, self.message)

#
# Manager of all SignalUUID instances.
#
class SignalUUIDManager:
    def __init__(self):
        self.signal_uuid_db_set = {}

    # Process a command line option with the format
    #  [prefix]:filename
    # If [prefix] is empty then all signals will match, regardless
    # of their name.
    #
    def process_command_line_option(self, option):
        try:
            [prefix, uuid_db_file_name] = option.split(":")

        except:
            return False

        self.create_signal_uuid_db(prefix, uuid_db_file_name, prefix)
        return True

    # Create a new SignalUUIDDB instance.
    #
    # 'prefix' is the prefix of the signal names that are to
    # be assigned ID's by the new object.
    #
    # 'id_db_file_name' is the file to read existing IDs
    # and store newly assigned IDs into forP prefix-matching signals.
    def create_signal_uuid_db(self, prefix, uuid_db_file_name):
        self.signal_uuid_db_set[prefix] = SignalUUID_DB(uuid_db_file_name)

    def find_hosting_uuid_db(self, signal):
        match_db = None
        match_len = 0

        # Find the longest matching prefix
        for prefix, signal_db in self.signal_uuid_db_set.items():
            prefix_len = len(prefix)
            if signal.find(prefix, 0, prefix_len) == -1:
                continue

            # Is this a longer prefix match than the previous one
            if prefix_len < match_len:
                continue

            match_db = signal_db
            match_len = prefix_len

        # match_db is None if no hosting uuid db was found for the
        # signal
        return match_db

    # Return the parent of the provded signal
    def parent_signal(self, signal_name):
        last_period = signal_name.rfind('.')

        if last_period == -1:
            return ""

        return signal_name[0:last_period]


    # Locate and return an existing signal ID, or create and return a new one.
    #
    # All SignalUUID instances created by create_signal_uuid_db() will
    # be prefix matched against the all prefix - SignalUUID mappings
    # setup through create_signal_uuid_db() calls.
    #
    # The Signal UUID mapped against the longest prefix match against signal_name
    # will be searched for an existing UUID assigned to signal_name.
    # If no UUID has been assigned, a new UUID is created and assigned to
    # 'signal_name' in the specified SignalUUID_DB object.
    #
    def get_or_assign_signal_uuid(self, signal_name):
        uuid_db = self.find_hosting_uuid_db(signal_name)

        if not uuid_db:
            print("Could not find UUID DB for signal {}".format(signal_name))
            sys.exit(255)

        parent = self.parent_signal(signal_name)

        try:
            return uuid_db.db[signal_name]
        except:
            # Nohting in the DB

            # If we have no parent, then we need to generate a new root UUID.
            if parent == "":
                print("Generating new root UUID")
                uuid_val = uuid.uuid1().hex
                uuid_db.db[signal_name] = uuid_val
                return uuid_val

            # Generate a new UUID, using our parent's UUID as namespace for UUID v5.
            parent_uuid = self.get_or_assign_signal_uuid(parent)
            uuid_val = uuid.uuid5(uuid.UUID(hex=parent_uuid), signal_name).hex

            uuid_db.db[signal_name] = uuid_val
            return uuid_val

    # Go through all SignalUUID instances and save them to disk.
    def save_all_signal_db(self):
        for _key, signal_uuid_db in self.signal_uuid_db_set.items():
            signal_uuid_db.save()

#
# Manage the UUIDs of a set of signals with a given prefix.
#
class SignalUUID_DB:
    # Create a new SignalUUID object.
    # id_db_file_name is the file to read existing IDs
    # and store newly assined IDs into for all signals whose IDs
    # are managed by this object.
    def __init__(self, id_file_name):
        self.id_file_name = id_file_name
        if os.path.isfile(id_file_name):
            with open (self.id_file_name, "r") as fp:
                text = fp.read()
                self.db = yaml.load(text, Loader=yaml.SafeLoader)
                if not self.db:
                    self.db = {}
                fp.close()
        else:
            self.db = {}


    #
    # Save all signal - ID mappings in self to a yaml file.  The file
    # read at object construction will be used to store all mappings
    # (including those added by get_or_assign_signal_uuid()).
    #
    def save(self):
        try:
            with open (self.id_file_name, "w") as fp:
                yaml.safe_dump(self.db, fp, default_flow_style=False)
                fp.close()
                return True
        except IOError as e:
            pass


# Try to open a file name that can reside
# in any directory listed in incude_paths.
# If successful, read context and return file
#
def search_and_read(file_name, include_paths):
    # If absolute path, then ignore include paths
    if file_name[0]=='/':
        with open (file_name, "r") as fp:
            text = fp.read()
            fp.close()
            return os.path.dirname(file_name), text

    for directory in include_paths:
        try:
            path = "{}/{}".format(directory, file_name)
            with open (path, "r") as fp:
                text = fp.read()
                fp.close()
                return os.path.dirname(path), text
        except IOError as e:
            pass

    # We failed, raise last exception we ran into.
    raise VSpecError(file_name, 0, "File error")


def assign_signal_uuids(flat_model):
    for elem in flat_model:
        elem["uuid"] = db_mgr.get_or_assign_signal_uuid(elem["$name$"])

    db_mgr.save_all_signal_db()
    return flat_model


def load(file_name, include_paths):
    flat_model = load_flat_model(file_name, "", include_paths)
    absolute_path_flat_model = create_absolute_paths(flat_model)
    absolute_path_flat_model_with_id = assign_signal_uuids(absolute_path_flat_model)
    deep_model = create_nested_model(absolute_path_flat_model_with_id, file_name)
    cleanup_deep_model(deep_model)
    return deep_model["children"]


def load_flat_model(file_name, prefix, include_paths):
    # Hooks into YAML parser to add line numbers
    # and file name into each elemeent
    def yaml_compose_node(parent, index):
        # the line number where the previous token has ended (plus empty lines)
        line = loader.line
        try:
            node = yaml.composer.Composer.compose_node(loader, parent, index)
        except yaml.scanner.ScannerError as e:
            raise VSpecError(file_name, line + 1, e)
        except yaml.parser.ParserError as e:
            raise VSpecError(file_name, line + 1, e)

        node.__line__ = line + 1
        node.__file_name__ = file_name
        return node

    def yaml_construct_mapping(node, deep=False):
        mapping = yaml.constructor.Constructor.construct_mapping(loader, node, deep=deep)
        # Find the name of the branch / signal and convert
        # it to a dictionary element '$name$'
        for key, val in list(mapping.items()):
            if key[0]=='$':
                continue

            if val == None:
                mapping['$name$'] = key
                del mapping[key]
                break

        mapping['$line$'] = node.__line__
        mapping['$file_name$'] = node.__file_name__
        return mapping


    directory, text = search_and_read(file_name, include_paths)
    text = yamilify_includes(text)

    # Setup a loader to include $line$ and $file_name$ as
    # added python objects to the parsed tree.
    loader = yaml.Loader(text)
    loader.compose_node = yaml_compose_node

    loader.construct_mapping = yaml_construct_mapping
    raw_yaml = loader.get_data()

    # Import signal IDs from the given database


    # Check for file with no objects.
    if not raw_yaml:
        return []

    # Sanity check of loaded code
    check_yaml_usage(raw_yaml, file_name)

    # Recursively expand all include files.
    expanded_includes = expand_includes(raw_yaml, prefix, list(set(include_paths + [directory])))

    # Add type: branch when type is missing.
    flat_model = cleanup_flat_entries(expanded_includes)

    return flat_model



#
# 1. If no type is specified, default it to "branch".
# 2. Check that the declared type is a FrancaIDL.
# 3. Correct the  casing of type.
# 4, Check that enums are provided as arrays.
#
def cleanup_flat_entries(flat_model):
    available_types =[ "sensor", "actuator", "stream", "branch", "attribute", "UInt8", "Int8", "UInt16", "Int16",
                       "UInt32", "Int32", "UInt64", "Int64", "Boolean",
                       "Float", "Double", "String", "ByteBuffer", "rbranch", "element" ]

    available_downcase_types =[ "sensor", "actuator", "stream", "branch", "attribute", "uint8", "int8", "uint16", "int16",
                                "uint32", "int32", "uint64", "int64", "boolean",
                                "float", "double", "string", "bytebuffer", "rbranch", "element" ]

    # Traverse the flat list of the parsed specification
    for elem in flat_model:
        # Is this an include element?
        if "type" not in elem:
            elem["type"] = "branch"

        # Check, without case sensitivity that we do have
        # a validated type.
        if not elem["type"].lower() in available_downcase_types:
            raise VSpecError(elem["$file_name$"], elem["$line$"], "Unknown type: {}".format(elem["type"]))

        # Get the correct casing for the type.
        elem["type"] = available_types[available_downcase_types.index(elem["type"].lower())]

        if "enum" in elem and not isinstance(elem["enum"], list):
            raise VSpecError(elem["$file_name$"], elem["$line$"], "Enum is not an array.")


    return flat_model

#
# Delete parser-specific elements
#
def cleanup_deep_model(deep_model):
    # Traverse the flat list of the parsed specification
    # Is this an include element?
    if "$file_name$" in deep_model:
        del deep_model["$file_name$"]

    if "$line$" in deep_model:
        del deep_model["$line$"]

    if "$prefix$" in deep_model:
        del deep_model["$prefix$"]

    if "$name$" in deep_model:
        del deep_model['$name$']

    if (deep_model["type"] == "branch") or (deep_model["type"] == "rbranch"):
        children = deep_model["children"]
        for child in deep_model["children"]:
            cleanup_deep_model(children[child])

    return None

#
# Verify that we are using correct YAML in the model
#
def check_yaml_usage(flat_model, file_name):
    for elem in flat_model:
        if isinstance(elem, list):
            raise VSpecError(file_name,  0,
                             "Element {} is not a list entry. (Did you forget a ':'?)".format(elem))


    # FIXME:
    # Add more usage checks, such as absence of nested models.
    # and mutually exclusive elements.



# Expand yaml include elements (inserted by yamilify_include())
#
def expand_includes(flat_model, prefix, include_paths):
    # Build up a new spec model based on the old one, but
    # with expanded include directives.

    new_flat_model = []

    # Traverse the flat list of the parsed specification
    for elem in flat_model:
        # Is this an include element?
        if "$include$" in elem:
            include_elem = elem["$include$"]
            include_prefix = include_elem.get("prefix", "")
            # Append include prefix to our current prefix.
            # Make sure we do not start new prefix with a "."
            if prefix != "":
                if include_prefix != "":
                    include_prefix = "{}.{}".format(prefix, include_prefix)
                else:
                    include_prefix = prefix

            # Recursively load included file
            inc_elem = load_flat_model(include_elem["file"], include_prefix, include_paths)

            # Add the loaded elements at the end of the new spec model
            new_flat_model.extend(inc_elem)
        else:
            # Add a prefix to the element
            elem["$prefix$"] = prefix
            # Add the existing elements at the end of the new spec model
            new_flat_model.append(elem)

    return new_flat_model



#
# Take the flat model created by _load() and merge all $prefix$ with its name
# I.e: $prefix$ = "Cabin.Doors.1"
#      name = "Window.Pos"
#      -> name = "Cabin.Doors.1.Window.Pos"
#
# $prefix$ is deleted
#
#
def create_absolute_paths(flat_model):
    for elem in flat_model:
        # Create a list of path components to the given element
        #
        # $prefix$='body.door.front.left' name='lock' ->
        # [ 'body', 'door', 'front', 'left', 'lock' ]
        name = elem['$name$']

        if elem["$prefix$"] == "":
            new_name = name
        else:
            new_name= "{}.{}".format(elem["$prefix$"], name)

        elem['$name$'] = new_name
        del elem["$prefix$"]


    return flat_model



#
# Take the flat model with absolute signal names parsed from the vspec
# file and create a nested variant where each component of a prefix
# becomes a branch.
#

def create_nested_model(flat_model, file_name):
    deep_model = {
        "children": {},
        "type": "branch",
        "$file_name$": file_name,
        "$line$": 0
    }

    # Traverse the flat list of the parsed specification
    for elem in flat_model:
        # Create children for branch type objects
        if elem["type"] == "branch":
            deep_model["type"] = "branch"
        if elem["type"] == "rbranch":
            deep_model["type"] = "rbranch"
        if (elem["type"] == "branch") or (elem["type"] == "rbranch"):
            elem["children"] = {}


        # Create a list of path components to the given element
        #  name='body.door.front.left.lock' ->
        # [ 'body', 'door', 'front', 'left', 'lock' ]
        name_list = elem['$name$'].split(".")

        # Extract prefix and name
        prefix = list_to_path(name_list[:-1])
        name = name_list[-1]

        # Locate the correct branch in the tree
        parent_branch = find_branch(deep_model, name_list[:-1], 0)

        # If an element with name is already in the parent branch
        # we update its fields with the fields from the new element
        if name in parent_branch["children"]:
            old_elem = parent_branch["children"][name]
            #print "Found: " + str(old_elem)
            # never update the type
            elem.pop("type", None)
            # concatenate file names
            fname = "{}:{}".format(old_elem["$file_name$"], elem["$file_name$"])
            old_elem.update(elem)
            old_elem["$file_name$"] = fname
            #print "Set: " + str(parent_branch["children"][name])
        else:
            parent_branch["children"][name] = elem

    return deep_model


# Find the given prefix somewhere under the tree rooted in branch.
def find_branch(branch, name_list, index):
    # Have we reached the end of the name list
    if len(name_list) == index:
        if (branch["type"] != "branch") and (branch["type"] != "rbranch"):
            raise VSpecError(branch.get("$file_name$","??"),
                             branch.get("$line$", "??"),
                             "Not a branch: {}.".format(branch['$name$']))

        return branch

    if (branch["type"] != "branch") and (branch["type"] != "rbranch"):
        raise VSpecError(branch.get("$file_name$","??"),
                         branch.get("$line$", "??"),
                         "{} is not a branch.".format(list_to_path(name_list[:index])))

    children = branch["children"]

    if name_list[index] not in children:
        raise VSpecError(branch.get("$file_name$","??"),
                         branch.get("$line$", "??"),
                         "Missing branch: {} in {}.".format(name_list[index],
                                                            list_to_path(name_list)))

    # Traverse all children, looking for the
    # Move on to next element in prefix.
    return find_branch(children[name_list[index]], name_list, index + 1)


def list_to_path(name_list):
    path = ""
    for name in name_list:
        if path == "":
            path = name
        else:
            path = "{}.{}".format(path, name)

    return path


# Convert a dot-notated element name to a list.
def element_to_list(elem):
    name = elem['$name$']

    if elem["$prefix$"] == "":
        path = name
    else:
        path = "{}.{}".format(elem["$prefix$"], name)

    return

#
# Convert
#   #include door.vspec, body.door.front.left
# to
#   - $include$:
#     file: door.vspec
#     prefix: body.door.front.left
#
# This yaml version of the include directive will
# then be further processed to actually include
# the given file.
#
def yamilify_includes(text):
    while True:
        st_index = text.find("\n#include")
        if st_index == -1:
            return text

        end_index = text.find("\n", st_index+1)
        if end_index == -1:
            return text

        include_arg = text[st_index+10:end_index].split()
        if len(include_arg) == 2:
            [ include_file, include_prefix] = include_arg
        else:
            include_prefix = '""'
            [include_file] = include_arg

        text = """{}
- $include$:
    file: {}
    prefix: {}
{}""".format(text[:st_index], include_file, include_prefix, text[end_index:])

    return text

db_mgr = SignalUUIDManager()
