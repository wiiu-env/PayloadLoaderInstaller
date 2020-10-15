#pragma once

#include "utils/pugixml.hpp"
#include "common/common.h"
#include <optional>

class InstallerService {
public:
    enum eResults {
        SUCCESS = 0,
        NO_COMPATIBLE_APP_INSTALLED = -1,
        FAILED_TO_COPY_FILES = -2,
        FAILED_TO_CHECK_HASH_COPIED_FILES = -3,
        SYSTEM_XML_INFORMATION_NOT_FOUND = -4,
        SYSTEM_XML_PARSING_FAILED = -5,
        SYSTEM_XML_HASH_MISMATCH_RESTORE_FAILED = -6,
        SYSTEM_XML_HASH_MISMATCH = -7,
        RPX_HASH_MISMATCH = -8,
        RPX_HASH_MISMATCH_RESTORE_FAILED = -9,
        COS_XML_PARSING_FAILED = -10,
        COS_XML_HASH_MISMATCH = -11,
        COS_XML_HASH_MISMATCH_RESTORE_FAILED = -12,
        MALLOC_FAILED = -13,
        FST_HASH_MISMATCH = -14,
        FST_HASH_MISMATCH_RESTORE_FAILED = -15,
        FST_HEADER_MISMATCH = -16,
        FST_NO_USABLE_SECTION_FOUND = -17,
        FAILED_TO_LOAD_FILE = -18,
    };

    static eResults checkCOS(const std::string &path, char *hash);

    static eResults checkSystemXML(const std::string &path, uint64_t titleId);

    static eResults checkFST(const std::string &path, const char *fstHash);

    static std::optional<appInformation> getInstalledAppInformation();

    static std::string ErrorMessage(eResults results);

private:
    static eResults patchFST(uint8_t *data, uint32_t size);

    static bool patchCOS(pugi::xml_document *doc);
};