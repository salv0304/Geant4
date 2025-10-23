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

DetectorConstruction::DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();

  // Build materials
  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");
       // There is no need to test if materials were built/found
       // as G4NistManager would issue an error otherwise
       // Try the code with "XYZ".
  G4Material *gold = nistManager->FindOrBuildMaterial("G4_Au");
  //G4Material* detMat = nistManager->FindOrBuildMaterial("G4_AIR");
  //detMat
  G4Element *Zn = nistManager->FindOrBuildElement("Zn");
  G4Element *S = nistManager->FindOrBuildElement("S");
  G4Material *detMat = new G4Material("ZnS", 4.09*g/cm3, 2);
  detMat->AddElement(Zn, 1);
  detMat->AddElement(S, 1);

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double hx = 10*cm;
  G4double hy = 3.*cm;
  G4double hz = 10*cm;

  // world volume
  G4Box* worldS = new G4Box("World", hx, hy, hz);

  G4LogicalVolume* worldLV
    = new G4LogicalVolume(worldS, air, "World");

  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      worldLV,               //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  //
  // Gold Foil
  //
  G4Box *foilSolid = new G4Box("Foil", 1*cm, 1*cm, 1*cm/10000);
  
  G4LogicalVolume *foilLogic = new G4LogicalVolume(foilSolid, gold, "Foil");
  

  G4VPhysicalVolume* foilPhysical  
    = new G4PVPlacement(0,                   //rotation
                      G4ThreeVector(),       //at (0,0,0)
                      foilLogic,             //its logical volume
                      "Foil",               //its name
                      worldLV,              //its mother  volume
                      false,                //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
//
// Detector 
//
  G4double rMin = 0.05*m;
  G4double rMax = 0.0505*m;
  G4double z = 0.02*m;
  G4double startPhi = 0.*deg;
  G4double endPhi = 360.*deg;  
  auto rotX90 = new G4RotationMatrix();	// Rotation
  rotX90->rotateX(90.*deg);
  
  G4Tubs *detectorSolid = new G4Tubs("Detector", rMin, rMax, z, startPhi, endPhi); 
  G4LogicalVolume *detectorLogic = new G4LogicalVolume(detectorSolid, detMat, "Detector"); 
  G4VPhysicalVolume* detectorPhysical = 
  new G4PVPlacement(rotX90, // rotation
  					G4ThreeVector(), //at (0,0,0) 
  					detectorLogic, //its logical volume 
  				    "Detector", //its name 
  				    worldLV, //its mother volume 
  				    false, //no boolean operation 
  				    0, //copy number 
  				    checkOverlaps); //overlaps checking
  //always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
