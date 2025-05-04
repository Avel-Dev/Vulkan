#include "Application.h"

int main()
{
	CHIKU::Application Application;

	Application.Init();
	Application.Run();
	Application.CleanUp();

	return 0;
}