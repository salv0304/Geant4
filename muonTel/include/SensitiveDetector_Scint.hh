#ifndef SENSITIVEDETECTOR_Scint_HH
#define SENSITIVEDETECTOR_Scint_HH

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"

#include "PMTHit.hh"



class G4Step;
class G4HCofThisEvent;


namespace RUT
{


class MySensitiveDetector_Scint : public G4VSensitiveDetector
{
// A Sensitive Detector (SD) is a volume that generates hits. A hit is like an empty container that stores information step by step (e.g., energy deposition, position, time) during the simulation. Hits are collected in a hit collection, which is then attached to an event.


public:
	MySensitiveDetector_Scint(G4String);	//constructor
	~MySensitiveDetector_Scint();	//Destructor
	
	virtual void Initialize(G4HCofThisEvent* hce);
	
	// This is the main method of this class, used to process hits
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
	
	G4int GetPhotonCount() const { return fPhotonCount; }
	void Reset() {fPhotonCount = 0;}

	
private:
	G4int fPhotonCount; // to count produced photons
};

}
#endif
