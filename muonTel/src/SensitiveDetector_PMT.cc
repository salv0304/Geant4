#include "SensitiveDetector_PMT.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4OpticalPhoton.hh"


namespace RUT
{

MySensitiveDetector_PMT::MySensitiveDetector_PMT(G4String name) 
: G4VSensitiveDetector(name),
  fHitsCollection(nullptr)
{
    collectionName.insert("PMTHitsCollection");
}

MySensitiveDetector_PMT::~MySensitiveDetector_PMT()
{}

void MySensitiveDetector_PMT::Initialize(G4HCofThisEvent* hce)
{
	
	// Create the hit collection
  fHitsCollection = new PMTHitsCollection(SensitiveDetectorName, collectionName[0]);
    
  // add the event HC
  G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID("SensitiveDetector_PMT/PMTHitsCollection");
  hce->AddHitsCollection(hcID, fHitsCollection);
}

// Main method of the class
G4bool MySensitiveDetector_PMT::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
// At each step in a detector defined as sensitive, the method ProcessHits() is invoked: it must create, fill and store the hit objects.

	// filters only optical photons
  if(aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
  	return false; 
    
  // Kill tracks
  aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    
  // Score hits (need the PMTHit class)
  PMTHit* newHit = new PMTHit();	// Create a new hit for each detected photon
  G4int pmtID = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber();
  newHit->SetPMTID(pmtID);
  newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
  newHit->SetPosition(aStep->GetPreStepPoint()->GetPosition());
  fHitsCollection->insert(newHit);
	
    return true;
}


}
