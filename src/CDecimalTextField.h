#pragma once
//SJC Libs
#include <SJCWinLib.h>
//Namespaces
using namespace SJCWinLib;

class CDecimalTextField : public CEdit
{
private:
	//Callbacks
	void OnChange();
public:
	//Functions
	void Create(const CWindow &refParentWindow);
};