//Class Header
#include "CHexTextField.h"
//Local
#include "CFindDialog.h"
//Global Variables
static WNDPROC g_origWndProc;

//Subclass procedure
LRESULT CALLBACK HexTextFieldSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CHAR:
		{
			switch(wParam)
			{
			case VK_BACK:
				return CallWindowProc(g_origWndProc, hWnd, msg, wParam, lParam);
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return CallWindowProc(g_origWndProc, hWnd, msg, wParam, lParam);
			case 'a':
			case 'A':
			case 'b':
			case 'B':
			case 'c':
			case 'C':
			case 'd':
			case 'D':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
				return CallWindowProc(g_origWndProc, hWnd, msg, toupper((int32)wParam), lParam);
			default:
				{
					MessageBeep(MB_OK);
					return 0;
				}
				break;
			}
		}
		break;
	}
	
	return CallWindowProc(g_origWndProc, hWnd, msg, wParam, lParam);
}

//Callbacks
void CHexTextField::OnChange()
{
	((CFindDialog *)(this->GetParent()))->UpdateValue();
}

//Public Functions
void CHexTextField::Create(const CWindow &refParentWindow)
{
	CEdit::Create(0, refParentWindow);
	g_origWndProc = (WNDPROC)SetWindowLongPtr(this->hWnd, GWLP_WNDPROC, (LONG_PTR)HexTextFieldSubclassProc);
}