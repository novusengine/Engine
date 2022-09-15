#pragma once
#include <Base/Types.h>

#include <robinhood/robinhood.h>
#include <vector>

namespace DB2::WDC3
{
	struct Layout;
}

class Bytebuffer;
class WDC3Parser
{
public:
	bool TryParse(std::shared_ptr<Bytebuffer> buffer, DB2::WDC3::Layout& out);
	bool TryReadRecord(const DB2::WDC3::Layout& db2, u32 index, std::vector<u8>& output);

	char* GetString(const DB2::WDC3::Layout& db2, u32 recordIndex, u32 fieldIndex);
	u32 GetFieldSize(const DB2::WDC3::Layout& db2, u32 fieldIndex);

	bool IsRecordEncrypted(const DB2::WDC3::Layout& db2, u32 recordIndex);
	u32 GetRecordIDFromIndex(const DB2::WDC3::Layout& db2, u32 recordIndex);

private:
	bool TryParseCompressionField(const DB2::WDC3::Layout& db2, u32 sectionIndex, const u8* fieldData, u32 fieldSize, std::vector<u8>& output);
	bool TryParseSparseField(const DB2::WDC3::Layout& db2, u32 sectionIndex, const u8* fieldData, u32 fieldSize, std::vector<u8>& output);

private:
	u32 RoundBitToNextByte(u32 bits);
	bool GetBitValue(const u8* fieldData, u32 bitIndex);
	u32 GetU32FromBits(const u8* fieldData, u32 bitOffset, u32 bitsToRead);

private:
	static robin_hood::unordered_set<u64> _cascEncryptionKeyLookup;
};