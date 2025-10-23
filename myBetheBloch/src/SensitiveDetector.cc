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


	// Ottieni l'energia del primario dal track
	G4Track* track = aStep->GetTrack();
	// FONDAMENTALE: filtra solo particelle primarie
	if (track->GetParentID() != 0) return true;
	G4double beamEnergy = track->GetVertexKineticEnergy();
    
    // POI: Controlla energia depositata
    G4double edep = aStep->GetTotalEnergyDeposit();
    if (edep == 0.) return false;
	
	// Identify which slice is the interested one
	const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int copyNo = touchable->GetCopyNumber();
    
    // Obtain step lenght
    G4double stepLength = aStep->GetStepLength();
    
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

/*	
	// ID ntuple
    const RunAction* runAction = static_cast<const RunAction*>
        (G4RunManager::GetRunManager()->GetUserRunAction());
    G4int ntupleId = runAction->GetCurrentNtupleId();
*/	
    // To have access at the event (we need also the G4RunManager.hh for this)
    
	G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    
    man->FillNtupleIColumn(0, evt);
    man->FillNtupleDColumn(1, beamEnergy);  // AGGIUNGI QUESTA RIGA
    man->FillNtupleDColumn(2, stepLength);
    man->FillNtupleDColumn(3, edep);
    man->FillNtupleDColumn(4, dEdx);
    man->AddNtupleRow(0);
	return true;
}

}
