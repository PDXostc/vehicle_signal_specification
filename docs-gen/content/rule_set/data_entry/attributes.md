---
title: "Attributes"
date: 2019-08-04T12:37:31+02:00
weight: 5
---

An attribute is a signal with a default value, specified by
its ```default``` member.

Attribute values can also change, similar to sensor values.
The latter can be useful for attribute values that are likely to change during the lifetime of the vehicle.
However, attribute values should typically not change more than once per ignition cycle,
or else it should be defined as a sensor instead.

Below is an example of a complete attribute describing engine power

```YAML
MaxPower:
  datatype: uint16
  type: attribute
  default: 0
  unit: kW
  description: Peak power, in kilowatts, that engine can generate.
```
