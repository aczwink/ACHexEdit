//Libs
#include <SJCCommCtrl.h>

class CDataTypeView : public SJCWinLibCommCtrl::CListView
{
private:
	//Callbacks
	bool OnRightClick(uint16 item, uint16 subItem, POINT pt);
};