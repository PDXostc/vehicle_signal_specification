---
title: "Data Entry"
date: 2019-08-04T11:11:30+02:00
chapter: true
weight: 2
---

# Data Entry
Leaf nodes of the tree contain metadata describing the data associated to the node.
In order to help application developers, who are using the specification, it makes a distinction between signals - in the following as [```sensor```](/vehicle_signal_specification/rule_set/data_entry/sensor_actuator), [```actuator```](/vehicle_signal_specification/rule_set/data_entry/sensor_actuator) and [```stream```](/vehicle_signal_specification/rule_set/data_entry/data_types/#data-streams) - and [```attribute```](/vehicle_signal_specification/rule_set/data_entry/attributes).
The difference between a signal and an attribute is that the signal has
a publisher (or producer) that continuously updates the signal value while an
attribute has a set value, defined in the specification, that never changes.
As summary, besides [```branch```](/vehicle_signal_specification/rule_set/branches) type can be:

* **```attribute```**: attributes are not expected to change once they're set (e.g. vehicle identification number)
* **```sensor```**: sensor values describe the current state of the vehicle and change over time, as the state of the vehicle changes (e.g. odometer).
* **```actuator```**: actuating signals describe current state of the vehicle and change, when the state of the vehicle changes. Writing the value, leads to a change of the vehicle state itself (e.g. door lock).
* **```stream```**, data stream like video.

Examples you'll find in the [sensor and actuator chapter](/vehicle_signal_specification/rule_set/data_entry/sensor_actuator).
