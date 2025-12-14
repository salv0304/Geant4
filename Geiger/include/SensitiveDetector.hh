#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"

#include "PMTHit.hh"



class G4Step;
class G4HCofThisEvent;


namespace RUT
{


class MySensitiveDetector : public G4VSensitiveDetector

{
public:
	MySensitiveDetector(G4String);	//constructor
	~MySensitiveDetector();	//Destructor
	virtual void Initialize(G4HCofThisEvent* hce);
	// This is the main method of this class, used to process hits
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
	virtual void EndOfEvent(G4HCofThisEvent* hitCollection);
	
private:
	PMTHitsCollection* fHitsCollection;
};

}
#endif
