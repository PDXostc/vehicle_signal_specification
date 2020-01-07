---
title: "Resource Branch"
date: 2019-08-04T13:10:15+02:00
weight: 6
---

A ```resource branch``` is an entity that can host only element nodes.
A resorce branch is identified as an entry with its node type set to ```rbranch```.
It can host zero or more element nodes, and it contains the format definition
of its element nodes. Besides the required fields ```type``` and ```description```,
are also the following.

* **```child-type```**<br>
An rbranch child must be of the generic type ```element```, but it also has a
uniquely specified part that can be referred to by the child type.

* **```child-properties```**<br>
An rbranch child format is defined through a number of ```properties```,
each property is defined by the attributes: ```name```, ```description```
,```type```, ```format```, ```unit```, and ```value```.

* **```prop-name```**
This is the key value used to refer to this property. An element must contain
the properties named ```id```, ```name```, and ```uri``` <inherited from VIWI?>.

* **```prop-description```**
This is a description of the property.

* **```prop-type```**
This is the type of the property.

* **```prop-format```**
This is the format of this property.

* **```prop-unit```**
This is the unit of this property.

* **```prop-value```**
If this property is a logical link to other elements, then the path to the
rbranch of these elements is given here. The ```id``` value of these
elements are provided in a list.
