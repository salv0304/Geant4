#include "run.hh" 

MyRunAction::MyRunAction()
{}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run*)
{
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->OpenFile("output.root");
	
	man->CreateNtuple("Hits", "Hits");
	man->CreateNtupleIColumn("fEvent");	//integer column
	man->CreateNtupleDColumn("fx");	//double column
	man->CreateNtupleDColumn("fy");
	man->CreateNtupleDColumn("fz");
	man->FinishNtuple(0);	// 0 since it is the first ntuple
}

void MyRunAction::EndOfRunAction(const G4Run*)
{
	G4AnalysisManager *man = G4AnalysisManager::Instance();
	
	man->Write();
	man->CloseFile();
}
