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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

namespace RUT
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// The constructor
DetectorConstruction::DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// The destructor
DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Construct method
G4VPhysicalVolume* DetectorConstruction::Construct()
{
// Define materials
	//Air
	G4NistManager *nist = G4NistManager::Instance();
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");	
	//Aluminium
	G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
	//Lead
	G4Material *Pb = nist->FindOrBuildMaterial("G4_Pb");
	//Polystyrene
	G4Material *Polystyrene = nist->FindOrBuildMaterial("G4_POLYSTYRENE");

//
//World	
//
	G4Box *solidWorld = new G4Box("World", 10*m, 10*m , 10*m);
	
	// (the solid, the volume, the name)
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	
	//(rotation, position, logic_volume, name, mother_volume, boolean operations, copy_number, check_overlaps)
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

/*
//                     
// Cryostat
//
	G4double rMin = 1.15*m;
	G4double rMax = 2.25*m;
	G4double z = 6.8*m;
	G4double startPhi = 0.*deg;
	G4double endPhi = 360.*deg;
	
	G4Tubs *solidCryostat = new G4Tubs("Cryostat", rMin, rMax, z, startPhi, endPhi);
	
	// (the solid, the volume, the name)
	G4LogicalVolume *logicCryostat = new G4LogicalVolume(solidCryostat, Al, "logicCryostat");
	
	//(rotation, position, logic_volume, name, mother_volume, boolean operations, copy_number, check_overlaps)
	G4VPhysicalVolume *physCryostat = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicCryostat, "physCryostat", logicWorld, false, 0, true);
*/


	// Parameters
	G4int Nlayers = 10;	// N layers
	G4double PbrDelta = 2*mm;	// Pb thickness
	G4double PolrDelta = 3*mm;	// Pol thickness
	
	G4double r_min = 5.*cm;	// starting radius position 
	G4double layer_halfZ = 6*m;	// z dimension
	G4double startPhi = 0.*deg;	// phi angle start
	G4double endPhi = 360.*deg;	// phi angle end
	
	// for loop since we need N layers
	for (G4int i = 0; i < Nlayers; i++) {

    // --- Pb layer ---
    G4double Pb_rMin = r_min;
    G4double Pb_rMax = Pb_rMin + PbrDelta;
	
	// Solid Vol
    G4Tubs *solidPbLay = new G4Tubs("PbLay", Pb_rMin, Pb_rMax, layer_halfZ, startPhi, endPhi);
	
	// Log Vol (the solid, the volume, the name)
	G4LogicalVolume *logicPbLay = new G4LogicalVolume(solidPbLay, Pb, "logicPbLay");
	
	// Phys Vol(rotation, position, logic_volume, name, mother_volume, boolean operations, copy_number, check_overlaps)
    new G4PVPlacement(0,
                      G4ThreeVector(0., 0., 0.),
                      logicPbLay,
                      "physPbLay",
                      logicWorld,
                      false,
                      i,  // copy number
                      true);

    // Update r_min 
    r_min = Pb_rMax;

    // --- Polystyrene layer ---   
    G4double Pol_rMin = r_min;
    G4double Pol_rMax = Pol_rMin + PolrDelta;
    
	// Solid Volume
	G4Tubs *solidPolLay = new G4Tubs("PolLay", Pol_rMin, Pol_rMax, layer_halfZ, startPhi, endPhi);

	// Logic Vol (the solid, the volume, the name)
	G4LogicalVolume *logicPolLay = new G4LogicalVolume(solidPolLay, Polystyrene, "logicPolLay");

	// Phys Vol (rotation, position, logic_volume, name, mother_volume, boolean operations, copy_number, check_overlaps)
    new G4PVPlacement(0,
                      G4ThreeVector(0., 0., 0.),
                      logicPolLay,
                      "physPolLay",
                      logicWorld,
                      false,
                      i,
                      true);

    // Update r_min 
    r_min = Pol_rMax;
}

	
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
