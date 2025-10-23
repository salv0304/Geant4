#include "action.hh"
#include "run.hh"

MyActionInitialization::MyActionInitialization()
{}

MyActionInitialization::~MyActionInitialization()
{}

void MyActionInitialization::Build() const
{
	// The generator
	MyPrimaryGenerator *generator = new MyPrimaryGenerator();
	SetUserAction(generator);
	
	// The run
	MyRunAction *runAction = new MyRunAction();
	SetUserAction(runAction);
}

