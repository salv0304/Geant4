//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4AnalysisManager.hh"

namespace RUT
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
{
	fParticleGun = new G4ParticleGun(1);	// n of events
	fDetector = detector;	// Initialize the detector
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //this function is called at the begining of each event

  // Define particle properties. I use for simplicity only mu- instead of mu- and mu+... properties are pratically the same (since there's not a magnetic field) 
  // When studying mu decay, generate mu+ that is more probable to observe decay!!!! (see why)
  G4String particleName = "mu-";
  //G4String particleName = "mu+";
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = particleTable->FindParticle(particleName);
  
  if ( ! particleDefinition ) 
  {
    G4cerr << "Error: " << particleName << " not found in G4ParticleTable" << G4endl;
    exit(1);
  }
 
  
//---- Initial position-----
  
  // Generate events on the plane just above the surface of the first scintillator
  G4double x = (G4UniformRand() - 0.5) * 0.12*m;
  G4double z = (G4UniformRand() - 0.5) * 1.2*m;
  G4double yOffset = fDetector->GetYOffset();	  // Access to the yoffset
  G4double y = 52*cm + yOffset; //Initial superior surface + offset 
  
  
  // To study the acceptance vs angle, I don't translate the source with the scintillator,
  // instead I just set the source in a higher position (and a wider plane) and generate the events... then the detector will be rotated by a script, without directly changing the position of the source
  //G4double x = (G4UniformRand() - 0.5) * 2.5*m;
  //G4double z = (G4UniformRand() - 0.5) * 2.5*m;
  //G4double y = 10*cm;
  
  G4ThreeVector position(x, y, z);

  
// ------------- Direction -----------------

	// MC method using the inversion of CDF
	// The probability distribution of choosing a direction in dtheta (dN/dtheta) is proportional to cos^2(theta)*sin(theta)
	// This is the case since dOmega = sin(theta)dthetadphi
	// The CDF turns out to be (1-cos^3(theta))-> inverting it
	G4double u = G4UniformRand();
	G4double cosTheta = std::pow(1.0 - u, 1.0/3.0);
  G4double theta = std::acos(cosTheta);
  // Isotropic in phi
  G4double phi = G4UniformRand() * 2.0 * CLHEP::pi; 
  
     
  // Direction vector in cartesian coordinate 
  G4double sinTheta = std::sin(theta);
  G4ThreeVector direction(sinTheta * std::cos(phi), -cosTheta, sinTheta * std::sin(phi));
  //G4ThreeVector direction(0, -1, 0);
  
//---------- Energy------------------

	// Assume for the energy spectrum a modified power law: (dN/dE) proportional to (E + epsilon)^-gamma
	// Typical values are epsilon = 3.7 GeV and gamma = 2.7	
	// The inverse CDF is obtained by defining X = (E + epsilon)^(1 − gamma)
	// and sampling X linearly between Xmin and Xmax. The final energy is: E = X^(1 / (1 − gamma)) − epsilon    
	G4double Emin = 0.5;   // GeV
	G4double Emax = 100.0; // GeV
	G4double gamma = 2.7;  // Spectral index
	G4double epsilon = 3.7; // GeV (Shift parameter to approximate energy loss)

	// Calculate Xmin and Xmax (extremes of integration)
	G4double termMin = std::pow(Emin + epsilon, 1.0 - gamma);
	G4double termMax = std::pow(Emax + epsilon, 1.0 - gamma);

	G4double u_energy = G4UniformRand();

	// Inverse transform (E + epsilon)^-gamma
	G4double E_val = std::pow(termMin + u_energy * (termMax - termMin), 1.0 / (1.0 - gamma)) - epsilon;

	G4double kineticEnergy = E_val * GeV;
	
	// For muon decay analysis, since we have a slab of Pb with only 30 cm of thickness, we generate muons at low energy
  //G4double kineticEnergy = 150*MeV; 


// ------------------ Set gun ---------------------------
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticlePosition(position);
  fParticleGun->SetParticleMomentumDirection(direction);
  fParticleGun->SetParticleEnergy(kineticEnergy); 
  
  fParticleGun->GeneratePrimaryVertex(event);

/*
	// DEBUG
	G4cout << "Debug: Event generated in position = " << position/cm << " mm" << G4endl;
	G4cout << "Debug: With dircetion " << direction << G4endl;
	G4cout << "Debug: With energy " << kineticEnergy/GeV << " GeV"<< G4endl;
*/	


// ==== Popolate the histos ====

	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	// Initial energy
	man->FillNtupleDColumn(5, kineticEnergy/GeV);
	
	// Initial position
	man->FillNtupleDColumn(6, position.x()/cm);
	man->FillNtupleDColumn(7, position.y()/cm);
	man->FillNtupleDColumn(8, position.z()/cm);
	
	// Slab distance
	// Import detector construction class to access the slab distance
	// Notice that when you're scanning the angle slab_distance is wrong since is not taking the right position that I chose for the angle scanning	
	G4double slab_distance = fDetector->GetSlabDistance();
	//G4cout << "Debug: slab distance " << slab_distance/cm << " cm"<< G4endl;
	man->FillNtupleDColumn(9, slab_distance/cm);
	

	
	// Flight direction
	man->FillNtupleDColumn(10, theta);
	man->FillNtupleDColumn(11, phi);
	
	// Detector Rotation
	G4double rotation = fDetector->GetAngle();
	//G4cout << "Debug: rot angle " << rotation/deg << " deg"<< G4endl;
	man->FillNtupleDColumn(17, rotation);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

