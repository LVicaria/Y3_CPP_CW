// Project-2 - Luca Vicaria - PHYS30762
// This file defines boson classes including their properties and interactions within the simulation.
// It deals with particle properties such as mass, charge, spin, and decay mechanisms.
// Last modified 08/05/2024

#ifndef BOSONS_HPP
#define BOSONS_HPP

#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "particle.hpp"
#include "quarks.hpp"
#include "leptons.hpp"

// Define properties for Bosons
template <>
std::map<BosonType, std::map<std::string, std::string>> GenericParticle<BosonType>::m_staticProps{
	{BosonType::Photon, {{"name", "Photon"}, {"mass", "0"}, {"charge", "0"}, {"spin", "1"}}},
	{BosonType::W,      {{"name", "W Boson"}, {"mass", "80360"}, {"charge", "+1"}, {"spin", "1"}}},
	{BosonType::Z,      {{"name", "Z Boson"}, {"mass", "91190"}, {"charge", "0"}, {"spin", "1"}}},
	{BosonType::Gluon,  {{"name", "Gluon"}, {"mass", "0"}, {"charge", "0"}, {"spin", "1"}}},
	{BosonType::Higgs,  {{"name", "Higgs Boson"}, {"mass", "125110"}, {"charge", "0"}, {"spin", "0"}}}
};

// Specific Boson class deriving from GenericParticle
class Boson : public GenericParticle<BosonType> {
public:
	Boson(BosonType type, std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false): GenericParticle<BosonType>(type, fourMomentum, isAntiParticle) {}
};

class Photon : public Boson {
public:
	Photon(std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false) : Boson(BosonType::Photon, fourMomentum, isAntiParticle) {}

	std::shared_ptr<Particle> getAntiParticle() const override {
		auto positron = std::make_shared<Photon>(m_fourMomentum, !m_isAntiParticle);
		return std::static_pointer_cast<Particle>(positron);
	}
};

class WBoson : public Boson {
public:
	WBoson(std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false) : Boson(BosonType::W, fourMomentum, isAntiParticle) {}

	std::shared_ptr<Particle> getAntiParticle() const override {
		// W boson antiparticle with opposite charge
		auto antiW = std::make_shared<WBoson>(m_fourMomentum, !m_isAntiParticle);
		return std::static_pointer_cast<Particle>(antiW);
	}

  	// Set the decay particles for the W boson
	void setDecayParticles(const std::vector<std::shared_ptr<Particle>>& decayParticles) {
		if(validateDecayParticles(decayParticles))
			m_decayParticles = decayParticles;
		else
			throw std::invalid_argument("Decay particles do not conserve the required properties.");
	}

	std::string getInfo() const override {
		std::stringstream ss;
		ss<<Boson::getInfo();
		if(!m_decayParticles.empty()) {
			ss<<", Decay Particles: ";
			for(const auto& particle : m_decayParticles) {
				ss<<particle->getName()<<", ";
			}
			std::string result = ss.str();
			return result.substr(0, result.size() - 2);
		}
		return ss.str();
	}

protected:
	bool validateDecayParticles(const std::vector<std::shared_ptr<Particle>>& decayParticles) {
		return decayParticles.size() == 2 && checkChargeConservation(decayParticles);
    }
};

class ZBoson : public Boson {
public:
	ZBoson(std::shared_ptr<FourMomentum> fourMomentum) : Boson(BosonType::Z, fourMomentum, false) {}

	std::shared_ptr<Particle> getAntiParticle() const override {
		// Z boson is its own antiparticle
		auto zBoson = std::make_shared<ZBoson>(m_fourMomentum);
		return std::static_pointer_cast<Particle>(zBoson);
	}

	void setDecayParticles(const std::vector<std::shared_ptr<Particle>>& decayParticles) {
		if(validateDecayParticles(decayParticles))
			m_decayParticles = decayParticles;
		else
			throw std::invalid_argument("\nDecay particles do not conserve the required properties.");
	}

	std::string getInfo() const override {
		std::stringstream ss;
		ss<<Boson::getInfo();
		if(!m_decayParticles.empty()) {
			ss<<", Decay Particles: ";
			for(const auto& particle : m_decayParticles) {
				ss<<particle->getName()<<", ";
			}
		std::string result = ss.str();
		return result.substr(0, result.size() - 2);
		}
		return ss.str();
	}

protected:
	bool validateDecayParticles(const std::vector<std::shared_ptr<Particle>>& decayParticles) {
		return decayParticles.size() == 2 && checkChargeConservation(decayParticles);
	}
};

class Gluon : public Boson {
public:
	Gluon(std::shared_ptr<FourMomentum> fourMomentum, ColourCharge colour, ColourCharge antiColour) 
		: Boson(BosonType::Gluon, fourMomentum, false), m_colourCharge(colour), m_antiColorCharge(antiColour) { checkConsistency(); }

	std::shared_ptr<Particle> getAntiParticle() const override {
		// Swaps color and anti-colour for the antiparticle
		auto gluon = std::make_shared<Gluon>(m_fourMomentum, m_antiColorCharge, m_colourCharge);
		return std::static_pointer_cast<Particle>(gluon);
	}

	void checkConsistency() const {
		// Checks if the gluon has valid color and anti-color charges
		if(!isValidColour(m_colourCharge) || !isValidColour(m_antiColorCharge))
			throw std::invalid_argument("Gluon must have both colour and anti-colour charges, with valid values.");
	}

	// Convert colour charge to string
	static bool isValidColour(ColourCharge colour) {
		return colour == ColourCharge::Red || colour == ColourCharge::Green || colour == ColourCharge::Blue ||
					 colour == ColourCharge::AntiRed || colour == ColourCharge::AntiGreen || colour == ColourCharge::AntiBlue;
	}

	std::string getInfo() const override {
		std::stringstream ss;
		ss<<Boson::getInfo(); // Get base particle information
		ss<<", Colour Charge: "<<Quark::colourChargeToString(m_colourCharge)
			<<", Anti-Colour Charge: "<<Quark::colourChargeToString(m_antiColorCharge);
		return ss.str();
}

private:
	ColourCharge m_colourCharge;
	ColourCharge m_antiColorCharge;
};

class HiggsBoson : public Boson {
public:
	HiggsBoson(std::shared_ptr<FourMomentum> fourMomentum) : Boson(BosonType::Higgs, fourMomentum, false) {}

	std::shared_ptr<Particle> getAntiParticle() const override {
		// Higgs boson is its own antiparticle
		auto higgsBoson = std::make_shared<HiggsBoson>(m_fourMomentum);
		return std::static_pointer_cast<Particle>(higgsBoson);
	}

	void setDecayParticles(const std::vector<std::shared_ptr<Particle>>& decayParticles) {
		if(validateDecayParticles(decayParticles))
			m_decayParticles = decayParticles;
		else
			throw std::invalid_argument("\nDecay particles do not conserve the required properties.");
	}

	std::string getInfo() const override {
		std::stringstream ss;
		ss<<Boson::getInfo();
		if(!m_decayParticles.empty()) {
			ss<<", Decay Particles: ";
			for(const auto& particle : m_decayParticles) {
				ss<<particle->getName()<<", ";
			}
		std::string result = ss.str();
		return result.substr(0, result.size() - 2);
		}
		return ss.str();
	}

protected:
	bool validateDecayParticles(const std::vector<std::shared_ptr<Particle>>& decayParticles) {
		// Higgs boson decays typically result in pairs (like ZZ, WW, or bb)
		return (decayParticles.size() == 2 || decayParticles.size() == 4) && checkChargeConservation(decayParticles) && checkDecayModes(decayParticles);
	}

	bool checkDecayModes(const std::vector<std::shared_ptr<Particle>>& decayParticles) {
		return true;
	}
};

#endif // BOSONS_HPP