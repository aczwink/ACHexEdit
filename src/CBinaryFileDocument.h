//SJC Libs
#include <SJCLib.h>
#include <SJCWinLib.h>
//Local
#include "IBinaryDocument.h"
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

//Forward Declarations
class CBinaryFileDocument;

class CFindThread : public AThread
{
private:
	//Callbacks
	uint32 Procedure();
public:
	//Variables
	const byte *pFindBuffer;
	uint32 findBufferSize;
	uint64 searchStartOffset;
	uint64 findResult;
	CBinaryFileDocument *pDoc;
};

class CBinaryFileDocument : public IBinaryDocument
{
	friend class CFindThread;
private:
	//Members
	byte buffer[4096];
	uint64 bufferStartOffset; //file offset from which the buffer was lastly filled
	uint16 validBytesInBuffer;
	CWString filename;
	CFileInputStream input;
	CProgressDialog findProgressDialog;
	//Functions
	uint64 Find1ByteOptimized(const byte *pSearchBuffer, uint32 bufferSize, uint64 searchStartOffset, AInputStream &refInput);
	uint64 Find2ByteOptimized(const byte *pSearchBuffer, uint32 bufferSize, uint64 searchStartOffset, AInputStream &refInput);
	uint64 Find4ByteOptimized(const byte *pSearchBuffer, byte *pTmpBuf, uint32 bufferSize, uint64 searchStartOffset, AInputStream &refInput);
public:
	//Constructor
	CBinaryFileDocument();
	//Destructor
	~CBinaryFileDocument();
	//Functions
	uint32 CopyData(void *pDest, uint64 offset, uint32 size);
	uint64 Find(const byte *pBuffer, uint32 bufferSize, uint64 searchStartOffset);
	byte *GetData(uint64 offset, uint8 requiredSize, uint16 &refValidSize);
	uint64 GetSize() const;
	bool Open(CWString filename);
};