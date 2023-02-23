#pragma once
//Libs
#include <SJCLib.h>
//Local
#include "IBinaryDocument.h"
//Namespaces
using namespace SJCLib;

enum EType
{
	TYPE_BINARY,
	TYPE_CHAR,
	TYPE_FLOAT32,
	TYPE_FLOAT64,
	TYPE_INT8,
	TYPE_INT16,
	TYPE_INT32,
	TYPE_INT64,
	TYPE_UINT8,
	TYPE_UINT16,
	TYPE_UINT32,
	TYPE_UINT64,
};

struct SStructureElement
{
	EType type;
	CString name;
	uint32 nElements;
	bool bigEndian;
};

struct SStructure
{
	CString name;
	CArray<SStructureElement> elements;
};

struct SStructureSet
{
	CString name;
	CArray<SStructure> structures;
};

struct SMountedStructureInfo
{
	uint32 structureIndex;
	uint64 offset;
};

//Functions
uint32 GetDataTypeSize(const SStructureElement &refElement);
CString TypeToString(const SStructureElement &refElement);
CString ValueToString(EType type, uint32 nElements, bool littleEndian, uint64 offset, IBinaryDocument &refCtrl);