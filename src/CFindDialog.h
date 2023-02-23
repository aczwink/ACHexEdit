//SJC Libs
#include <SJCWinLib.h>
#include <SJCCommCtrl.h>
//Local
#include "CDecimalTextField.h"
#include "CHexTextField.h"
//Namespaces
using namespace SJCWinLib;

class CSearchTypeSelect : public CComboBox
{
private:
	//Callbacks
	void OnChangeSelection();
};

class CStdTextField : public CEdit
{
private:
	//Callbacks
	void OnChange();
};

class CEndianRadioButton : public SJCWinLibCommCtrl::CRadioButton
{
private:
	//Callbacks
	void OnClick();
};

class CGoButton : public CButton
{
private:
	//Callbacks
	void OnClick();
};

class CFindDialog : public CDialog
{
	friend class CSearchTypeSelect;
	friend class CEndianRadioButton;
private:
	//Variables
	SJCWinLib::CFont font;
	byte findBuffer[100];
	uint16 findBufferSize;
	//Layout Variables
	CStatic type;
	CSearchTypeSelect typeSelect;
	CStatic value;
	CStdTextField stdEditField;
	CHexTextField valueEdit;
	CDecimalTextField decimalEdit;
	CComboBox byteWidthSelect;
	CEndianRadioButton littleEndian;
	CEndianRadioButton bigEndian;
	CStatic hexValues;
	CEdit hexValuesEdit;
	CGoButton goButton;
	CCommonButtonClose closeButton;
	//Callbacks
	void OnInit();
public:
	//Functions
	void Create(const CWindow &refParentWindow);
	void UpdateValue();

	//Inline
	inline const byte *GetFindBuffer() const
	{
		return this->findBuffer;
	}

	inline uint16 GetFindBufferSize() const
	{
		return this->findBufferSize;
	}
};