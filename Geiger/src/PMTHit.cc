#include "PMTHit.hh"
#include "G4UnitsTable.hh"


namespace RUT
{


G4ThreadLocal G4Allocator<PMTHit>* PMTHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PMTHit::Print()
{
    G4cout << "PMT[" << fPMTID << "] "
           << "Time: " << G4BestUnit(fTime, "Time")
           << G4endl;
}


}
