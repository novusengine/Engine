#pragma once
#include <Base/Types.h>

#include "FileFormat/Novus/ClientDB/ClientDB.h"
#include "FileFormat/Novus/ClientDB/Definitions.h"

#include <robinhood/robinhood.h>
#include <vector>

class Bytebuffer;
namespace DB2::WDC3
{
	struct Layout;
	class Parser
	{
	public:
		bool TryParse(std::shared_ptr<Bytebuffer>& buffer, DB2::WDC3::Layout& out);
		bool TryReadRecord(const DB2::WDC3::Layout& layout, u32 index, u32& outSectionID, u32& outRecordID, u8*& outRecordData);

		template <typename T>
		const T* GetField(const DB2::WDC3::Layout& layout, const u32 sectionID, const u32 recordID, const u8* recordData, u32 fieldIndex)
		{
			const T* result = nullptr;

			const Layout::FieldStorageInfo& fieldStorageInfo = layout.fieldStorageInfos[fieldIndex];
			//u32 bytesToRead = fieldStorageInfo.sizeInBits >> 0x3; // This converts from Bits -> Bytes

			if (layout.header.flags.HasOffsetMap)
			{
				// Read Sparse Field Data Here
				//const u8* fieldData = &recordData[0];
				//if (!TryParseSparseField(layout, sectionID, fieldData, bytesToRead))
				//	return false;
			}
			else
			{
				// Read Field Data Here based on CompressionType
				switch (fieldStorageInfo.compressionType)
				{
					case Layout::FieldStorageInfo::CompressionType::None:
					{
						u32 byteOffset = fieldStorageInfo.offsetInBits >> 3; // This converts from Bits -> Bytes

						const u8* fieldData = &recordData[byteOffset];
						result = reinterpret_cast<const T*>(fieldData);

						break;
					}

					case Layout::FieldStorageInfo::CompressionType::Bitpacked:
					case Layout::FieldStorageInfo::CompressionType::BitpackedSigned:
					{
						u32 bitOffset = fieldStorageInfo.offsetInBits;
						u32 bitsToRead = fieldStorageInfo.sizeInBits;

						u32 unpackedBits = GetU32FromBits(recordData, bitOffset, bitsToRead);

						if (fieldStorageInfo.compressionType == Layout::FieldStorageInfo::CompressionType::BitpackedSigned)
						{
							unpackedBits = unpackedBits << (32 - bitsToRead);

							i32 unpackedBitsSigned = static_cast<i32>(unpackedBits);
							unpackedBitsSigned = unpackedBitsSigned >> (32 - bitsToRead);

							unpackedBits = *reinterpret_cast<u32*>(&unpackedBitsSigned);
						}

						const u8* fieldData = reinterpret_cast<u8*>(&unpackedBits);
						result = reinterpret_cast<const T*>(fieldData);

						break;
					}

					case Layout::FieldStorageInfo::CompressionType::CommonData:
					{
						u32 value = fieldStorageInfo.fieldCommonData.defaultValue;

						if (fieldStorageInfo.extraDataSize)
						{
							auto itr = layout.perFieldCommonDataIDToValue[fieldIndex].find(recordID);
							if (itr != layout.perFieldCommonDataIDToValue[fieldIndex].end())
								value = itr->second;
						}

						const u8* fieldData = reinterpret_cast<const u8*>(&value);
						result = reinterpret_cast<const T*>(fieldData);

						break;
					}

					case Layout::FieldStorageInfo::CompressionType::BitpackedIndexed:
					case Layout::FieldStorageInfo::CompressionType::BitpackedIndexedArray:
					{
						u32 bitOffset = fieldStorageInfo.offsetInBits;
						u32 bitsToRead = fieldStorageInfo.fieldBitpackedIndexed.bitpackingSizeInBits;

						const std::vector<u32>& fieldPalleteData = layout.perFieldPalleteData[fieldIndex];
						u32 palleteMaxIndex = static_cast<u32>(fieldPalleteData.size()) - 1;
						u32 palleteIndex = GetU32FromBits(recordData, bitOffset, bitsToRead);

						if (fieldStorageInfo.compressionType == Layout::FieldStorageInfo::CompressionType::BitpackedIndexed)
						{
							assert(palleteIndex <= palleteMaxIndex);

							const u8* fieldData = reinterpret_cast<const u8*>(&fieldPalleteData[palleteIndex]);
							result = reinterpret_cast<const T*>(fieldData);
						}
						else
						{
							u32 arrayCount = fieldStorageInfo.fieldBitpackedIndexedArray.arrayCount;

							// We must multiply palleteIndex with arrayCount here, because the pallete data stores "arrayCount" of values for every "unique" set of values.
							u32 palleteArrayIndex = palleteIndex * arrayCount;
							const u8* fieldData = reinterpret_cast<const u8*>(&fieldPalleteData[palleteArrayIndex]);
							result = reinterpret_cast<const T*>(fieldData);
						}

						break;
					}

					default: break;
				}
			}

			return result;
		}

		char* GetString(const DB2::WDC3::Layout& db2, u32 recordIndex, u32 fieldIndex);
		char* GetStringInArr(const DB2::WDC3::Layout& db2, u32 recordIndex, u32 fieldIndex, u32 arrIndex);
		u32 GetFieldSize(const DB2::WDC3::Layout& db2, u32 fieldIndex);

		bool IsRecordEncrypted(const DB2::WDC3::Layout& db2, u32 recordIndex);
		u32 GetRecordIDFromIndex(const DB2::WDC3::Layout& db2, u32 recordIndex);

	private:
		u32 RoundBitToNextByte(u32 bits);
		bool GetBitValue(const u8* fieldData, u32 bitIndex);
		u32 GetU32FromBits(const u8* fieldData, u32 bitOffset, u32 bitsToRead);

	private:
		static robin_hood::unordered_set<u64> _cascEncryptionKeyLookup;
	};
}