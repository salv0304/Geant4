#ifndef PMTHit_h
#define PMTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"

namespace RUT
{

/// PMT hit class
///
// A hit is an information that is registered when something happens in a sensitive detector
/// It defines data members to store the...
///
/// 


class PMTHit : public G4VHit
{
	public:
	    PMTHit() = default;
    	PMTHit(const PMTHit&) = default;
    	virtual ~PMTHit() override = default;
    	
    	
		// operators
    	PMTHit& operator=(const PMTHit&) = default;
    	G4bool operator==(const PMTHit&) const;
    	
    	inline void* operator new(size_t);
    	inline void operator delete(void*);
	
		// methods from base class
		virtual void Print();
		
		// Set methods
    	void SetPMTID(G4int id) { fPMTID = id; }
    	void SetTime(G4double time) { fTime = time; }
    	void SetPosition(G4ThreeVector pos) { fPosition = pos; }
	
		// Get methods
    	G4int GetPMTID() const { return fPMTID; }
    	G4double GetTime() const { return fTime; }
    	G4ThreeVector GetPosition() const { return fPosition; }
	
	private:
		G4int fPMTID;              // PMT ID
    	G4double fTime;            // Photon TOF
    	G4ThreeVector fPosition;   // Hit position 

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hits collection has a generic type
using PMTHitsCollection = G4THitsCollection<PMTHit>;


// specific allocator of G4 to deal rapidly with numerous hits
extern G4ThreadLocal G4Allocator<PMTHit>* PMTHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Definition of the operators... for performance reasons.... just copy and forget!


inline void* PMTHit::operator new(size_t)
{
  if(!PMTHitAllocator)
      PMTHitAllocator = new G4Allocator<PMTHit>;
  return (void *) PMTHitAllocator->MallocSingle();
}

inline void PMTHit::operator delete(void *hit)
{
  PMTHitAllocator->FreeSingle((PMTHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


}

#endif
