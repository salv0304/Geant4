#include "detector.hh"
#include "G4RunManager.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhlist)
{
	// We want to get information about the position of the photons
	G4Track *track = aStep->GetTrack();
	G4StepPoint *preStepPoint = aStep->GetPreStepPoint();	//when photons enter
//	G4StepPoint *postStepPoint = aStep->GetPostStepPoint();	//when photons exit
	//in our case, we choose to kill the track when entering the detector
	track->SetTrackStatus(fStopAndKill);
	// Position of the photon when it enters the detector!
	G4ThreeVector posPhoton = preStepPoint->GetPosition();
	//G4cout << "Photon position: " << posPhoton << G4endl;
	
	// But in reality we don't have access to the true position of the photons, but only to the detector cell that is hit
	const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
	G4int copyNo = touchable->GetCopyNumber();	//Tells me which copy is the one that was hit (here why you need an unique ID) 
	//G4cout << "Copy number: " << copyNo << G4endl;
	G4VPhysicalVolume *physVol = touchable->GetVolume();
	G4ThreeVector posDetector = physVol->GetTranslation();	//To get the position of the detector that was hit.
	G4cout << "Detector position: " << posDetector << G4endl;
	
	// For the analysis
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	// To have access at the event (we need also the G4RunManager.hh for this)
	G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	
	man->FillNtupleIColumn(0, evt);	// column number, variable name
	man->FillNtupleDColumn(1, posDetector[0]);	 
	man->FillNtupleDColumn(2, posDetector[1]);
	man->FillNtupleDColumn(3, posDetector[2]);
	man->AddNtupleRow(0);

	
	return true;
}
