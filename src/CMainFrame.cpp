//Class Header
#include "CMainFrame.h"
//Local
#include "CBinaryFileDocument.h"
#include "CFindDialog.h"
#include "CHexEdit.h"
#include "CManageStructureSetsDialog.h"
#include "Definitions.h"
#include "globals.h"
#include "resource.h"

//Constructor
CMainFrame::CMainFrame()
{
	this->activeStructureSet = IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT;
}

//Callbacks
void CMainFrame::OnAcceleratorCommand(uint16 acceleratorCmdId)
{
	switch(acceleratorCmdId)
	{
	case IDH_EDIT_FIND:
		this->OnMenuCommand(IDM_EDIT_FIND);
		break;
	case IDH_EDIT_GOTO:
		this->OnMenuCommand(IDM_EDIT_GOTO);
		break;
	}
}

bool CMainFrame::OnCreate()
{
	int32 groupId, index;

	this->CreateMenu();
	this->tabs.Create(*this);
	this->DragAcceptFiles(true);
	
	//Create the data type viewer
	this->dataTypeViewer.CreateReportView(WS_BORDER | LVS_NOSORTHEADER, *this);
	this->dataTypeViewer.EnableGroupView();
	this->dataTypeViewer.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	this->dataTypeViewer.AddColumn(L"Data Type");
	this->dataTypeViewer.AddColumn(L"Value");

	//Create the data structure view
	this->dataStructureView.CreateReportView(WS_BORDER | LVS_NOSORTHEADER, *this);
	this->dataStructureView.EnableGroupView();
	this->dataStructureView.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	this->dataStructureView.AddColumn(L"Offset");
	this->dataStructureView.AddColumn(L"Name");
	this->dataStructureView.AddColumn(L"Type");
	this->dataStructureView.AddColumn(L"Size in bytes");
	
	//Create the mounted data structure view
	this->mountedDataStructuresView.CreateReportView(WS_BORDER | LVS_NOSORTHEADER, *this);
	this->mountedDataStructuresView.EnableGroupView();
	this->mountedDataStructuresView.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	this->mountedDataStructuresView.AddColumn(L"Offset");
	this->mountedDataStructuresView.AddColumn(L"Name");
	this->mountedDataStructuresView.AddColumn(L"Type");
	this->mountedDataStructuresView.AddColumn(L"Value");
	
	//General
	groupId = this->dataTypeViewer.InsertGroup(L"Endian independent");
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"int8", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"uint8", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"Binary", index, 0);

	//Little Endian
	groupId = this->dataTypeViewer.InsertGroup(L"Little Endian");

	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"int16", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"uint16", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"int32", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"uint32", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"int64", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"uint64", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"float32", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"float64", index, 0);

	//Big Endian
	groupId = this->dataTypeViewer.InsertGroup(L"Big Endian");

	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"int16", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"uint16", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"int32", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"uint32", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"int64", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"uint64", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"float32", index, 0);
	index = this->dataTypeViewer.InsertItem(groupId);
	this->dataTypeViewer.SetItemText(L"float64", index, 0);

	//Status bar
	this->statusBar.Create(0, *this);
	
	return true;
}

void CMainFrame::OnDropFiles(const CDrop &refDrop)
{
	uint32 i;

	for(i = 0; i < refDrop.GetNumberOfFiles(); i++)
	{
		this->OpenFile(refDrop.GetFileName(i));
	}
}

void CMainFrame::OnHotkey(int32 hotKeyId, uint8 combinationKeys, uint8 virtualKey)
{
	switch(hotKeyId)
	{
	case IDH_EDIT_FIND:
		{
			this->OnMenuCommand(IDM_EDIT_FIND);
		}
		break;
	}
}

void CMainFrame::OnMenuCommand(uint16 menuItemId)
{
	switch(menuItemId)
	{
	case IDM_FILE_OPEN:
		{
			CCommonItemOpenDialog dlg;

			dlg.Create(this);
			if(dlg.Run())
			{
				this->OpenFile(dlg.GetResult());
			}
		}
		break;
	case IDM_FILE_EXIT:
		{
			this->PostMessageA(MsgClose());
		}
		break;
	case IDM_EDIT_FIND:
		{
			int32 tabIndex;

			tabIndex = this->tabs.GetActiveTab();
			if(tabIndex != -1)
			{
				CFindDialog dlg;
				
				dlg.Create(*this);
				if(dlg.Run() == IDOK)
				{
					CHexEdit *pHexEdit;
					
					pHexEdit = (CHexEdit *)this->tabs.GetChildWindow(tabIndex);
					
					pHexEdit->Find(dlg.GetFindBuffer(), dlg.GetFindBufferSize());
					pHexEdit->Focus();
				}
			}
		}
		break;
	case IDM_EDIT_GOTO:
		{
			int32 tabIndex;

			tabIndex = this->tabs.GetActiveTab();
			if(tabIndex != -1)
			{
				CPromptDialog dlg;
				
				dlg.Create("Enter offset", "", this);
				if(dlg.Run() == IDOK)
				{
					CHexEdit *pHexEdit;
					
					pHexEdit = (CHexEdit *)this->tabs.GetChildWindow(tabIndex);
					
					pHexEdit->GotoOffset(dlg.GetValue().ToUInt64());
					pHexEdit->Focus();
				}
			}
		}
		break;
	case IDM_STRUCTUREMANAGEMENT_MANAGE_STRUCTURE_SETS:
		{
			CManageStructureSetsDialog dlg;

			dlg.Create(*this);
			dlg.Run();
		}
		break;
	case IDM_STRUCTUREMANAGEMENT_MOUNT_STRUCTURE:
		{
			CSelectDialog dlg;
			
			dlg.Create("Select Structure to mount", *this);

			foreach(it, g_structureSets[this->activeStructureSet - 1].structures)
			{
				dlg.AddSelection((*it).name);
			}
			
			if(dlg.Run() == IDOK)
			{
				CPromptDialog offsetDlg;

				offsetDlg.Create("Offset where to mount", "", this);
				if(offsetDlg.Run() == IDOK)
				{
					SMountedStructureInfo info;

					info.structureIndex = dlg.GetSelection();
					info.offset = offsetDlg.GetValue().ToUInt64();

					this->mountedStructures.Push(info);
					this->UpdateMountedDataStructureView();
				}
			}
		}
		break;
	}
	
	if(menuItemId >= IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT)
	{
		this->ActivateStructureSet(menuItemId & ~IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT);
	}
}

void CMainFrame::OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight)
{
	CRect rcClient, rcStatusBar, rcTabs, rcDataTypeViewer, rcDataStructureView, rcMountedDataStructuresView;

	this->GetClientRect(rcClient);

	//Status Bar
	this->statusBar.AutoSize();
	this->statusBar.GetClientRect(rcStatusBar);
	
	int32 partsRightEdges[3] = {rcStatusBar.GetWidth() / 3, rcStatusBar.GetWidth() * 2 / 3, -1};
	
	this->statusBar.SetParts(3, partsRightEdges);
	
	//Tabs subwindow
	rcTabs.right = rcClient.right - LAYOUT_DATATYPEVIEWER_WIDTH;
	rcTabs.bottom = rcClient.bottom * 2 / 3;
	
	this->tabs.SetPos(rcTabs, SWP_NOZORDER);
	
	//Data Type Viewer
	rcDataTypeViewer.left = rcTabs.right;
	rcDataTypeViewer.right = rcClient.right;
	rcDataTypeViewer.bottom = rcTabs.bottom;
	
	this->dataTypeViewer.SetPos(rcDataTypeViewer, SWP_NOZORDER);
	this->dataTypeViewer.SetColumnWidth(0, LAYOUT_DATATYPEVIEWER_COLUMN1_WIDTH);
	this->dataTypeViewer.SetColumnWidth(1, LAYOUT_DATATYPEVIEWER_COLUMN2_WIDTH);

	//Data Structure View
	rcDataStructureView.left = 5;
	rcDataStructureView.top = rcTabs.bottom + 5;
	rcDataStructureView.right = rcClient.right / 2 - 3;
	rcDataStructureView.bottom = rcClient.bottom - rcStatusBar.GetHeight() - 5;

	this->dataStructureView.SetPos(rcDataStructureView, SWP_NOZORDER);
	for(uint32 i = 0; i < 4; i++)
	{
		this->dataStructureView.SetColumnWidth(i, (uint16)(rcDataStructureView.GetWidth() / 4));
	}

	//Mounted data structures view
	rcMountedDataStructuresView.left = rcDataStructureView.right + 6;
	rcMountedDataStructuresView.top = rcDataStructureView.top;
	rcMountedDataStructuresView.right = rcClient.right - 5;
	rcMountedDataStructuresView.bottom = rcDataStructureView.bottom;

	this->mountedDataStructuresView.SetPos(rcMountedDataStructuresView, SWP_NOZORDER);
	for(uint32 i = 0; i < 4; i++)
	{
		this->mountedDataStructuresView.SetColumnWidth(i, (uint16)(rcMountedDataStructuresView.GetWidth() / 4));
	}
}

//Private Functions
void CMainFrame::ActivateStructureSet(uint16 i)
{
	this->mountedDataStructuresView.Clear();

	this->activeStructureSet = i;
	this->CreateMenu();
	this->UpdateDataStructureView();
}

void CMainFrame::CreateMenu()
{
	uint32 i;
	CMenu menu, subMenu;
	
	menu.Create();
	
	subMenu.CreatePopup();
	subMenu.AppendItem("Open", IDM_FILE_OPEN);
	subMenu.AppendSeperator();
	subMenu.AppendItem("Exit", IDM_FILE_EXIT);
	menu.AppendSubMenu("File", subMenu);

	if(this->tabs.GetActiveTab() != -1)
	{
		subMenu.CreatePopup();
		subMenu.AppendItem("Find\tCTRL+F", IDM_EDIT_FIND);
		subMenu.AppendItem("Go to offset\tCTRL+G", IDM_EDIT_GOTO);
		menu.AppendSubMenu("Edit", subMenu);
	}
	
	subMenu.CreatePopup();
	subMenu.AppendItem("Manage structure sets", IDM_STRUCTUREMANAGEMENT_MANAGE_STRUCTURE_SETS);
	subMenu.AppendItem("Mount structure", IDM_STRUCTUREMANAGEMENT_MOUNT_STRUCTURE);
	if(this->tabs.GetActiveTab() == -1)
	{
		subMenu.EnableItem(IDM_STRUCTUREMANAGEMENT_MOUNT_STRUCTURE, false);
	}
	subMenu.AppendSeperator();
	subMenu.AppendItem("(none)", IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT);
	i = 1;
	foreach(it, g_structureSets)
	{
		subMenu.AppendItem((*it).name, IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT | i);
		i++;
	}
	
	subMenu.CheckItemRadio(IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT, IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT | (i - 1), IDM_STRUCTUREMANAGEMENT_FIRSTSTRUCT | this->activeStructureSet);
	menu.AppendSubMenu("Structure Management", subMenu);
	
	this->SetMenu(menu);
}

void CMainFrame::OpenFile(CWString filename)
{
	int32 activeTab;
	CPointer<CHexEdit> pHexEditCtrl;
	SOpenFile of;
	
	if(!FileExists(filename))
	{
		this->MessageBox(L"File '" + filename + L"' does not exist.", L"Error", MB_ICONERROR);
		return;
	}
	
	of.pDoc = new CBinaryFileDocument;
	if(!((CBinaryFileDocument *)of.pDoc)->Open(filename))
	{
		delete of.pDoc;
		this->MessageBox(L"Failed to open file:\n\"" + filename + '"', L"Error", MB_ICONERROR);
		return;
	}

	activeTab = this->tabs.GetActiveTab();
	
	pHexEditCtrl->Create(of.pDoc, this->tabs);
	
	of.tabIndex = this->tabs.AddTab(GetFullFileName(filename), pHexEditCtrl.Cast<CWindow>());
	of.filename = filename;
	
	this->openFiles.Push(of);

	if(activeTab == -1) //we have a tab now
	{
		this->CreateMenu();
	}

	g_mainFrame.UpdateInfo(true);
}

void CMainFrame::UpdateDataStructureView()
{
	int32 groupId, itemId;
	uint32 offset;
	
	this->dataStructureView.Clear();

	if(this->activeStructureSet == 0) //(none)
		return;
	
	//put structures into data structure view
	foreach(it, g_structureSets[this->activeStructureSet - 1].structures)
	{
		groupId = this->dataStructureView.InsertGroup((*it).name);
		offset = 0;
		
		foreach(it2, (*it).elements)
		{
			itemId = this->dataStructureView.InsertItem(groupId);
			this->dataStructureView.SetItemText(CString(offset, NS_HEX), itemId, 0);
			this->dataStructureView.SetItemText((*it2).name, itemId, 1);
			this->dataStructureView.SetItemText(TypeToString(*it2), itemId, 2);
			this->dataStructureView.SetItemText(CString(GetDataTypeSize(*it2)), itemId, 3);

			offset += GetDataTypeSize(*it2);
		}
	}
}

void CMainFrame::UpdateDataTypeViewer(uint64 start, uint64 end, IBinaryDocument *pDoc)
{
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_INT8, 1, false, start, *pDoc), 0, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_UINT8, 1, false, start, *pDoc), 1, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_BINARY, 1, false, start, *pDoc), 2, 1);

	//Little Endian
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_INT16, 1, false, start, *pDoc), 3, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_UINT16, 1, false, start, *pDoc), 4, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_INT32, 1, false, start, *pDoc), 5, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_UINT32, 1, false, start, *pDoc), 6, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_INT64, 1, false, start, *pDoc), 7, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_UINT64, 1, false, start, *pDoc), 8, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_FLOAT32, 1, false, start, *pDoc), 9, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_FLOAT64, 1, false, start, *pDoc), 10, 1);
	
	//Big Endian
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_INT16, 1, true, start, *pDoc), 11, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_UINT16, 1, true, start, *pDoc), 12, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_INT32, 1, true, start, *pDoc), 13, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_UINT32, 1, true, start, *pDoc), 14, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_INT64, 1, true, start, *pDoc), 15, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_UINT64, 1, true, start, *pDoc), 16, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_FLOAT32, 1, true, start, *pDoc), 17, 1);
	this->dataTypeViewer.SetItemText(ValueToString(TYPE_FLOAT64, 1, true, start, *pDoc), 18, 1);
}

void CMainFrame::UpdateMountedDataStructureView()
{
	int32 groupId, itemId;
	uint64 offset;
	
	this->mountedDataStructuresView.Clear();

	foreach(it, this->mountedStructures)
	{
		const SStructure &refStruct = g_structureSets[this->activeStructureSet - 1].structures[(*it).structureIndex];
		
		groupId = this->mountedDataStructuresView.InsertGroup(refStruct.name);
		offset = (*it).offset;
		
		foreach(it2, refStruct.elements)
		{
			itemId = this->mountedDataStructuresView.InsertItem(groupId);
			this->mountedDataStructuresView.SetItemText(CString(offset, NS_HEX), itemId, 0);
			this->mountedDataStructuresView.SetItemText((*it2).name, itemId, 1);
			this->mountedDataStructuresView.SetItemText(TypeToString(*it2), itemId, 2);
			this->mountedDataStructuresView.SetItemText(ValueToString((*it2).type, (*it2).nElements, (*it2).bigEndian, offset, *this->openFiles[this->tabs.GetActiveTab()].pDoc), itemId, 3);
			
			offset += GetDataTypeSize((*it2));
		}
	}
}

//Public Functions
void CMainFrame::LoadConfig()
{
	if(FileExists(STRUCTURESETS_FILENAME))
	{
		ASSERT(0);
	}
	else
	{
		//Create a default structure set as example
		SStructureElement structElem;
		SStructure structure;
		SStructureSet structSet;
		CArray<SStructure> structures;

		structure.name = "AVI File Header";

		//"RIFF"
		structElem.type = TYPE_CHAR;
		structElem.name = "signature";
		structElem.nElements = 4;

		structure.elements.Push(structElem);

		//fileSize
		structElem.type = TYPE_UINT32;
		structElem.name = "fileSize";
		structElem.nElements = 1;
		structElem.bigEndian = false;

		structure.elements.Push(structElem);

		//"AVI "
		structElem.type = TYPE_CHAR;
		structElem.name = "fileType";
		structElem.nElements = 4;

		structure.elements.Push(structElem);

		structures.Push(structure);

		structSet.name = "AVI";
		structSet.structures = structures;
		
		g_structureSets.Push(structSet);
	}

	//Recreate the menu because of the structures
	this->CreateMenu();
}

void CMainFrame::UpdateInfo(bool selectionChanged)
{
	int32 activeTab;
	uint64 start, end;
	
	activeTab = this->tabs.GetActiveTab();
	if(activeTab != -1)
	{
		CHexEdit *pHexEdit = (CHexEdit *)this->tabs.GetChildWindow(activeTab);
		
		if(selectionChanged)
		{
			this->statusBar.SetText(0, "Caret: " + CString(pHexEdit->GetCaretOffset(), NS_HEX));
			
			if(pHexEdit->IsSelectionValid())
			{
				pHexEdit->GetNormalizedSelection(start, end);
				
				this->statusBar.SetText(2, "Selection: " + CString(end - start, NS_HEX));
			}
			else
			{
				start = pHexEdit->GetCaretOffset();
				end = start + 1;
				
				if(selectionChanged)
					this->statusBar.SetText(2, "Selection: 0");
			}
			
			this->UpdateDataTypeViewer(start, end, this->openFiles[activeTab].pDoc);
		}
		else
		{
			this->statusBar.SetText(1, "Cursor: " + CString(pHexEdit->GetCursorOffset(), NS_HEX));
		}
	}
}