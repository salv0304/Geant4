#include "SensitiveDetector_Scint.hh"
#include "EventAction.hh"

#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Step.hh"
#include "G4THitsCollection.hh"


namespace RUT
{

MySensitiveDetector_Scint::MySensitiveDetector_Scint(G4String name) 
: G4VSensitiveDetector(name), fPhotonCount(0) //Initialize
{}

MySensitiveDetector_Scint::~MySensitiveDetector_Scint()
{}

void MySensitiveDetector_Scint::Initialize(G4HCofThisEvent* hce)
{}

G4bool MySensitiveDetector_Scint::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
		G4Track* track = aStep->GetTrack();	// obtain the track	
		
// ======== Count the number of photons ======
	
	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() 
		&& track->GetCurrentStepNumber() == 1)  // count only the first step! to avoid multi counting
	
	{
  	fPhotonCount++;
	}
	
	
// ========== Deposited energy and Path ===========

		// get the deposited energy for each step (MeV)
  	G4double edep = aStep->GetTotalEnergyDeposit();
  	
  	// Access to the run manager, retrieve the UserEventAction and cast it to the right type 
  	// Why edep (and stepLenght) and its methods aren't defined in the SD class?
  	// Because SD is not "event aware", so the total deposited energy (and its methods) must be in the eventaction class
  	auto eventAction = (EventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
  	
  	// Calculate the total deposited energy
  	if (edep > 0)
  		eventAction->AddEdep(edep);
 
	// For the step lenght i need to filter only the primaries 		
	if (track->GetParentID() == 0)	// filter only Primary particles (the muons)
	{ 		 
  	G4double stepLength = aStep->GetStepLength(); // get step lenght (mm)
  		
  	// Calculate the total lenght in the scintillator slabs
  	if (stepLength > 0)
  		eventAction->AddPath(stepLength);
	}
	
	
// =============== Time of Flight =================
	if (track->GetParentID() == 0)	// filter only Primary particles (the muons)
	{ 	
		G4double globalTime = track->GetGlobalTime(); // ns
	
		G4int copyNo = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
                     
		// First scintillator
		if (copyNo == 0) 
		{
    	eventAction->SetTOF1(globalTime);
		}
	
		// Third scintillator
		if (copyNo == 2) 
		{
    	eventAction->SetTOF3(globalTime);
		}
			
	}


/*
// ============== Secondary positrons =================
if (track->GetDefinition() == G4Positron::PositronDefinition())
{
    if (track->GetParentID() > 0)  // secondary particle
    {
        G4int copyNo = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
        
        // Check if this is the first step of the track in the scintillator
        //if (track->GetCurrentStepNumber() == 1)
        //{
        	// the third scintillator is the one that we are interested in
        	if(copyNo == 2)
        	{
            const G4VProcess* creatorProcess = track->GetCreatorProcess();
            // Get parent particle info
            G4int parentID = track->GetParentID();
            
            G4cout << ">>> Secondary positron detected in scint No " 
                       << copyNo 
                       << " Process: " << creatorProcess->GetProcessName()
                       << G4endl;
            }
           
            if (creatorProcess && creatorProcess->GetProcessName() == "Decay")
            {
                // Get parent particle info
                G4int parentID = track->GetParentID();
                
                // You could also check parent particle type if needed:
                // const G4DynamicParticle* parent = track->GetDynamicParticle();
                
                G4cout << ">>> Electron from muon decay detected in scintillator No " 
                       << copyNo 
                       << " - Parent ID: " << parentID
                       << " - Energy: " << track->GetKineticEnergy()/CLHEP::MeV << " MeV"
                       << G4endl;
                
            }
            
        }
    }
}
*/

	
	return true;
}

}
