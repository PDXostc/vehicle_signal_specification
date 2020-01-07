---
title: "Private Branch"
date: 2019-08-04T12:46:30+02:00
weight: 7
---
The private branch offers a way to clearly separate OEM specific definitions
and the specification. It helps adapting the specification to the needs of the
organisation.

## Extending and overriding Data Entries
The core signal specification can be extended with additional signals through the
use of private branches, as is shown in Fig 3.


![Signal Extension](/vehicle_signal_specification/images/private_extensions.png)<br>
*Fig 3. Extended signals*

In this case the core signal specification, ```vss_23.vspec``` is
included by a OEM-specific master vspec file that adds the two
proprietary signals ```Private.OEM_X.AntiGravity.Power```
and ```Private.OEM_X.Teleport.TargetLoc```.

Signals can, in a similar manner, be overridden and replaced with a new definition,
as is shown in Fig 4.


![Signal Extension](/vehicle_signal_specification/images/signal_override.png)<br>
*Fig 4. Overridden signals*

In this case, the ```GearChangeMode``` signal provided by the core
specification lacks an additional semi-automatic mode featured by an
OEM-specific vehicle.

By having an OEM master spec file, ```oem_x_proprietary.vspec```
include the core spec file, ```vss_23.vspec``` and then overriding
the original ```GearChangeMode``` signal and add the ```semi-auto```
element as an enumerated value

## DECLARING VS. DEFINING ATTRIBUTES
The signal extension mechanism described above is also used to declare
an attribute in one vspec file and define it in another.  This is used
to setup a attribute structure standard in the core specification that
is to be defined on a per-deployment (vehicle) basis.

An example is given in Fig 5.

![Attributes](/vehicle_signal_specification/images/attributes.png)<br>
*Fig 5. Declaring and defining attributes*

The ```Attributes.Engine.Displacement``` and ```Attributes.Chassis.Weight``` attributes
are declare in the ```vss_23.vspec``` file with a default value of zero.

A project/vehicle specific vspec file, ```oem_x_proprietary.vspec```
then overrides the attributes with the correct values.
