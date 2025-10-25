#include "SensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "RunAction.hh"


namespace B1
{

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhlist)
{

	G4cout << "ProcessHits called!" << G4endl;  // DEBUG


	// Obtain the energy of the primary from the track
	G4Track* track = aStep->GetTrack();
	
	// I want the energy only of the primary particles
	if (track->GetParentID() != 0) return true;
	
    
    // Get the deposited energy
    G4double edep = aStep->GetTotalEnergyDeposit();
    //if (edep == 0.) return false;
    if (edep == 0.) edep = 0.;
	
	// Identify which slice is the interested one
	const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int copyNo = touchable->GetCopyNumber();
    
    // Obtain step lenght
    G4double stepLength = aStep->GetStepLength();
    
	// Use the energy before the deposition (pre-step)
    G4double kineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();
    
    // Calculate dE/dx
    G4double dEdx = 0.;
    if (stepLength > 0.) {
        dEdx = edep / stepLength;  // MeV/mm
    }
    
    // Print output
    G4cout << "Slice " << copyNo 
           << " | Edep = " << edep/MeV << " MeV"
           << " | Step = " << stepLength/mm << " mm"
           << " | dE/dx = " << dEdx/(MeV/mm) << " MeV/mm"
           << G4endl;
	
	
	// For the analysis
	G4AnalysisManager *man = G4AnalysisManager::Instance();

    // To have access at the event (we need also the G4RunManager.hh for this)    
	G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    // Fill the Ntuple    
    man->FillNtupleIColumn(0, evt);
    man->FillNtupleDColumn(1, kineticEnergy); 
    man->FillNtupleDColumn(2, stepLength);
    man->FillNtupleDColumn(3, edep);
    man->FillNtupleDColumn(4, dEdx);
    man->AddNtupleRow(0);
	return true;
}

}
