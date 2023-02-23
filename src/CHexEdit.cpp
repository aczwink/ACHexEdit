//Class Header
#include "CHexEdit.h"
//Local
#include "globals.h"
//Namespaces
using namespace SJCLib;
//Definitions
#define CHEXEDIT_TIMER_SCROLL_ID 1
#define CHEXEDIT_TIMER_SCROLL_TIMEOUT 25

//Constructor
CHexEdit::CHexEdit()
{
	this->font.Create(18, 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_MODERN, "Consolas");
	this->pDoc = NULL;
	this->isCaretVisible = false;
	this->isCaretInHexView = true;
	this->isCaretInHighBits = true;
	this->selecting = false;
	this->isScrollTimerActive = false;
	this->caretOffset = 0;
	this->cursorOffset = 0;
	this->selectionStartOffset = 0;
	this->selectionEndOffset = 0;
	
	this->charWidth = 0;
	this->charHeight = 0;
	this->lineHeight = 0;
	this->lineSpacingUpper = 1;
	this->lineSpacingLower = 1;
	this->nCharsPerLineOffset = 0;
	this->nBytesPerLine = 0;
	this->nLinesPerWindow = 0;
	this->offsetPanelWidth = 0;
	this->offsetPanelLeftSpacing = 1;
	this->offsetPanelRightSpacing = 4;
	this->hexViewInnerSpacing = 5;
	this->hexViewSpacingLeftRight = 4;
	this->asciiViewInnerSpacing = 4;
	
	//Color Variables
	this->backgroundColor.Set(255, 255, 255); //White
	this->mouseOverBackgroundColor.Set(230, 230, 230); //light grey
	this->hotOffsetPanelColor.Set(20, 150, 220); //light blue 0,120,200
	this->selectionBackgroundColor.Set(173, 214, 255); //light blue
	this->lineOffsetPanelTextColor.Set(128, 128, 128); //mid grey
	this->hexTextColorEvenColumn.Set(85, 85, 85); //mid-dark grey
	this->hexTextColorOddColumn.Set(150, 150, 150); //mid-light grey
	this->asciiViewTextColor.Set(10, 10, 10); //very dark grey

	//Resource Variables
	this->defaultCursor.LoadSystemCursor(IDC_ARROW);
	this->beamCursor.LoadSystemCursor(IDC_IBEAM);
}

//Callbacks
void CHexEdit::OnChangeSelection()
{
	g_mainFrame.UpdateInfo(true);
}

void CHexEdit::OnDestroy()
{
	this->RevokeDragDrop();
}

void CHexEdit::OnFocus(CWindow *pPreviousFocusedWindow)
{
	this->CreateSolidCaret(this->charWidth, this->charHeight);
	this->ShowCaret();
	this->isCaretVisible = true;

	this->RepositionCaret();
}

void CHexEdit::OnKeyDown(uint16 characterCode, uint16 repeatCount, byte scanCode, bool isExtendedKey, bool previousKeyState)
{
	uint64 oldOffset;

	oldOffset = this->caretOffset;

	//Handle CTRL+
	if(this->IsKeyPressed(VK_CONTROL))
	{
		switch(characterCode)
		{
		case 0x43: //C
			this->Copy();
			break;
		}
	}

	//Normal key
	switch(characterCode)
	{
	case VK_TAB:
		{
			this->isCaretInHexView = !this->isCaretInHexView;
			this->RepositionCaret();
			this->InvalidateLine(this->GetLineNumberFromOffset(this->caretOffset));
		}
		break;
	case VK_PRIOR: //page up
		this->MoveCaret(-(this->nLinesPerWindow * this->nBytesPerLine), this->nLinesPerWindow);
		break;
	case VK_NEXT: //page down
		this->MoveCaret(this->nLinesPerWindow * this->nBytesPerLine, this->nLinesPerWindow);
		break;
	case VK_END: //end
		{
			this->caretOffset = this->GetOffsetFromLineNumber(this->GetLineNumberFromOffset(this->caretOffset) + 1) - 1;
			this->RepositionCaret();
			this->InvalidateColumnOffsetLine();
			this->InvalidateLine(this->GetLineNumberFromOffset(this->caretOffset));
		}
		break;
	case VK_HOME: //pos1
		{
			this->caretOffset = this->GetOffsetFromLineNumber(this->GetLineNumberFromOffset(this->caretOffset));
			this->RepositionCaret();
			this->InvalidateColumnOffsetLine();
			this->InvalidateLine(this->GetLineNumberFromOffset(this->caretOffset));
		}
		break;
	case VK_LEFT:
		{
			if(this->isCaretInHexView && !this->isCaretInHighBits)
			{
				//move to lower bits
				this->isCaretInHighBits = true;
				this->RepositionCaret();
			}
			else if(this->isCaretInHexView && this->isCaretInHighBits && !this->caretOffset)
			{
				//do nothing, as we are at the beginning of the file
			}
			else
			{
				this->isCaretInHighBits = false;
				this->MoveCaret(-1, 1);
			}
		}
		break;
	case VK_UP:
		this->MoveCaret(-this->nBytesPerLine, 1);
		break;
	case VK_RIGHT:
		{
			if(this->isCaretInHexView && this->isCaretInHighBits)
			{
				//move to lower bits
				this->isCaretInHighBits = false;
				this->RepositionCaret();
			}
			else
			{
				this->isCaretInHighBits = true;
				this->MoveCaret(1, 1);
			}
		}
		break;
	case VK_DOWN:
		this->MoveCaret(this->nBytesPerLine, 1);
		break;
	}

	if(oldOffset != this->caretOffset)
	{
		this->OnChangeSelection();
	}
}

void CHexEdit::OnLeftMouseButtonDown(uint16 keys, uint16 x, uint16 y)
{
	uint64 offset;

	offset = this->GetOffsetFromCoord(x, y, this->isCaretInHexView);
	this->MoveCaret(int32(offset - this->caretOffset), 0);
	
	this->selecting = true;
	this->SetCapture();
}

void CHexEdit::OnLeftMouseButtonUp(uint16 keys, uint16 x, uint16 y)
{
	this->selecting = false;
	
	if(this->isScrollTimerActive)
	{
		this->KillTimer(CHEXEDIT_TIMER_SCROLL_ID);
		this->isScrollTimerActive = false;
	}
	ReleaseCapture();

	this->OnChangeSelection();
}

void CHexEdit::OnLooseFocus(CWindow *pUpcomingFocusedWindow)
{
	DestroyCaret();
	this->isCaretVisible = false;
}

uint8 CHexEdit::OnMouseActivate(CPopupWindow *pActivatedWnd, uint16 hitTest, uint16 mouseMessage)
{
	if(hitTest == HTCLIENT)
	{
		this->Focus();
		return MA_ACTIVATE;
	}
	return MA_NOACTIVATE;
}

void CHexEdit::OnMouseMove(uint16 keys, int16 x, int16 y)
{
	bool isInHexView;
	
	if(this->selecting)
	{
		POINT pt;
		CRect rcClient, rc;

		pt.x = x;
		pt.y = y;
		this->GetClientRect(rcClient);
		
		//Clip x,y
		if(this->isCaretInHexView)
			this->GetHexViewRect(rc);
		else
			this->GetAsciiViewRect(rc);
		
		if(x < rc.left)
			x = (int16)rc.left;
		if(x > rc.right)
			x = (int16)rc.right;
		if(y < rc.top)
			y = (int16)rc.top;
		if(y > rc.bottom)
			y = (int16)rc.bottom;
		
		//Check if the mouse is outside of the view
		if(rcClient.PointInRect(pt))
		{
			//...Mouse is inside the window
			if(this->isScrollTimerActive)
			{
				this->KillTimer(CHEXEDIT_TIMER_SCROLL_ID);
				this->isScrollTimerActive = false;
			}
		}
		else
		{
			//...Mouse is outside the window
			if(!this->isScrollTimerActive)
			{
				this->SetTimer(CHEXEDIT_TIMER_SCROLL_ID, CHEXEDIT_TIMER_SCROLL_TIMEOUT);
				this->isScrollTimerActive = true;
			}
		}
		
		this->caretOffset = this->GetOffsetFromCoord(x, y, isInHexView);
		this->InvalidateRange(this->selectionEndOffset, this->caretOffset);
		this->selectionEndOffset = this->caretOffset;
		if(this->selectionStartOffset <= this->selectionEndOffset)
		{
			this->selectionEndOffset++;
			this->isCaretInHighBits = false;
		}
		else
		{
			this->isCaretInHighBits = true;
		}
	}

	this->RepositionCaret();
	this->UpdateCursorPos();
	this->OnMoveCursor();
}

void CHexEdit::OnMouseWheel(int16 zDelta, uint16 keys)
{
	uint32 scrollLines;
	
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
	
	if(scrollLines <= 1)
		scrollLines = 3;
	
	this->Scroll((-zDelta / WHEEL_DELTA) * scrollLines);
}

void CHexEdit::OnMoveCursor()
{
	g_mainFrame.UpdateInfo(false);
}

void CHexEdit::OnPaint()
{
	uint16 top, bottom;
	uint32 i, firstLine, lastLine;
	CRect rcClient, rcPaint;
	CPointer<CDeviceContext> pMemDC;
	CPointer<CBitmap> pBackbuffer;
	CPaintDC dc(this);
	
	this->GetClientRect(rcClient);
	
	//Get the area we need to draw
	dc.GetPaintRect(rcPaint);
	
	//Create a backbuffer, which we can render to
	pMemDC = dc.CreateCompatibleDC();
	pBackbuffer = dc.CreateCompatibleBitmap(rcClient.GetWidth(), rcClient.GetHeight()); //This must be created from paint dc!
	pMemDC->SelectObject(pBackbuffer);

	//Always erase the back buffer
	pMemDC->FillSolidRect(rcPaint, this->backgroundColor);
	//Select the font
	pMemDC->SelectObject(this->font);
	//Always draw without background as we erased the back buffer
	pMemDC->SetBackgroundMode(false);
	
	//Check if we need to redraw the column-panel
	if(rcPaint.top == 0)
	{
		this->DrawColumnOffsetLine(*pMemDC);
	}
	top = 0;
	bottom = 0;
	if(rcPaint.top >= this->GetColumnOffsetLineHeight())
		top = (uint16)(rcPaint.top - this->GetColumnOffsetLineHeight());
	if(rcPaint.bottom >= this->GetColumnOffsetLineHeight())
		bottom = (uint16)(rcPaint.bottom - this->GetColumnOffsetLineHeight());
	
	//Check which lines we need to draw
	firstLine = this->GetScrollPos() + (top / this->lineHeight);
	lastLine = this->GetScrollPos() + (bottom / this->lineHeight);
	
	//Draw lines
	for(i = firstLine; i <= lastLine; i++)
	{
		this->DrawLine(i, (uint16)rcPaint.left, (uint16)rcPaint.right, *pMemDC);
	}
	
	//Present the backbuffer
	dc.BitBlt(rcPaint.left, rcPaint.top, rcPaint.GetWidth(), rcPaint.GetHeight(), pMemDC, rcPaint.left, rcPaint.top, SRCCOPY);
}

bool CHexEdit::OnRequestCursor(const CWindow *pContainingWindow, uint16 hitTest, uint16 msgId)
{
	if(hitTest == HTCLIENT)
	{
		POINT pt;
		CRect rcHexView, rcAsciiView;
		
		GetCursorPos(&pt);
		this->ScreenToClient(pt);
		this->GetHexViewRect(rcHexView);
		this->GetAsciiViewRect(rcAsciiView);

		if(rcHexView.PointInRect(pt) || rcAsciiView.PointInRect(pt))
		{
			this->beamCursor.SetAsActive();
		}
		else
		{
			this->defaultCursor.SetAsActive();
		}
		
		return true;
	}
	
	CWindow::RespondDefault();
	return false;
}

void CHexEdit::OnSetFont(const SJCWinLib::CFont &refFont)
{
	SIZE size;
	TEXTMETRIC tm;
	CPointer<CDeviceContext> pDC;

	this->font = refFont;

	//Get metrics for the font
	pDC = this->GetDC();
	pDC->SelectObject(refFont);
	pDC->GetTextMetricsA(&tm);
	size = pDC->GetTextExtentPoint(CString("FF")); //monospaced

	this->lineHeight = (uint8)(tm.tmHeight + tm.tmExternalLeading + this->lineSpacingUpper + this->lineSpacingLower);
	this->charWidth = (uint8)tm.tmAveCharWidth;
	this->charHeight = (uint8)tm.tmHeight;
	this->hexViewByteWidth = (uint8)size.cx;
}

void CHexEdit::OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight)
{
	this->CalcLayoutInfo();
	this->RepositionCaret();
}

void CHexEdit::OnVerticalScroll(uint16 scrollCode)
{
	switch(scrollCode)
	{
	case SB_LINEUP:
		{
			this->Scroll(-1);
		}
		break;
	case SB_LINEDOWN:
		{
			this->Scroll(1);
		}
		break;
	case SB_PAGEUP:
		{
			this->Scroll(-(int16)this->nLinesPerWindow);
		}
		break;
	case SB_PAGEDOWN:
		{
			this->Scroll(this->nLinesPerWindow);
		}
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			this->ScrollToLine(this->GetScrollTrackPos());
		}
		break;
	case SB_TOP:
		{
			this->ScrollToLine(0);
		}
		break;
	case SB_BOTTOM:
		{
			this->ScrollToLine(this->GetScrollRange());
		}
		break;
	}
}

//Private Functions
void CHexEdit::CalcLayoutInfo()
{
	uint16 useableWidth, elementWidth;
	uint64 maxOffset;
	CRect rcClient;
	
	this->GetClientRect(rcClient);
	
	//Get size of the line offset panel
	maxOffset = this->pDoc->GetSize();
	this->nCharsPerLineOffset = CString(maxOffset, NS_HEX, false).GetLength();
	this->offsetPanelWidth = this->offsetPanelLeftSpacing + this->nCharsPerLineOffset * this->charWidth + this->offsetPanelRightSpacing;
	
	//Calculate how many bytes fit a line
	useableWidth = (uint16)rcClient.right;
	useableWidth -= this->GetLineOffsetPanelWidth();
	useableWidth--; //left 1px vertical border
	useableWidth -= 2 * this->hexViewInnerSpacing; //in hex view we have spacing left and right
	useableWidth--; //right 1px vertical border
	useableWidth -= 2 * this->asciiViewInnerSpacing; //in ascii view we have spacing left and right

	elementWidth = this->GetHexViewByteWidth() + this->charWidth;

	this->nBytesPerLine = useableWidth / elementWidth; //one 'element' has a hex and a ascii representation
	if(!this->nBytesPerLine)
		this->nBytesPerLine = 1;

	//Number of lines
	this->nLinesPerWindow = (rcClient.GetHeight() - this->lineHeight - 1 - this->hexViewInnerSpacing) / this->lineHeight; //there is a blank at the top followed by a 1px border line
	this->nLines = (uint32)(this->pDoc->GetSize() / this->nBytesPerLine);

	if(this->nLinesPerWindow > this->nLines)
	{
		this->ShowScrollBar(false);
		this->SetScrollRange(0);
	}
	else
	{
		this->SetScrollPageSize(this->nLinesPerWindow);
		this->SetScrollRange(this->nLines);
	}
	
	//Update
	this->Invalidate();
}

void CHexEdit::Copy()
{
	if(this->IsSelectionValid())
	{
		if(OpenClipboard(this->hWnd))
		{
			byte *pClipBoardBuffer;
			uint32 size;
			uint64 start, end;
			HGLOBAL hGlobal;
			
			this->GetNormalizedSelection(start, end);
			size = (uint32)(end-start);
			
			EmptyClipboard();

			if(this->isCaretInHexView)
			{
				byte b;
				uint16 outSize;
				byte *pTmp;
				
				hGlobal = GlobalAlloc(GMEM_MOVEABLE, 3 * size+1);
				pClipBoardBuffer = (byte *)GlobalLock(hGlobal);
				pClipBoardBuffer[3 * size] = 0;
				pTmp = pClipBoardBuffer;

				for(uint32 i = 0; i < size; i++)
				{
					b = *this->pDoc->GetData(start + i, 1, outSize);
					const CString &refByte = CString((uint32)b, NS_HEX, false, 2);

					*pTmp++ = refByte[0];
					*pTmp++ = refByte[1];
					*pTmp++ = ' ';
				}
			}
			else
			{
				hGlobal = GlobalAlloc(GMEM_MOVEABLE, size+1);
				pClipBoardBuffer = (byte *)GlobalLock(hGlobal);
				pClipBoardBuffer[size] = 0;
				this->pDoc->CopyData(pClipBoardBuffer, start, size);
			}
			GlobalUnlock(hGlobal);
			SetClipboardData(CF_TEXT, hGlobal);
			CloseClipboard();
		}
	}
}

void CHexEdit::DrawColumnOffsetLine(CDeviceContext &refDC) const
{
	uint8 columnIndex;
	uint16 x;
	SIZE size;
	CRect rcClient, rcBackground;
	CString asciiText;

	this->GetClientRect(rcClient);
	
	//Draw seperator
	x = this->GetLineOffsetPanelWidth();
	refDC.MoveTo(x, 0);
	refDC.LineTo(x, this->GetColumnOffsetLineHeight());

	//Draw hex view column offset
	x = this->GetLineOffsetPanelWidth() + this->hexViewInnerSpacing; //left spacing
	
	//Draw the background for the mouse over column
	rcBackground.left = x + this->GetColumnFromOffset(this->cursorOffset) * this->GetHexViewByteWidth();
	rcBackground.bottom = this->lineHeight;
	rcBackground.right = rcBackground.left + this->GetHexViewByteWidth();

	refDC.FillSolidRect(rcBackground, this->mouseOverBackgroundColor);
	
	for(columnIndex = 0; columnIndex < this->nBytesPerLine; columnIndex++)
	{
		//Set text color
		if(this->GetColumnFromOffset(this->caretOffset) == columnIndex)
		{
			refDC.SetTextColor(this->hotOffsetPanelColor);
		}
		else
		{
			refDC.SetTextColor(this->lineOffsetPanelTextColor);
		}

		this->DrawTextA(x, this->lineSpacingUpper, columnIndex, refDC);
		x += this->GetHexViewByteWidth();
	}

	x += this->hexViewInnerSpacing; //right spacing

	//Draw seperator
	x++;
	refDC.MoveTo(x, 0);
	refDC.LineTo(x, this->GetColumnOffsetLineHeight());

	//Draw ascii view column offset
	x += this->asciiViewInnerSpacing;

	//Draw the background for the mouse over column
	rcBackground.left = x + this->GetColumnFromOffset(this->cursorOffset) * this->charWidth;
	rcBackground.bottom = this->lineHeight;
	rcBackground.right = rcBackground.left + this->charWidth;
	
	refDC.FillSolidRect(rcBackground, this->mouseOverBackgroundColor);
	
	for(columnIndex = 0; columnIndex < this->nBytesPerLine; columnIndex++)
	{
		//Set text color
		if(this->GetColumnFromOffset(this->caretOffset) == columnIndex)
		{
			refDC.SetTextColor(this->lineOffsetPanelTextColor);
			size = refDC.GetTextExtentPoint(asciiText);
			refDC.TextOutA(x, this->lineSpacingUpper, asciiText);
			x += (uint16)size.cx;
			asciiText.Release();
		}
		else if(this->GetColumnFromOffset(this->caretOffset)+1 == columnIndex)
		{
			refDC.SetTextColor(this->hotOffsetPanelColor);
			size = refDC.GetTextExtentPoint(asciiText);
			refDC.TextOutA(x, this->lineSpacingUpper, asciiText);
			x += (uint16)size.cx;
			asciiText.Release();
		}
		asciiText += CString((uint32)(columnIndex % 16), NS_HEX, false);
	}
	refDC.SetTextColor(this->lineOffsetPanelTextColor);
	refDC.TextOutA(x, this->lineSpacingUpper, asciiText);

	//Draw vertical seperator
	refDC.MoveTo(0, this->GetColumnOffsetLineHeight());
	refDC.LineTo(rcClient.right, this->GetColumnOffsetLineHeight());
}

void CHexEdit::DrawAsciiViewLine(uint64 bufOffset, uint8 hotColumn, uint8 selectedColumn, uint8 nSelectedColumns, uint16 y, CDeviceContext &refDC) const
{
	byte *pBuffer;
	uint8 columnIndex;
	uint16 x, nCharsToDraw;
	CRect rcAsciiView, rcSelection;
	CString asciiText;
	
	this->GetAsciiViewRect(rcAsciiView);
	refDC.SetTextColor(this->asciiViewTextColor);

	//Draw the selection
	rcSelection.left = rcAsciiView.left + this->asciiViewInnerSpacing + selectedColumn * this->charWidth;
	rcSelection.top = y;
	rcSelection.right = rcSelection.left + nSelectedColumns * this->charWidth;
	rcSelection.bottom = rcSelection.top + this->charHeight;

	refDC.FillSolidRect(rcSelection, this->selectionBackgroundColor);

	//Get the buffer
	pBuffer = this->pDoc->GetData(bufOffset, this->nBytesPerLine, nCharsToDraw);
	
	//Draw the text
	for(columnIndex = 0; columnIndex < nCharsToDraw; columnIndex++)
	{
		asciiText += this->GetDisplayChar(pBuffer[columnIndex]);
		
		//Mark the caret pos if in hex view
		if(this->isCaretInHexView && columnIndex == hotColumn)
		{
			x = (uint16)rcAsciiView.left + this->asciiViewInnerSpacing + this->GetColumnFromOffset(this->caretOffset) * this->charWidth;
			
			refDC.MoveTo(x, y + this->charHeight);
			refDC.LineTo(x + this->charWidth, y + this->charHeight);
		}
	}
	refDC.TextOutA(rcAsciiView.left + this->asciiViewInnerSpacing, y, asciiText);
}

void CHexEdit::DrawHexViewLine(uint64 bufOffset, uint8 hotColumn, uint8 selectedColumn, uint8 nSelectedColumns, uint16 y, CDeviceContext &refDC) const
{
	byte *pBuffer;
	uint8 columnIndex;
	uint16 x, x1, nCharsToDraw;
	CRect rcSelection;
	
	x = this->GetLineOffsetPanelWidth() + this->hexViewInnerSpacing; //left spacing
	
	//Draw the selection
	rcSelection.left = x + this->asciiViewInnerSpacing + selectedColumn * this->GetHexViewByteWidth();
	rcSelection.top = y;
	rcSelection.right = rcSelection.left + nSelectedColumns * this->GetHexViewByteWidth() - 2*this->hexViewSpacingLeftRight;
	rcSelection.bottom = rcSelection.top + this->charHeight;
	
	refDC.FillSolidRect(rcSelection, this->selectionBackgroundColor);

	//Get the buffer
	pBuffer = this->pDoc->GetData(bufOffset, this->nBytesPerLine, nCharsToDraw);

	for(columnIndex = 0; columnIndex < nCharsToDraw; columnIndex++)
	{
		if(columnIndex & 1)
		{
			refDC.SetTextColor(this->hexTextColorOddColumn);
		}
		else
		{
			refDC.SetTextColor(this->hexTextColorEvenColumn);
		}
		
		this->DrawTextA(x, y, pBuffer[columnIndex], refDC);
		x += this->GetHexViewByteWidth();

		//Mark the caret pos if in ascii view
		if(!this->isCaretInHexView && columnIndex == hotColumn)
		{
			x1 = this->GetLineOffsetPanelWidth() + this->hexViewInnerSpacing + this->GetColumnFromOffset(this->caretOffset) * this->GetHexViewByteWidth();
			x1 += this->hexViewInnerSpacing;
			
			refDC.MoveTo(x1, y + this->charHeight);
			refDC.LineTo(x1 + this->hexViewByteWidth, y + this->charHeight);
		}
	}
}

void CHexEdit::DrawLine(uint32 lineNumber, uint16 xFrom, uint16 xTo, CDeviceContext &refDC) const
{
	uint8 screenLineNumber, hotColumn, selectedColumnFrom, selectedColumns;
	uint16 x, yLine, y;
	uint64 lineBufferOffset;
	CRect rcHexView, rcAsciiView;
	
	if(lineNumber > this->nLines)
		return; //there can be no text here
	
	screenLineNumber = lineNumber - this->GetScrollPos();
	lineBufferOffset = this->GetOffsetFromLineNumber(lineNumber);
	yLine = this->GetColumnOffsetLineHeight() + screenLineNumber * this->lineHeight + this->hexViewInnerSpacing; //y offset of the line
	y = yLine + this->lineSpacingUpper; //the text is putted a bit down
	hotColumn = (lineNumber == this->GetLineNumberFromOffset(this->caretOffset) ? this->GetColumnFromOffset(this->caretOffset) : -1);

	this->GetSelectionParams(lineNumber, selectedColumnFrom, selectedColumns);

	this->GetHexViewRect(rcHexView);
	this->GetAsciiViewRect(rcAsciiView);

	//Draw the line offset panel if needed
	if(xFrom <= this->GetLineOffsetPanelWidth())
		this->DrawLineOffsetPanel(lineNumber, yLine, refDC);
	
	//Draw a seperator
	x = this->GetLineOffsetPanelWidth();
	//x++; //1 pixel width
	refDC.MoveTo(x, y - this->hexViewInnerSpacing);
	refDC.LineTo(x, y - this->hexViewInnerSpacing + this->lineHeight + 1);
	
	//Draw hex view
	if(xTo > rcHexView.right && xFrom < rcHexView.left || IN_RANGE(xTo, rcHexView.left, rcHexView.right))
	{
		this->DrawHexViewLine(lineBufferOffset, hotColumn, selectedColumnFrom, selectedColumns, y, refDC);
	}
	
	//Draw a seperator
	x = (uint16)(rcHexView.right + 1);
	refDC.MoveTo(x, yLine - this->hexViewInnerSpacing);
	refDC.LineTo(x, yLine - this->hexViewInnerSpacing + this->lineHeight + 1);
	
	//Draw the ascii view part if needed
	if(IN_RANGE(xTo, rcAsciiView.left, rcAsciiView.right))
	{
		this->DrawAsciiViewLine(lineBufferOffset, hotColumn, selectedColumnFrom, selectedColumns, y, refDC);
	}
}

void CHexEdit::DrawLineOffsetPanel(uint32 lineNumber, uint16 y, CDeviceContext &refDC) const
{
	//If the mouse is over here, draw a mouseover background
	if(this->GetLineNumberFromOffset(this->cursorOffset) == lineNumber)
	{
		CRect rcBackground;

		rcBackground.left = this->offsetPanelLeftSpacing;
		rcBackground.top = y;
		rcBackground.right = rcBackground.left + this->offsetPanelWidth;
		rcBackground.bottom = y + this->lineHeight;

		refDC.FillSolidRect(rcBackground, this->mouseOverBackgroundColor);
	}

	//Set text color
	if(this->GetLineNumberFromOffset(this->caretOffset) == lineNumber)
	{
		refDC.SetTextColor(this->hotOffsetPanelColor);
	}
	else
	{
		refDC.SetTextColor(this->lineOffsetPanelTextColor);
	}

	refDC.TextOutA(this->offsetPanelLeftSpacing, y + this->lineSpacingUpper, CString(this->GetOffsetFromLineNumber(lineNumber), NS_HEX, false, this->nCharsPerLineOffset));
}

void CHexEdit::DrawText(uint16 x, uint16 y, byte b, CDeviceContext &refDC) const
{
	const CString &refHexByteText = CString((uint64)b, NS_HEX, false, 2);
	
	refDC.TextOutA(x + this->hexViewSpacingLeftRight, y, refHexByteText);
}

void CHexEdit::GetAsciiViewRect(CRect &rcOut) const
{
	CRect rcHexView;
	
	this->GetClientRect(rcOut);
	this->GetHexViewRect(rcHexView);
	
	rcOut.left = rcHexView.right + 2; //we have a 1px border after the hexview
	rcOut.top = rcHexView.top;
}

char CHexEdit::GetDisplayChar(char c) const
{
	switch(c)
	{
	case -113:
	case -115:
	case -127:
		return '.';
	}
	
	if(IN_RANGE(c, 0, 32))
		return '.';
	return c;
}

void CHexEdit::GetHexViewRect(CRect &rcOut) const
{
	this->GetClientRect(rcOut);
	
	rcOut.left = this->GetLineOffsetPanelWidth();
	rcOut.top = this->GetColumnOffsetLineHeight();
	rcOut.right = rcOut.left + this->hexViewInnerSpacing + this->nBytesPerLine * this->GetHexViewByteWidth() + this->hexViewInnerSpacing;
}

void CHexEdit::GetLineRect(uint32 lineNumber, CRect &rcOut) const
{
	this->GetClientRect(rcOut);

	rcOut.top = this->GetColumnOffsetLineHeight() + this->hexViewInnerSpacing + (lineNumber - this->GetScrollPos()) * this->lineHeight;
	rcOut.bottom = rcOut.top + this->lineHeight;
}

uint64 CHexEdit::GetOffsetFromCoord(uint16 x, uint16 y, bool &refInHexView)
{
	uint8 column;
	uint32 lineNumber;
	CRect rcAsciiView;
	
	if(y >= this->GetColumnOffsetLineHeight())
	{
		y -= this->GetColumnOffsetLineHeight();
		//y is now dependant to first line
		lineNumber = y / this->lineHeight + this->GetScrollPos();

		if(x < this->GetLineOffsetPanelWidth())
			return this->GetOffsetFromLineNumber(lineNumber);

		this->GetAsciiViewRect(rcAsciiView);

		if(x >= rcAsciiView.left)
		{
			refInHexView = false; //coord is in ascii view
			
			x -= (uint16)rcAsciiView.left;
			
			column = x / this->charWidth;
			if(column >= this->nBytesPerLine)
				column = this->nBytesPerLine - 1;
			
			//x is now dependat to first column in ascii view
			return this->GetOffsetFromLineNumber(lineNumber) + column;
		}

		refInHexView = true; //position is in hex view
		
		x -= this->GetLineOffsetPanelWidth();
		//x is now dependant to first column in hex view

		column = x / this->GetHexViewByteWidth();
		if(column > 0 && x % this->GetHexViewByteWidth() < this->hexViewSpacingLeftRight)
			column--;
		if(column > this->nBytesPerLine)
			column = this->nBytesPerLine;
		
		return this->GetOffsetFromLineNumber(lineNumber) + column;
	}
	
	return UINT64_MAX;
}

void CHexEdit::GetSelectionParams(uint32 lineNumber, uint8 &refColumn, uint8 &refCount) const
{
	uint64 start, end;

	//Values for "not in the selection"
	refColumn = -1;
	refCount = 0;

	//We don't have a selection
	if(!this->IsSelectionValid())
		return;
	
	this->GetNormalizedSelection(start, end);

	//the selection begins beyond the line
	if(start > this->GetOffsetFromLineNumber(lineNumber+1) - 1)
		return;

	//the selection begins before the line
	if(start < this->GetOffsetFromLineNumber(lineNumber))
	{
		//end is at least inside the line
		if(end > this->GetOffsetFromLineNumber(lineNumber))
		{
			refColumn = 0;
			refCount = (uint8)MIN(end - this->GetOffsetFromLineNumber(lineNumber), this->nBytesPerLine - refColumn);
		}

		return;
	}

	//the selection begins in the line
	refColumn = uint8(start - this->GetOffsetFromLineNumber(lineNumber));
	refCount = (uint8)MIN(end - start, this->nBytesPerLine - refColumn);
}

void CHexEdit::InvalidateRange(uint64 offsetFrom, uint64 offsetTo)
{
	uint32 currentLine;
	
	if(offsetTo < offsetFrom)
	{
		uint64 tmp;
		
		tmp = offsetTo;
		offsetTo = offsetFrom;
		offsetFrom = tmp;
	}
	
	for(currentLine = this->GetLineNumberFromOffset(offsetFrom); currentLine <= this->GetLineNumberFromOffset(offsetTo); currentLine++)
	{
		this->InvalidateLine(currentLine);
	}
}

void CHexEdit::MoveCaret(int32 delta, uint8 scrollAmount)
{
	uint8 oldColumn;
	uint32 oldLine, newLine;

	oldColumn = this->GetColumnFromOffset(this->caretOffset);
	oldLine = this->GetLineNumberFromOffset(this->caretOffset);
	//move caret
	if(delta < 0 && -delta > this->caretOffset)
	{
		//means we would move to negative caret offset
		//simply stay where we are
		return;
	}
	else if(this->caretOffset + delta < this->pDoc->GetSize())
	{
		this->caretOffset += delta;
	}
	else
	{
		this->caretOffset = this->pDoc->GetSize() - 1;
	}
	newLine = this->GetLineNumberFromOffset(this->caretOffset);
	
	//Check if we need to scroll
	if(oldLine != newLine && !this->IsLineVisible(newLine))
	{
		if(delta > 0)
			this->Scroll(scrollAmount);
		else
			this->Scroll(-scrollAmount);
	}
	else
	{
		this->RepositionCaret();
	}
	if(!this->isCaretVisible)
	{
		this->ScrollToLine(this->GetLineNumberFromOffset(this->caretOffset));
	}

	//we need to redraw the line offset panel because of the hot displaying
	if(oldColumn != this->GetColumnFromOffset(this->caretOffset))
	{
		this->InvalidateColumnOffsetLine();
	}

	//Check the selection and what we need to redraw
	if(this->IsKeyPressed(VK_SHIFT)) //extend selection
	{
		this->InvalidateRange(this->selectionEndOffset, this->caretOffset);
		this->selectionEndOffset = this->caretOffset+1;
	}
	else //clear selection
	{
		this->InvalidateRange(this->selectionStartOffset, this->selectionEndOffset);
		this->selectionStartOffset = this->caretOffset;
		this->selectionEndOffset = this->caretOffset;
	}

	//If we changed the line then both need to be redrawn
	if(oldLine != newLine)
	{
		this->InvalidateLine(oldLine);
		this->InvalidateLine(newLine);
	}
	
	this->RepositionCaret();
}

void CHexEdit::RepositionCaret()
{
	uint32 x, y;
	
	if(this->GetFocusedWindow() != this)
		return;

	if(this->caretOffset > this->pDoc->GetSize())
		this->caretOffset = this->pDoc->GetSize() - 1;
	
	if(!this->IsLineVisible(this->GetLineNumberFromOffset(this->caretOffset)))
	{
		this->ShowCaret(false);
		return;
	}
	
	if(this->isCaretInHexView)
	{
		x = this->GetLineOffsetPanelWidth() + this->hexViewInnerSpacing + this->hexViewSpacingLeftRight + this->GetColumnFromOffset(this->caretOffset) * this->GetHexViewByteWidth();
		if(!this->isCaretInHighBits)
			x += this->charWidth;
	}
	else
	{
		CRect rc;

		this->GetAsciiViewRect(rc);
		x = rc.left + this->asciiViewInnerSpacing + this->GetColumnFromOffset(this->caretOffset) * this->charWidth;
	}

	y = this->GetColumnOffsetLineHeight() + this->hexViewInnerSpacing + (this->GetLineNumberFromOffset(this->caretOffset) - this->GetScrollPos()) * this->lineHeight;
	
	SetCaretPos(x, y);
	this->ShowCaret();
}

void CHexEdit::Scroll(int32 dy)
{
	CRect rcClip, rcUpdate;
	
	this->GetClientRect(rcClip);
	rcClip.top = this->GetColumnOffsetLineHeight() + 1; //the top bar does not need to be scrolled
	rcUpdate = rcClip;
	
	//scroll up
	if(dy < 0)
	{
		dy = -(int32)MIN((uint32)-dy, this->GetScrollPos());
		rcClip.top += -dy * this->lineHeight;
	}
	//scroll down
	else if(dy > 0)
	{
		dy = MIN((uint32)dy, this->nLines - this->nLinesPerWindow - this->GetScrollPos() + 1);
		rcClip.bottom = rcClip.top + (this->nLinesPerWindow - dy) * this->lineHeight;
	}
	
	if(dy)
	{
		//adjust the scrollbar positions
		this->SetScrollPos(this->GetScrollPos() + dy);
		this->UpdateCursorPos();
		this->RepositionCaret();
		
		CWindow::Scroll(0, -dy * this->lineHeight, NULL, &rcClip);
		
		rcUpdate.Subtract(rcClip);
		this->InvalidateRect(rcUpdate);
	}
}

void CHexEdit::ScrollToLine(uint32 lineNumber)
{
	uint32 currentLineNumber;
	
	currentLineNumber = this->GetScrollPos();
	if(lineNumber > this->GetScrollRange())
		lineNumber = this->GetScrollRange();

	if(currentLineNumber != lineNumber)
	{
		this->SetScrollPos(lineNumber);
		this->UpdateCursorPos();
		this->RepositionCaret();
		this->Invalidate();
	}
}

void CHexEdit::UpdateCursorPos()
{
	bool isInHexView;
	uint32 oldLine, newLine;
	uint64 oldOffset;
	POINT pt;

	GetCursorPos(&pt);
	this->ScreenToClient(pt);

	oldOffset = this->cursorOffset;
	this->cursorOffset = this->GetOffsetFromCoord((uint16)pt.x, (uint16)pt.y, isInHexView);
	
	if(this->GetColumnFromOffset(oldOffset) != this->GetColumnFromOffset(this->cursorOffset))
	{
		//user has moved the cursor left or right
		this->InvalidateColumnOffsetLine();
	}

	oldLine = this->GetLineNumberFromOffset(oldOffset);
	newLine = this->GetLineNumberFromOffset(this->cursorOffset);

	if(oldLine != newLine)
	{
		//Redraw the old hot line and the new one
		this->InvalidateLine(oldLine);
		this->InvalidateLine(newLine);
	}
}

//Public Functions
void CHexEdit::Create(IBinaryDocument *pDoc, const CWindow &refParentWindow)
{
	this->pDoc = pDoc;
	
	CWindow::Create(WS_VSCROLL, 0, refParentWindow);
	this->OnSetFont(this->font);
}

void CHexEdit::Find(const byte *pBuffer, uint32 bufferSize)
{
	uint32 lineNumber;
	uint64 offset;
	
	offset = this->pDoc->Find(pBuffer, bufferSize, this->caretOffset);
	if(offset == IBINARYDOCUMENT_FIND_NOMATCH)
	{
		ASSERT(0);
	}
	else
	{
		lineNumber = this->GetLineNumberFromOffset(offset);
		
		this->selectionStartOffset = offset;
		this->selectionEndOffset = offset + bufferSize;
		this->caretOffset = this->selectionEndOffset;
		this->isCaretInHighBits = false;
		
		if(!this->IsLineVisible(lineNumber))
		{
			this->ScrollToLine(lineNumber);
		}
		else
		{
			this->RepositionCaret();
			this->InvalidateRange(this->selectionStartOffset, this->selectionEndOffset);
		}
		this->OnChangeSelection();
	}
}

void CHexEdit::GotoOffset(uint64 offset)
{
	uint32 lineNumber;
	uint64 oldCaretOffset;

	oldCaretOffset = this->caretOffset;
	this->caretOffset = offset;
	
	lineNumber = this->GetLineNumberFromOffset(offset);
	if(!this->IsLineVisible(lineNumber))
	{
		this->ScrollToLine(lineNumber);
	}
	else
	{
		this->RepositionCaret();
		this->InvalidateLine(this->GetLineNumberFromOffset(oldCaretOffset));
		this->InvalidateLine(this->GetLineNumberFromOffset(this->caretOffset));
	}
}