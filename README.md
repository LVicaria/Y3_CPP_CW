
# Particle Catalogue Project

This project implements a particle catalogue using C++, designed to store and manage data on particles from the Standard Model. The catalogue utilizes a class hierarchy for different particle types and includes functionalities for particle properties, categorization, and particle-specific behaviors. The project is structured to accommodate additional functionality for extra credit.

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Class Structure](#class-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Advanced Functionalities](#advanced-functionalities)
- [Contributing](#contributing)
- [License](#license)

## Project Overview

The Particle Catalogue Project was created for the PHYS30762 - Object-Oriented Programming in C++ course. The primary goal is to develop a catalogue for Standard Model particles and provide a container for particle management. The project involves implementing abstract classes, polymorphism, and a particle catalogue wrapped around STL containers.

## Features

The project includes the following features:
- A class hierarchy representing various particles.
- Abstract base classes and derived classes for specific particle types.
- Implementation of the "Rule of Five" for proper memory management.
- Specialized properties for each particle type.
- A customizable container class for particle management.
- Advanced functionalities for challenge marks, such as templates, lambdas, and exception handling.

## Class Structure

The project is built on a three-level class hierarchy as follows:
1. **Abstract Base Class** - Represents the general properties of a particle.
2. **Derived Classes** - Represent specific particle types (e.g., leptons, quarks, bosons).
3. **Further Derived Classes** - Implement additional specifics like color charge for quarks, isolation for muons, decay patterns for bosons, etc.

### Key Classes

- **Particle**: The base class for all particles, containing common properties like charge, spin, and a four-momentum object.
- **FourMomentum**: Represents the four-momentum of a particle, with overloaded operators for vector operations and a method for invariant mass calculation.
- **ParticleContainer**: A catalogue for managing particles using STL containers and smart pointers.

## Getting Started

### Prerequisites

- C++17 or later
- CMake (for build automation)
- A C++ compiler (e.g., GCC, Clang)

### Installation

1. Clone this repository.
   ```bash
   git clone https://github.com/your-username/particle-catalogue.git
   cd particle-catalogue
   ```

2. Build the project using CMake.
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

To run the project, execute the compiled binary from the `build` directory:

```bash
./particle_catalogue
```

### Example

The `main()` function demonstrates the usage of the particle container and the instantiation of particles with different properties. You can customize the particle properties by modifying the `main()` function to suit your requirements.

## Advanced Functionalities

For additional challenge marks, the following advanced C++ features are implemented:
- **Templates**: For generalizing functions and classes.
- **Lambda Functions**: For inline expressions and operations.
- **Exception Handling**: To manage errors and ensure robustness.
- **Custom STL Containers**: Experimenting with alternatives to `std::vector` where possible.

## Contributing

Contributions are welcome. Please submit a pull request or open an issue for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.
