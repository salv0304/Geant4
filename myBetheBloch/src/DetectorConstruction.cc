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
//
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "G4UserLimits.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"


namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  // Box parameters
  G4double slice_sizeXY = 10*cm, slice_sizeZ = 10*cm;
  G4Material* slice_mat = nist->FindOrBuildMaterial("G4_Pb");
  G4cout << "Material: " << slice_mat->GetName()
       << " density(g/cm3)=" << slice_mat->GetDensity()/(g/cm3) << G4endl;

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXY = 10*m;
  G4double world_sizeZ  = 10*m;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  auto solidWorld = new G4Box("World",                           // its name
    0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
    world_mat,                                       // its material
    "World");                                        // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                           // at (0,0,0)
    logicWorld,                                // its logical volume
    "World",                                   // its name
    nullptr,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    checkOverlaps);                            // overlaps checking

  //
  // slice (Sensitive detector)
  //
  G4Box *solidSlice = new G4Box("Slice",                    // its name
    0.5 * slice_sizeXY, 0.5 * slice_sizeXY, 0.5 * slice_sizeZ);  // its size
    
	
  //Now the logical volume, but since it has to be sensitive, i need to add the definition of the logical volume in the .hh file, since i need it also outside of the Construct method.
  logicSlice = new G4LogicalVolume(solidSlice, slice_mat, "logicSlice");
	
  //Array of detectors
  for(G4int i = 0; i < 200; i++)
  {
    G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(0, 0, -0.5*m+(i+0.5)*m/100), logicSlice, "physSlice", logicWorld, false, i, true);	// each detector has a different index
  }
  
/*
  G4LogicalVolume *logicSlice = new G4LogicalVolume(solidSlice,  // its solid
    slice_mat,                                     // its material
    "Slice");                                 // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    logicSlice,                 // its logical volume
    "Slice",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/    
    return physWorld;
}

// For sensitive detector (include the SensitiveDetector.hh file)
void DetectorConstruction::ConstructSDandField()
{
	MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
	
	// Tell to the logicVolume that this is a sensitive detector
	logicSlice->SetSensitiveDetector(sensDet);
	
  // Nel tuo DetectorConstruction.cc, dopo aver definito il materiale piombo:
  G4double maxStep = 0.1*mm;  // Step massimo di 0.1 mm
  G4UserLimits* stepLimit = new G4UserLimits(maxStep);
  logicSlice->SetUserLimits(stepLimit);  // applica al tuo volume sensibile
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
