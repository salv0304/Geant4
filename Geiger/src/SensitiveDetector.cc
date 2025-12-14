#include "SensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4OpticalPhoton.hh"


namespace RUT
{

MySensitiveDetector::MySensitiveDetector(G4String name) 
: G4VSensitiveDetector(name),
  fHitsCollection(nullptr)
{
    collectionName.insert("PMTHitsCollection");
}

MySensitiveDetector::~MySensitiveDetector()
{}

void MySensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
G4cout << ">>> DEBUG: Initialize SD chiamato" << G4endl;

    // Create the hit collection
    fHitsCollection = new PMTHitsCollection(SensitiveDetectorName, collectionName[0]);
    
    // add to the event
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID("SensitiveDetector/PMTHitsCollection");
    hce->AddHitsCollection(hcID, fHitsCollection);
    
   G4cout << ">>> DEBUG: Collection added, entries = " << fHitsCollection->entries() << G4endl;
}


G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{

	//G4cout << ">>> ProcessHits called!" << G4endl;

	// filters only optical photons
    if(aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
        return false; 
    
    // Kill tracks
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    
    // Score hits
    PMTHit* newHit = new PMTHit();
    G4int pmtID = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber();
    newHit->SetPMTID(pmtID);
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    newHit->SetPosition(aStep->GetPreStepPoint()->GetPosition());
    fHitsCollection->insert(newHit);
    
    return true;
}


void MySensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    if(verboseLevel > 1) 
    {
        G4int nofHits = fHitsCollection->entries();
        G4cout << G4endl << "PMT Hits in this event: " << nofHits << G4endl;
    }
}

}
