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
#include "SensitiveDetector_PMT.hh"
#include "SensitiveDetector_Scint.hh"
#include "RunAction.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "G4RunManager.hh"


namespace RUT
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// The constructor
DetectorConstruction::DetectorConstruction()
{ 
	fphysScint = nullptr;
	fphysPMT_0 = nullptr;
	fphysPMT_1 = nullptr;
	fYOffset = 0.0*cm;
	fSlabDistance = 102*cm;
	fAngle = 0.0*deg;
	
	fAllScints;
	fAllPMTs;
	
  // define commands for this class
  DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// The destructor
DetectorConstruction::~DetectorConstruction()
{ 
	delete fMessenger;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Construct method: the main method of the class
G4VPhysicalVolume* DetectorConstruction::Construct()
{

// =========== Define materials ===============

	G4NistManager *nist = G4NistManager::Instance();
	//Air
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");	
	//Aluminium
	//G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
	//Lead
	G4Material *Pb = nist->FindOrBuildMaterial("G4_Pb");
	//Polystyrene
	G4Material *Polystyrene = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
	//Glass 
	G4Material *Glass = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");


//============ Optical Properties ============

	const G4int nEntries = 2;
	G4double photonEnergy[nEntries] = {2.0*eV, 3.5*eV};  // 350–620 nm

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
	mpt->AddConstProperty("SCINTILLATIONYIELD", 10000./MeV);	// Scint yeld for Polystyrene is 10k/MeV
	mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
	mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.*ns);

	Polystyrene->SetMaterialPropertiesTable(mpt);


	// Glass refractive index
	G4double rindexGlass[nEntries] = {1.5, 1.5};
	G4MaterialPropertiesTable* aptGlass = new G4MaterialPropertiesTable();
	aptGlass->AddProperty("RINDEX", photonEnergy, rindexGlass, nEntries);
	Glass->SetMaterialPropertiesTable(aptGlass);


//========== Build the Geometry =============


// ---------- World -----------	

	G4Box *solidWorld = new G4Box("World", 2*m, 2*m , 2*m);
	
	// (the solid, the volume, the name)
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	
	//(rotation, position, logic_volume, name, mother_volume, boolean operations, copy_number, check_overlaps)
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);


// ---------- Scintillators ------------

	G4Box* solidScint = new G4Box("Scintillator", 6*cm, 1*cm, 60*cm);
	
	//Now the logical volume, but since it has to be a sensitive detector, I need to add the definition of the logical volume in the .hh file, since i need it also outside of the Construct method.
	logicScint = new G4LogicalVolume(solidScint, Polystyrene, "logicScint");
	

	// Positions 
	std::vector<G4ThreeVector> positions = 
	{
		G4ThreeVector(0., 51*cm, 0.),
		G4ThreeVector(0., -11*cm, 0.),
		G4ThreeVector(0., -51*cm, 0.)
	};

	// Physical volumes 
	for (size_t i = 0; i < positions.size(); ++i) 
	{
		G4VPhysicalVolume* physScint = new G4PVPlacement(
		nullptr,                 // no rotation
		positions[i],            // different positions
		logicScint,              // same logical volume
		"physScint",             // name
		logicWorld,              // mother
		false,                   // no boolean operation
		i,                       // copy number
		true                     // check overlaps
		);
		
  // Save as a member of the class the first scintillator
	if (i == 0) 
	{
  	fphysScint = physScint;
  	// G4cout << "Scintillator pointer saved: " << fphysScint << G4endl;
  }	
  
  // Save as a member of the class the vector of all the scintillators
  fAllScints.push_back(physScint);	
  
//	G4cout << " Debug: Constructing scintillator at = " << positions[i] << G4endl;

	}

// ------------- PMTs ------------------

	G4Box* solidPMT = new G4Box("PMT", 6*cm, 1*cm, 2*cm);
	
	//Now the logical volume, but since it has to be sensitive, i need to add the definition of the logical volume in the .hh file, since i need it also outside of the Construct method.
	logicPMT = new G4LogicalVolume(solidPMT, Glass, "logicPMT");

	//Positions
	std::vector<G4ThreeVector> positions_PMT = 
	{
		G4ThreeVector(0., 51*cm, 62*cm),
		G4ThreeVector(0., 51*cm, -62*cm),
		G4ThreeVector(0., -11*cm, 62*cm),
		G4ThreeVector(0., -11*cm, -62*cm),
		G4ThreeVector(0., -51*cm, 62*cm),
		G4ThreeVector(0., -51*cm, -62*cm),
	};
	
	//Physical volumes
	for (size_t i = 0; i < positions_PMT.size(); ++i) 
	{
		G4VPhysicalVolume* physPMT = new G4PVPlacement(
		nullptr,				// no rotation
		positions_PMT[i],		// different positions
		logicPMT,				// same logical volume
		"physPMT",				// name
		logicWorld,				// mother
		false,					// no boolean operation
		i,						// copy number
		true					// check overlaps
		);
		
		// Save as members of the class the first two PMTs
		if (i == 0) 
		{
    	fphysPMT_0 = physPMT;
		}
		if (i == 1) 
		{
    	fphysPMT_1 = physPMT;
		}
	
	// Save as a member of the class the vector containing all the PMTs
	fAllPMTs.push_back(physPMT);
	
	}  
	
/*	
// -------- Pb slab ----------

	G4Box *solidSlab = new G4Box("Slab", 6*cm, 5*cm , 60*cm);	
	G4LogicalVolume *logicSlab = new G4LogicalVolume(solidSlab, Pb, "logicSlab");
	
	G4VPhysicalVolume *physSlab = new G4PVPlacement(0, G4ThreeVector(0., -45*cm, 0.), logicSlab, "physSlab", logicWorld, false, 0, true);
*/
  
  return physWorld;
}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


// Method for assigning the properties of sensitive detector 
void DetectorConstruction::ConstructSDandField()
{
	MySensitiveDetector_PMT *sensDet_PMT = new MySensitiveDetector_PMT("SensitiveDetector_PMT");
	MySensitiveDetector_Scint *sensDet_Scint = new MySensitiveDetector_Scint("SensitiveDetector_Scint");
	
	// Tell the SD manager
  G4SDManager::GetSDMpointer()->AddNewDetector(sensDet_PMT);
	G4SDManager::GetSDMpointer()->AddNewDetector(sensDet_Scint);
	
	// Tell to the logicVolumes that this is a sensitive detector
	logicPMT->SetSensitiveDetector(sensDet_PMT);
	logicScint->SetSensitiveDetector(sensDet_Scint);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Define commands method 
void DetectorConstruction::DefineCommands()
{
// Note that in define commands you just describe the commands, it is in the setter method that you execute the command, and notify G4 that the geometry changed

	fMessenger = new G4GenericMessenger(this, "/geo/", "Detector control");
	
// ------------ YOffset command -----------
	auto& YOffsetCmd = fMessenger->DeclareMethodWithUnit("yoffset", "cm", &DetectorConstruction::SetYOffset, "Offset in Y for the first scintillator");  // use the setter 
																				
//Notice: you need to pass the setter method instead of only "fYOffset" because otherway you can't update the geometry, in fact you can't call GeometryHasBeenModified() method

	YOffsetCmd.SetParameterName("offset", true);
	YOffsetCmd.SetDefaultValue("0.");
	
// --------------- Angle command --------------
	auto& AngleCmd = fMessenger->DeclareMethodWithUnit("angle", "deg", &DetectorConstruction::SetAngle,"Theta Angle of the detector");  // use the setter 
																				
																			
	AngleCmd.SetParameterName("angle", true);
	AngleCmd.SetDefaultValue("0.");
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//Setter method for YOffset
void DetectorConstruction::SetYOffset(G4double val)
{
	// Error
	if (!fphysScint) 
	{
		G4cerr << "Detector not yet constructed." << G4endl;
		return;
	}
	
	fYOffset = val;
    
	// Change the y of the first scintillator 
	G4ThreeVector posScint = fphysScint->GetTranslation();
  posScint.setY(51*cm + fYOffset); 
  fphysScint->SetTranslation(posScint);
  
  // Change only the y of the first two PMT
	G4ThreeVector posPMT_0 = fphysPMT_0->GetTranslation();
	G4ThreeVector posPMT_1 = fphysPMT_1->GetTranslation();
  posPMT_0.setY(51*cm + fYOffset); 
  posPMT_1.setY(51*cm + fYOffset);
  fphysPMT_0->SetTranslation(posPMT_0);  
  fphysPMT_1->SetTranslation(posPMT_1); 	
  
  // Save the distance between the first and third scintillator, so I can put it in an histogram later
  fSlabDistance = 102*cm + fYOffset;
    
  // Notify the geometry
	G4RunManager::GetRunManager()->GeometryHasBeenModified();

/*	
	// Debug
	G4cout << "Debug: YOffset = " << fYOffset/cm << " cm" << G4endl;
	G4cout << "Debug: Scint position = " << fphysScint->GetTranslation()/cm << " cm" << G4endl;
	G4cout << "Debug: Slab Distance = " << fSlabDistance/cm << " cm" << G4endl;	
*/
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Setter method for the Angle
void DetectorConstruction::SetAngle(G4double val)
{
	if (!fphysScint) 
	{
		G4cerr << "Detector not yet constructed." << G4endl;
		return;
	}
	
	// Save the new angle (deg)
	fAngle = val;
	
	// Create rotation matrix
	G4RotationMatrix rotm;
	rotm.rotateZ(fAngle);  // rotation axis

	// Update scintillator placement 
	std::vector<G4ThreeVector> basePosScint = 	// Original positions: I choose to set a relative distance of 50 cm since i need a "good" acceptance
    {
			G4ThreeVector(0., -1*cm, 0.),
			G4ThreeVector(0., -11*cm, 0.),
			G4ThreeVector(0., -51*cm, 0.)
    };

	for (size_t i = 0; i < fAllScints.size(); ++i)
	{ 

    // Apply the rotation 
    G4ThreeVector posNew = rotm * basePosScint[i];
    
    // Compensation:
    // Since we want that the relative orientation of the scintillators to be the same, we need to rotate the volumes in the opposite direction
    G4RotationMatrix rotmInverse;
    rotmInverse.rotateZ(-fAngle);  // Opposite rotation

    // Update the position
    fAllScints[i]->SetTranslation(posNew);

    // Update the rotation
    G4RotationMatrix* newRot = new G4RotationMatrix(rotmInverse);
    fAllScints[i]->SetRotation(newRot);
	}

	// Update PMTs in the same way
	std::vector<G4ThreeVector> basePosPMT = 	// Original positions
	{
		G4ThreeVector(0., -1*cm, 62*cm),
		G4ThreeVector(0., -1*cm, -62*cm),
		G4ThreeVector(0., -11*cm, 62*cm),
		G4ThreeVector(0., -11*cm, -62*cm),
		G4ThreeVector(0., -51*cm, 62*cm),
		G4ThreeVector(0., -51*cm, -62*cm),
	};
	
	for (size_t i = 0; i < fAllPMTs.size(); ++i)
	{
	
    G4ThreeVector posNew = rotm * basePosPMT[i];
    
    G4RotationMatrix rotmInverse;
    rotmInverse.rotateZ(-fAngle);

		G4RotationMatrix* newRot = new G4RotationMatrix(rotmInverse);
        
    fAllPMTs[i]->SetTranslation(posNew);
    fAllPMTs[i]->SetRotation(newRot);
	}

    
  // Notify the geometry
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


}
