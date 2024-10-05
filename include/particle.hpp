// Project-2 - Luca Vicaria - PHYS30762
// This file contains the abstract base class for particles and a generic particle template.
// It defines fundamental particle properties and functionalities essential across various types of particles.
// Last modified 08/05/2024


#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <iostream>
#include <map>
#include <string>
#include <typeinfo>
#include <sstream>
#include <cxxabi.h>
#include "four_momentum.hpp"

// Enumerations for different particle types
enum class LeptonType { Electron, Muon, Tau, Neutrino };
enum class QuarkType { UpQuark, DownQuark, StrangeQuark, CharmQuark, TopQuark, BottomQuark };
enum class BosonType { Photon, W, Z, Gluon, Higgs };
enum class ForceType { Strong, Electromagnetic, Weak, Gravity, None};

const std::string ANTI_PREFIX = "Anti-";

// Abstract base class for all particles
class Particle {
public:	
	virtual std::string getInfo() const = 0;
	virtual void print() const = 0; 
	virtual std::shared_ptr<Particle> getAntiParticle() const = 0;
	virtual std::string getName() const = 0;
	virtual std::string getType() const = 0;
	virtual std::string getMass() const = 0;
	virtual std::string getCharge() const = 0;
	virtual std::string getSpin() const = 0;
	virtual bool isAntiParticle() const = 0;
	virtual std::shared_ptr<FourMomentum> getFourMomentum() const = 0;
	virtual std::vector<std::shared_ptr<Particle>> getDecayParticles() const = 0;
	virtual bool hasDecayParicles() const = 0;

	virtual int getLeptonNumber() const = 0;
	virtual double getBaryonNumber() const = 0;
};

// Generic particle template class
template <typename ParticleType>
class GenericParticle : public Particle {
private:
	// Helper Functions
	static std::string demangle(const char* mangledName) {
		int status;
		char* demangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
		std::string result = (status == 0 ? demangled : mangledName);
		free(demangled); // Free the memory allocated by __cxa_demangle
		return result;
	}

	double convertFractionStrToDouble(std::string fracStr) const {
		double double_value = 0.0;

		if(fracStr[0] == '+')
			fracStr = fracStr.substr(1);

		// Check for fraction
		if(fracStr.find('/') != std::string::npos) {
			size_t slashPos = fracStr.find('/');
			double numerator = std::stod(fracStr.substr(0, slashPos));
			double denominator = std::stod(fracStr.substr(slashPos + 1));
			double_value = (numerator / denominator);
		}
		else
			double_value += std::stod(fracStr);

		return double_value;
	}
	
protected:
	static std::map<ParticleType, std::map<std::string, std::string>> m_staticProps;
	ParticleType m_type;
	bool m_isAntiParticle;
	std::map<std::string, std::string> m_instanceProps; 
	std::shared_ptr<FourMomentum> m_fourMomentum;
	std::vector<std::shared_ptr<Particle>> m_decayParticles;

	int m_leptonNumber = 0;
	double m_baryonNumber = 0.0;
public:
	// Constructor
	GenericParticle(ParticleType type, std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false)
		: m_type(type), m_isAntiParticle(isAntiParticle), m_fourMomentum(fourMomentum) {

			m_instanceProps = m_staticProps[type];
			if(isAntiParticle) {
				// Modify the properties for anti-particles
				if(m_instanceProps["charge"][0] == '+')
					m_instanceProps["charge"] = "-" + m_instanceProps["charge"].substr(1);
				else if(m_instanceProps["charge"][0] == '-')
					m_instanceProps["charge"] = "+" + m_instanceProps["charge"].substr(1);
				
				m_instanceProps["name"] = ANTI_PREFIX + m_instanceProps["name"];
			}
			// Set the rest mass of the four-momentum		
			m_fourMomentum->set_rest_mass(std::stod(this->getMass()));
	}

	// Destructor
	virtual ~GenericParticle() {}

	// Copy constructor
	GenericParticle(const GenericParticle& other)
			: Particle(other),
				m_type(other.m_type),
				m_isAntiParticle(other.m_isAntiParticle),
				m_instanceProps(other.m_instanceProps),
				m_fourMomentum(other.m_fourMomentum),
				m_decayParticles(other.m_decayParticles),
				m_leptonNumber(other.m_leptonNumber),
				m_baryonNumber(other.m_baryonNumber) {}

	// Move constructor
	GenericParticle(GenericParticle&& other) noexcept
			: Particle(std::move(other)),
				m_type(std::move(other.m_type)),
				m_isAntiParticle(std::move(other.m_isAntiParticle)),
				m_instanceProps(std::move(other.m_instanceProps)),
				m_fourMomentum(std::move(other.m_fourMomentum)),
				m_decayParticles(std::move(other.m_decayParticles)),
				m_leptonNumber(other.m_leptonNumber),
				m_baryonNumber(other.m_baryonNumber) {}

	// Copy assignment operator
	GenericParticle& operator=(const GenericParticle& other) {
		if(this != &other) {
			Particle::operator=(other);
			m_type = other.m_type;
			m_isAntiParticle = other.m_isAntiParticle;
			m_instanceProps = other.m_instanceProps;
			m_fourMomentum = other.m_fourMomentum;
			m_decayParticles = other.m_decayParticles;
			m_leptonNumber = other.m_leptonNumber;
			m_baryonNumber = other.m_baryonNumber;
			m_decayParticles = std::move(other.m_decayParticles);
		}
		return *this;
	}

	// Move assignment operator
	GenericParticle& operator=(GenericParticle&& other) noexcept {
		if(this != &other) {
			Particle::operator=(std::move(other));
			m_type = std::move(other.m_type);
			m_isAntiParticle = std::move(other.m_isAntiParticle);
			m_instanceProps = std::move(other.m_instanceProps);
			m_fourMomentum = std::move(other.m_fourMomentum);
			m_leptonNumber = other.m_leptonNumber;
			m_baryonNumber = other.m_baryonNumber;
			m_decayParticles = std::move(other.m_decayParticles);
		}
		return *this;
	}

	// Check if the decay particles conserve charge
	bool checkChargeConservation(const std::vector<std::shared_ptr<Particle>>& decayParticles) const {
		double totalCharge = 0.0;
		for(const auto &particle : decayParticles) {
			auto charge = convertFractionStrToDouble(particle->getCharge());
			totalCharge += charge;
		}

		auto parent_charge = convertFractionStrToDouble(getCharge());

		auto result = abs(totalCharge - parent_charge) < 0.1; // Allow for floating-point precision issues due to 1/3 charge of quarks
		if(!result)
			std::cerr<<"\nCharge conservation violated!"<<std::endl;

		return result;
	}

	// Get the information of the particle
	virtual std::string getInfo() const override {
		std::stringstream ss;
		ss<<"Name="<<getName()
		  <<", Type="<<getType()
		  <<", Mass="<<m_instanceProps.at("mass")
		  <<", Charge="<<m_instanceProps.at("charge")
		  <<", Spin="<<m_instanceProps.at("spin")
		  <<", FourMomentum="<<m_fourMomentum->print_four_momentum();
		return ss.str();
	}

	// Print the information to the standard output
	virtual void print() const override {
		std::cout<<getInfo()<<std::endl;
	}

	virtual std::vector<std::shared_ptr<Particle>> getDecayParticles() const override {
		return m_decayParticles;
	}

	virtual std::shared_ptr<Particle> getAntiParticle() const override { return nullptr;};
	virtual bool hasDecayParicles() const override { return m_decayParticles.size() > 0;	}
	virtual bool isAntiParticle() const override { return m_isAntiParticle; }
	virtual std::string getName() const override { return m_instanceProps.at("name"); }

 	// Get the particle type
	virtual std::string getType() const override { 
		std::string type = demangle(typeid(ParticleType).name());

		if(type.find("LeptonType") != std::string::npos)
			type = "Lepton";
		else if(type.find("QuarkType") != std::string::npos)
			type = "Quark";
		else if(type.find("BosonType") != std::string::npos)
			type = "Boson";
		else if(type.find("ForceType") != std::string::npos)
			type = "Force";
		return type; 
	}
	virtual std::string getCharge() const override { return m_instanceProps.at("charge"); }
	virtual std::string getMass() const override { return m_instanceProps.at("mass"); }
	virtual std::string getSpin() const override { return m_instanceProps.at("spin"); }
	virtual std::shared_ptr<FourMomentum> getFourMomentum() const override { return m_fourMomentum; }
	virtual int getLeptonNumber() const override { return m_leptonNumber; }
	virtual double getBaryonNumber() const override { return m_baryonNumber; }
};

#endif // PARTICLE_HPP