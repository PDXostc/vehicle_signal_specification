---
title: "Overview"
date: 2019-07-30T14:46:01+02:00
weight: 1
chapter: false
---

## What is VSS?
The Vehicle Signal Specification introduces a domain taxonomy for vehicle signals.
It can be used as standard in automotive applications to communicate information
around the vehicle, which is semantically well defined. It focuses on vehicle
signals, in the sense of classical sensors and actuators with the raw data
communicated over vehicle buses and data which is more commonly associated with
the infotainment system alike.

A standardized vehicle data specification allows an industry actor to use a
common naming space for communication and, ultimately, abstracts underlying
vehicle implementation details.

The representation of vehicle data specifications is vendor independent.
Vendor-specific extensions can be specified in a dedicated and uncontrolled
branch of the specification tree.

#### What's in
* Standardized data definition for vehicle signals
* Same semantic understanding across different domains
* Basic definition for interfaces working on vehicle data (w3c, etc.)

#### What's out
* Everything outside the vehicle signal domain (customer, weather, etc.)
* Concrete interface definition

## Example
The figure below shows an example snapshot of a generated tree of the
specification. The leafs contain the actual information as shown in the figure.
Before going into detail of the specification, let's dig deeper into taxonomies.

![Example tree](/vehicle_signal_specification/images/tree.png?classes=shadow&width=60pc)
