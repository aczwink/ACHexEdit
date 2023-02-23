//Class Header
#include "CDataTypeView.h"
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

//Callbacks
bool CDataTypeView::OnRightClick(uint16 item, uint16 subItem, POINT pt)
{
	void *pData;
	HGLOBAL hGlobal;
	CMenu menu;

	this->ClientToScreen(pt);

	menu.CreatePopup();

	menu.AppendItem("Copy", 1);
	if(menu.TrackPopupMenu(ALIGN_LEFT, VALIGN_TOP, pt.x, pt.y, *this->GetParent()) == 1)
	{
		const CString &refData = this->GetItemText(item, subItem);
		
		if(OpenClipboard(this->hWnd))
		{
			EmptyClipboard();
			hGlobal = GlobalAlloc(GMEM_MOVEABLE, refData.GetLength() + 1);

			if(hGlobal)
			{
				pData = GlobalLock(hGlobal);
				MemCopy(pData, refData.GetC_Str(), refData.GetLength());
				((byte *)pData)[refData.GetLength()] = 0;
				GlobalUnlock(hGlobal);

				SetClipboardData(CF_TEXT, hGlobal);
			}

			CloseClipboard();
		}
	}


	return true;
}