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
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "DetectorConstruction.hh"

class G4Event;

// The first primary generator action class.
// G4VUserPrimaryGeneratorAction is one of the mandatory classes  (along with G4VUserDetectorConstruction and G4VUserPhysicsList) from which you derive your concrete class PrimaryGeneratorAction where you set the properties of the source.

namespace RUT
{

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
  // I pass to the constructor the pointer to the DetectorConstruction, so it can access fYOffset
  // I need to do this since I want to translate the source along with the scintillator, in this way I'm sure to have a larger number of events that cross the detector... if I only set the source with a set distance from the detector I would need an increasing number of runs as the scintillator is translated away from the source, and since the detector acceptance is small, translating the source is for me the best option.
    PrimaryGeneratorAction(DetectorConstruction* detector); 
    ~PrimaryGeneratorAction() override;

    // method from the base class
    void GeneratePrimaries(G4Event*) override;
    
    
  private:
    G4ParticleGun *fParticleGun;
    DetectorConstruction* fDetector;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


