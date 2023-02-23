//Local
#include "DataTypes.h"
//Global
#include <iostream>

//Functions
uint32 GetDataTypeSize(const SStructureElement &refElement)
{
	uint32 elemSize;

	switch(refElement.type)
	{
	case TYPE_BINARY:
	case TYPE_CHAR:
	case TYPE_INT8:
	case TYPE_UINT8:
		elemSize = 1;
		break;
	case TYPE_INT16:
	case TYPE_UINT16:
		elemSize = 2;
		break;
	case TYPE_FLOAT32:
	case TYPE_INT32:
	case TYPE_UINT32:
		elemSize = 4;
		break;
	case TYPE_FLOAT64:
	case TYPE_INT64:
	case TYPE_UINT64:
		elemSize = 8;
		break;
	}

	return elemSize * refElement.nElements;
}

CString TypeToString(const SStructureElement &refElement)
{
	CString result;

	switch(refElement.type)
	{
	case TYPE_CHAR:
		result += "char";
		break;
	case TYPE_FLOAT32:
		result += "float32";
		break;
	case TYPE_FLOAT64:
		result += "float64";
		break;
	case TYPE_INT8:
		result += "int8";
		break;
	case TYPE_INT16:
		result += "int16";
		break;
	case TYPE_INT32:
		result += "int32";
		break;
	case TYPE_INT64:
		result += "int64";
		break;
	case TYPE_UINT8:
		result += "uint8";
		break;
	case TYPE_UINT16:
		result += "uint16";
		break;
	case TYPE_UINT32:
		result += "uint32";
		break;
	case TYPE_UINT64:
		result += "uint64";
		break;
	}

	if(refElement.nElements > 1)
		result += '[' + CString(refElement.nElements) + ']';

	return result;
}

CString ValueToString(EType type, bool bigEndian, byte *pBuffer)
{
	switch(type)
	{
	case TYPE_BINARY:
		{
			uint8 i;
			CString result;

			for(i = 0x80; i != 0; i >>= 1)
			{
				if(*pBuffer & i)
					result += '1';
				else
					result += '0';
			}

			return result;
		}
	case TYPE_CHAR:
		return (char)*pBuffer;
	case TYPE_FLOAT32:
		{
			union
			{
				float32 f;
				uint32 i;
			} value;

			value.i = *(uint32 *)pBuffer;
			if(bigEndian)
				value.i = ENDIAN_SWAP32(value.i);

			return CString(value.f);
		}
	case TYPE_FLOAT64:
		{
			union
			{
				float64 f;
				uint64 i;
			} value;

			value.i = *(uint64 *)pBuffer;
			if(bigEndian)
				value.i = ENDIAN_SWAP64(value.i);

			return CString(value.f);
		}
	case TYPE_INT8:
		return CString((int32)(char)*pBuffer);
	case TYPE_INT16:
		{
			int16 i;

			i = *(int16 *)pBuffer;
			if(bigEndian)
				i = ENDIAN_SWAP16(i);

			return CString((int32)i);
		}
	case TYPE_INT32:
		{
			int32 i;
			
			i = *(int32 *)pBuffer;
			if(bigEndian)
				i = ENDIAN_SWAP32(i);

			return CString(i);
		}
	case TYPE_INT64:
		{
			int64 i;
			
			i = *(int64 *)pBuffer;
			if(bigEndian)
				i = ENDIAN_SWAP64(i);
			
			return CString(i);
		}
	case TYPE_UINT8:
		return CString((uint32)*pBuffer);
	case TYPE_UINT16:
		{
			uint16 i;

			i = *(uint16 *)pBuffer;
			if(bigEndian)
				i = ENDIAN_SWAP16(i);

			return CString((uint32)i);
		}
	case TYPE_UINT32:
		{
			uint32 i;
			
			i = *(uint32 *)pBuffer;
			if(bigEndian)
				i = ENDIAN_SWAP32(i);

			return CString(i);
		}
	case TYPE_UINT64:
		{
			uint64 i;
			
			i = *(uint64 *)pBuffer;
			if(bigEndian)
				i = ENDIAN_SWAP64(i);
			
			return CString(i);
		}
	}

	return CString();
}

CString ValueToString(EType type, uint32 nElements, bool bigEndian, uint64 offset, IBinaryDocument &refCtrl)
{
	byte *pBuffer;
	uint16 readSize;
	uint32 i, size, elemSize;
	CString result;
	SStructureElement elem;

	elem.nElements = 1;
	elem.type = type;
	elemSize = GetDataTypeSize(elem);
	
	elem.nElements = nElements;
	size = GetDataTypeSize(elem);
	
	pBuffer = refCtrl.GetData(offset, size, readSize);
	
	if(readSize < size)
	{
		return CString();
	}

	if(nElements > 1)
	{
		//Array
		if(type == TYPE_CHAR)
		{
			result = '"';
			for(i = 0; i < nElements; i++)
			{
				result += (char)pBuffer[i];
			}
			result += '"';
		}
		else
		{
			result = '{';
			for(i = 0; i < nElements; i++)
			{
				result += ValueToString(type, bigEndian, pBuffer + i * elemSize) + ", ";
			}
			result = result.SubString(0, result.GetLength() - 2) + '}';
		}
	}
	else
	{
		result = ValueToString(type, bigEndian, pBuffer);
	}
	
	return result;
}