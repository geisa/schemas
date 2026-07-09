<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# GEISA profile schemas

This directory contains profile schemas for GEISA device and capability
profiles. For example,
`geisa-metered-quantities-ac-meter-profile-v0.9.json` defines the GEISA 0.9
AC meter profile for scalar metered quantities, and
`geisa-metered-quantities-billing-profile-v0.9.json` defines the GEISA 0.9
billing profile.

Base schemas define capability-tolerant message structures. Profile schemas add
required fields or field families for specific device types or capability
classes without implying that all GEISA devices implement all APIs or expose
all measurements/values.

Profile schemas are intended for profile-specific validation by examples,
implementations, test tooling, and conformance tooling. A validator can select
profiles from Platform Discovery metadata such as device type, metrology
capability, meter form, phase count, and advertised API support.

Later specification versions may add additional profiles. Implementers may also
provide device or application-specific validation profiles for operator use,
but GEISA 0.9 does not require per-application profiles.
