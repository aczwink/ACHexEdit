//Class Header
#include "CBinaryFileDocument.h"
//Global
#include <iostream>
//Local
#include "globals.h"

//CFindThread Callbacks
uint32 CFindThread::Procedure()
{
	byte *pTmpBuf;
	CBufferedInputStream &refInput = CBufferedInputStream(this->pDoc->input);

	stdOut << "hello from findthread" << endl;
	while(!this->pDoc->findProgressDialog.WindowExists());
	
	this->pDoc->input.SetCurrentOffset(this->searchStartOffset);
	
	if(this->findBufferSize >= 4)
	{
		pTmpBuf = (byte *)malloc(this->findBufferSize);
		this->findResult = this->pDoc->Find4ByteOptimized(this->pFindBuffer, pTmpBuf, this->findBufferSize, searchStartOffset, refInput);
		free(pTmpBuf);
	}
	else if(this->findBufferSize >= 2)
	{
		this->findResult = this->pDoc->Find2ByteOptimized(this->pFindBuffer, this->findBufferSize, searchStartOffset, refInput);
	}
	else
	{
		this->findResult = this->pDoc->Find1ByteOptimized(this->pFindBuffer, this->findBufferSize, searchStartOffset, refInput);
	}
	this->pDoc->findProgressDialog.PostMessageA(MsgClose());

	stdOut << "bye from findthread" << endl;
	return EXIT_SUCCESS;
}

//Constructor
CBinaryFileDocument::CBinaryFileDocument()
{
	this->bufferStartOffset = 0;
	this->validBytesInBuffer = 0;
}

//Destructor
CBinaryFileDocument::~CBinaryFileDocument()
{
}

//Private Functions
uint64 CBinaryFileDocument::Find1ByteOptimized(const byte *pSearchBuffer, uint32 bufferSize, uint64 searchStartOffset, AInputStream &refInput)
{
	register byte searchBuf;
	uint64 bytesPerPercent, currentBytes, offset;

	bytesPerPercent = (this->GetSize() - searchStartOffset) / 100;
	currentBytes = bytesPerPercent;

	offset = searchStartOffset;
	
	//Read the first byte
	searchBuf = refInput.ReadByte();
	
	while(!refInput.HitEnd())
	{
		if(searchBuf == *pSearchBuffer)
			return offset;
		
		searchBuf = refInput.ReadByte();
		offset++;

		if(!--currentBytes)
		{
			currentBytes = bytesPerPercent;
			this->findProgressDialog.Advance(1);
		}
	}
	
	return IBINARYDOCUMENT_FIND_NOMATCH;
}

uint64 CBinaryFileDocument::Find2ByteOptimized(const byte *pSearchBuffer, uint32 bufferSize, uint64 searchStartOffset, AInputStream &refInput)
{
	byte b3;
	register uint16 searchBuf;
	uint64 offset;

	offset = searchStartOffset;

	//Read the first 2 bytes
	refInput.ReadUInt16(searchBuf);

	while(!refInput.HitEnd())
	{
		if(searchBuf == *(uint16 *)pSearchBuffer)
		{
			if(bufferSize == 3)
			{
				b3 = refInput.ReadByte();
				if(pSearchBuffer[2] == b3)
					return offset;

				searchBuf = (searchBuf >> 8) | (b3 << 8);
				offset++;
				continue;
			}
			
			return offset;
		}
		
		searchBuf = (searchBuf >> 8) | (refInput.ReadByte() << 8);
		offset++;
	}
	
	return IBINARYDOCUMENT_FIND_NOMATCH;
}

uint64 CBinaryFileDocument::Find4ByteOptimized(const byte *pSearchBuffer, byte *pTmpBuf, uint32 bufferSize, uint64 searchStartOffset, AInputStream &refInput)
{
	uint32 readSize;
	register uint32 searchBuf;
	uint64 bytesPerPercent, currentBytes, offset, currentOffset;
	
	bytesPerPercent = (this->GetSize() - searchStartOffset) / 100;
	currentBytes = bytesPerPercent;
	offset = searchStartOffset;

	//Read the first 4 bytes
	refInput.ReadUInt32(searchBuf);

	while(!refInput.HitEnd())
	{
		if(searchBuf == *(uint32 *)pSearchBuffer)
		{
			currentOffset = this->input.GetCurrentOffset();
			this->input.SetCurrentOffset(offset + 4);
			readSize = this->input.ReadBytes(pTmpBuf, bufferSize - 4);
			if(readSize < bufferSize - 4)
				return IBINARYDOCUMENT_FIND_NOMATCH;
			if(MemCmp(pSearchBuffer + 4, pTmpBuf, readSize) == 0)
				return offset;
			
			this->input.SetCurrentOffset(currentOffset);
		}

		searchBuf = (searchBuf >> 8) | (refInput.ReadByte() << 24);
		offset++;

		if(!--currentBytes)
		{
			currentBytes = bytesPerPercent;
			this->findProgressDialog.Advance(1);
		}
	}
	
	return IBINARYDOCUMENT_FIND_NOMATCH;
}

//Public Functions
uint32 CBinaryFileDocument::CopyData(void *pDest, uint64 offset, uint32 size)
{
	this->input.SetCurrentOffset(offset);
	return this->input.ReadBytes(pDest, size);
}

uint64 CBinaryFileDocument::Find(const byte *pBuffer, uint32 bufferSize, uint64 searchStartOffset)
{
	CFindThread thread;
	
	this->findProgressDialog.Create("Searching...", g_mainFrame);
	
	thread.pFindBuffer = pBuffer;
	thread.findBufferSize = bufferSize;
	thread.searchStartOffset = searchStartOffset;
	thread.pDoc = this;
	thread.Create();
	this->findProgressDialog.Run();
	
	return thread.findResult;
}

byte *CBinaryFileDocument::GetData(uint64 offset, uint8 requiredSize, uint16 &refValidSize)
{
	if(
		//offset is before
		(offset < this->bufferStartOffset) ||
		//offset is in the middle
		(offset >= this->bufferStartOffset && (requiredSize > this->validBytesInBuffer - (this->bufferStartOffset - offset))) ||
		//offset is to far away
		(offset > this->bufferStartOffset + this->validBytesInBuffer)
	)
	{
		//update buffer
		this->bufferStartOffset = offset;
		this->input.SetCurrentOffset(offset);
		this->validBytesInBuffer = this->input.ReadBytes(this->buffer, sizeof(this->buffer));
	}

	refValidSize = (uint16)(this->validBytesInBuffer - (offset - this->bufferStartOffset));
	if(refValidSize > requiredSize)
		refValidSize = requiredSize;
	return &this->buffer[offset - this->bufferStartOffset];
}

uint64 CBinaryFileDocument::GetSize() const
{
	return this->input.GetSize();
}

bool CBinaryFileDocument::Open(CWString filename)
{
	if(this->input.Open(filename))
	{
		this->filename = filename;
		
		return true;
	}

	return false;
}