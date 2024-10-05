// Project-2 - Luca Vicaria - PHYS30762
// This file includes definitions and implementations for quark particles.
// It handles quark-specific properties, including color charge and particle interactions.
// Last modified 08/05/2024

#ifndef QUARKS_HPP
#define QUARKS_HPP

#include <iostream>
#include <map>
#include <string>
#include <memory>

#include "particle.hpp"
#include "four_momentum.hpp"

// Enumeration for colour charge
enum class ColourCharge { Red, Green, Blue, AntiRed, AntiGreen, AntiBlue };

template <>
std::map<QuarkType, std::map<std::string, std::string>> GenericParticle<QuarkType>::m_staticProps{
	{QuarkType::UpQuark,      {{"name", "Up Quark"}, {"mass", "2.2"}, {"charge", "+2/3"}, {"spin", "0.5"}}},
	{QuarkType::DownQuark,    {{"name", "Down Quark"}, {"mass", "4.7"}, {"charge", "-1/3"}, {"spin", "0.5"}}},
	{QuarkType::StrangeQuark, {{"name", "Strange Quark"}, {"mass", "96"}, {"charge", "-1/3"}, {"spin", "0.5"}}},
	{QuarkType::CharmQuark,   {{"name", "Charm Quark"}, {"mass", "1280"}, {"charge", "+2/3"}, {"spin", "0.5"}}},
	{QuarkType::TopQuark,     {{"name", "Top Quark"}, {"mass", "173100"}, {"charge", "+2/3"}, {"spin", "0.5"}}},
	{QuarkType::BottomQuark,  {{"name", "Bottom Quark"}, {"mass", "4180"}, {"charge", "-1/3"}, {"spin", "0.5"}}}
};

// Specific Quark class deriving from GenericParticle
class Quark : public GenericParticle<QuarkType> {
protected:
    ColourCharge m_colourCharge;
public:
	// Parameterized constructor
	Quark(QuarkType type, ColourCharge colour, std::shared_ptr<FourMomentum> fourMomentum, bool isAntiParticle = false)
		: GenericParticle<QuarkType>(type, fourMomentum, isAntiParticle), m_colourCharge(colour) {
			m_baryonNumber = isAntiParticle ? -1.0 / 3 : 1.0 / 3;
	}

	// Copy constructor
	Quark(const Quark& other)
		: GenericParticle<QuarkType>(other), m_colourCharge(other.m_colourCharge) {
	}

	// Move constructor
	Quark(Quark&& other) noexcept
		: GenericParticle<QuarkType>(std::move(other)), m_colourCharge(std::move(other.m_colourCharge)) {
	}

	// Destructor
	virtual ~Quark() {}

	// Copy assignment operator
	Quark& operator=(const Quark& other) {
		if(this != &other) {
			GenericParticle<QuarkType>::operator=(other);
			m_colourCharge = other.m_colourCharge;
		}
		return *this;
	}

	// Move assignment operator
	Quark& operator=(Quark&& other) noexcept {
		if(this != &other) {
			GenericParticle<QuarkType>::operator=(std::move(other));
			m_colourCharge = std::move(other.m_colourCharge);
		}
		return *this;
    }

    virtual std::string getInfo() const override {
			std::string baseInfo = GenericParticle::getInfo();
			std::stringstream ss;
			ss<<baseInfo
				<<", Colour Charge="<<colourChargeToString(m_colourCharge)
				<<", Baryon Number="<<(m_isAntiParticle ? "-1/3" : "+1/3");
			return ss.str();
    }

    ColourCharge getColourCharge() const {
      return m_colourCharge;
    }

    double getBaryonNumber() const {
      return m_baryonNumber;
    }

		// Get the antiparticle of the quark
    std::shared_ptr<Particle> getAntiParticle() const override {
			// Quark antiparticle with opposite colour charge
			ColourCharge colour; 
			if(m_colourCharge == ColourCharge::Red) 
				colour = ColourCharge::AntiRed;		
			else if(m_colourCharge == ColourCharge::Green)			
				colour = ColourCharge::AntiGreen;
			else if(m_colourCharge == ColourCharge::Blue)
				colour = ColourCharge::AntiBlue;
			else if(m_colourCharge == ColourCharge::AntiRed)
				colour = ColourCharge::Red;
			else if(m_colourCharge == ColourCharge::AntiGreen)		
				colour = ColourCharge::Green;
			else if(m_colourCharge == ColourCharge::AntiBlue)
				colour = ColourCharge::Blue;

			auto antiQuark = std::make_shared<Quark>(m_type, colour, m_fourMomentum, !m_isAntiParticle);
			return std::static_pointer_cast<Particle>(antiQuark);
    }

	// Static method to convert colour charge to string
	static std::string colourChargeToString(ColourCharge colour) {
		switch (colour) {
			case ColourCharge::Red: return "Red";
			case ColourCharge::Green: return "Green";
			case ColourCharge::Blue: return "Blue";
			case ColourCharge::AntiRed: return "AntiRed";
			case ColourCharge::AntiGreen: return "AntiGreen";
			case ColourCharge::AntiBlue: return "AntiBlue";
			default: return "Unknown";
		}
	}
};

#endif // QUARKS_HPP