// Project-2 - Luca Vicaria - PHYS30762
// This file defines the FourMomentum class used to manage energy and momentum of particles.
// It includes methods to compute invariant mass and perform vector operations on four-momenta.
// Last modified 08/05/2024

#ifndef FOUR_MOMENTUM_HPP
#define FOUR_MOMENTUM_HPP

#include <cmath>
#include <stdexcept>
#include <sstream>
#include <iostream>

class FourMomentum {
private:
	double m_rest_mass;
	double m_energy;
	double m_px;
	double m_py;
	double m_pz;

public:
	// Default constructor initializes to zero which is always valid
	FourMomentum() : m_energy(0.0), m_px(0.0), m_py(0.0), m_pz(0.0) {}

	// Parameterized constructor with validation
	FourMomentum(double e, double x, double y, double z) : m_energy(e), m_px(x), m_py(y), m_pz(z) {
	}

	// Copy and move constructors can rely on the validity of the source object
	FourMomentum(const FourMomentum& other) = default;
	FourMomentum(FourMomentum&& other) noexcept = default;

	// Destructor
	~FourMomentum() {}

	// Assignment operators
	FourMomentum& operator=(const FourMomentum& other) = default;
	FourMomentum& operator=(FourMomentum&& other) noexcept = default;

	// Getters
	double get_energy() const { return m_energy; }
	double get_px() const { return m_px; }
	double get_py() const { return m_py; }
	double get_pz() const { return m_pz; }

	// Validates the four-momentum to ensure energy is greater than 0 and the invariant mass is equal to the rest mass of the particle that the four-momentum belongs to.
	bool validate() {
		const double tolerance = 1e-5;  // Define a suitable tolerance level for floating-point comparisons
		if(this->m_energy < 0 || std::abs(this->invariant_mass() - m_rest_mass) > tolerance) {
			std::cerr<<"Physical inconsistency: Energy cannot be negative and the invariant mass must be equal to the rest mass of the particle."<< std::endl;
			return false;
		} else {
			return true;
		}
	}

	double get_rest_mass() const {
		return m_rest_mass;
	}

	void set_rest_mass(double rest_mass) {
		m_rest_mass = rest_mass;
	}

	// Setters with validation
	void set_energy(double e) {
		m_energy = e;
		if(validate())
			return;
		else
			throw std::invalid_argument("Invalid four-momentum: Energy must be greater than or equal to the magnitude of the momentum vector and invariant mass must be non-negative.");
	}

	void set_px(double x) {
		m_px = x;
		if(validate())
			return;
		else
			throw std::invalid_argument("Invalid four-momentum: Energy must be greater than or equal to the magnitude of the momentum vector and invariant mass must be non-negative.");
	}

	void set_py(double y) {
		m_py = y;
		if(validate())
			return;
		else
			throw std::invalid_argument("Invalid four-momentum: Energy must be greater than or equal to the magnitude of the momentum vector and invariant mass must be non-negative.");
	}

	void set_pz(double z) {
		m_pz = z;
		if(validate())
			return;
		else
			throw std::invalid_argument("Invalid four-momentum: Energy must be greater than or equal to the magnitude of the momentum vector and invariant mass must be non-negative.");
	}

	// Operator overload for addition and subtraction of four-momenta
	FourMomentum operator+(const FourMomentum& other) const {
		return FourMomentum(m_energy + other.m_energy, m_px + other.m_px, m_py + other.m_py, m_pz + other.m_pz);
	}

	FourMomentum operator-(const FourMomentum& other) const {
		return FourMomentum(m_energy - other.m_energy, m_px - other.m_px, m_py - other.m_py, m_pz - other.m_pz);
	}

	// Dot product calculation
	double dot_product(const FourMomentum& other) const {
		return m_energy * other.m_energy - (m_px * other.m_px + m_py * other.m_py + m_pz * other.m_pz);
	}

	// Invariant mass calculation using *this pointer to refer to the current object's data members explicitly
	double invariant_mass() const {
		return std::sqrt(std::max(0.0, this->m_energy * this->m_energy - (this->m_px * this->m_px + this->m_py * this->m_py + this->m_pz * this->m_pz)));
	}

	std::string print_four_momentum() const {
		std::ostringstream out;
		out<<"(E="<<m_energy<<", Px="<<m_px<<", Py="<<m_py<<", Pz="<<m_pz<<")";
		return out.str();
	}
};

#endif // FOUR_MOMENTUM_HPP
