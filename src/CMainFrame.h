#pragma once
//SJC
#include <SJCWinLib.h>
#include <SJCCommCtrl.h>
//Local
#include "CDataTypeView.h"
#include "DataTypes.h"
#include "IBinaryDocument.h"
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

class CMainFrame : public CFrame
{
	struct SOpenFile
	{
		uint32 tabIndex;
		CWString filename;
		IBinaryDocument *pDoc;
	};
private:
	//Variables
	uint32 activeStructureSet;
	CArray<SOpenFile> openFiles;
	CArray<SMountedStructureInfo> mountedStructures;
	//Layout Variables
	CTab tabs;
	CDataTypeView dataTypeViewer;
	SJCWinLibCommCtrl::CListView dataStructureView;
	SJCWinLibCommCtrl::CListView mountedDataStructuresView;
	CStatusBar statusBar;
	//Callbacks
	void OnAcceleratorCommand(uint16 acceleratorCmdId);
	bool OnCreate();
	void OnDropFiles(const CDrop &refDrop);
	void OnHotkey(int32 hotKeyId, uint8 combinationKeys, uint8 virtualKey);
	void OnMenuCommand(uint16 menuItemId);
	void OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight);
	//Functions
	void ActivateStructureSet(uint16 i);
	void CreateMenu();
	void OpenFile(CWString filename);
	void UpdateDataStructureView();
	void UpdateDataTypeViewer(uint64 start, uint64 end, IBinaryDocument *pDoc);
	void UpdateMountedDataStructureView();
public:
	//Constructor
	CMainFrame();
	//Functions
	void LoadConfig();
	void UpdateInfo(bool selectionChanged);
};