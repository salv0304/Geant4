#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

namespace B1 {

class MySensitiveDetector : public G4VSensitiveDetector {
public:
    MySensitiveDetector(G4String name);
    virtual ~MySensitiveDetector();
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
};

} // namespace B1

#endif

