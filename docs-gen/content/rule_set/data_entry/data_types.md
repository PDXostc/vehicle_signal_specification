---
title: "Data Types"
date: 2019-08-04T11:11:48+02:00
weight: 1
---

Each [data entry](/vehicle_signal_specification/rule_set/data_entry) specifies a ```datatype``` from the following set (from Franca IDL). Datatypes shall not be used in [branch entry](/vehicle_signal_specification/rule_set/branches)

## Supported datatypes

Name       | Type                       | Min  | Max
:----------|:---------------------------|:-----|:---
uint8      | unsigned 8-bit integer     | 0    | 255
int8       | signed 8-bit integer       | -128 | 127
uint16     | unsigned 16-bit integer    |  0   | 65535
int16      | signed 16-bit integer      | -32768 | 32767
uint32     | unsigned 32-bit integer    | 0 | 4294967295
int32      | signed 32-bit integer      | -2147483648 | 2147483647
uint64     | unsigned 64-bit integer    | 0    | 2^64
int64      | signed 64-bit integer      | -2^63 | 2^63 - 1
boolean    | boolean value              | 0/false | 1/true
float      | floating point number      | -3.4e -38 | 3.4e 38
double     | double precision floating point number | -1.7e -300 | 1.7e 300
string     | character string           | n/a  | n/a
byteBuffer | buffer of bytes (aka BLOB) | n/a | n/a


## Arrays

Besides the datatypes described above, VSS supports as well the concept of
`arrays`, as a *collection of elements based on the data entry
definition*, wherein it's specified. By default the size of the array is undefined.
By the optional keyword `arraysize` the size of the array can be specified.
The following syntax shall be used to declare an array:

```YAML
# Array of datatype uint32, by default size of the array is undefined
datatype: uint32[]
# Optional: specified number of elements in the array
arraysize: 5
```

An example for the usage of `arrays` is `Vehicle.OBD.DTCList` which contains a list
of Diagnostic Trouble Codes (DTCs) present in a vehicle.

## Data Streams

Data Entries, which describe sensors offering binary streams
(e.g. cameras), are not supported directly by VSS with a
dedicated data type. Instead, they are described through the
meta data about the sensor itself and how to retrieve the
corresponding data stream.

A camera can be a good example of it. The Data Entry for the camera
and the corresponding video stream could look like:

```YAML
Camera:
  type: branch
  description: Information about the camera and how to connect to the video stream

Camera.IsActive:
  type: actuator
  datatype: boolean
  description: If the camera is active, the client is able to retrieve the video stream

Camera.URI:
  type: sensor
  datatype: string
  description: URI for retrieving the video stream, with information on how to access the stream (e.g. protocol,  data format, encoding, etc.)

```

In this example, it shows the usage of meta data about
the status of the sensor. The camera can be set to active through
the same data entry (`actuator`). A dynamic data entry (`sensor`)
is used for the URI of the video stream. Information on how to access
the stream is expected.
