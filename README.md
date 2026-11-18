<!--
SPDX-FileCopyrightText: 2025 CERN for the benefit of the AdePT project
SPDX-License-Identifier: CC-BY-4.0
-->

# AdePT Examples

This repository hosts standalone examples for
[AdePT](https://github.com/apt-sim/AdePT).  
Its goals are to:

- provide minimal and focused demonstrations of AdePT features,
- offer templates for integrating AdePT into custom applications,
- serve as a basis for validation and sanity checks in continuous integration.

---

## 📚 Documentation

Documentation for all examples is located in the [`docs/`](docs/) directory.

- **Overview & Example Index:**  
  👉 [`docs/README.md`](docs/README.md)

- **Template for creating a new example:**  
  👉 [`docs/example_template.md`](docs/example_template.md)

Each example under [`examples/`](examples/) contains its own `README.md` based on
this template.

---

## 🔧 Build Instructions

The examples assume an existing AdePT installation (and its dependencies).  
In a typical setup:

1. Install AdePT as described in the AdePT [README](https://github.com/apt-sim/AdePT).  
2. Configure this repository with CMake, pointing `CMAKE_PREFIX_PATH` to the AdePT installation:

   ```bash
   cmake -S . -B build -DCMAKE_PREFIX_PATH="<path_to_adept_installation>;<other_prefixes>"

   cmake --build build -- -j
   ```

Individual example targets will appear under `build/examples/<name>/`.

For details, follow the documentation of each example.

---

## 📦 Repository Structure

```bash
.
├── CMakeLists.txt
├── docs/
│   ├── README.md               # Documentation hub and example index
│   └── example_template.md     # Template for documenting new examples
├── examples/                   # Each example lives in its own subdirectory
├── LICENSE
└── LICENSES/                   # REUSE-compliant license texts
```

---

## 🤝 Contributing

Contributions of new examples are welcome!
Please follow the steps and documentation guidelines described in:

👉 [`docs/example_template.md`](docs/example_template.md)

Also see the project’s [`CONTRIBUTING.md`](CONTRIBUTING.md) for coding style and workflow.

---

## 📜 License

- Code in this repository is licensed under Apache-2.0
- Documentation is licensed under CC-BY-4.0
- `examples/ATLLArBarrel/` keeps its original MIT license (see `.reuse/dep5`)

See [`LICENSE`](LICENSE) and [`LICENSES/`](LICENSES/) for details.
