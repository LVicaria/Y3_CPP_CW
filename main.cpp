// Project-2 - Luca Vicaria - PHYS30762
// This file is the main entry point of the particle simulation project.
// It initialises particles, sets up the catalogue, and manages the interaction loop.
// Last modified 08/05/2024

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <limits>
#include <algorithm>  
#include <cstdlib> 

#include "particle.hpp"
#include "leptons.hpp"
#include "quarks.hpp"
#include "bosons.hpp"
#include "four_momentum.hpp"

// Function to set the console text colour for output, user input, and reset to default
#ifdef _WIN32
#include <windows.h>  // Include Windows.h for Windows API functions
#else
#include <unistd.h>   // For isatty()
#endif

void setConsoleColour(unsigned int colour) {
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	switch(colour) {
		case 1: // Output (green text)
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case 2: // User input (yellow text)
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case 0: // Reset to default (white)
		default:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

#else
	if(isatty(STDOUT_FILENO)) { // Check if standard output is a terminal
		switch(colour) {
			case 1: // Green
					std::cout << "\033[32m";
					break;
			case 2: // Yellow
					std::cout << "\033[33m";
					break;
			case 0: // Reset
			default:
					std::cout << "\033[0m";
		}
	}
#endif
}

// Function to count the number of particles of a given type in the catalogue without using size() method
int countParticleType(const std::map<std::string, std::unique_ptr<Particle>>& catalogue, const std::string& type) {
	int count = 0;
	for(const auto& pair : catalogue) {
		if(pair.second->getType() == type)
			++count;
	}
	return count;
}

// Function to create a sub container of particles of a given type in the catalogue
std::vector<std::shared_ptr<Particle>> getParticlesOfType(const std::map<std::string, std::unique_ptr<Particle>>& catalogue, const std::string& type) {
	std::vector<std::shared_ptr<Particle>> particles;
	for(const auto& pair : catalogue) {
		if(pair.second->getType() == type) {
			// Temporarily create a shared_ptr from the raw pointer managed by unique_ptr
			std::shared_ptr<Particle> temp_shared_ptr(pair.second.get(), [](Particle*){});
			particles.push_back(temp_shared_ptr);
		}
	}
	return particles;
}

// Function to sum the four-momenta of all particles in the catalogue
FourMomentum sumFourMomenta(const std::map<std::string, std::unique_ptr<Particle>>& catalogue) {
	FourMomentum totalMomentum;
	for(const auto& pair : catalogue) {
		totalMomentum = totalMomentum + *(pair.second->getFourMomentum());
	}
	return totalMomentum;
}

// Main interactive loop to display particle information and allow user to query specific particles
void loop(std::map<std::string, std::unique_ptr<Particle>>& particleCatalogue) {
	std::string input;
	setConsoleColour(1); // Set text colour to green for output
	std::cout<<"Particle Information Catalogue\n\n";
	std::cout<<"Available particles: all particles within the standard model"<<std::endl;
	std::cout<<"Contains: "<<particleCatalogue.size()<<" particles."<<std::endl;

	// find number of leptons/ quarks/ bosons
	std::cout<<"Number of each particle type:"<<std::endl;
	auto sub_container_leptons = getParticlesOfType(particleCatalogue, "Lepton");  // shallow copy of the catalogue map into a vector containing only shared pointers to the leptons
	std::cout<<"Leptons: "<<sub_container_leptons.size()<<std::endl;

	auto sub_container_quarks = getParticlesOfType(particleCatalogue, "Quark");
	std::cout<<"Quarks: "<<sub_container_quarks.size()<<std::endl;

	auto sub_container_bosons = getParticlesOfType(particleCatalogue, "Boson");
	std::cout<<"Bosons: "<<sub_container_bosons.size()<<std::endl;

	// sum of four momenta of all particles in the catalogue
	FourMomentum totalMomentum = sumFourMomenta(particleCatalogue);
	std::cout<<"Total four-momentum of all particles: "<<totalMomentum.print_four_momentum()<<std::endl<<'\n';

	std::cout<<"All particle information:\n"<<std::endl;
	setConsoleColour(0); // Reset colour to default before printing all particles

	// Lambda function to print all particles in the catalogue
	auto printAllParticles = [&particleCatalogue]() {
		for(const auto& pair : particleCatalogue) {
			pair.second->print();  // Using print() which internally calls getInfo()
		}
	};

	// Invoke the lambda to print all particle details
	printAllParticles();

	while(true) {
		setConsoleColour(2); // Set text colour to yellow for user input
		std::cout<<"\nEnter a particle name to get it's information or 'quit' to exit: ";
		setConsoleColour(0); // Reset to default before reading input to avoid colouring input text
		std::getline(std::cin, input);

		std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c){ return std::tolower(c); });

		if(input == "quit")
			break;

		auto it = particleCatalogue.find(input);
		setConsoleColour(1); // Set text colour to green for output
		if(it != particleCatalogue.end()) {
			std::cout<<std::endl; 
			it->second->print();
			std::cout<<std::endl;  // Ensure there's a clear separation after the interaction
		} else {
			std::cout<<"\nParticle not found. Please try again.\n";
		}
		setConsoleColour(0); // Reset colour at the end of the loop
	}

	setConsoleColour(1); 
	std::cout<< "\nExiting Particle Catalogue.\n";
	setConsoleColour(0); // Reset the console colour to default before exiting
}

// Function to initialize the particle catalogue with all particles in the standard model
std::map<std::string, std::unique_ptr<Particle>> initialiseParticles() {
	std::map<std::string, std::unique_ptr<Particle>> particleCatalogue;  // STL map template with string key and unique_ptr value

	particleCatalogue["electron"] = std::make_unique<Electron>(std::make_shared<FourMomentum>(0.511, 0, 0, 0)); // Create an electron with energy 0.511 MeV
	particleCatalogue["muon"] = std::make_unique<Muon>(std::make_shared<FourMomentum>(105.66, 0, 0, 0)); 
	particleCatalogue["tau"] = std::make_unique<Tau>(std::make_shared<FourMomentum>(1776.8, 0, 0, 0));   
	particleCatalogue["electron neutrino"] = std::make_unique<Neutrino>(NeutrinoType::ElectronNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), false, false);
	particleCatalogue["muon neutrino"] = std::make_unique<Neutrino>(NeutrinoType::MuonNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), false, false);
	particleCatalogue["tau neutrino"] = std::make_unique<Neutrino>(NeutrinoType::TauNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), false, false);
	
	particleCatalogue["anti-electron"] = std::make_unique<Electron>(std::make_shared<FourMomentum>(0.511, 0, 0, 0), true);
	particleCatalogue["anti-muon"] = std::make_unique<Muon>(std::make_shared<FourMomentum>(105.66, 0, 0, 0), true);
	particleCatalogue["anti-tau"] = std::make_unique<Tau>(std::make_shared<FourMomentum>(1776.8, 0, 0, 0), true);
	particleCatalogue["anti-electron neutrino"] = std::make_unique<Neutrino>(NeutrinoType::ElectronNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), true, false);
	particleCatalogue["anti-muon neutrino"] = std::make_unique<Neutrino>(NeutrinoType::MuonNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), true, false);
	particleCatalogue["anti-tau neutrino"] = std::make_unique<Neutrino>(NeutrinoType::TauNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), true, false);

	particleCatalogue["upquark"] = std::make_unique<Quark>(QuarkType::UpQuark, ColourCharge::Red, std::make_shared<FourMomentum>(2.2, 0, 0, 0));
	particleCatalogue["downquark"] = std::make_unique<Quark>(QuarkType::DownQuark, ColourCharge::Blue, std::make_shared<FourMomentum>(4.7, 0, 0, 0));
	particleCatalogue["strangequark"] = std::make_unique<Quark>(QuarkType::StrangeQuark, ColourCharge::Green, std::make_shared<FourMomentum>(96, 0, 0, 0));
	particleCatalogue["charmquark"] = std::make_unique<Quark>(QuarkType::CharmQuark, ColourCharge::Red, std::make_shared<FourMomentum>(1280, 0, 0, 0));
	particleCatalogue["topquark"] = std::make_unique<Quark>(QuarkType::TopQuark, ColourCharge::Blue, std::make_shared<FourMomentum>(173100, 0, 0, 0));
	particleCatalogue["bottomquark"] = std::make_unique<Quark>(QuarkType::BottomQuark, ColourCharge::Green, std::make_shared<FourMomentum>(4180, 0, 0, 0));

	particleCatalogue["anti-upquark"] = std::make_unique<Quark>(QuarkType::UpQuark, ColourCharge::AntiRed, std::make_shared<FourMomentum>(2.2, 0, 0, 0), true);
	particleCatalogue["anti-downquark"] = std::make_unique<Quark>(QuarkType::DownQuark, ColourCharge::AntiBlue, std::make_shared<FourMomentum>(4.7, 0, 0, 0), true);
	particleCatalogue["anti-strangequark"] = std::make_unique<Quark>(QuarkType::StrangeQuark, ColourCharge::AntiGreen, std::make_shared<FourMomentum>(96, 0, 0, 0), true);
	particleCatalogue["anti-charmquark"] = std::make_unique<Quark>(QuarkType::CharmQuark, ColourCharge::AntiRed, std::make_shared<FourMomentum>(1280, 0, 0, 0), true);
	particleCatalogue["anti-topquark"] = std::make_unique<Quark>(QuarkType::TopQuark, ColourCharge::AntiBlue, std::make_shared<FourMomentum>(173100, 0, 0, 0), true);
	particleCatalogue["anti-bottomquark"] = std::make_unique<Quark>(QuarkType::BottomQuark, ColourCharge::AntiGreen, std::make_shared<FourMomentum>(4180, 0, 0, 0), true);

	particleCatalogue["photon"] = std::make_unique<Photon>(std::make_shared<FourMomentum>(0, 0, 0, 0));
	particleCatalogue["Wboson"] = std::make_unique<WBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0));
	particleCatalogue["Zboson"] = std::make_unique<ZBoson>(std::make_shared<FourMomentum>(91190, 0, 0, 0));
	particleCatalogue["gluon"] = std::make_unique<Gluon>(std::make_shared<FourMomentum>(0, 0, 0, 0), ColourCharge::Red, ColourCharge::AntiRed);
	particleCatalogue["higgs boson"] = std::make_unique<HiggsBoson>(std::make_shared<FourMomentum>(125110, 0, 0, 0));

	particleCatalogue["anti-Wboson"] = std::make_unique<WBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0), true);  // True specifies it's an antiparticle
	particleCatalogue["anti-gluon"] = std::make_unique<Gluon>(std::make_shared<FourMomentum>(0, 0, 0, 0), ColourCharge::AntiRed, ColourCharge::Red);  // Swapped colors for antiparticle
	// No antiparticle for Z boson, Higgs boson and photon

	return particleCatalogue;
}

void createAndPrintParticleDecays(std::map<std::string, std::unique_ptr<Particle>>& particleCatalogue) {

// create a tau which decays into a lepton, lepton neutrino, and tau neutrino
auto tau1 = std::make_unique<Tau>(std::make_shared<FourMomentum>(1776.8, 0, 0, 0));
try {
// Set a valid set of decay particles
	tau1->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<Electron>(std::make_shared<FourMomentum>(0.511, 0, 0, 0))), 
		std::static_pointer_cast<Particle>(std::make_shared<Neutrino>(NeutrinoType::ElectronNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), true, false)),
		std::static_pointer_cast<Particle>(std::make_shared<Neutrino>(NeutrinoType::TauNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), false, false))
	});

	// print the tau information
	std::cout<<"\nCreating a tau which decays into a lepton, lepton neutrino, and tau neutrino and printing it's information:"<<std::endl;
	tau1->print();
}
catch(const std::exception& e) {
	std::cerr<<"Error: "<<e.what()<<'\n';
}

// create a tau which decays into two quarks and a tau neutrino
auto tau2 = std::make_unique<Tau>(std::make_shared<FourMomentum>(1776.8, 0, 0, 0));
try {
	// Set a valid set of decay particles
	tau2->setDecayParticles({
		std::make_shared<Quark>(QuarkType::UpQuark, ColourCharge::AntiRed, std::make_shared<FourMomentum>(2.2, 0, 0, 0), true),
		std::make_shared<Quark>(QuarkType::DownQuark, ColourCharge::Blue, std::make_shared<FourMomentum>(4.7, 0, 0, 0)),
		std::make_shared<Neutrino>(NeutrinoType::TauNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), false, false)
	});

	std::cout<<"\nCreating a Tau which decays into two quarks and a tau neutrino and printing it's information:"<<std::endl;
	tau2->print();
}
catch(const std::exception& e) {
	std::cerr<<"Error: "<<e.what()<<'\n';
}

// Create a W boson which decays into a quark and an anti-quark
auto wboson1 = std::make_unique<WBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0));
try {
	// Set an valid set of decay particles
	wboson1->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::UpQuark, ColourCharge::Red, std::make_shared<FourMomentum>(2.2, 0, 0, 0))),
		std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::DownQuark, ColourCharge::Blue, std::make_shared<FourMomentum>(4.7, 0, 0, 0), true)),
	});

	std::cout<<"\nCreating a W Boson which decays into a quark and an anti-quark and printing it's information:"<<std::endl;
	wboson1->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// Create a W boson which decays into a lepton and a neutrino
auto wboson2 = std::make_unique<WBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0));
try {
	// Set an valid set of decay particles
	wboson2->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<Electron>(std::make_shared<FourMomentum>(0.511, 0, 0, 0), true)), 
		std::static_pointer_cast<Particle>(std::make_shared<Neutrino>(NeutrinoType::ElectronNeutrino, std::make_shared<FourMomentum>(0, 0, 0, 0), false, false)),
	});

	std::cout<<"\nCreating a W Boson which decays into a lepton and a neutrino and printing it's information:"<<std::endl;
	wboson2->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// Create a Z boson which decays into a quark and an anti-quark
auto zboson1 = std::make_unique<ZBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0));
try {
	// Set an valid set of decay particles
	zboson1->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::UpQuark, ColourCharge::Red, std::make_shared<FourMomentum>(2.2, 0, 0, 0))),
		std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::UpQuark, ColourCharge::AntiRed, std::make_shared<FourMomentum>(4.7, 0, 0, 0), true)),
	});

	std::cout<<"\nCreating a Z Boson which decays into a quark and an anti-quark and printing it's information:"<<std::endl;
	zboson1->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// Create a Z boson which decays into a lepton and an anti-lepton
auto zboson2 = std::make_unique<ZBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0));
try {
	// Set an valid set of decay particles
	zboson2->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<Electron>(std::make_shared<FourMomentum>(0.511, 0, 0, 0))),
		std::static_pointer_cast<Particle>(std::make_shared<Electron>(std::make_shared<FourMomentum>(0.511, 0, 0, 0), true)),
	});

	std::cout<<"\nCreating a Z Boson which decays into a lepton and an anti-lepton and printing it's information:"<<std::endl;
	zboson2->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// // Create a Higgs boson which decays into two Z bosons
auto higgsboson1 = std::make_unique<HiggsBoson>(std::make_shared<FourMomentum>(125110, 0, 0, 0));
try {
	// Set an valid set of decay particles
	higgsboson1->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<ZBoson>(std::make_shared<FourMomentum>(91190, 0, 0, 0))),
		std::static_pointer_cast<Particle>(std::make_shared<ZBoson>(std::make_shared<FourMomentum>(91190, 0, 0, 0))),
	});

	std::cout<<"\nCreating a higgs Boson which decays into two Z bosons and printing it's information:"<<std::endl;
	higgsboson1->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// Create a Higgs boson which decays into two W bosons of opposite signs
auto higgsboson2 = std::make_unique<HiggsBoson>(std::make_shared<FourMomentum>(125110, 0, 0, 0));
try {
	// Set an valid set of decay particles
	higgsboson2->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<WBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0))),
		std::static_pointer_cast<Particle>(std::make_shared<WBoson>(std::make_shared<FourMomentum>(80360, 0, 0, 0), true)),
	});

	std::cout<<"\nCreating a higgs Boson which decays into two W bosons of opposite signs and printing it's information:"<<std::endl;
	higgsboson2->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// Create a Higgs boson which decays into two photons
auto higgsboson3 = std::make_unique<HiggsBoson>(std::make_shared<FourMomentum>(125110, 0, 0, 0));
try {
	// Set an valid set of decay particles
	higgsboson3->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<Photon>(std::make_shared<FourMomentum>(0, 0, 0, 0))),
		std::static_pointer_cast<Particle>(std::make_shared<Photon>(std::make_shared<FourMomentum>(0, 0, 0, 0))),
	});

	std::cout<<"\nCreating a higgs Boson which decays into two photons and printing it's information:"<<std::endl;
	higgsboson3->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// Create a Higgs boson which decays into a b quark and an b antiquark
auto higgsboson4 = std::make_unique<HiggsBoson>(std::make_shared<FourMomentum>(125110, 0, 0, 0));
try {
	// Set an valid set of decay particles
	higgsboson4->setDecayParticles({
		std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::BottomQuark, ColourCharge::Green, std::make_shared<FourMomentum>(4180, 0, 0, 0))),
		std::static_pointer_cast<Particle>(std::make_shared<Quark>(QuarkType::BottomQuark, ColourCharge::AntiGreen, std::make_shared<FourMomentum>(4180, 0, 0, 0), true)),
	});

	std::cout<<"\nCreating a higgs Boson which decays into a b quark and an b antiquark and printing it's information:"<<std::endl;
	higgsboson4->print();
}
catch(const std::exception& e) {
  std::cerr<<"Error: "<<e.what()<<'\n';
}

// Example of handling an invalid set of decay particles for a Tau
auto tau = std::make_unique<Tau>(std::make_shared<FourMomentum>(1776.8, 0, 0, 0));
try {
	tau->setDecayParticles({std::static_pointer_cast<Particle>(std::make_shared<Neutrino>(NeutrinoType::MuonNeutrino, std::make_shared<FourMomentum>(0.511, 0, 0, 0), true, true))});
} 
catch (const std::exception& e) {
  std::cerr<<"Handled Exception: "<<e.what()<<'\n';
}

// Example of handling a particle with an invalid four-momentum
auto fm = std::make_shared<FourMomentum>(50, 0, 0, 0); // Invalid energy
auto electron = std::make_unique<Electron>(fm);

auto valid = fm->validate(); // Call validate() on the FourMomentum object itself
if(valid) {
	std::cout<<"Four-momentum is valid."<<std::endl;
}
else
  std::cerr<<"Four-momentum is invalid. \n"<<std::endl;

// show functionality of converting particle to anti-particle
auto positron = particleCatalogue["electron"]->getAntiParticle(); 
 }

// Main function
int main() {
	// Clear the console screen
	#ifdef _WIN32
	system("cls");
	#else
	system("clear");
	#endif

	auto particleCatalogue = initialiseParticles(); // Initialise particles using a dedicated function

	loop(particleCatalogue);  // Start the interactive loop 

	createAndPrintParticleDecays(particleCatalogue);

	// // Wait for user input before exiting
	// std::cout<<"\nPress Enter to exit...";
	// std::cin.get(); // Wait for user to press enter before exiting

	return 0;
}
