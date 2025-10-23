//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file EmCalorimeterHit.hh
/// \brief Definition of the EmCalorimeterHit class
//

#ifndef EmCalorimeterHit_h
#define EmCalorimeterHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

namespace EX3
{

class EmCalorimeterHit : public G4VHit
{
  public:
    EmCalorimeterHit();
    ~EmCalorimeterHit() override;
    EmCalorimeterHit(const EmCalorimeterHit& right);
    const EmCalorimeterHit& operator=(const EmCalorimeterHit& right);
    int operator==(const EmCalorimeterHit& right) const;

    inline void* operator new(size_t);
    inline void  operator delete(void* hit);

    void Print() override;

    // add setter/getter methods
    void SetLayerNumber(G4int number) { fLayerNumber = number; }
    void AddEdep(G4double edep)       { fEdep += edep; }

    G4int    GetLayerNumber() const { return fLayerNumber;}
    G4double GetEdep() const        { return fEdep; }

  private:
    // add data members
    G4int     fLayerNumber = -1;
    G4double  fEdep = 0.;
};

typedef G4THitsCollection<EmCalorimeterHit> EmCalorimeterHitsCollection;

extern G4Allocator<EmCalorimeterHit>* EmCalorimeterHitAllocator;

inline void* EmCalorimeterHit::operator new(size_t)
{
  if (! EmCalorimeterHitAllocator)
        EmCalorimeterHitAllocator = new G4Allocator<EmCalorimeterHit>;
  return (void*)EmCalorimeterHitAllocator->MallocSingle();
}

inline void EmCalorimeterHit::operator delete(void* hit)
{
  EmCalorimeterHitAllocator->FreeSingle((EmCalorimeterHit*) hit);
}

}

#endif


