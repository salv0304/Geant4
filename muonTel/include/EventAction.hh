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
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

// Event action class
// An event in G4 is a particle with all its products: in our case a muon + all the photons it generates during the detection process.
// When you want to accumulate physics quantities, you can use (primitive or custom) scorers.
// Each scorer creates a HitCollection (created by the sensitivedetector class), attached to the G4Event object.
// Then you access to these quantities in the EventAction class

namespace RUT
{

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    ~EventAction() override;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;
    
    // Accumulate energy
    void AddEdep(G4double edep) { fEdep += edep; }
    // Accumulate path
    void AddPath(G4double L) { fPath += L; }  
    
    // At the beginning of each event, set the quantities to zero invoking this function  
    void Reset() 
    {
    	fEdep = 0.;
    	fPath = 0.;
    }
    
    // For the time of flight
    void SetTOF1(G4double t) { if (fTOF_t1 < 0) fTOF_t1 = t; }
    void SetTOF3(G4double t) { if (fTOF_t3 < 0) fTOF_t3 = t; }
    G4double GetTOF() const 
    {
    	if (fTOF_t1 >= 0 && fTOF_t3 >= 0) return fTOF_t3 - fTOF_t1;
    	else return -1; // TOF not defined
    }

    
  private:
    G4int fPMTHitsCollectionID;
    G4double fEdep; 
    G4double fPath;
    G4double fTOF_t1;
    G4double fTOF_t3;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


