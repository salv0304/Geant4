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
#include "SensitiveDetector.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"


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

// =========== Define materials ===============

	//Air
	G4NistManager *nist = G4NistManager::Instance();
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");	
	//Aluminium
	G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
	//Lead
	G4Material *Pb = nist->FindOrBuildMaterial("G4_Pb");
	//Polystyrene
	G4Material *Polystyrene = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
	//Glass 
	G4Material *Glass = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

	
	const G4int nEntries = 2;
	G4double photonEnergy[nEntries] = {2.0*eV, 3.5*eV};  // 350–620 nm


//============ Optical Properties ============
/*
	// refraction index for polystyrene
	G4double rIndex[nEntries] = {1.58, 1.58};
	G4double absorption[nEntries] = {5*m, 5*m};	// absorption lenght
	G4double scintEmission[nEntries] = {1.0, 1.0}; // Scint emission

	// Make table
	auto mpt = new G4MaterialPropertiesTable();
	mpt->AddProperty("RINDEX", photonEnergy, rIndex, nEntries);
	mpt->AddProperty("ABSLENGTH", photonEnergy, absorption, nEntries);
	mpt->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintEmission, nEntries);

	// Scintillation light properties (photons per MeV)
	mpt->AddConstProperty("SCINTILLATIONYIELD", 100000./MeV);
	mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
	mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.*ns);

	Polystyrene->SetMaterialPropertiesTable(mpt);


	// Glass refractive index
	G4double rindexGlass[nEntries] = {1.5, 1.5};
	G4MaterialPropertiesTable* aptGlass = new G4MaterialPropertiesTable();
	aptGlass->AddProperty("RINDEX", photonEnergy, rindexGlass, nEntries);
	Glass->SetMaterialPropertiesTable(aptGlass);
*/
//========== Build the Geometry =============

//
// World	
//
	G4Box *solidWorld = new G4Box("World", 1*m, 1*m , 1*m);
	
	// (the solid, the volume, the name)
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	
	//(rotation, position, logic_volume, name, mother_volume, boolean operations, copy_number, check_overlaps)
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

//
// Tube
// 
	G4double rMin = 0*m;
	G4double rMax = 5*cm;
	G4double z = 30*cm;
	G4double startPhi = 0.*deg;
	G4double endPhi = 360.*deg;
	
	G4Tubs* solidTube = new G4Tubs("Tube", rMin, rMax, z, startPhi, endPhi);
	G4LogicalVolume* logicTube = new G4LogicalVolume(solidTube, worldMat, "logicTube");	


// --- Physical volume ---
	new G4PVPlacement
	(
		nullptr,                   // no rotation
		G4ThreeVector(0., 0., 0.), // position
		logicTube,                 // logical volume
		"physTube",                // name
		logicWorld,                // mother
		false,                     // no boolean operation
		0,                         // copy number
		true                       // check overlaps
	);

//
// Wire
//	
	G4double rMax_Wire = 1*mm;
	
	G4Tubs* solidWire = new G4Tubs("Wire", rMin, rMax_Wire, z, startPhi, endPhi);
	G4LogicalVolume* logicWire= new G4LogicalVolume(solidWire, worldMat, "logicWire");	


// --- Physical volume ---
	new G4PVPlacement
	(
		nullptr,                   // no rotation
		G4ThreeVector(0., 0., 0.), // position
		logicWire,                 // logical volume
		"physWire",                // name
		logicTube,                 // mother
		false,                     // no boolean operation
		0,                         // copy number
		true                       // check overlaps
	);

/*
//	
// Pb slab
//
	G4Box *solidSlab = new G4Box("Slab", 5*cm, 5*cm , 25*cm);	
	G4LogicalVolume *logicSlab = new G4LogicalVolume(solidSlab, Pb, "logicSlab");
	
	G4VPhysicalVolume *physSlab = new G4PVPlacement(0, G4ThreeVector(0., -45*cm, 0.), logicSlab, "physSlab", logicWorld, false, 0, true);
*/

/*
//
// PMT (has to be a sensitive detector)
//
	G4Box* solidPMT = new G4Box("PMT", 6*cm, 1*cm, 2*cm);
	
	//Now the logical volume, but since it has to be sensitive, i need to add the definition of the logical volume in the .hh file, since i need it also outside of the Construct method.
	logicPMT = new G4LogicalVolume(solidPMT, Glass, "logicPMT");

// --- Positions ---
	std::vector<G4ThreeVector> positions_PMT = 
	{
		G4ThreeVector(0., 51*cm, 62*cm),
		G4ThreeVector(0., 51*cm, -62*cm),
		G4ThreeVector(0., -11*cm, 62*cm),
		G4ThreeVector(0., -11*cm, -62*cm),
		G4ThreeVector(0., -51*cm, 62*cm),
		G4ThreeVector(0., -51*cm, -62*cm),
	};
	
// --- Physical volumes ---
	for (size_t i = 0; i < positions_PMT.size(); ++i) 
	{
		new G4PVPlacement(
		nullptr,				// no rotation
		positions_PMT[i],		// different positions
		logicPMT,				// same logical volume
		"physPMT",				// name
		logicWorld,				// mother
		false,					// no boolean operation
		i,						// copy number
		true					// check overlaps
		);
	}
	
*/
    
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


// For sensitive detector (include the detector.hh file)
void DetectorConstruction::ConstructSDandField()
{
/*
	MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
	
	// When you use hit collections you need the sd manager
    G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);

	// Tell to the logicVolume that this is a sensitive detector
	logicPMT->SetSensitiveDetector(sensDet);
*/
}

}
