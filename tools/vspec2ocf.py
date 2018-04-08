#!/usr/bin/python

#
# (C) 2018 Open Connectivity Foundation
#
# All files and artifacts in this repository are licensed under the
# provisions of the license provided by the LICENSE file in this repository.
#
#
# Concert vspec file to OCF
#

import sys
import vspec
import json
import getopt
from pprint import pprint
sys.setrecursionlimit(10000)

def usage():
	print "Usage:", sys.argv[0], "[-I include_dir] ... [-i prefix:id_file:start_id] vspec_file json_file"
	print "  -I include_dir			  Add include directory to search for included vspec"
	print "							  files. Can be used multiple timees."
	print
	print "  -i prefix:id_file:start_id  Add include directory to search for included vspec"
	print "							  files. Can be used multiple timees."
	print
	print " vspec_file				   The vehicle specification file to parse."
	print " json_file					The file to output the JSON objects to."
	sys.exit(255)

uritracker = []
uniques = {}


# print the whole tree hierarchy
def print_subtree_full(key, value):
	global maxi
	global uritracker
	if value['type'] == 'branch':
		for k,v in value['children'].iteritems():
			uritracker.append(k)
			print_subtree_full(k, v)
			uritracker.pop()
	else:
		print(str(value['id']))
		uristr = ''
		for uri in uritracker:
			uristr += uri;
			uristr += '.';
		uristr = uristr[:-1]
		print(uristr)
		t = value['type'].encode('utf-8')
		print(t)
		print(str(value['description']))

		if (t == 'String'):
			if 'enum' in value.keys():
				for e in value['enum']:
					sys.stdout.write(e.encode('utf-8') + ' ')
		if 'Int' in t :
			if 'value' in value.keys():
				sys.stdout.write ('value : ' + str(value['value']))
			if 'max' in value.keys():
				sys.stdout.write(' max : ' + str(value['max']))
			if 'min' in value.keys():
				sys.stdout.write(' min : ' + str(value['min']))
			if 'unit' in value.keys():
				sys.stdout.write (' unit : ' + str(value['unit']))
		print('\n')
	return


# print unique
def print_subtree_unique(key, value):
	global uritracker
	global uniques
	if value['type'] == 'branch':
		for k,v in value['children'].iteritems():
			uritracker.append(k)
			print_subtree_unique(k, v)
			uritracker.pop()
	else:
		mykey = key + ':' + value['type']
		if mykey not in uniques.keys():
			uristr = ''
			unique = value
			for uri in uritracker:
				uristr += uri;
				uristr += '.';
			uristr = uristr[:-1]
			unique['uri'] = uristr
			uniques[mykey] = unique
	return



if __name__ == "__main__":
	opts, args= getopt.getopt(sys.argv[1:], "I:i:")
	include_dirs = ["."]
	for o, a in opts:
		if o == "-I":
			include_dirs.append(a)
		elif o == "-i":
			id_spec = a.split(":")
			if len(id_spec) != 3:
				print "ERROR: -i needs a 'prefix:id_file:start_id' argument."
				usage()
			[prefix, file_name, start_id] = id_spec
			vspec.db_mgr.create_signal_db(prefix, file_name, int(start_id))
		else:
			usage()
	if len(args) != 2:
		usage()
	json_out = open (args[1], "w")
	try:
		tree = vspec.load(args[0], include_dirs)
	except vspec.VSpecError as e:
		print "Error: {}".format(e)
		exit(255)
	json.dump(tree, json_out, indent=2)
	json_out.write("\n")
	json_out.close()
	with open(args[1]) as data_file:
		data = json.load(data_file)
		for key,value in data.iteritems():
			uritracker.append(key)
			print_subtree_unique(key, value)
			uritracker.pop()
	for p in uniques.keys():
		prop = uniques[p];
		sys.stdout.write(str(prop['id']));
		ss = str(prop['uri'])
		if ss.startswith("Attribute"):
			sys.stdout.write(":ro:" + ss[10:])
		elif ss.startswith("Signal"):
			sys.stdout.write(":rw:" + ss[7:])
		else:
			sys.stdout.write(":" + ss)
		t = prop['type'].encode('utf-8')
		sys.stdout.write (":" + prop['description'] + ":" + t + ":" )

		if (t == 'String'):
			if 'enum' in prop.keys():
				for e in prop['enum']:
					sys.stdout.write(e.encode('utf-8') + ' ')
			sys.stdout.write('NA:NA:NA:NA')
		if t == 'Boolean' :
			sys.stdout.write('NA:NA:NA:NA')
		if 'Int' in t or t == 'Double' or t == 'Float':
			if 'value' in prop.keys():
				sys.stdout.write (str(prop['value']) + ':')
			else:
				sys.stdout.write('NA:')
			if 'max' in prop.keys():
				sys.stdout.write(str(prop['max']) + ':')
			else:
				sys.stdout.write('NA:')
			if 'min' in prop.keys():
				sys.stdout.write(str(prop['min']) + ':')
			else:
				sys.stdout.write('NA:')
			if 'unit' in prop.keys():
				sys.stdout.write (str(prop['unit']) + ':')
			else:
				sys.stdout.write('NA:')
		sys.stdout.write('\n')
