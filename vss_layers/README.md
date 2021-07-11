# VSS-Layers

VSS layers is an extension mechanism that has been implicitly included since
the VSS development started, and recently there have been more discussions to
formalize the concept.

Even before the concept had a name it was in practice implemented in the
initial tools because they behaved like layers in the following ways:

1. Overriding definitions was possible with subsequent re-definition of
   the same signal
2. It was possible to add custom metadata fields to each signal, in
   addition to the VSS "core" model metadata ("name", "type", "datatype",
   "description", and in recent editions "instances", "deprecation" etc.)

There are two main ways to add a metadata relationship in the plain VSS
(meta)model.  In the VSSo (ontology) environment there may also be additional
ways.

The most obvious one is to list new metadata below a signal definition.

VSS core model metadata:
```
Motor.CoolantTemperature
  datatype: int16
  type: sensor
  unit: celsius
  description: Motor coolant temperature (if applicable).
```

Example of layered additional metadata:
```
Motor.CoolantTemperature
  my_unique_concept: true
```

### A note about signal names and paths

The VSS is defined by a hierarchy of sub-trees each defined in their own file.

An example file (ElectricMotor.vspec) may include:

```
Motor.CoolantTemperature
  ...
  ...
```

but since this information is in the file ElectricMotor.vspec, and
that file was included at the location of Vehicle.Powertrain.ElectricMotor,
the above shortened definition actually spells out the following full-path
signal:

`Vehicle.Powertrain.ElectricMotor.Motor.CoolantTemperature`

This principle for file-inclusions and namespacing applies to VSS-layers in
the same way as the core VSS.  For full details, refer to the VSS
documentation.

Note however, that later examples in this text will use the full path for
clarity.  E.g.: `Vehicle.Chassis.Wheel.Brake.Fluidlevel`

## Usage of VSS-layers

VSS-layers is a fully generic extension mechanism that is likely to find new
uses over time, but some of the primary drivers are:

1. The general separation of a "deployment model" that is not included in the core definition.
1. Defining supplementary layers that deal with, for example, data classification.
1. Ability for companies to tie VSS into existing software and processes that needs additional concepts or information

Some of these usages may lead to agreed-upon layers used by the whole industry, a.k.a. standard layers.
Others will be system, product, or company specific.

### Deployment model

This concept, also provided by Franca IDL, is very powerful as it
ensures that the basic interface definition (in this case the VSS signals) is
not encumbered by details that are unique to a particular usage of the
information.

A Deployment Model keeps the information necessary to realize the
described interface in a particular environment, platform, or even programming
language.  Anything that is not related to the interface (data) description
itself, but rather to how it is used, should be separated into a deployment
model outside of the main definition file.

This makes the data or interface definition more reusable because the same
definition can be used in many different environments and situations.

As an example, in a local request, the name of the data or function might be
enough to address it.  When the same definition is to be used in a distributed
system, the data access or function invocation may need some kind of
address of the location to find this particular item.  In some environments,
this could be a logical Node name it belongs to, or an IP-address in
another environment, or a numeric service ID in another.  A particular
protocol may offer data requests to be synchronous or asynchronous, or cached
or on-demand.  While that is a feature and is likely part of the request sent
via the protocol, it may be that some data items can support only one or the
other, and that must then be defined for each data item.

Whatever they may be, such details are deployment-specific and should be
layered on top of the core model and catalog, not included in it.

### Data classification

An example of information that would be applicable only in some situations is to classify data into privacy sensitivity categories.
Other classification reasons can be envisioned as well.

Noteworthy for this example, and likely many others:

1. What falls into each privacy category differs depending on country
 jurisdiction, and perhaps sometimes on the usage situation.  The way data is
 classified might differ between car brands.
1. The same vehicle model may be sold in different markets and the same
   product would then need different definitions on the individual level.

The nature of local privacy laws (and likely many other examples) shows that
such information cannot be defined as part of the common model or catalog --
it simply differs too often between usage situations.  Sometimes it is
required to view the system's data model with different layer configurations
even within a single implementation.  Therefore, a layered approach is needed
in the creation of a standard data model and data catalog.

The fact that this may differ even on instances of vehicles of the same make
shows that the ability to add/remove layers dynamically might be needed in the
technology stack implementation.  Another consideration is for layers to be
individually updated during software updates.

### Access control

A frequent augmentation of the data model information is definition of access
permission rules.  These boil down to specific signals that a specific actor
is allowed or not allowed to access.  Once again, this differs depending on
the situation and should be defined as a separate layer.

## General definition of VSS-layers

The layer concept definition should impose very little restrictions.
Conventions and particular tools may introduce their own restrictions
for certain usage, at a later time.

The layer concept itself is able to do all of the following:

1. Modify existing metadata and ultimately redefine any signal (i.e. change
   its metadata) compared to a previous definition for example the definition
   in the standard catalog.
1. Add new signals. It can be done in the private/ branch as recommended but the
   mechanism allows adding them anywhere.
1. Add new metadata, or relationships to new concepts for the purpose of
   creating deployment-models, bindings to existing technology, classification
   and many other things.
1. Remove signals from the final data model (SYNTAX TO BE DEFINED)

VSS-layers can also define new concepts as described in the following
scenarios:

## VSS-layer definition variants

There are two obvious approaches to add new information to a data model like
VSS.

#### Standard Layer

The first option is to list the signal first and add metadata below
it. This is straight forward since it mirrors the core VSS definition.

Let's call that a `Standard Layer`.

Here we see an arbitrary example of adding the known accuracy of a particular
measurement in this particular system (likely to be unique per vehicle model
due to the sensors involved), and defining which ECU in the electrical
architecture is responsible to produce the original data for this signal.

```
Vehicle.Chassis.Wheel2.Brake.Fluidlevel:
   accuracy:  -1% - +2%
   ECU:  BrakeManagementUnit
```

#### Reverse Layer

The second way is to flip the definition around.  First define the new
concept, and below it list metadata (that is specific to the concept).
The definitions of the metadata is is likely to refer back to the VSS signal
names.

Let's call that a `Reverse Layer`.

The following arbitrary example defines a permission concept, which writes the
name of the permission first, and below it lists all signals that are allowed
to be accessed by an actor that holds this permission.
Here the idea is written with one example permission named FINE_LOCATION.
It defines the permission concept by using a prefix and by defining metadata
names that are unique to the permission concept (`readable` and `writable`).

Here the metadata fields refer back to VSS signals using an `[` array `]` of
fully-qualified path names (FQN).  Tools might also support #include with a
designated namespace/branch name and partially-qualified path names, in the
same way as the VSS core definition does.

```
vss.remote-interface.permissions.gnss.FINE_LOCATION:
  readable: [ Vehicle.Location.Latitude, Vehicle.Location.Longitude ]
  writable: [ ]
```

This could be modeled as a Standard Layer but it would require listing some of
the permission names under many of the signals.  For some signal groups
these may be even spread out in different parts of the VSS definition.

Access control groups are likely to be a many-to-many relationship so there is
a challenge either way it is done, but defining this using a Reverse Layer
appears to be the more convenient and familiar option for most cases.

Noteworthy details in the Reverse Layer example is:

The new concept can use namespacing as deep as desired (in this example, the
prefix `vss.remote-interface.permissions.` is used by all definitions).  The
definitions themselves may also add additional branches (in this case `gnss.`
), as desired.

## Tool support

Both `Standard Layer` and `Reverse Layer` concepts shall be supported in
the VSS Layer general concept and depending on the needs and preferences, any
combination of these two approaches can be chosen.  Individual tools may
however prefer a particular layer format and content.

VSS processing tools shall generally support any number of input layers of the
supported type, and if nothing else is documented then tools shall process the
layers in the given order (later layers redefine or override earlier layers).
Tools may optionally provide information (warning or other logs) when
information is redefined.

Tools are recommended to have an appropriate input parameter (-r) to define
the files that are to be processed using `Reverse Layer` logic, or
alternatively a way to specify the unique _prefix_ of the non-signal concepts.
The latter opens up for having both standard and reverse definitions in the same
file, if this is ever desired.   Through this designation of reverse logic the
tools will not interpret concepts as signal names.


