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

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "Randomize.hh"

namespace RUT
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
	fParticleGun = new G4ParticleGun(1);	// n of events
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
  //

  // Define particle properties
  G4String particleName = "mu-";
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = particleTable->FindParticle(particleName);
  
  if ( ! particleDefinition ) {
    G4cerr << "Error: " << particleName << " not found in G4ParticleTable" << G4endl;
    exit(1);
  }
 
  
  // Initial position
  //G4ThreeVector position(0, 0.95*m, 0);
  G4double x = (G4UniformRand() - 0.5) * 0.12*m;
  G4double z = (G4UniformRand() - 0.5) * 1.2*m;
  G4double y = 0.55*m;
  
  G4ThreeVector position(x, y, z);

  
  // Direction is a distribution! cos^2(theta)
  G4ThreeVector direction(0, -1, 0);
  //G4double cosTheta = std::pow(G4UniformRand(), 1.0/3.0);
  //G4double theta = std::acos(cosTheta);
  //G4double phi = G4UniformRand() * 2.0 * CLHEP::pi;    
  // flight direction
  //G4double sinTheta = std::sin(theta);
  //G4ThreeVector direction(sinTheta * std::cos(phi), -cosTheta, sinTheta * std::sin(phi));
  
  
  // Energy
  G4double kineticEnergy = 10*GeV;  // Energy

  // Set gun  
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticlePosition(position);
  fParticleGun->SetParticleMomentumDirection(direction);
  fParticleGun->SetParticleEnergy(kineticEnergy); 
  
  fParticleGun->GeneratePrimaryVertex(event);

	

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

