#include<iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
// Physics Lists
#include "QBBC.hh"
#include "G4OpticalPhysics.hh"


#include "construction.hh"
#include "action.hh"


int main(int argc, char** argv)
{
	// always use run manager, that takes care of all the actions
	G4RunManager *runManager = new G4RunManager();
	
	// Detector Construction
	runManager->SetUserInitialization(new MyDetectorConstruction()); 
	
	// Physics list
	auto physicsList = new QBBC;
	physicsList->SetVerboseLevel(1);
	// Optical module
	physicsList->RegisterPhysics(new G4OpticalPhysics());
	runManager->SetUserInitialization(physicsList);
 	
 	// Action Inizialization
	runManager->SetUserInitialization(new MyActionInitialization()); 
	
	// Initialize
	runManager->Initialize();
	
	// to have user interface
	G4UIExecutive *ui = new G4UIExecutive(argc, argv);
	
	// to have the visualization
	G4VisManager *visManager = new G4VisExecutive();
	visManager->Initialize();
	
	// UI Manager 
	G4UImanager *UImanager = G4UImanager::GetUIpointer();
	UImanager->ApplyCommand("/vis/open OGL");
	UImanager->ApplyCommand("/vis/viewer/viewpointVector 1 1 1");
	UImanager->ApplyCommand("/vis/drawVolume");
	UImanager->ApplyCommand("/vis/viewer/set/autoRefresh true");
	UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");
	
	// Start the session
	ui->SessionStart();
	
	return 0;
}
