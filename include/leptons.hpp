// Project-2 - Luca Vicaria - PHYS30762
// This file defines lepton classes and their specific behaviors in the particle simulation.
// It manages lepton properties, interactions, and anti-particle conversions.
// Last modified 08/05/2024

#ifndef LEPTONS_HPP
#define LEPTONS_HPP

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <random>
#include <memory>

#include "particle.hpp"
#include "quarks.hpp"

// Define properties for Leptons
template <>
std::map<LeptonType, std::map<std::string, std::string>> GenericParticle<LeptonType>::m_staticProps {
	{LeptonType::Electron, {{"name", "Electron"}, {"mass", "0.511"}, {"charge", "-1"}, {"spin", "0.5"}}},
	{LeptonType::Muon, {{"name", "Muon"}, {"mass", "105.66"}, {"charge", "-1"}, {"spin", "0.5"}}},
	{LeptonType::Tau, {{"name", "Tau"}, {"mass", "1776.8"}, {"charge", "-1"}, {"spin", "0.5"}}},
	{LeptonType::Neutrino, {{"name", "Neutrino"}, {"mass", "0"}, {"charge", "0"}, {"spin", "0.5"}}}};

// Enumeration for different lepton types
enum class NeutrinoType {
	ElectronNeutrino,
	MuonNeutrino,
	TauNeutrino
};

// Forward declaration of classes
class Electron;
class Muon;
class Tau;
class Neutrino;
class Quark;

// Lepton class template
class Lepton : public GenericParticle<LeptonType> {
public:
	// Parameterized constructor
	Lepton(LeptonType type, std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false)
		: GenericParticle<LeptonType>(type, fourMomentum, isAntiParticle) {
		if(type == LeptonType::Electron || type == LeptonType::Muon || type == LeptonType::Tau || type == LeptonType::Neutrino)
			m_leptonNumber = isAntiParticle ? -1 : 1;
		else
			m_leptonNumber = 0;
	}

	// Copy constructor
	Lepton(const Lepton &other)
		: GenericParticle(other) {}

	// Move constructor
	Lepton(Lepton &&other) noexcept
		: GenericParticle(std::move(other)) {}

	// Destructor
	virtual ~Lepton() {}

	// Copy assignment operator
	Lepton &operator=(const Lepton &other) {
		if(this != &other) {
			GenericParticle<LeptonType>::operator=(other);
		}
		return *this;
	}

	// Move assignment operator
	Lepton &operator=(Lepton &&other) noexcept {
		if(this != &other) {
			GenericParticle<LeptonType>::operator=(std::move(other));
		}
		return *this;
	}

  // override the getInfo method to include the lepton number
	virtual std::string getInfo() const override {
		std::string baseInfo = GenericParticle::getInfo(); // Get the base class information
		std::stringstream ss;
		ss<<baseInfo
		  <<", Lepton Number="<<m_leptonNumber;
		return ss.str();
	}

	int getLeptonNumber() const { return m_leptonNumber; }
};

class Electron : public Lepton {
private:
	std::vector<double> m_layerEnergies; // Stores energy deposited in each of four calorimeter layers

	// Randomly distribute total energy from FourMomentum across four calorimeter layers
	void distributeEnergy() {
		if(!m_fourMomentum)
			return; // Ensure FourMomentum is present

		double totalEnergy = m_fourMomentum->get_energy();
		std::default_random_engine generator(std::random_device{}());
		std::uniform_real_distribution<double> distribution(0.0, 1.0);

		double sumFrac = 0.0;
		std::vector<double> fractions(3);
		for(int i = 0; i < 3; ++i) {
			fractions[i] = distribution(generator);
			sumFrac += fractions[i];
		}

		double cumulativeEnergy = 0.0;
		for(int i = 0; i < 3; ++i) {
			m_layerEnergies[i] = (fractions[i] / sumFrac) * totalEnergy;
			cumulativeEnergy += m_layerEnergies[i];
		}

		m_layerEnergies[3] = totalEnergy - cumulativeEnergy; // Ensure exact energy conservation
	}

public:
	// Constructor
	Electron(std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false)
		: Lepton(LeptonType::Electron, fourMomentum, isAntiParticle), m_layerEnergies(4, 0.0) {
		if(fourMomentum)
			distributeEnergy();
	}

	std::shared_ptr<Particle> getAntiParticle() const override {
		auto antiParticle = std::make_shared<Electron>(m_fourMomentum, !m_isAntiParticle);
		antiParticle->m_layerEnergies = m_layerEnergies; // Copy the energy values to the antiparticle
		return std::static_pointer_cast<Particle>(antiParticle);
	}

	std::string getInfo() const override {
		std::string info = Lepton::getInfo();
		std::stringstream ss;
		ss<<info<<", Calorimeter Energies=[";
		for(size_t i = 0; i < m_layerEnergies.size(); ++i) {
			ss<<m_layerEnergies[i] << (i < m_layerEnergies.size() - 1 ? ", " : "");
		}
		ss<<"]";
		return ss.str();
	}
};

class Muon : public Lepton {
private:
	bool m_isIsolated; // Isolation variable specific to muons

public:
	// Parameterized constructor with isolation variable
	Muon(std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false, bool isIsolated = false)
		: Lepton(LeptonType::Muon, fourMomentum, isAntiParticle), m_isIsolated(isIsolated) {}

	virtual std::shared_ptr<Particle> getAntiParticle() const override {
		auto antiParticle = std::make_shared<Muon>(m_fourMomentum, !m_isAntiParticle, m_isIsolated);
		return std::static_pointer_cast<Particle>(antiParticle);
	}

	// Getter for the isolation status
	bool isIsolated() const {
		return m_isIsolated;
	}

	virtual std::string getInfo() const override {
		std::stringstream ss;
		ss<<Lepton::getInfo()<<", Isolation: "<<(m_isIsolated ? "Yes" : "No");
		return ss.str();
	}
};

class Tau : public Lepton {
public:
	Tau(std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false)
		: Lepton(LeptonType::Tau, fourMomentum, isAntiParticle) { selectDecayMode(); }

	std::shared_ptr<Particle> getAntiParticle() const override {
		auto antiParticle = std::make_shared<Tau>(m_fourMomentum, !m_isAntiParticle);
		return antiParticle;
	}

	// Allow re-setting of decay particles, overriding if valid
	// This will be a vector of particles that the Tau decays into
	void setDecayParticles(const std::vector<std::shared_ptr<Particle>> &decayParticles) {		
		m_decayParticles.clear();
		
		if(validateDecayParticles(decayParticles))		
			std::copy(decayParticles.begin(), decayParticles.end(), std::back_inserter(m_decayParticles));
		else
			std::cerr<<"\nInvalid decay particles for Tau"<<std::endl;
	}

	std::string getInfo() const override {
		std::string baseInfo = Lepton::getInfo();
		std::stringstream ss;
		ss<<baseInfo;
		// Only add decay particle info if there are any decay particles
		if(!m_decayParticles.empty()) {
			ss<<", Decay Particles: ";
			for(const auto &particle : m_decayParticles) {
				ss<<particle->getName()<<", ";
			}
			// Remove the last comma and space
			std::string result = ss.str();
			return result.substr(0, result.size() - 2);
		}

		return ss.str();
	}

private:
	// Randomly select and populate decay particles using a single RNG
	void selectDecayMode() {
		static std::random_device rd;  // Static to use the same random device
		static std::mt19937 gen(rd()); // Static to use the same generator, seeded once
		std::uniform_int_distribution<> dis(1, 2);

		if(dis(gen) == 1)
			decayLeptonic();
		else
			decayHadronic();
	}

	// Decay to a lepton (muon) and corresponding antineutrino, and a tau antineutrino as this is the most probable and stable decay mode
	void decayLeptonic() {
		m_decayParticles.push_back(std::static_pointer_cast<Particle>(std::make_shared<Muon>(std::make_shared<FourMomentum>(105.66, 0, 0, 0), m_isAntiParticle)));
		m_decayParticles.push_back(std::static_pointer_cast<Particle>(std::make_shared<Neutrino>(NeutrinoType::MuonNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), !m_isAntiParticle, false)));
		m_decayParticles.push_back(std::static_pointer_cast<Particle>(std::make_shared<Neutrino>(NeutrinoType::TauNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), m_isAntiParticle, false)));
	}

	// Decay to two quarks (up and anti-up) and a tau antineutrino as this is the most probable and stable decay mode
	void decayHadronic() {
		m_decayParticles.push_back(std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::UpQuark, ColourCharge::Red, std::make_shared<FourMomentum>(2.2, 0, 0, 0), m_isAntiParticle)));
		m_decayParticles.push_back(std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::UpQuark, ColourCharge::AntiRed, std::make_shared<FourMomentum>(2.2, 0, 0, 0), !m_isAntiParticle)));
		m_decayParticles.push_back(std::static_pointer_cast<Particle>(std::make_shared<Neutrino>(NeutrinoType::TauNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), m_isAntiParticle, false)));
	}

	// Validation checks
	bool validateDecayParticles(const std::vector<std::shared_ptr<Particle>> &decayParticles) {
		return decayParticles.size() == 3 &&
					 checkChargeConservation(decayParticles) &&
					 checkLeptonNumberConservation(decayParticles) &&
					 checkBaryonNumberConservation(decayParticles);
		}

	// Check lepton number conservation
	bool checkLeptonNumberConservation(const std::vector<std::shared_ptr<Particle>> &decayParticles) {
		int totalLeptonNumber = 0;
		for (const auto &particle : decayParticles) {
			if (particle->getType() == "Lepton")
				totalLeptonNumber += particle->getLeptonNumber();
		}

		auto result = totalLeptonNumber == getLeptonNumber();
		if (!result)
			std::cerr<<"Lepton number conservation violated for Tau decay\n"<<std::endl;

		return result;
	}

	// Check baryon number conservation
	bool checkBaryonNumberConservation(const std::vector<std::shared_ptr<Particle>> &decayParticles) {
		double totalBaryonNumber = 0.0;
		for (const auto &particle : decayParticles) {
			if (particle->getType() == "Quark")
				totalBaryonNumber += particle->getBaryonNumber();
		}

		auto result = abs(totalBaryonNumber) < 0.1; // Allow for floating-point precision issues due to 1/3 charge of quarks
		if (!result)
			std::cerr << "Baryon number conservation violated for Tau decay\n" << std::endl;

		return result; // Tau lepton does not carry baryon number.
	}
};

class Neutrino : public Lepton {
private: 
	std::string getNeutrinoTypeStr() {
		if(m_neutrinoType == NeutrinoType::ElectronNeutrino)
			return "Electron";
		else if(m_neutrinoType == NeutrinoType::MuonNeutrino)
			return "Muon";
		else
			return "Tau";
	}

protected:
	NeutrinoType m_neutrinoType;
	bool m_interactsWithDetector; // Indicates interaction with the detector

public:
	// Constructor with interaction property
	Neutrino(NeutrinoType type, std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false, bool interactsWithDetector = false)
		: Lepton(LeptonType::Neutrino, fourMomentum, isAntiParticle),
		  m_neutrinoType(type), m_interactsWithDetector(interactsWithDetector) {}

	// Getter for neutrino type
	NeutrinoType getNeutrinoType() const {
		return m_neutrinoType;
	}

	// Getter and Setter for interaction property
	bool getInteractsWithDetector() const {
		return m_interactsWithDetector;
	}

	void setInteractsWithDetector(bool interacts) {
		m_interactsWithDetector = interacts;
	}

	std::string getName() const override { 
		if(m_isAntiParticle)
			return ANTI_PREFIX + getNeutrinoTypeStr() + "-Neutrino"; 
		else
			return getNeutrinoTypeStr() + "-Neutrino"; 
	}

	// Override the getAntiParticle to handle neutrino specific properties
	virtual std::shared_ptr<Particle> getAntiParticle() const override {
		auto antiParticle = std::make_shared<Neutrino>(m_neutrinoType, m_fourMomentum, !m_isAntiParticle, m_interactsWithDetector);
		return std::static_pointer_cast<Particle>(antiParticle);
	}

	// Override to provide information specific to Neutrinos
	virtual std::string getInfo() const override {
		std::stringstream ss;
		ss<<Lepton::getInfo()<<", Interacts with Detector="<<std::boolalpha<<m_interactsWithDetector;
		return ss.str();
	}

	// Get the neutrino type as a string
	std::string getNeutrinoTypeStr() const {
		return  m_neutrinoType == NeutrinoType::ElectronNeutrino ? "Electron" : m_neutrinoType == NeutrinoType::MuonNeutrino ? "Muon"																																		 : "Tau";
	}
};

#endif // LEPTONS_HPP
