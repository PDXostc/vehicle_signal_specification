---
title: "Basic Rules"
date: 2019-08-04T13:05:11+02:00
weight: 1
---
## Specification format
A domain specification is written as a flat YAML list, where each signal and
branch is a self-contained YAML list element.

The YAML list is a single file, called a *vspec* file.

A vspec can, in addition to a YAML list, also contain include directives.

An include directive refers to another vspec file that is to replace the
directive, much like ```#include``` in C/C++. Please note that, from a YAML
perspective, the include directive is just another comment.

## Addressing Nodes

Tree nodes are addressed, left-to-right, from the root of the tree
toward the node itself. Each element in the name is delimited with
a period ("."). The element hops from the root to the leaf is called ```path```.

For example, the dimming status of the rearview mirror in the cabin is addressed:


    Cabin.RearviewMirror.Dimmed


If there are an array of elements, such as door rows 1-3, they will be
addressed with an index branch:

```
Cabin.Door.Row1.Left.IsLocked
Cabin.Door.Row1.Left.Window.Position

Cabin.Door.Row2.Left.IsLocked
Cabin.Door.Row2.Left.Window.Position

Cabin.Door.Row3.Left.IsLocked
Cabin.Door.Row3.Left.Window.Position
```

In a similar fashion, seats are located by row and their left-to-right position.

```
Cabin.Seat.Row1.Pos1.IsBelted  # Left front seat
Cabin.Seat.Row1.Pos2.IsBelted  # Right front seat

Cabin.Seat.Row2.Pos1.IsBelted  # Left rear seat
Cabin.Seat.Row2.Pos2.IsBelted  # Middle rear seat
Cabin.Seat.Row2.Pos3.IsBelted  # Right rear seat
```

The exact use of ```PosX``` elements and how they correlate to actual
positions in the car, is dependent on the actual vehicle using the
spec.

## PARENT NODES
If a new leaf node is defined, all parent branches included in its name must
be included as well, as shown below:

```
[Signal] Cabin.Door.Row1.Left.IsLocked
[Branch] Cabin.Door.Row1.Left
[Branch] Cabin.Door.Row1
[Branch] Cabin.Door
[Branch] Cabin
```

The branches do not have to be defined in any specific order as long
as each branch component is defined somewhere in the vspec file (or an
included vspec file).
