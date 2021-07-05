---
title: "Software"
date: 2021-07-04T13:31:46+0000
weight: 3
---

Various software frameworks exist to work with VSS data. If something is missing here, please [open an issue](https://github.com/GENIVI/vehicle_signal_specification/issues) and tell us.

### W3C VISS2 reference server
A reference server written in Go for the VISSv2 specification can be found at [https://github.com/UlfBj/W3C_VehicleSignalInterfaceImpl](https://github.com/UlfBj/W3C_VehicleSignalInterfaceImpl).

### KUKSA.val in-vehicle VISS server
A VISS server written in C++, optimized for containerized deployments on vehicle computers is provided by the [KUKSA project](https://github.com/eclipse/kuksa.val).

### IoT Event Analytics Plattform
[IoT Event Analytics](https://github.com/GENIVI/iot-event-analytics) is a Complex Event Processing platform for the Vehicle Edge. The IoTEA tooling can automatically import VSS an data model, and it can optionally use KUKSA.val to ingest live data from a vehicle.

### AOS
The [AOS platform](https://aoscloud.io/) can support VSS to access vehicle data.