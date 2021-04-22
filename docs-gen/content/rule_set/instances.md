---
title: "Instances"
date: 2019-07-31T15:27:36+02:00
weight: 5
---

As VSS resembles primarily the physical structure of the vehicle, so
quite often you'll find repetitions of branches and data entries
(e.g. doors, axles, etc), which leads to hard-coded repetitions of
branches and data entries in the specification. Instances remove the need of
repeating definitions, by defining at the node itself how often it occurs in
the resulting tree. They are meant as a short-cut in the specification and
interpreted by the tools.

![Example tree](/vehicle_signal_specification/images/instances.png?width=60pc)



## Definition

1. An instance can be defined in any branch.
2. The instantiation is done for every node in the following path.
3. Instances are defined with the key-word `instances`, followed by its
   definition, which can be either:
   * a list of strings, where each element defines a single instance, e.g.
     `['Left','Right']` results into two instances of every following
     data entry in the path, named `Left` and `Right`
   * a string, followed by a range defined through `[n,m]`, with `n,m` as integer and `n <= m`, which defines the number of instances:
     `Position[1,4]` results into 4 instances of every following
     data entry in the path, named `Position1`, `Position2`, `Position3`
     and `Position4`.
4. If multiple instances occur in one node or on the path to a data entry,
   the instances get combined, by the order of occurrence. Following the example above,
   four position instances will be created for each of the 'Left' and 'Right' instances,
   resulting into a total number of 8 instances.

## Example

The example from above in the specification:

```YAML
# Cabin.vspec
- Door:
  type: branch
  instances:
    - Row[1,4]
    - ["Left","Right"]
  description: All doors, including windows and switches
#include SingleDoor.vspec Door


# SingleDoor.vspec

#
# Definition of a single door
#
- IsOpen:
  datatype: boolean
  type: actuator
  description: Is door open or closed
```

Results in the following dot-notated output:

```
Vehicle.Cabin.Door
Vehicle.Cabin.Door.Row1
Vehicle.Cabin.Door.Row1.Left
Vehicle.Cabin.Door.Row1.Left.IsOpen
Vehicle.Cabin.Door.Row1.Right
Vehicle.Cabin.Door.Row1.Right.IsOpen
Vehicle.Cabin.Door.Row2
Vehicle.Cabin.Door.Row2.Left
Vehicle.Cabin.Door.Row2.Left.IsOpen
Vehicle.Cabin.Door.Row2.Right
Vehicle.Cabin.Door.Row2.Right.IsOpen
Vehicle.Cabin.Door.Row3
Vehicle.Cabin.Door.Row3.Left
Vehicle.Cabin.Door.Row3.Left.IsOpen
Vehicle.Cabin.Door.Row3.Right
Vehicle.Cabin.Door.Row3.Right.IsOpen
Vehicle.Cabin.Door.Row4
Vehicle.Cabin.Door.Row4.Left
Vehicle.Cabin.Door.Row4.Left.IsOpen
Vehicle.Cabin.Door.Row4.Right
Vehicle.Cabin.Door.Row4.Right.IsOpen
```

## Redefinition

It is possible to override the default instantiation provided by VSS by redefining the branch with
different instantiation information. If multiple definitions of a branch exist with different
instance definitions, then the last found definition will be used. 
As an example, if only two rows of doors are needed, then the default VSS instance definition
can be overridden by redefining the Door branch as shown in the example below. 

```YAML
#Redefinition changing number of rows from 4 to 2
#The redefinition must appear "after" the original definition
- Vehicle.Cabin.Door:
  type: branch
  instances:
    - Row[1,2]
    - ["Left","Right"]
  description: All doors, including windows and switches
```

## Recommendations

VSS is designed to cover a wide range of vehicles.
This means that the default instantiation used in VSS may not fit every vehicle.
An example can be seen in the windshield signals defined in `Body.vspec`, parts of them are shown below.
VSS offers the possibility to control windshield heating separately for front and rear windshield,
and VSS also gives the possibility to report washer fluid level separately for each windshield.
This fits very well for a vehicle that has separate washer fluid containers for front and rear windshield
and that offers heating for both windshields. But that is not the case for all vehicles,
it is not even certain that all vehicles have two windshields. this sections gives recommendations on how
to use VSS for a vehicle if the VSS specification does not offer an exact match of the capabilities of the vehicle.

```YAML
- Windshield:
  type: branch
  instances: ["Front", "Rear"]
  description: Windshield signals

- Windshield.Heating:
  type: branch
  description: Windshield heater signals

- Windshield.Heating.Status:
  datatype: boolean
  type: actuator
  description: Windshield heater status. 0 - off, 1 - on

- Windshield.WasherFluid:
  type: branch
  description: Windshield washer fluid signals

- Windshield.WasherFluid.LevelLow:
  datatype: boolean
  type: sensor
  description: Low level indication for washer fluid. True = Level Low. False = Level OK.
```

### Recommendation: Instance Mismatch

If a vehicle does not have as many instances as specified in VSS then one
of the following methods are recommended:

- Redefine the branch. If a vehicle for example does not have a rear windshield
then append a redefinition at the end of the VSS:

```YAML
- Vehicle.Body.Windshield:
  type: branch
  instances: ["Front"]
  description: Windshield signals
```

- Accept that a `branch Vehicle.Body.Windshield.Rear` will exist in the generated VSS representation,
  use mechanisms outside VSS to ignore that branch
  
### Recommendation: Features shared among instances

If a feature is shared among instances, it is recommended to publish that feature for all concerned instances.

Example: In VSS washer fluid can be handled separately for front and rear windshield.
If a vehicle use a common container serving both front and rear windshield,
then it is recommended that the vehicle report information on that container in both
`Vehicle.Body.Windshield.Front.WasherFluid.LevelLow` and `Vehicle.Body.Windshield.Rear.WasherFluid.LevelLow`.

### Recommendation: Features lacking for some instances

Not all instances in a vehicle might have the same features. If e.g. the front windshield
from the example above lack a heater, then it is recommended to use mechanisms outside VSS
to ignore `Vehicle.Body.Windshield.Front.Heating`.
