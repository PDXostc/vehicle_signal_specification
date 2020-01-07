---
title: "Attributes"
date: 2019-08-04T12:37:31+02:00
weight: 5
---

An attribute is a signal with a default value, specified by
its ```value``` member.

The value set for an attribute by a vspec file can be read by a
consumer without the need of having the value sent out by a
publisher. The attribute, in effect, is configuration data that
can be specified when the vspec file is installed.

Below is an example of a complete attribute describing engine power

```YAML
- MaxPower:
  type: attribute
  datatype:  uint16
  default: 0
  description: Peak power, in kilowatts, that engine can generate.
```
