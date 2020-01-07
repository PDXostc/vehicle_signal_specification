---
title: "Includes"
date: 2019-08-04T12:59:44+02:00
weight: 6
---

An include directive in a vspec file will read the file it refers to and the
contents of that file will be inserted into the current buffer in place of the
include directive.  The included file will, in its turn, be scanned for
include directives to be replaced, effectively forming a tree of included
files.

See Fig 6 for an example of such a tree.

![Include directive](/vehicle_signal_specification/images/include_directives.png)<br>
*Fig 6. Include directives*


The include directive has the following format:

    #include <filename> [prefix]

The ```<filename>``` part specifies the path, relative to the file with the ```#include``` directive, to the vspec file to replace the directive with.

The optional ```[prefix]``` specifies a branch name to be
prepended to all signal entries in the included file. This allows a vspec file
to be reused multiple times by different files, each file specifying their
own branch to attach the included file to.

An example of an include directive is:

    #include doors.vpsec chassis.doors

The ```door.vspec``` section specifies the file to include.

The ```chassis.doors``` section specifies that all signal entries in ```door.vspec``` should have their names prefixed with ```chassis.doors```.

If an included vspec file has branch or signal specifications that have
already been defined prior to the included file, the new specifications in the
included file will override the previous specifications.


## REUSING SIGNAL TREES
Complete subtrees of signals and attributes can be reused by including
them multiple times, attaching them to different branches each time
they are included.

An example is given in Fig 7 where a generic door signal specification is
included four times to describe all doors in the vehicle.

![Include directrive](/vehicle_signal_specification/images/spec_file_reuse.png)<br>
*Fig 7. Reusing signal trees*

The ```door.vspec``` file is included four times by the master ```root.vspec``` file.
The signals of ```door.vspec```, ```Locked```, ```WinPos```, and ```Open``` are attached
on the front left and right doors of row 1 (front) and row 2 (back).

If ```door.vspec``` is changed, the changes will be propagated to all four doors.
