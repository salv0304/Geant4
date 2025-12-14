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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "PMTHit.hh"
#include "PMTPulse.hh"
#include "SensitiveDetector_Scint.hh"
#include "DetectorConstruction.hh"

#include "G4AnalysisManager.hh"
#include <map>
#include <vector>
#include <algorithm>
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4VHitsCollection.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh" 
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

namespace RUT
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction(), fPMTHitsCollectionID(-1)  // initialize
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{
	// Reset Edep and Path at the start of each event
	// (Notice that since you do this, you don't need to initialize theseFfPMT variables in the constructor
	Reset();
	
	// At the beginning of each event set the values for the fTOF
	fTOF_t1 = -1; 
	fTOF_t3 = -1;
	
	// print event ID each 100
	G4int evt = event->GetEventID();
	if (evt % 500 == 0)
		G4cout << ">>> Start event: " << evt << G4endl;
	
	// Get ID of the Hit collection
	fPMTHitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("SensitiveDetector_PMT/PMTHitsCollection");
	//G4cout << "PMT Collection ID = " << fPMTHitsCollectionID << G4endl;
    
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{ 
// Here I can get the Hit collection of each event and process the informations to fill histograms with the interested data
	
	// Get event ID	
  G4int evt = event->GetEventID();  
  	
	// Get the PMT hit collection
	G4HCofThisEvent* hce = event->GetHCofThisEvent();

  // G4HCofThisEvent can have different types of HC (depending on the number of SD); for this reason GetHC gives a generic  type... then you cast the right type
  PMTHitsCollection* pmtHC = static_cast<PMTHitsCollection*>(hce->GetHC(fPMTHitsCollectionID));
    
  if(!pmtHC) 
  {
  	//G4cout << "pmtHC is void" << G4endl;
  	return;
  }
        
  // get the number of hits 
  G4int nHits = pmtHC->entries();
 
 
// ========= Number of photons produced =========
	
	// Find the scintillator SD
	G4SDManager* sdManager = G4SDManager::GetSDMpointer();
	G4VSensitiveDetector* ScintSD = sdManager->FindSensitiveDetector("SensitiveDetector_Scint");
	
	// ScintSD is a generic pointer to  G4VSensitiveDetector, it needs to be casted to the correct derivated class
	MySensitiveDetector_Scint* hitsSD = static_cast<MySensitiveDetector_Scint*>(ScintSD);

	// Get photons number
	G4int nPhotons = hitsSD->GetPhotonCount();
	
  // Reset
  hitsSD->Reset();
  
   
/*
	// Print	
  G4cout << "Event " << event->GetEventID() << "\n"
         << ": Number of photons = " << nPhotons << "\n" 
         << "Deposited Energy = " << fEdep/MeV << "MeV \n "
         << "Lenght = " << fPath/cm << "cm "
         << G4endl;
*/
   

// ======== PULSE FINDING ========
    
	// Group hits for each PMT
	std::unordered_map<G4int, std::vector<G4double>> hitsPerPMT;	// Map the PMT ID (key type) to the photons time (value type) to group all the hit of the same PMT
    
  // For each hit in the HC get the PMTID and the time; then add to the list of the same PMT
	for(G4int i = 0; i < nHits; i++) 
  {
  	PMTHit* hit = (*pmtHC)[i];
  	
  	// Select the hits for each ID -> populate the vector with the times
  	hitsPerPMT[hit->GetPMTID()].push_back(hit->GetTime());
  }
    
  // For each PMT, create the pulses
  G4double timeWindow = 30.0 * ns;  // Define the time window
  std::vector<PMTPulse> allPulses;	// To memorize the Pulses
   
  // pair is an object that represents the key-value couple 
  for(std::pair<const G4int, std::vector<G4double>>& pair : hitsPerPMT) 
  {
  	G4int pmtID = pair.first;	// The key (ID)
    std::vector<G4double>& times = pair.second;	// The value (times)
        
    // Sort the photons in time
    std::sort(times.begin(), times.end());
        
    // Group
    if(times.size() > 0) 
    {
    	// Initialize
    	G4double pulseStartTime = times[0];
      G4int photonsInPulse = 1;
            
      for(size_t i = 1; i < times.size(); i++) 
      {
      	// if the following photon time is in the time window, add to the pulse
      	if(times[i] - pulseStartTime < timeWindow) 
      	{
        	// Same pulse
        	photonsInPulse++;
        } 
        else 
        {
        	// Otherwise, create a new pulse
          allPulses.push_back(PMTPulse(pmtID, pulseStartTime, photonsInPulse));
          pulseStartTime = times[i];
          photonsInPulse = 1;
        }
      }
      
      // Add the new pulse
      allPulses.push_back(PMTPulse(pmtID, pulseStartTime, photonsInPulse));
    }
	}
    
    
    // 3. Print the pulses
/*
    //G4cout << "\n--- Pulses found: " << allPulses.size() << " ---" << G4endl;
    for(size_t i = 0; i < allPulses.size(); i++) 
    {  	
      PMTPulse& pulse = allPulses[i];
      
      if (pulse.GetNPhotons() != 0)
      {
      
        G4cout << "Pulse " << i << ": "
               << "PMT[" << pulse.GetPMTID() << "] "
               << "Time = " << pulse.GetTime()/ns << " ns, "
               << "Photons = " << pulse.GetNPhotons()
               << G4endl;
      
      }

    }
*/

    
// ======== Popolate the histos =============

	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->FillNtupleIColumn(0, evt);	// column number, variable name
	man->FillNtupleIColumn(1, nHits);	

	
	//------- Which PMT has signal -----------
	
	// set is a collection of unique elements (no duplicates)
	// Here it is useful because allows to identify the PMT with at least one pulse
	std::set<G4int> activePMTs;
	
	for(PMTPulse& pulse : allPulses) 
	{
    activePMTs.insert(pulse.GetPMTID());
	}

	// Counter 1: active if PMT[0] and PMT[1] give signal
	G4int counter1 = 0;
	if(activePMTs.count(0) > 0 && activePMTs.count(1) > 0) 
	{
    counter1 = 1;
	}
	
	// Counter 2: active if PMT[2] and PMT[3] give signal
	G4int counter2 = 0;
	if(counter1 == 1 && activePMTs.count(2) > 0 && activePMTs.count(3) > 0) // Coincidenze with counter 1 !! 
	{
    counter2 = 1;
	}

	// Counter 3: active if PMT[4] and PMT[5] give signal
	G4int counter3 = 0;
	if(counter2 == 1 && activePMTs.count(4) > 0 && activePMTs.count(5) > 0)  // Coincidenze with counter 1, 2, 3 !!    
	{
    counter3 = 1;
	}

	// Print	
/*
	G4cout << "Debug: \n" << "Counter 1 =" << counter1 << "\n"
				 << "Counter 2 =" << counter2 << "\n"
				 << "Counter 3 =" << counter3 << "\n" << G4endl;
*/

	// ----------- Counters --------------
	
	man->FillNtupleIColumn(2, counter1);
	man->FillNtupleIColumn(3, counter2);
	man->FillNtupleIColumn(4, counter3);
	
	
	
	// --------- Edep, lenght and average dE/dx ------------- 
	
	man->FillNtupleDColumn(12, fEdep);
	man->FillNtupleDColumn(13, fPath/cm );
	
	// Calculate dE/dx
	G4double dEdx = 0.;
	if (fPath > 0)
		dEdx = fEdep /(fPath/cm);   // MeV/cm
		
	man->FillNtupleDColumn(14, dEdx );
	
	
	
	//------------ NofPhotons, pulses	--------------------
	
	man->FillNtupleIColumn(15, nPhotons );
	


	// ------------- TOF ----------------	
	
	G4double tof = GetTOF();  // ns
	//G4cout << "Debug: TOF = " << tof << " ns" << G4endl;

	if (tof > 0) 
	{
    man->FillNtupleDColumn(16, tof);
	}
	
	// ---------- End the row --------------------
	
	// Only one time (usually in endofeventaction)
	man->AddNtupleRow(); 
	
	// Notice that this forces a value even when you have not one for that event (see for example a non valid tof). This assures the corrispondence of ech row to an event. (For a double, like tof, it will force a zero).
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
