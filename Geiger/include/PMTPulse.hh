#ifndef PMTPulse_h
#define PMTPulse_h 1

#include "globals.hh"
#include <vector>

namespace RUT
{

struct PMTPulse
{
    G4int pmtID;           // PMT ID
    G4double time;         // Time
    G4int nPhotons;        // Number of photons in the pulse
    
    PMTPulse(G4int id, G4double t, G4int n = 1)
    : pmtID(id), time(t), nPhotons(n) {}
};

}

#endif
