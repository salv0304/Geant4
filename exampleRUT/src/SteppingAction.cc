#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4AnalysisManager.hh"


SteppingAction::SteppingAction() : G4UserSteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  // Only the primary particle
  G4Track* track = step->GetTrack();
  if (track->GetTrackID() != 1) return;
  
  G4String postVolName = step->GetPostStepPoint()->GetPhysicalVolume()->GetName(); 
  
  // Print when particle enters foil and detector
  if (postVolName == "Foil" || postVolName == "Detector") {
  
    G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
    G4cout << "Entering " << postVolName << " at " << pos << G4endl;
    
    // For the analysis
    G4AnalysisManager *man = G4AnalysisManager::Instance();
	
    man->FillNtupleDColumn(0, pos[0]);	 
    man->FillNtupleDColumn(1, pos[1]);
    man->FillNtupleDColumn(2, pos[2]);
    man->AddNtupleRow(0);
  }
  
  // Kill after the detector is hit
  if (postVolName == "Detector") {
    step->GetTrack()->SetTrackStatus(fStopAndKill);
	
  }
}
