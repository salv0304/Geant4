#ifndef SENSITIVEDETECTOR_PMT_HH
#define SENSITIVEDETECTOR_PMT_HH

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"

#include "PMTHit.hh"



class G4Step;
class G4HCofThisEvent;


namespace RUT
{
// A Sensitive Detector (SD) is a volume that generates hits. A hit is like an empty container that stores information step by step (e.g., energy deposition, position, time) during the simulation. Hits are collected in a hit collection, which is then attached to an event.
// The PMT will need an appropriate hit class, since to do coincidence analysis I need to act on hit-level (to see which hit goes to which PMT) -> this goes in the Initialize method

class MySensitiveDetector_PMT : public G4VSensitiveDetector

{
public:
	MySensitiveDetector_PMT(G4String);	//constructor
	~MySensitiveDetector_PMT();	//Destructor	
	
	virtual void Initialize(G4HCofThisEvent* hce);
	
	// This is the main method of this class, used to process hits
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);	
	
	
private:
	PMTHitsCollection* fHitsCollection;
};

}
#endif
