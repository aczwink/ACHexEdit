//SJC
#include <SJCWinLib.h>
//Local
#include "CMainFrame.h"
#include "globals.h"
#include "resource.h"
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

//Global Variables
CMainFrame g_mainFrame;
CArray<SStructureSet> g_structureSets;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nCmdShow)
{
	int32 result;
	HACCEL hAccelTable;
	ACCEL hotKeys[2];
	CApplication app;
	CBrush backgroundBrush;
	CIcon icon;

#ifdef _DEBUG
	AllocateConsole();
	SetConsoleTitle("Debug Console");
	CWindow::GetConsoleWindow()->Move(0, 0, 500, 300);
	stdOut << "Launching in Debug Mode" << endl;
#endif
	
	g_mainFrame.Create(APPLICATION_TITLE, 0, 0, 800, 600);
	g_mainFrame.Show(nCmdShow);
	g_mainFrame.Update();
	g_mainFrame.Show(SW_MAXIMIZE);
	g_mainFrame.LoadConfig();
	
	//Register hot keys
	hotKeys[0].cmd = IDH_EDIT_FIND;
	hotKeys[0].fVirt = FCONTROL | FVIRTKEY;
	hotKeys[0].key = 0x46;
	
	hotKeys[1].cmd = IDH_EDIT_GOTO;
	hotKeys[1].fVirt = FCONTROL | FVIRTKEY;
	hotKeys[1].key = 0x47;
	
	hAccelTable = CreateAcceleratorTable(hotKeys, sizeof(hotKeys) / sizeof(hotKeys[0]));
	result = app.Run(g_mainFrame, hAccelTable);
	DestroyAcceleratorTable(hAccelTable);
	
	return result;
}