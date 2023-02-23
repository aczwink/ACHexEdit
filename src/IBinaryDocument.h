#pragma once
//Definitions
#define IBINARYDOCUMENT_FIND_NOMATCH UINT64_MAX

class IBinaryDocument
{
public:
	//Abstract
	virtual uint32 CopyData(void *pDest, uint64 offset, uint32 size) = NULL;
	virtual uint64 Find(const byte *pBuffer, uint32 bufferSize, uint64 searchStartOffset) = NULL;
	virtual byte *GetData(uint64 offset, uint8 requiredSize, uint16 &refValidSize) = NULL;
	virtual uint64 GetSize() const = NULL;
};