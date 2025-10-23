#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
	MySensitiveDetector(G4String);	//constructor
	~MySensitiveDetector();	//Destructor
	
private:
	// This is the main method of this class, used to process hits
	virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
};

#endif
