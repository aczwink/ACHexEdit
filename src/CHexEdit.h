#pragma once
//Libs
#include <SJCWinLib.h>
//Local
#include "IBinaryDocument.h"
//Namespaces
using namespace SJCWinLib;

/*
Limitations
	-Works only with monospaced fonts

Layout:
	HexViewColumnOffset-Bar 1px-vertical-border AsciiViewColumnOffset-Bar
	1px horizontal border
	line-offset-panel 1-px-vertical-border HexView 1px-vertical-border AsciiView
*/
class CHexEdit : public CWindow
{
	friend class CHexViewDropTarget;
private:
	//Variables
	SJCWinLib::CFont font;
	IBinaryDocument *pDoc;
	bool isCaretVisible;
	bool isCaretInHexView;
	bool isCaretInHighBits; //valid only if caret is in hex view
	bool selecting;
	bool isScrollTimerActive;
	uint64 caretOffset;
	uint64 cursorOffset;
	uint64 selectionStartOffset; //inclusive
	uint64 selectionEndOffset; //exclusive
	//Layout Variables
	uint8 charWidth;
	uint8 charHeight;
	uint8 lineHeight;
	uint8 lineSpacingUpper;
	uint8 lineSpacingLower;
	uint8 nCharsPerLineOffset;
	uint8 nBytesPerLine;
	uint8 nLinesPerWindow;
	uint32 nLines;
	uint8 offsetPanelWidth; //not including spacings and borders
	uint8 offsetPanelLeftSpacing;
	uint8 offsetPanelRightSpacing;
	uint8 hexViewByteWidth; //without spacings
	uint16 hexViewInnerSpacing; //inner spacing at the border
	uint16 hexViewSpacingLeftRight;
	uint8 asciiViewInnerSpacing; //same as 'hexViewInnerSpacing'
	//Color Variables
	CColor backgroundColor;
	CColor mouseOverBackgroundColor;
	CColor hotOffsetPanelColor;
	CColor selectionBackgroundColor;
	CColor lineOffsetPanelTextColor;
	CColor hexTextColorEvenColumn;
	CColor hexTextColorOddColumn;
	CColor asciiViewTextColor;
	//Resource Variables
	CCursor defaultCursor;
	CCursor beamCursor;
	//Callbacks
	void OnChangeSelection();
	void OnDestroy();
	void OnFocus(CWindow *pPreviousFocusedWindow);
	void OnKeyDown(uint16 characterCode, uint16 repeatCount, byte scanCode, bool isExtendedKey, bool previousKeyState);
	void OnLeftMouseButtonDown(uint16 keys, uint16 x, uint16 y);
	void OnLeftMouseButtonUp(uint16 keys, uint16 x, uint16 y);
	void OnLooseFocus(CWindow *pUpcomingFocusedWindow);
	uint8 OnMouseActivate(CPopupWindow *pActivatedWnd, uint16 hitTest, uint16 mouseMessage);
	void OnMouseMove(uint16 keys, int16 x, int16 y);
	void OnMouseWheel(int16 zDelta, uint16 keys);
	void OnMoveCursor();
	void OnPaint();
	bool OnRequestCursor(const CWindow *pContainingWindow, uint16 hitTest, uint16 msgId);
	void OnSetFont(const SJCWinLib::CFont &refFont);
	void OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight);
	void OnVerticalScroll(uint16 scrollCode);
	//Functions
	void CalcLayoutInfo();
	void Copy();
	void DrawColumnOffsetLine(CDeviceContext &refDC) const;
	void DrawAsciiViewLine(uint64 bufOffset, uint8 hotColumn, uint8 selectedColumn, uint8 nSelectedColumns, uint16 y, CDeviceContext &refDC) const;
	void DrawHexViewLine(uint64 bufOffset, uint8 hotColumn, uint8 selectedColumn, uint8 nSelectedColumns, uint16 y, CDeviceContext &refDC) const;
	void DrawLine(uint32 lineNumber, uint16 xFrom, uint16 xTo, CDeviceContext &refDC) const;
	void DrawLineOffsetPanel(uint32 lineNumber, uint16 y, CDeviceContext &refDC) const;
	void DrawText(uint16 x, uint16 y, byte b, CDeviceContext &refDC) const;
	void GetAsciiViewRect(CRect &rcOut) const;
	char GetDisplayChar(char c) const;
	void GetHexViewRect(CRect &rcOut) const;
	void GetLineRect(uint32 lineNumber, CRect &rcOut) const;
	uint64 GetOffsetFromCoord(uint16 x, uint16 y, bool &refInHexView);
	void GetSelectionParams(uint32 lineNumber, uint8 &refColumn, uint8 &refCount) const;
	void InvalidateRange(uint64 offsetFrom, uint64 offsetTo);
	void MoveCaret(int32 delta, uint8 scrollAmount);
	void RepositionCaret();
	void Scroll(int32 dy);
	void ScrollToLine(uint32 lineNumber);
	void UpdateCursorPos();
	
	//Inline
	inline uint8 GetColumnFromOffset(uint64 offset) const
	{
		return offset % this->nBytesPerLine;
	}

	inline uint8 GetColumnOffsetLineHeight() const
	{
		return this->lineHeight + 1; //1px vertical border
	}

	inline uint8 GetHexViewByteWidth() const //includes spacings
	{
		return this->hexViewSpacingLeftRight + this->hexViewByteWidth + this->hexViewSpacingLeftRight;
	}
	
	inline uint32 GetLastVisibleLine() const
	{
		return this->GetScrollPos() + this->nLinesPerWindow;
	}
	
	inline uint32 GetLineNumberFromOffset(uint64 offset) const
	{
		return (uint32)(offset / this->nBytesPerLine);
	}

	inline uint8 GetLineOffsetPanelWidth() const
	{
		return this->offsetPanelLeftSpacing + this->offsetPanelWidth + this->offsetPanelRightSpacing + 1; //1px border
	}
	
	inline uint64 GetOffsetFromLineNumber(uint32 lineNumber) const
	{
		return lineNumber * this->nBytesPerLine;
	}

	inline void InvalidateColumnOffsetLine()
	{
		CRect rc;

		this->GetClientRect(rc);
		rc.bottom = this->GetColumnOffsetLineHeight();

		this->InvalidateRect(rc);
	}

	inline void InvalidateLine(uint32 lineNumber)
	{
		CRect rc;

		this->GetLineRect(lineNumber, rc);
		this->InvalidateRect(rc);
	}
	
	inline bool IsKeyPressed(uint16 virtualKey) const
	{
		return GetKeyState(virtualKey) < 0;
	}
	
	inline bool IsLineVisible(uint32 lineNumber) const
	{
		return IN_RANGE(lineNumber, this->GetScrollPos(), this->GetScrollPos() + this->nLinesPerWindow-1);
	}

	inline void ShowCaret(bool show = true)
	{
		if(show && !this->isCaretVisible)
		{
			CWindow::ShowCaret(true);
			this->isCaretVisible = true;
		}
		else if(!show && this->isCaretVisible)
		{
			CWindow::ShowCaret(false);
			this->isCaretVisible = false;
		}
	}
public:
	//Constructor
	CHexEdit();
	//Functions
	void Create(IBinaryDocument *pDoc, const CWindow &refParentWindow);
	void Find(const byte *pBuffer, uint32 bufferSize);
	void GotoOffset(uint64 offset);

	//Inline
	inline uint64 GetCaretOffset() const
	{
		return this->caretOffset;
	}

	inline uint64 GetCursorOffset() const
	{
		return this->cursorOffset;
	}

	inline void GetNormalizedSelection(uint64 &refStart, uint64 &refEnd) const
	{
		if(this->selectionStartOffset <= this->selectionEndOffset)
		{
			refStart = this->selectionStartOffset;
			refEnd = this->selectionEndOffset;
		}
		else
		{
			refStart = this->selectionEndOffset;
			refEnd = this->selectionStartOffset;
		}
	}

	inline bool IsSelectionValid() const
	{
		return this->selectionStartOffset != this->selectionEndOffset;
	}
};

/*
class CHexEdit : public CControl
{
private:
	CFont font;
	uint64 cursorOffset;
	bool isCaretVisible;
	bool isCaretInHexView;
	bool selecting;
	//Layout Variables
	uint32 nLines;
	uint16 nLinesToDraw;
	uint16 charWidth;
	uint16 charWidthWithAdvance; //with the advance which is a kind of spacing between to glyphs
	uint16 charHeight;
	uint16 nCharsPerOffset;
	uint16 offsetLineLeft;
	uint16 offsetPanelTop;
	uint16 offsetPanelWidth;
	uint16 offsetPanelLeftSpacing;
	uint16 offsetPanelRightSpacing;
	uint16 asciiLeft;
	CColor borderColor;
	CColor offsetPanelBackgroundColor;
	CColor offsetPanelHotBackgroundColor;
	CColor offsetPanelTextColor;
	CColor offsetPanelFocusedTextColor;
	CColor asciiTextColor;
	CColor asciiInactiveTextColor;
	CColor selectionBackgroundColor;
	//Overrideable Callbacks
	virtual void OnChangeSelection();

	void OnChar(uint16 characterCode, uint16 repeatCount, byte scanCode, bool isExtendedKey, bool isAltKeyPressed, bool previousKeyState, bool transitionTable);
	void OnKeyDown(uint16 characterCode, uint16 repeatCount, byte scanCode, bool isExtendedKey, bool previousKeyState);
	void OnLeftMouseButtonDown(uint16 keys, uint16 x, uint16 y);
	void OnLeftMouseButtonUp(uint16 keys, uint16 x, uint16 y);
	void OnMouseMove(uint16 keys);
	void OnMouseWheel(int16 zDelta, uint16 keys);
	void OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight);
	void OnVerticalScroll(uint16 scrollCode);
	//Functions
	void CalculatePaintingInfo();
	void ComputeFontMetrics();
	void DrawAsciiLine(uint32 lineIndex, CDC &refMemDC);
	void DrawHexColumn(uint32 columnIndex, CDC &refMemDC);
	void DrawLineOffsetPanel(CDC &refMemDC);
	void DrawOffsetPanel(CDC &refMemDC);
	CRect GetAsciiViewRect() const;
	CRect GetHexViewRect() const;
	void Register(const CModule &refModule);
	void RepositionCaret();
	void Scroll(int32 dy);
	void ScrollToLine(uint32 line);
	void UpdateCursorPos();
	//Inline
	inline uint32 GetCaretLine() const
	{
		return this->GetLineFromOffset(this->caretOffset);
	}

	inline uint64 GetEndOffsetFromLine(uint32 line) const
	{
		return line * this->bytesPerLine + this->bytesPerLine - 1;
	}

	inline uint32 GetFirstVisibleLine() const
	{
		return this->GetLineFromOffset(this->firstLineOffset);
	}
	
	inline uint16 GetStringWidth(uint16 nChars) const
	{
		return (nChars-1) * this->charWidthWithAdvance + this->charWidth;
	}

	inline bool IsCaretVisible() const
	{
		return this->GetFirstVisibleLine() <= this->GetCaretLine() && this->GetLastVisibleLine()  >= this->GetCaretLine();
	}

	inline bool IsLineVisible(uint32 lineNumber) const
	{
		return IN_RANGE(lineNumber, this->GetFirstVisibleLine(), this->GetLastVisibleLine());
	}

	inline bool IsOffsetInSelection(uint64 offset) const
	{
		uint64 start, end;

		this->GetNormalizedSelection(start, end);
		if(start < this->firstLineOffset)
		{
			start = 0;
		}
		else
		{
			start -= this->firstLineOffset;
		}
		if(end < this->firstLineOffset)
		{
			return false;
		}
		else
		{
			end -= this->firstLineOffset;
		}
		
		return this->IsSelectionValid() && start <= offset && offset < end;
	}

	inline bool IsPointInAsciiView(uint16 x, uint16 y) const
	{
		POINT pt = {x, y};
		
		return this->GetAsciiViewRect().PointInRect(pt);
	}

	inline bool IsPointInHexView(uint16 x, uint16 y) const
	{
		POINT pt = {x, y};
		
		return this->GetHexViewRect().PointInRect(pt);
	}
};*/