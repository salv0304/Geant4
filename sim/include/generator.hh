#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
	MyPrimaryGenerator ();
	~MyPrimaryGenerator ();
	
	// It is the basic function of this class to run our particle gun
	virtual void GeneratePrimaries(G4Event*);	
	
private:
	G4ParticleGun *fParticleGun;
};

#endif
