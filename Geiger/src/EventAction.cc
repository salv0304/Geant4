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
#include <map>
#include <vector>
#include <algorithm>

#include "G4Event.hh"
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
	// print event ID
	G4cout << ">>> Start event: " << event->GetEventID() << G4endl;
	
	// Get ID of the Hit collection
	fPMTHitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("SensitiveDetector/PMTHitsCollection");
	G4cout << "DEBUG: PMT Collection ID = " << fPMTHitsCollectionID << G4endl;
    
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{

/* doesn't work
// Total photons produced
	G4int nPhotonsProduced = 0;
	for(auto track : *secondaries) 
	{
    	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
        nPhotonsProduced++;
	}
	G4cout << "Event " << event->GetEventID()
    	<< " produced " << nPhotonsProduced << " photons in scintillator"
    	<< G4endl;
*/    	
    	
	// Get the hit collection
	G4HCofThisEvent* hce = event->GetHCofThisEvent();

    // G4HCofThisEvent can have different types of HC (depending on the number of SD); for this reason GetHC gives a generic  type... then you cast the right type
    PMTHitsCollection* pmtHC = static_cast<PMTHitsCollection*>(hce->GetHC(fPMTHitsCollectionID));
    
    if(!pmtHC) 
    {
        G4cout << ">>> DEBUG: pmtHC is NULL!" << G4endl;
        return;
    }
        
    // get the number of hits 
    G4int nHits = pmtHC->entries();
    
    G4cout << ">>> DEBUG: nHits = " << nHits << G4endl;

	// print events and pmt hits
    G4cout << "\n=== Event " << event->GetEventID() << " ===" << G4endl;
	G4cout << "Total PMT hits: " << nHits << G4endl;
        

// ==== PULSE FINDING ====
    
    // 1. Group hits for each PMT
    std::map<G4int, std::vector<G4double>> hitsPerPMT;
    
    for(G4int i = 0; i < nHits; i++) {
        PMTHit* hit = (*pmtHC)[i];
        hitsPerPMT[hit->GetPMTID()].push_back(hit->GetTime());
    }
    
    // 2. For each PMT, create the pulses
    G4double timeWindow = 20.0 * ns;  // window to group photons
    std::vector<PMTPulse> allPulses;
    
    for(auto& pair : hitsPerPMT) {
        G4int pmtID = pair.first;
        std::vector<G4double>& times = pair.second;
        
        // sort in time
        std::sort(times.begin(), times.end());
        
        // Group
        if(times.size() > 0) {
            G4double pulseStartTime = times[0];
            G4int photonsInPulse = 1;
            
            for(size_t i = 1; i < times.size(); i++) {
                if(times[i] - pulseStartTime < timeWindow) {
                    // Same pulse
                    photonsInPulse++;
                } else {
                    // New pulse
                    allPulses.push_back(PMTPulse(pmtID, pulseStartTime, photonsInPulse));
                    pulseStartTime = times[i];
                    photonsInPulse = 1;
                }
            }
            // add the new pulse
            allPulses.push_back(PMTPulse(pmtID, pulseStartTime, photonsInPulse));
        }
    }
    
    // 3. Print the pulses
    G4cout << "\n--- Pulses found: " << allPulses.size() << " ---" << G4endl;
    for(size_t i = 0; i < allPulses.size(); i++) 
    {
        const PMTPulse& pulse = allPulses[i];
        G4cout << "Pulse " << i << ": "
               << "PMT[" << pulse.pmtID << "] "
               << "Time = " << pulse.time/ns << " ns, "
               << "Photons = " << pulse.nPhotons
               << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
