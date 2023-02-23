//Libs
#include <SJCWinLib.h>
//Namespaces
using namespace SJCWinLib;

class CAddButton : public CButton
{
private:
	//Callbacks
	void OnClick();
};

class CManageStructureSetsDialog : public CDialog
{
private:
	//Variables
	SJCWinLib::CFont font;
	CListBox structureSets;
	CAddButton addButton;
	CButton editButton;
	CButton deleteButton;
	//Callbacks
	void OnInit();
public:
	//Public Functions
	void Create(const CWindow &refParentWindow);
	void UpdateList();
};