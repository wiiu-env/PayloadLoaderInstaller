#pragma once

#include <wut.h>

#define FSTHEADER_MAGIC "FST"

typedef struct WUT_PACKED FSTHeader {
    char magic[3];
    uint8_t version;
    uint32_t blockSize;
    uint32_t numberOfSections;
    uint8_t hashDisabled;
    WUT_PADDING_BYTES(32 - 13);
} FSTHeader;
WUT_CHECK_OFFSET(FSTHeader, 0x00, magic);
WUT_CHECK_OFFSET(FSTHeader, 0x03, version);
WUT_CHECK_OFFSET(FSTHeader, 0x04, blockSize);
WUT_CHECK_OFFSET(FSTHeader, 0x08, numberOfSections);
WUT_CHECK_OFFSET(FSTHeader, 0x0C, hashDisabled);
WUT_CHECK_SIZE(FSTHeader, 0x20);

typedef struct WUT_PACKED FSTSectionEntry {
    uint32_t addressInVolumeBlocks;
    uint32_t sizeInVolumeBlocks;
    uint64_t ownerID;
    uint32_t groupID;
    uint8_t hashMode;
    WUT_PADDING_BYTES(32 - 21);
} FSTSectionEntry;
WUT_CHECK_OFFSET(FSTSectionEntry, 0x00, addressInVolumeBlocks);
WUT_CHECK_OFFSET(FSTSectionEntry, 0x04, sizeInVolumeBlocks);
WUT_CHECK_OFFSET(FSTSectionEntry, 0x08, ownerID);
WUT_CHECK_OFFSET(FSTSectionEntry, 0x10, groupID);
WUT_CHECK_OFFSET(FSTSectionEntry, 0x14, hashMode);
WUT_CHECK_SIZE(FSTSectionEntry, 0x20);

typedef struct WUT_PACKED FSTNodeEntry {
    uint8_t type;
    char nameOffset[3]; // 24 bit int
    union {
        struct {
            uint32_t parentEntryNumber;
            uint32_t lastEntryNumber;
        } directory;
        struct {
            uint32_t addressInBlocks;
            uint32_t size;
        } file;
    };
    uint16_t permission;
    uint16_t sectionNumber;
} FSTNodeEntry;
WUT_CHECK_OFFSET(FSTNodeEntry, 0x00, type);
WUT_CHECK_OFFSET(FSTNodeEntry, 0x01, nameOffset);
WUT_CHECK_OFFSET(FSTNodeEntry, 0x04, directory.parentEntryNumber);
WUT_CHECK_OFFSET(FSTNodeEntry, 0x08, directory.lastEntryNumber);
WUT_CHECK_OFFSET(FSTNodeEntry, 0x04, file.addressInBlocks);
WUT_CHECK_OFFSET(FSTNodeEntry, 0x08, file.size);
WUT_CHECK_OFFSET(FSTNodeEntry, 0x0C, permission);
WUT_CHECK_OFFSET(FSTNodeEntry, 0x0E, sectionNumber);
WUT_CHECK_SIZE(FSTNodeEntry, 0x10);

