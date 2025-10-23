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
/// \file ChamberSD.cc
/// \brief Implementation of the ChamberSD class
//

#include "ChamberSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"
#include "G4ios.hh"

namespace EX3
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ChamberSD::ChamberSD(const G4String& name)
 : G4VSensitiveDetector(name)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ChamberSD::~ChamberSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ChamberSD::Initialize(G4HCofThisEvent* /*hce*/)			//Initialization, creating the collection of hits
{
  G4String hcName = SensitiveDetectorName + "HitsCollection";

  fHitsCollection
    = new ChamberHitsCollection(SensitiveDetectorName, hcName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool ChamberSD::ProcessHits(G4Step* step,
                              G4TouchableHistory* /*history*/)
{
  // Change the following lines to get the charge of the tracked particle	Only if charged particles i create a step
  G4double charge = 1.;
  if ( charge == 0. ) return false;

  // Create new hit
  ChamberHit* newHit = new ChamberHit();

  // Step info			//punctual step infos are in the prestep or poststep... here we decide to work with prestep
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  
  // Time
  G4double time = preStepPoint->GetGlobalTime();
  newHit->SetTime(time);				// Once i have defined time, I insert it in time

  // Position		//It is similar as time!
  G4ThreeVector position = preStepPoint->GetPosition();
  newHit->SetPosition(position);

  // Layer number
  G4int layer = preStepPoint->GetTouchable()->GetCopyNumber();	//G4Touchable tells you where you are in the volume hierarchy; to get he copy number you do the method GetReplicaNumber.
  newHit->SetLayer(layer);

  // Add hit in the collection
  fHitsCollection->insert(newHit);

  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ChamberSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
{
  G4cout << "\n-------->" <<  fHitsCollection->GetName()
         << ": in this event: " << G4endl;

  G4int nofHits = fHitsCollection->entries();
  for ( G4int i=0; i<nofHits; i++ ) {
    (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

