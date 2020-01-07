---
title: "Enumerated Sensors & Actuators"
date: 2019-08-04T12:37:12+02:00
weight: 4
---


A data entry can optionally be enumerated, allowing it to be assigned a
value from a specified set of values. An example of an enumerated sensor
is given below:


```YAML
- Chassis.Transmission.Gear:
  type: sensor
  datatype: uint16
  enum: [ -1, 1, 2, 3, 4, 5, 6, 7, 8 ]
  description: The selected gear. -1 is reverse.
```

An enumerated signal entry has no ```min```, ```max```, or ```unit```
element.

The ```enum``` element is an array of values, all of which must be specified
in the emum list.  This signal can only be assigned one of the values
specified in the enum list.
The ```type``` specifier is the type of the individual elements of the enum
list.
