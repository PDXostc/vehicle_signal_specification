---
title: "Sensors & Actuators"
date: 2019-08-04T12:37:03+02:00
weight: 3
---
A data entry defines a single sensor/actuator and its members. A data
entry example is given below:

```YAML
- Speed:
  type: sensor
  datatype: uint16
  unit: km/h
  min: 0
  max: 300
  description: The vehicle speed, as measured by the drivetrain.
```

* **```Drivetrain.Transmission.Speed```**<br>
Defines the dot-notated name of the data entry. Please note that
all parental branches included in the name must be defined as well.

* **```type```**<br>
Defines the type of the node. This can be ```branch```,
```sensor```, ```actuator```, ```stream``` or attribute.


* **```datatype```**<br>
The string value of the type specifies the scalar type of the data entry
value. See [data type](#data-type) chapter for a list of available types.

* **```min``` [optional]**<br>
The minimum value, within the interval of the given ```type```, that the
data entry can be assigned.<br>
If omitted, the minimum value will be the "Min" value for the given type.<br>
Cannot be specified if ```enum``` is specified for the same data entry.

* **```max``` [optional]**<br>
The maximum value, within the interval of the given ```type```, that the
data entry can be assigned.<br>
If omitted, the maximum value will be the "Max" value for the given type.<br>
Cannot be specified if ```enum``` is specified for the same data entry.

* **```unit``` [optional]**<br>
The unit of measurement that the data entry has. See [Unit
Type](#data-unit-type) chapter for a list of available unit types.<br> This
cannot be specified if ```enum``` is specified as the signal type.

* **```description```**<br>
A description string to be included (when applicable) in the various
specification files generated from this data entry.

* **```sensor```[optional]**<br>
The sensing appliance used to produce the data entry.

* **```actuator```[optional]**<br>
The actuating appliance consuming the data entry.
