//Class Header
#include "CManageStructureSetsDialog.h"
//Local
#include "globals.h"
//Definitions
#define DIALOG_WIDTH 300
#define DIALOG_HEIGHT 350
#define DIALOG_STDSPACING 5
#define DIALOG_STRUCTURESETSLISTBOX_WIDTH (DIALOG_WIDTH * 2 / 3)
#define DIALOG_BUTTONHEIGHT 14
#define DIALOG_BUTTONSPACING 2

#define DIALOG_EDITBUTTON_Y (DIALOG_STDSPACING + DIALOG_BUTTONHEIGHT + DIALOG_BUTTONSPACING)
#define DIALOG_DELETEBUTTON_Y (DIALOG_EDITBUTTON_Y + DIALOG_BUTTONHEIGHT + DIALOG_BUTTONSPACING)

//CAddButton Callbacks
void CAddButton::OnClick()
{
	CPromptDialog dlg;
	CManageStructureSetsDialog *pParent;

	pParent = (CManageStructureSetsDialog *)this->GetParent();

	dlg.Create("Add structure set", "", this->GetParent());
	if(dlg.Run() == IDOK)
	{
		SStructureSet structSet;
		
		foreach(it, g_structureSets)
		{
			if((*it).name.ToLowercase() == dlg.GetValue().ToLowercase())
			{
				pParent->MessageBox(L"This set does already exist.", L"Error", MB_ICONERROR);
				return;
			}
		}

		structSet.name = dlg.GetValue();

		g_structureSets.Push(structSet);
		pParent->UpdateList();
	}
}

//Callbacks
void CManageStructureSetsDialog::OnInit()
{
	this->structureSets.Create(WS_BORDER, 0, *this);
	this->structureSets.SetFont(this->font);
	this->structureSets.SetPos(this->DialogToScreenUnits(CRect(DIALOG_STDSPACING, DIALOG_STDSPACING, DIALOG_STRUCTURESETSLISTBOX_WIDTH, DIALOG_HEIGHT - DIALOG_STDSPACING)));

	this->addButton.Create("Add", 0, *this);
	this->addButton.SetFont(this->font);
	this->addButton.SetPos(this->DialogToScreenUnits(CRect(DIALOG_STRUCTURESETSLISTBOX_WIDTH + DIALOG_STDSPACING, DIALOG_STDSPACING, DIALOG_WIDTH - DIALOG_STDSPACING, DIALOG_STDSPACING + DIALOG_BUTTONHEIGHT)));

	this->editButton.Create("Edit", 0, *this);
	this->editButton.SetFont(this->font);
	this->editButton.SetPos(this->DialogToScreenUnits(CRect(DIALOG_STRUCTURESETSLISTBOX_WIDTH + DIALOG_STDSPACING, DIALOG_EDITBUTTON_Y, DIALOG_WIDTH - DIALOG_STDSPACING, DIALOG_EDITBUTTON_Y + DIALOG_BUTTONHEIGHT)));
	
	this->deleteButton.Create("Delete", 0, *this);
	this->deleteButton.SetFont(this->font);
	this->deleteButton.SetPos(this->DialogToScreenUnits(CRect(DIALOG_STRUCTURESETSLISTBOX_WIDTH + DIALOG_STDSPACING, DIALOG_DELETEBUTTON_Y, DIALOG_WIDTH - DIALOG_STDSPACING, DIALOG_DELETEBUTTON_Y + DIALOG_BUTTONHEIGHT)));

	this->UpdateList();
}

//Public Functions
void CManageStructureSetsDialog::Create(const CWindow &refParentWindow)
{
	this->font.LoadSystemFont(DEFAULT_GUI_FONT);
	
	CDialog::Create("Manage structure sets", DS_MODALFRAME | DS_CENTER | WS_CAPTION | WS_SYSMENU, 0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, &refParentWindow, this->font);
}

void CManageStructureSetsDialog::UpdateList()
{
	foreach(it, g_structureSets)
	{
		this->structureSets.AddItem((*it).name);
	}
}