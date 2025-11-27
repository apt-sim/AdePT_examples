<!--
SPDX-FileCopyrightText: 2025 CERN for the benefit of the AdePT project
SPDX-License-Identifier: CC-BY-4.0
-->

# AdePT Examples — Documentation

This directory contains the documentation for the `AdePT-examples` repository.
It includes an overview of the available examples, guidance for contributors,
and a template for documenting new examples.

Each example in [`examples/`](../examples/) contains its own `README.md`
explaining its purpose, features, and usage.

---

## 📚 Purpose of This Repository

The AdePT Examples project provides:

1. **Minimal, self-contained demonstrations** of AdePT usage  
2. **Focused feature examples** (geometry, physics, navigation, GPU workflows)  
3. **Reference applications** that combine multiple components  
4. **Sanity/regression examples** suitable for CI validation  

All examples are intentionally modular so users can copy them into their own
applications.

---

## 🗂 Example Index

(*Examples will appear here as they are added.*)

### **1. Basics**
| Example | Description | Status |
|--------|-------------|--------|
| `example_minimal` | The smallest possible AdePT application; context setup + simple loop. | _planned_ |

### **2. Geometry & Navigation**
| Example | Description | Status |
|--------|-------------|--------|
| `navigation_basics` | Using VecGeom navigation through AdePT. | _planned_ |
| `custom_geometry` | Loading arbitrary GDML and GPU navigation checks. | _planned_ |

### **3. Physics Integration**
| Example | Description | Status |
|--------|-------------|--------|
| `g4hepem_electrons` | Electron/positron transport with G4HepEm via AdePT. | _planned_ |
| `multi_physics` | Combining several physics models. | _planned_ |

### **4. CUDA / GPU Features**
| Example | Description | Status |
|--------|-------------|--------|
| `cuda_rdc_usage` | Demonstrates CUDA RDC and AdePT CMake helpers. | _planned_ |
| `memory_management` | Device/host memory pools and particle queues. | _planned_ |
| `streams_async` | Using CUDA streams for multi-stage transport. | _planned_ |

### **5. Full Applications**
| Example | Description | Status |
|--------|-------------|--------|
| `simple_calorimeter` | Minimal calorimeter simulation using AdePT. | _planned_ |
| `benchmark_app` | A performance benchmark for transport kernels. | _planned_ |

---

## 📄 Template for Per-Example Documentation

To ensure consistency across all examples, contributors should follow the common
documentation format provided in:

➡️ **[`example_template.md`](example_template.md)**

This template includes sections for:

- Purpose  
- Features Demonstrated  
- Build & Run Instructions  
- Expected Output  
- Validation  
- File Structure  
- Troubleshooting  
- Licensing  

---

## 🧩 How to Add a New Example

1. Create a new subdirectory under `examples/<name>/`
2. Add source files and a `CMakeLists.txt`
3. Document your exam
