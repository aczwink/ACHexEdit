//Class Header
#include "CFindDialog.h"
//Namespaces
using namespace SJCLib;
//Definitions
#define DIALOG_WIDTH 200
#define DIALOG_HEIGHT 150

#define DIALOG_TEXTOFFSET_TOP 2
#define DIALOG_TEXTHEIGHT 8
#define DIALOG_INPUTCTRLHEIGHT 14
#define DIALOG_ROWSPACING 8
#define DIALOG_BUTTONWIDTH 40

#define DIALOG_MARGIN 5
#define DIALOG_LEFTCOL DIALOG_MARGIN
#define DIALOG_LEFTCOL_WIDTH 30
#define DIALOG_RIGHTCOL (DIALOG_LEFTCOL + DIALOG_LEFTCOL_WIDTH + DIALOG_MARGIN)
#define DIALOG_RIGHTCOL_WIDTH (DIALOG_WIDTH - DIALOG_RIGHTCOL - DIALOG_MARGIN)

#define DIALOG_ROW1 5
#define DIALOG_ROW_INFO(x) (DIALOG_ROW(x) + DIALOG_TEXTOFFSET_TOP)
#define DIALOG_ROW(x) (DIALOG_ROW1 + (x - 1) * (DIALOG_INPUTCTRLHEIGHT + DIALOG_ROWSPACING))

//Callbacks
void CSearchTypeSelect::OnChangeSelection()
{
	CFindDialog *pDlg;

	pDlg = (CFindDialog *)this->GetParent();


	switch(this->GetSelection())
	{
	case 0: //Hex
		{
			pDlg->stdEditField.Show(SW_HIDE);

			pDlg->valueEdit.Show(SW_SHOW);
			
			pDlg->decimalEdit.Show(SW_HIDE);
			pDlg->byteWidthSelect.Show(SW_HIDE);
			pDlg->littleEndian.Show(SW_HIDE);
			pDlg->bigEndian.Show(SW_HIDE);
		}
		break;
	case 1: //Decimal
		{
			pDlg->stdEditField.Show(SW_HIDE);

			pDlg->valueEdit.Show(SW_HIDE);

			pDlg->decimalEdit.Show(SW_SHOW);
			pDlg->byteWidthSelect.Show(SW_SHOW);
			pDlg->littleEndian.Show(SW_SHOW);
			pDlg->bigEndian.Show(SW_SHOW);
		}
		break;
	case 2: //Text
		{
			pDlg->stdEditField.Show(SW_SHOW);
			
			pDlg->valueEdit.Show(SW_HIDE);
			
			pDlg->decimalEdit.Show(SW_HIDE);
			pDlg->byteWidthSelect.Show(SW_HIDE);
			pDlg->littleEndian.Show(SW_HIDE);
			pDlg->bigEndian.Show(SW_HIDE);
		}
		break;
	}

	pDlg->UpdateValue();
}

//CStdTextField Callbacks
void CStdTextField::OnChange()
{
	CFindDialog *pDlg;
	
	pDlg = (CFindDialog *)this->GetParent();

	pDlg->UpdateValue();
}

//Callbacks
void CEndianRadioButton::OnClick()
{
	CFindDialog *pDlg;
	
	pDlg = (CFindDialog *)this->GetParent();

	if(&pDlg->littleEndian == this)
	{
		pDlg->littleEndian.Check();
		pDlg->bigEndian.Check(false);
	}
	else
	{
		pDlg->littleEndian.Check(false);
		pDlg->bigEndian.Check();
	}

	pDlg->UpdateValue();
}

//CGoButton Callbacks
void CGoButton::OnClick()
{
	((CFindDialog *)this->GetParent())->End(IDOK);
}

//Callbacks
void CFindDialog::OnInit()
{
	//Row 1
	this->type.Create("Type: ", 0, *this);
	this->type.SetFont(font);
	this->type.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_LEFTCOL, DIALOG_ROW_INFO(1), DIALOG_LEFTCOL_WIDTH, DIALOG_TEXTHEIGHT)));

	this->typeSelect.CreateDropDownList(0, *this);
	this->typeSelect.SetFont(font);
	this->typeSelect.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_RIGHTCOL, DIALOG_ROW1, DIALOG_RIGHTCOL_WIDTH, DIALOG_INPUTCTRLHEIGHT)));
	this->typeSelect.AddItem("Hex Values");
	this->typeSelect.AddItem("Decimal");
	this->typeSelect.AddItem("Text");
	this->typeSelect.Select(0);

	//Row 2
	this->value.Create("Value: ", 0, *this);
	this->value.SetFont(font);
	this->value.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_LEFTCOL, DIALOG_ROW_INFO(2), DIALOG_LEFTCOL_WIDTH, DIALOG_TEXTHEIGHT)));
	
	this->stdEditField.Create(0, *this);
	this->stdEditField.SetFont(font);
	this->stdEditField.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_RIGHTCOL, DIALOG_ROW(2), DIALOG_RIGHTCOL_WIDTH, DIALOG_INPUTCTRLHEIGHT)));
	
	this->valueEdit.Create(*this);
	this->valueEdit.SetFont(font);
	this->valueEdit.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_RIGHTCOL, DIALOG_ROW(2), DIALOG_RIGHTCOL_WIDTH, DIALOG_INPUTCTRLHEIGHT)));

	this->decimalEdit.Create(*this);
	this->decimalEdit.SetFont(font);
	this->decimalEdit.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_RIGHTCOL, DIALOG_ROW(2), DIALOG_RIGHTCOL_WIDTH, DIALOG_INPUTCTRLHEIGHT)));
	this->decimalEdit.Show(SW_HIDE);
	
	//Row 3
	this->byteWidthSelect.CreateDropDownList(0, *this);
	this->byteWidthSelect.SetFont(font);
	this->byteWidthSelect.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_LEFTCOL, DIALOG_ROW(3), DIALOG_WIDTH / 2 - DIALOG_LEFTCOL, DIALOG_INPUTCTRLHEIGHT)));
	this->byteWidthSelect.Show(SW_HIDE);
	this->byteWidthSelect.SetCueBanner(L"Byte width");
	this->byteWidthSelect.AddItem("1 byte");
	this->byteWidthSelect.AddItem("2 byte");
	this->byteWidthSelect.AddItem("4 byte");
	this->byteWidthSelect.AddItem("8 byte");
	this->byteWidthSelect.Select(2);

	this->littleEndian.Create(0, *this);
	this->littleEndian.SetFont(font);
	this->littleEndian.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_WIDTH / 2 + DIALOG_MARGIN, DIALOG_ROW(3) - DIALOG_MARGIN, DIALOG_WIDTH / 2 - DIALOG_MARGIN, DIALOG_INPUTCTRLHEIGHT)));
	this->littleEndian.Show(SW_HIDE);
	this->littleEndian.SetText("Little Endian");
	this->littleEndian.Check();
	
	this->bigEndian.Create(0, *this);
	this->bigEndian.SetFont(font);
	this->bigEndian.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_WIDTH / 2 + DIALOG_MARGIN, DIALOG_ROW(3) - DIALOG_MARGIN + DIALOG_INPUTCTRLHEIGHT, DIALOG_WIDTH / 2 - DIALOG_MARGIN, DIALOG_INPUTCTRLHEIGHT)));
	this->bigEndian.Show(SW_HIDE);
	this->bigEndian.SetText("Big Endian");
	
	//Row 4
	this->hexValues.Create("Hex:", 0, *this);
	this->hexValues.SetFont(font);
	this->hexValues.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_LEFTCOL, DIALOG_ROW(4), (DIALOG_WIDTH - DIALOG_LEFTCOL - DIALOG_MARGIN), DIALOG_TEXTHEIGHT)));
	
	this->hexValuesEdit.Create(ES_READONLY, *this);
	this->hexValuesEdit.SetFont(font);
	this->hexValuesEdit.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_LEFTCOL, DIALOG_ROW(4) + DIALOG_TEXTHEIGHT + 2, (DIALOG_WIDTH - DIALOG_LEFTCOL - DIALOG_MARGIN), DIALOG_INPUTCTRLHEIGHT * 3)));

	//Buttons
	this->goButton.Create("Go", 0, *this);
	this->goButton.SetFont(font);
	this->goButton.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_WIDTH - 2 * DIALOG_MARGIN - 2 * DIALOG_BUTTONWIDTH, DIALOG_HEIGHT - DIALOG_INPUTCTRLHEIGHT - DIALOG_MARGIN, DIALOG_BUTTONWIDTH, DIALOG_INPUTCTRLHEIGHT)));
	this->goButton.SetDefault();

	this->closeButton.Create("Close", 0, *this);
	this->closeButton.SetFont(font);
	this->closeButton.SetPos(this->DialogToScreenUnits(CRect().Init(DIALOG_WIDTH - DIALOG_MARGIN - DIALOG_BUTTONWIDTH, DIALOG_HEIGHT - DIALOG_INPUTCTRLHEIGHT - DIALOG_MARGIN, DIALOG_BUTTONWIDTH, DIALOG_INPUTCTRLHEIGHT)));
}

//Public Functions
void CFindDialog::Create(const CWindow &refParentWindow)
{
	this->font.LoadSystemFont(DEFAULT_GUI_FONT);

	this->findBufferSize = 0;
	
	CDialog::Create("Find", DS_MODALFRAME | DS_CENTER | WS_CAPTION | WS_SYSMENU, 0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, &refParentWindow, this->font);
}

void CFindDialog::UpdateValue()
{
	uint32 i;
	byte *pFindBuffer;
	CString text, hexText;
	
	pFindBuffer = this->findBuffer;
	
	switch(this->typeSelect.GetSelection())
	{
	case 0: //Hex
		{
			text = this->valueEdit.GetText();
			
			if(text.GetLength() & 1)
				text += '0';
			this->findBufferSize = text.GetLength() / 2;
			
			for(i = 0; i < text.GetLength(); i += 2)
			{
				*pFindBuffer = 0;
				repeat(2, j)
				{
					if(IN_RANGE(text[i + j], '0', '9'))
					{
						*pFindBuffer |= ((text[i + j] - '0') << (j == 0 ? 4 : 0));
					}
					else if(IN_RANGE(text[i + j], 'A', 'F'))
					{
						*pFindBuffer |= ((text[i + j] - 'A' + 10) << (j == 0 ? 4 : 0));
					}
				}
				pFindBuffer++;
			}
		}
		break;
	case 1: //Decimal
		{
			int64 si64;
			uint64 ui64;
			
			text = this->decimalEdit.GetText();
			si64 = text.ToInt64();
			ui64 = text.ToUInt64();
			
			MemZero(pFindBuffer, 8);
			
			switch(this->byteWidthSelect.GetSelection())
			{
			case 0: //1byte
				{
					int8 si8;
					uint8 ui8;

					si8 = (int8)si64;
					ui8 = (uint8)ui64;
					
					this->findBufferSize = 1;
					
					if(text[0] == '-')
					{
						if(text.GetLength() > 4 || si64 < INT8_MIN)
						{
							this->MessageBox(L"Value is less than minimum value '" + INT8_MIN + '\'', L"Error", MB_ICONERROR);
						}
						else
						{
							*pFindBuffer = si8;
						}
					}
					else
					{
						if(text.GetLength() > 3 || ui64 > UINT8_MAX)
						{
							this->MessageBox(L"Value is greater than maximum value '" + CWString(UINT8_MAX) + L"'", L"Error", MB_ICONERROR);
						}
						else
						{
							*pFindBuffer = ui8;
						}
					}
				}
				break;
			case 1: //2byte
				{
					int16 si16;
					uint16 ui16;
					
					this->findBufferSize = 2;

					if(this->littleEndian.IsChecked())
					{
						si16 = si64 & 0xFFFF;
						ui16 = ui64 & 0xFFFF;
					}
					else
					{
						si16 = ENDIAN_SWAP16(si64);
						ui16 = ENDIAN_SWAP16(ui64);
					}

					if(text[0] == '-')
					{
						if(text.GetLength() > 6 || si64 < INT16_MIN)
						{
							this->MessageBox(L"Value is less than minimum value '" + INT16_MIN + '\'', L"Error", MB_ICONERROR);
						}
						else
						{
							MemCopy(pFindBuffer, &si16, 2);
						}
					}
					else
					{
						if(text.GetLength() > 5 || ui64 > UINT16_MAX)
						{
							this->MessageBox(L"Value is greater than maximum value '" + CWString(UINT16_MAX) + L"'", L"Error", MB_ICONERROR);
						}
						else
						{
							MemCopy(pFindBuffer, &ui16, 2);
						}
					}
				}
				break;
			case 2: //4byte
				{
					int32 si32;
					uint32 ui32;
					
					this->findBufferSize = 4;

					if(this->littleEndian.IsChecked())
					{
						si32 = si64 & 0xFFFFFFFF;
						ui32 = ui64 & 0xFFFFFFFF;
					}
					else
					{
						si32 = ENDIAN_SWAP32(si64);
						ui32 = ENDIAN_SWAP32(ui64);
					}

					if(text[0] == '-')
					{
						if(text.GetLength() > 11 || si64 < INT32_MIN)
						{
							this->MessageBox(L"Value is less than minimum value '" + INT32_MIN + '\'', L"Error", MB_ICONERROR);
						}
						else
						{
							MemCopy(pFindBuffer, &si32, 4);
						}
					}
					else
					{
						if(text.GetLength() > 10 || ui64 > UINT32_MAX)
						{
							this->MessageBox(L"Value is greater than maximum value '" + CString(UINT32_MAX) + L"'", L"Error", MB_ICONERROR);
						}
						else
						{
							MemCopy(pFindBuffer, &ui32, 4);
						}
					}
				}
				break;
			case 3: //8byte
				{
					this->findBufferSize = 8;
					
					if(text[0] == '-')
					{
						if(text.GetLength() > 20 || si64 < INT64_MIN)
						{
							this->MessageBox(L"Value is less than minimum value '" + CString(INT64_MIN) + '\'', L"Error", MB_ICONERROR);
						}
						else
						{
							if(this->bigEndian.IsChecked())
								si64 = ENDIAN_SWAP64(si64);
							
							MemCopy(pFindBuffer, &si64, 8);
						}
					}
					else
					{
						if(text.GetLength() > 19 || ui64 > UINT64_MAX)
						{
							this->MessageBox(L"Value is greater than maximum value '" + CString(UINT64_MAX) + L"'", L"Error", MB_ICONERROR);
						}
						else
						{
							if(this->bigEndian.IsChecked())
								ui64 = ENDIAN_SWAP64(ui64);
							
							MemCopy(pFindBuffer, &ui64, 8);
						}
					}
				}
				break;
			}
		}
		break;
	case 2: //Text
		{
			const CString &refText = this->stdEditField.GetText();
			
			MemCopy(pFindBuffer, refText.GetC_Str(), refText.GetLength());
			this->findBufferSize = refText.GetLength();
		}
		break;
	}

	ASSERT(this->findBufferSize <= sizeof(this->findBuffer));

	//Present find buffer
	for(i = 0; i < this->findBufferSize; i++)
	{
		hexText += CString((uint64)this->findBuffer[i], NS_HEX, false, 2) + ' ';
	}
	this->hexValuesEdit.SetText(hexText);
}