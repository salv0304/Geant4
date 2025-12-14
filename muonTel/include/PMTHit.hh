#ifndef PMTHit_h
#define PMTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"


// PMT hit class

// A hit is an information that is registered whenever something happens in a sensitive detector
// Hit class is a user-defined class which derives from the base class G4VHit. It has two virtual methods: Draw() and Print()
// user can store various types of information by implementation: position, time, step, ...
// Hits store the information retrieved step by step; They are created and filled by the SensitiveDetector class → Hits are stored in the HitCollection and can be retrieved at the end of the event

// A dedicated Hit class is necessary here because I have multiple PMTs, and I want to distinguish which PMT was hit. I need to preserve individual hit information for each event in order to analyze TOF and coincidences
// In contrast, for the scintillator volumes, a Hit class is not needed because we need only the total deposited energy and the total length per event, that can be accumulated directly in the sensitive detector without storing individual hits.
  
  
namespace RUT
{


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
    G4double fTime;            // Photon Time when enters the SD w.r.t the start of the run (t=0)
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
