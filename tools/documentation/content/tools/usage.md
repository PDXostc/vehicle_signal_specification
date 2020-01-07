---
title: "Usage"
date: 2019-08-04T13:21:34+02:00
weight: 2
---
After installing the dependencies, as described in the
[requirements](/tools/requirements), you can simply run:

    make

The results will be stored in ```vss_$VERSION.[xxx]```,
where ```$VERSION``` is the contents of the ```VERSION``` file and ```xxx``` is
the appropriate file extension for the type of output being produced.  For
example, the JSON version of the output will have a ```.json``` extension.

Changes to files under the spec/ directory results in changes to the generated
files, namely ```.json```, ```.fidl```, ```.csv``` etc.
Hence, it is recommended to run ```make```, post spec/ file changes.

By default, the ```make``` processor will produce all of the currently
installed output formats.  If only a single format is desired, specify it as
an arguement.  For example, to generate only the json format, type:

    make json

## Supported Serialization Formats:

| Serialization | Command |
| ------ | ----------- |
| json | make json |
| csv | make csv |
| C | make cnative |
| franca | make franca |
