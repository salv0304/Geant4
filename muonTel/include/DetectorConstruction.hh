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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 

#include "G4VUserDetectorConstruction.hh"
#include "G4GenericMessenger.hh"

#include <vector>

class G4VPhysicalVolume;

// Detector construction class to define materials and geometry.
// I added two custom commands to modify the geometry of the detector. To do this, I added new members, getter and setter methods.
// This allows me to modify the geometry via external scripts, being able to study acceptance vs slab distance and theta angle.
namespace RUT
{

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

  public:
  	// Main method of the class, where you implement your geometry
    G4VPhysicalVolume* Construct() override;
    
    
    // Getter methods
    G4double GetYOffset() const { return fYOffset;} // Use in the PrimaryGeneratorAction.cc to be able to move the source with yoffset
    G4double GetSlabDistance() const { return fSlabDistance;} 
    G4double GetAngle() const { return fAngle;} // Use to be able to rotate the detector
    
  private:
  	// This are members because they need to be sensitive detectors
		G4LogicalVolume *logicPMT;
		G4LogicalVolume *logicScint;
		virtual void ConstructSDandField();
	
		// Custom commands
		void DefineCommands();
		void SetYOffset(G4double val);
		void SetAngle(G4double val);
	
		// New members
 		G4double fYOffset;	// Translation the first scintillator from the initial position
 		G4double fSlabDistance;	// Access the distance and populate the histogram
 		G4double fAngle; 	// rotate the detector
		G4GenericMessenger* fMessenger;	
		G4VPhysicalVolume* fphysScint;  // Pointer to the PV of the first scintillator
		G4VPhysicalVolume* fphysPMT_0;	// Pointer to the PV of the two first PMT
		G4VPhysicalVolume* fphysPMT_1;
		std::vector<G4VPhysicalVolume*> fAllScints;	// Vector of pointers to save all the scintillators
		std::vector<G4VPhysicalVolume*> fAllPMTs;		// Vector of pointers to save all the PMTs
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

