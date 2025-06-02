#include "Application.h"

int main()
{
	ZoneScoped;    // Profile this block

	CHIKU::Application Application;

	Application.Init();
	Application.Run();
	Application.CleanUp();

	return 0;
}