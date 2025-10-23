#include "construction.hh"
#include "detector.hh"

// The constructor
MyDetectorConstruction::MyDetectorConstruction()
{}

// The destructor
MyDetectorConstruction::~MyDetectorConstruction()
{}

// Construct method
G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
// Define materials
	//Air
	G4NistManager *nist = G4NistManager::Instance();
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");	
	//SiO2
	G4Material *SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2); 
	SiO2->AddElement(nist->FindOrBuildElement("Si"),1);
	SiO2->AddElement(nist->FindOrBuildElement("O"),2);
	//H2O
	G4Material *H20 = new G4Material("H20", 1.00*g/cm3, 2); 
	H20->AddElement(nist->FindOrBuildElement("H"),2);
	H20->AddElement(nist->FindOrBuildElement("O"),1);
	//C
	G4Element *C = nist->FindOrBuildElement("C");
	//AeroGel
	G4Material *Aerogel = new G4Material("Aerogel", 0.200*g/cm3,3);
	Aerogel->AddMaterial(SiO2, 62.5*perCent);
	Aerogel->AddMaterial(H20, 37.4*perCent);
	Aerogel->AddElement(C, 0.1*perCent);
	
	// define refractive index for aerogel and world for optical photons
	G4double energy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
	G4double rindexAerogel[2] = {1.1, 1.1};
	G4MaterialPropertiesTable *aptAerogel = new G4MaterialPropertiesTable();
	aptAerogel->AddProperty("RINDEX", energy, rindexAerogel, 2);
	Aerogel->SetMaterialPropertiesTable(aptAerogel);
	
	G4double rindexWorld[2] = {1.0, 1.0};
	G4MaterialPropertiesTable *aptWorld = new G4MaterialPropertiesTable();
	aptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
	worldMat->SetMaterialPropertiesTable(aptWorld);
	
//World								 	
	//	(name, hx, hy, hz)
	G4Box *solidWorld = new G4Box("solidWorld", 0.5*m, 0.5*m , 0.5*m);
	
	// (the solid, the volume, the name)
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	
	//(rotation, position, logic_volume, name, mother_volume, boolean operations, copy_number, check_overlaps)
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);


// Radiator
	G4Box *solidRadiator = new G4Box("solidRadiator", 0.4*m, 0.4*m, 0.01*m);
	
	G4LogicalVolume *logicRadiator = new G4LogicalVolume(solidRadiator, Aerogel, "logicRadiator");
	
	G4VPhysicalVolume *physRadiator = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.25*m), logicRadiator, "physRadiator", logicWorld, false, 0, true);	

//Photosensors (has to be a sensitive detector)
	G4Box *solidDetector = new G4Box("solidDetector", 0.005*m, 0.005*m, 0.01*m);
	
	//Now the logical volume, but since it has to be sensitive, i need to add the definition of the logical volume in the .hh file, since i need it also outside of the Construct method.
	logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicDetector");
	
	//Array of detectors
	for(G4int i = 0; i < 100; i++)
	{
		for(G4int j = 0; j < 100; j++)
		{
			G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(-0.5*m+(i+0.5)*m/100, -0.5*m+(j+0.5)*m/100, 0.49*m), logicDetector, "physDetector", logicWorld, false, j+i*100, true);	//j+i*100 assures that each detector has a different index; 0.49*m is the result of 0.5*m - 0.01*m
		}
	}
	return physWorld;
}

// For sensitive detector (include the detector.hh file)
void MyDetectorConstruction::ConstructSDandField()
{
	MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
	
	// Tell to the logicVolume that this is a sensitive detector
	logicDetector->SetSensitiveDetector(sensDet);
}
	
