# <Example Title>

**Location:** `examples/<example-name>/`  
**Category:** <Basics / Geometry / Physics / CUDA / Full Application / ...>  
**AdePT version tested:** <vX.Y or "development">  
**Status:** <working / experimental / WIP>

---

## 1. Purpose

Describe in a few sentences what this example demonstrates and why it exists.

---

## 2. Features Demonstrated

List concrete AdePT features shown here, e.g.:

- Geometry loading  
- Navigation on GPU (VecGeom)  
- G4HepEm physics integration  
- CUDA RDC usage  
- Device memory pools  
- Particle queues  
- Scoring  
- Multi-stage or streamed transport  

---

## 3. Prerequisites

- AdePT installed and discoverable via `CMAKE_PREFIX_PATH`
- CUDA toolkit (version required by AdePT)
- Dependencies typically transitively found (VecGeom, VecCore, G4HepEm, G4VG)

If the example requires additional input files (geometry, materials), list them.

---

## 4. Build Instructions

General build:

```bash
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH="<AdePT installation prefix>"
cmake --build build -- -j
```

To build only this example target:

```bash
cmake --build build --target <example-target>
```

---

## 5. How to Run

```bash
/build/examples/<example-name>/<binary> [options]
```

Describe:
- Input parameters
- Required files
- Typical use cases

---

## 6. Expected Output

```yaml
Transported 10000 electrons
Kernel time: 12.3 ms
Energy deposit: 4.5 MeV
```
If the example writes output files, list them here.

---

## 7. Validation or Checks (Optional)

For physics or geometry examples, explain:
- expected physical behavior
- reference results
- CI checks or comparison criteria

---

## 8. File structure

```objectivec

examples/<example-name>/
  README.md
  CMakeLists.txt
  src/
    main.cu
    kernels.cu
    helpers.hh
  data/      # if needed
  scripts/   # validation or plotting
```

---

## 9. Notes / Troubleshooting

Record:
- common build issues
- CUDA limitations
- required GPU compute capability
- known bugs or TODOs

---

## 10. License

Use the repository-wide SPDX identifiers:

```text
SPDX-FileCopyrightText: 2025 CERN for the benefit of the AdePT project
SPDX-License-Identifier: Apache-2.0
```
Documentation in this file is licensed under CC-BY-4.0.

---
