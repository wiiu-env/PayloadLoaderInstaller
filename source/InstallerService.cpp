#include "common/common.h"
#include "utils/logger.h"
#include "WiiUScreen.h"
#include "StringTools.h"
#include "fs/FSUtils.h"
#include "common/fst_structs.h"
#include "InstallerService.h"
#include "utils/utils.h"
#include "utils/pugixml.hpp"
#include <coreinit/mcp.h>
#include <string>
#include <memory>
#include <cstdlib>
#include <malloc.h>
#include <sstream>

systemXMLInformation systemXMLHashInformation[] = {
        {WII_U_MENU_JAP,    0x0005001010040000L, "2645065A42D18D390C78543E3C4FE7E1D1957A63", "5E5C707E6DAF82393E93971BE98BE3B12204932A"},
        {WII_U_MENU_USA,    0x0005001010040100L, "124562D41A02C7112DDD5F9A8F0EE5DF97E23471", "DC0F9941E99C629625419F444B5A5B177A67309F"},
        {WII_U_MENU_EUR,    0x0005001010040200L, "F06041A4E5B3F899E748F1BAEB524DE058809F1D", "A0273C466DE15F33EC161BCD908B5BFE359FE6E0"},
        {HEALTH_SAFETY_JAP, 0x000500101004E000L, "066D672824128713F0A7D156142A68B998080148", "2849DE91560F6667FE7415F89FC916BE3A27DE75"},
        {HEALTH_SAFETY_USA, 0x000500101004E100L, "0EBCA1DFC0AB7A6A7FE8FB5EAF23179621B726A1", "83CF5B1CE0B64C51D15B1EFCAD659063790EB590"},
        {HEALTH_SAFETY_EUR, 0x000500101004E200L, "DE46EC3E9B823ABA6CB0638D0C4CDEEF9C793BDD", "ED59630448EC6946F3E51618DA3681EC3A84D391"}
};

appInformation supportedApps[] = {
        {0x000500101004E000L, "Health and Safety Information [JPN]", false, {'\0'}, "9D34DDD91604D781FDB0727AC75021833304964C", "F6EBF7BC8AE3AF3BB8A42E0CF3FDA051278AEB03"},
        {0x000500101004E100L, "Health and Safety Information [USA]", false, {'\0'}, "045734666A36C7EF0258A740855886EBDB20D59B", "F6EBF7BC8AE3AF3BB8A42E0CF3FDA051278AEB03"},
        {0x000500101004E200L, "Health and Safety Information [EUR]", false, {'\0'}, "130A76F8B36B36D43B88BBC74393D9AFD9CFD2A4", "F6EBF7BC8AE3AF3BB8A42E0CF3FDA051278AEB03"},
};

InstallerService::eResults InstallerService::checkCOS(const std::string &path, char *hash) {
    std::string cosFilePath = path + "/code/cos.xml";
    DEBUG_FUNCTION_LINE("Loading %s", cosFilePath.c_str());

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(cosFilePath.c_str());
    if (!result) {
        DEBUG_FUNCTION_LINE("failed to open %s : %s", cosFilePath.c_str(), result.description());
        return COS_XML_PARSING_FAILED;
    }

    patchCOSXMLData(&doc);

    std::stringstream ss;
    doc.save(ss, "  ", pugi::format_default, pugi::encoding_utf8);

    std::string newHash = Utils::calculateSHA1(ss.str().c_str(), ss.str().size());

    if (std::string(hash) == newHash) {
        DEBUG_FUNCTION_LINE("Success! cos.xml is compatible");
        return SUCCESS;
    } else {
        DEBUG_FUNCTION_LINE("Hash mismatch! cos.xml is NOT compatible. Expected hash: %s actual hash: %s",hash, newHash.c_str());
    }

    return COS_XML_HASH_MISMATCH;
}

InstallerService::eResults InstallerService::checkSystemXML(const std::string &path, uint64_t titleId) {
    std::string inputFile = std::string(path + "/system.xml");

    systemXMLInformation *data = nullptr;
    int arrayLength = (sizeof(systemXMLHashInformation) / sizeof(*systemXMLHashInformation));
    for (int i = 0; i < arrayLength; i++) {
        if (systemXMLHashInformation[i].titleId == titleId) {
            data = &systemXMLHashInformation[i];
            break;
        }
    }

    if (data == nullptr) {
        DEBUG_FUNCTION_LINE("system xml information were not found.");
        return SYSTEM_XML_INFORMATION_NOT_FOUND;
    }

    DEBUG_FUNCTION_LINE("Setting coldboot to %016llX", data->titleId);


    pugi::xml_document doc;
    pugi::xml_parse_result resultSystem = doc.load_file(inputFile.c_str());

    if (!resultSystem) {
        DEBUG_FUNCTION_LINE("Error while parsing %s: %s", inputFile.c_str(), resultSystem.description());
        return SYSTEM_XML_PARSING_FAILED;
    }

    char tmp[18];
    snprintf(tmp, 17, "%016llX", data->titleId);

    doc.child("system").child("default_title_id").first_child().set_value(tmp);

    std::stringstream ss;
    doc.save(ss, "  ", pugi::format_default, pugi::encoding_utf8);

    std::string newHash = Utils::calculateSHA1(ss.str().c_str(), ss.str().size());

    if (std::string(data->hash) == newHash) {
        DEBUG_FUNCTION_LINE("Success! system.xml is compatible");
        return SUCCESS;
    } else {
        DEBUG_FUNCTION_LINE("Hash mismatch! system.xml is NOT compatible. Expected hash: %s actual hash: %s", data->hash, newHash.c_str());
    }
    return SYSTEM_XML_HASH_MISMATCH;
}

InstallerService::eResults InstallerService::checkFST(const std::string &path, const char *fstHash) {
    std::string fstFilePath = path + "/code/title.fst";

    uint8_t *fstData = nullptr;
    uint32_t fstDataSize = 0;

    DEBUG_FUNCTION_LINE("Trying to load FST from %s", fstFilePath.c_str());
    if (FSUtils::LoadFileToMem(fstFilePath.c_str(), &fstData, &fstDataSize) < 0) {
        DEBUG_FUNCTION_LINE("Failed to load title.fst");
        return FAILED_TO_LOAD_FILE;
    }
    InstallerService::eResults res = patchFSTData(fstData, fstDataSize);
    if (res != SUCCESS) {
        free(fstData);
        fstData = nullptr;
        return res;
    }

    std::string newHash = Utils::calculateSHA1((const char *) fstData, fstDataSize);

    free(fstData);
    fstData = nullptr;

    if (std::string(fstHash) == newHash) {
        DEBUG_FUNCTION_LINE("title.fst is compatible");
        return SUCCESS;
    } else {
        DEBUG_FUNCTION_LINE("title.fst is NOT compatible. expected hash: %s actual hash: %s", fstHash, newHash.c_str());
        return FST_HASH_MISMATCH;
    }
}

bool InstallerService::patchCOSXMLData(pugi::xml_document *doc) {
    pugi::xml_node appEntry = doc->child("app");
    appEntry.child("argstr").first_child().set_value("safe.rpx");
    appEntry.child("avail_size").first_child().set_value("00000000");
    appEntry.child("codegen_size").first_child().set_value("02000000");
    appEntry.child("codegen_core").first_child().set_value("80000001");
    appEntry.child("max_size").first_child().set_value("40000000");
    appEntry.child("max_codesize").first_child().set_value("00800000");
    for (pugi::xml_node permission: appEntry.child("permissions").children()) {
        auto mask = permission.child("mask");
        mask.first_child().set_value("FFFFFFFFFFFFFFFF");
    }
    return true;
}

std::optional<appInformation> InstallerService::getInstalledAppInformation() {
    auto mcpHandle = (int32_t) MCP_Open();
    auto titleCount = (uint32_t) MCP_TitleCount(mcpHandle);
    auto *titleList = (MCPTitleListType *) memalign(32, sizeof(MCPTitleListType) * titleCount);

    MCP_TitleListByAppType(mcpHandle, MCP_APP_TYPE_SYSTEM_APPS, &titleCount, titleList, sizeof(MCPTitleListType) * titleCount);

    MCP_Close(mcpHandle);

    DEBUG_FUNCTION_LINE("%d titles found on the WiiU", titleCount);
    bool success = false;

    int arrayLength = (sizeof(supportedApps) / sizeof(*supportedApps));
    for (uint32_t i = 0; i < titleCount; ++i) {
        for (int j = 0; j < arrayLength; ++j) {
            if (titleList[i].titleId == supportedApps[j].titleId) {
                DEBUG_FUNCTION_LINE("%s is on the Wii U (%s) %d", supportedApps[j].appName, titleList[i].path, sizeof(supportedApps[j].path));
                supportedApps[j].onTheWiiU = true;
                strncpy(supportedApps[j].path, titleList[i].path, 255);
                success = true;
                break;
            }
        }
    }
    free(titleList);

    if (success) {
        success = false;
        for (int j = 0; j < arrayLength; ++j) {
            if (supportedApps[j].onTheWiiU) {
                std::string path(supportedApps[j].path);
                if (!StringTools::replace(path, "/vol/storage_mlc01", "storage_mlc_installer:")) {
                    DEBUG_FUNCTION_LINE("Title is not on MLC. This is not expected. %s", path.c_str());
                    return {};
                }
                strncpy(supportedApps[j].path, path.c_str(), 255);
                return supportedApps[j];
            }
        }
    }
    return {};
}

InstallerService::eResults InstallerService::patchFSTData(uint8_t *fstData, uint32_t size) {
    auto *fstHeader = (FSTHeader *) fstData;
    if (strncmp(FSTHEADER_MAGIC, fstHeader->magic, 3) != 0) {
        DEBUG_FUNCTION_LINE("FST magic is wrong %s", fstHeader->magic);
        return InstallerService::FST_HEADER_MISMATCH;
    }

    auto numberOfSections = fstHeader->numberOfSections;
    DEBUG_FUNCTION_LINE("Found %d sections", numberOfSections);
    auto *sections = (FSTSectionEntry *) (fstData + sizeof(FSTHeader));

    auto usableSectionIndex = -1;

    for (uint32_t i = 0; i < numberOfSections; i++) {
        if (sections[i].hashMode == 2) {
            usableSectionIndex = i;
        }
    }

    if (usableSectionIndex < 0) {
        DEBUG_FUNCTION_LINE("Failed to find suitable section");
        return InstallerService::FST_NO_USABLE_SECTION_FOUND;
    } else {
        DEBUG_FUNCTION_LINE("Section %d can be used as a base", usableSectionIndex);
    }

    auto *rootEntry = (FSTNodeEntry *) (fstData + sizeof(FSTHeader) + numberOfSections * sizeof(FSTSectionEntry));
    auto numberOfNodeEntries = rootEntry->directory.lastEntryNumber;

    char *stringTableOffset = (char *) ((uint32_t) rootEntry + (sizeof(FSTNodeEntry) * numberOfNodeEntries));

    FSTNodeEntry *nodeEntries = rootEntry;
    for (uint32_t i = 0; i < numberOfNodeEntries; i++) {
        if (sections[nodeEntries[i].sectionNumber].hashMode != 2) {
            auto nameOffset = (*((uint32_t *) nodeEntries[i].nameOffset) & 0xFFFFFF00) >> 8;
            DEBUG_FUNCTION_LINE("Updating sectionNumber for %s (entry %d)", stringTableOffset + nameOffset, i);
            nodeEntries[i].sectionNumber = usableSectionIndex;
        }
    }
    return InstallerService::SUCCESS;
}

std::string InstallerService::ErrorDescription(InstallerService::eResults error) {
    if (error == SUCCESS) {
        return "Success";
    } else if (error == NO_COMPATIBLE_APP_INSTALLED) {
        return "No compatible application was found on the console.";
    } else if (error == FAILED_TO_COPY_FILES) {
        return "Unable to copy files.";
    } else if (error == FAILED_TO_CHECK_HASH_COPIED_FILES) {
        return "The copy of a file has a different hash.";
    } else if (error == SYSTEM_XML_INFORMATION_NOT_FOUND) {
        return "Expected hashes for the target system.xml were not found.";
    } else if (error == SYSTEM_XML_PARSING_FAILED) {
        return "Failed to parse the system.xml";
    } else if (error == SYSTEM_XML_HASH_MISMATCH_RESTORE_FAILED) {
        return "DO NOT REBOOT BEFORE FIXING THIS: Failed to restore the system.xml after an error.";
    } else if (error == SYSTEM_XML_HASH_MISMATCH) {
        return "The patched system.xml had an unexpected hash but was successfully restored";
    } else if (error == RPX_HASH_MISMATCH) {
        return "The installed safe.rpx had an unexpected hash but was successfully restored";
    } else if (error == RPX_HASH_MISMATCH_RESTORE_FAILED) {
        return "DO NOT REBOOT BEFORE FIXING THIS: Failed to restore the safe.rpx after an error.";
    } else if (error == COS_XML_PARSING_FAILED) {
        return "Failed to parse the cos.xml";
    } else if (error == COS_XML_HASH_MISMATCH) {
        return "The patched cos.xml had an unexpected hash but was successfully restored";
    } else if (error == COS_XML_HASH_MISMATCH_RESTORE_FAILED) {
        return "DO NOT REBOOT BEFORE FIXING THIS: Failed to restore the cos.xml after an error";
    } else if (error == MALLOC_FAILED) {
        return "Failed to allocate memory";
    } else if (error == FST_HASH_MISMATCH) {
        return "The patched title.fst had an unexpected hash but was successfully restored";
    } else if (error == FST_HASH_MISMATCH_RESTORE_FAILED) {
        return "DO NOT REBOOT BEFORE FIXING THIS: Failed to restore the title.fst after an error";
    } else if (error == FST_HEADER_MISMATCH) {
        return "Unexpected header in title.fst found. The file is probably broken.";
    } else if (error == FST_NO_USABLE_SECTION_FOUND) {
        return "Unable to patch title.fst to allow FailST";
    } else if (error == FAILED_TO_LOAD_FILE) {
        return "Failed to load file.";
    } else {
        return "UNKNOWN ERROR";
    }
}

std::string InstallerService::ErrorMessage(InstallerService::eResults error) {
    if (error == SUCCESS) {
        return "Success";
    } else if (error == NO_COMPATIBLE_APP_INSTALLED) {
        return "NO_COMPATIBLE_APP_INSTALLED";
    } else if (error == FAILED_TO_COPY_FILES) {
        return "FAILED_TO_COPY_FILES";
    } else if (error == FAILED_TO_CHECK_HASH_COPIED_FILES) {
        return "FAILED_TO_CHECK_HASH_COPIED_FILES";
    } else if (error == SYSTEM_XML_INFORMATION_NOT_FOUND) {
        return "SYSTEM_XML_INFORMATION_NOT_FOUND";
    } else if (error == SYSTEM_XML_PARSING_FAILED) {
        return "SYSTEM_XML_PARSING_FAILED";
    } else if (error == SYSTEM_XML_HASH_MISMATCH_RESTORE_FAILED) {
        return "SYSTEM_XML_HASH_MISMATCH_RESTORE_FAILED";
    } else if (error == SYSTEM_XML_HASH_MISMATCH) {
        return "SYSTEM_XML_HASH_MISMATCH";
    } else if (error == RPX_HASH_MISMATCH) {
        return "RPX_HASH_MISMATCH";
    } else if (error == RPX_HASH_MISMATCH_RESTORE_FAILED) {
        return "RPX_HASH_MISMATCH_RESTORE_FAILED";
    } else if (error == COS_XML_PARSING_FAILED) {
        return "COS_XML_PARSING_FAILED";
    } else if (error == COS_XML_HASH_MISMATCH) {
        return "COS_XML_HASH_MISMATCH";
    } else if (error == COS_XML_HASH_MISMATCH_RESTORE_FAILED) {
        return "COS_XML_HASH_MISMATCH_RESTORE_FAILED";
    } else if (error == MALLOC_FAILED) {
        return "MALLOC_FAILED";
    } else if (error == FST_HASH_MISMATCH) {
        return "FST_HASH_MISMATCH";
    } else if (error == FST_HASH_MISMATCH_RESTORE_FAILED) {
        return "FST_HASH_MISMATCH_RESTORE_FAILED";
    } else if (error == FST_HEADER_MISMATCH) {
        return "FST_HEADER_MISMATCH";
    } else if (error == FST_NO_USABLE_SECTION_FOUND) {
        return "FST_NO_USABLE_SECTION_FOUND";
    } else if (error == FAILED_TO_LOAD_FILE) {
        return "FAILED_TO_LOAD_FILE";
    } else {
        return "UNKNOWN ERROR";
    }

}

InstallerService::eResults InstallerService::patchFST(const std::string &path, const char *fstHash) {
    std::string fstFilePath = path + "/code/title.fst";
    std::string fstBackupFilePath = path + "/code/backup.fst";
    std::string fstTargetFilePath = path + "/code/title.fst";

    if (!FSUtils::copyFile(fstFilePath, fstBackupFilePath)) {
        DEBUG_FUNCTION_LINE("Failed to copy files");
        return FAILED_TO_COPY_FILES;
    }

    std::string srcHash = Utils::hashFile(fstFilePath);
    std::string dstHash = Utils::hashFile(fstBackupFilePath);

    if (srcHash != dstHash) {
        ::remove(fstBackupFilePath.c_str());
        DEBUG_FUNCTION_LINE("Hashes do not match. %s %s", srcHash.c_str(), dstHash.c_str());
        return FAILED_TO_CHECK_HASH_COPIED_FILES;
    }

    uint8_t *fstData = nullptr;
    uint32_t fstDataSize = 0;

    DEBUG_FUNCTION_LINE("Trying to load FST from %s", fstFilePath.c_str());
    if (FSUtils::LoadFileToMem(fstFilePath.c_str(), &fstData, &fstDataSize) < 0) {
        DEBUG_FUNCTION_LINE("Failed to load title.fst");
        return FAILED_TO_LOAD_FILE;
    }
    InstallerService::eResults res = patchFSTData(fstData, fstDataSize);
    if (res != SUCCESS) {
        free(fstData);
        return res;
    }
    FSUtils::saveBufferToFile(fstTargetFilePath.c_str(), fstData, fstDataSize);
    free(fstData);
    fstData = nullptr;

    std::string newHash = Utils::hashFile(fstTargetFilePath);

    if (std::string(fstHash) == newHash) {
        ::remove(fstBackupFilePath.c_str());
        DEBUG_FUNCTION_LINE("Successfully patched the title.fst");
        return SUCCESS;
    } else {
        DEBUG_FUNCTION_LINE("Hash mismatch! Expected %s but got %s while patching FST", fstHash, newHash.c_str());
    }

    FSUtils::copyFile(fstBackupFilePath, fstTargetFilePath);

    std::string srcHash2 = Utils::hashFile(fstTargetFilePath);

    if (srcHash != srcHash2) {
        DEBUG_FUNCTION_LINE("Something went wrong. Failed to restore the title.fst. DO NOT RESTART THE SYSTEM until you manually restored the title.fst");
        return FST_HASH_MISMATCH_RESTORE_FAILED;
    }

    ::remove(fstBackupFilePath.c_str());

    return FST_HASH_MISMATCH;
}

InstallerService::eResults InstallerService::patchCOS(const std::string &path, char *hash) {
    std::string cosFilePath = path + "/code/cos.xml";
    std::string cosBackupFilePath = path + "/code/cback.xml";
    std::string cosTargetFilePath = path + "/code/cos.xml";

    if (!FSUtils::copyFile(cosFilePath, cosBackupFilePath)) {
        DEBUG_FUNCTION_LINE("Failed to copy files");
        return FAILED_TO_COPY_FILES;
    }

    std::string srcHash = Utils::hashFile(cosFilePath);
    std::string dstHash = Utils::hashFile(cosBackupFilePath);

    if (srcHash != dstHash) {
        ::remove(cosBackupFilePath.c_str());
        DEBUG_FUNCTION_LINE("Hashes do not match. %s %s", srcHash.c_str(), dstHash.c_str());
        return FAILED_TO_CHECK_HASH_COPIED_FILES;
    }

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(cosFilePath.c_str());

    if (!result) {
        ::remove(cosBackupFilePath.c_str());
        DEBUG_FUNCTION_LINE("failed to open %s : %s", cosFilePath.c_str(), result.description());
        return COS_XML_PARSING_FAILED;
    }

    patchCOSXMLData(&doc);

    doc.save_file(cosTargetFilePath.c_str(), "  ", pugi::format_default, pugi::encoding_utf8);

    std::string newHash = Utils::hashFile(cosTargetFilePath);

    if (std::string(hash) == newHash) {
        ::remove(cosBackupFilePath.c_str());
        DEBUG_FUNCTION_LINE("Successfully patched the cos.xml");
        return SUCCESS;
    } else {
        DEBUG_FUNCTION_LINE("Hash mismatch! Expected %s but got %s while patching cos.xml", hash, newHash.c_str());
    }

    FSUtils::copyFile(cosBackupFilePath, cosTargetFilePath);

    std::string srcHash2 = Utils::hashFile(cosTargetFilePath);

    if (srcHash != srcHash2) {
        DEBUG_FUNCTION_LINE("Something went wrong. Failed to restore the title.fst. DO NOT RESTART THE SYSTEM until you manually restored the cos.xml");
        return COS_XML_HASH_MISMATCH_RESTORE_FAILED;
    }
    ::remove(cosBackupFilePath.c_str());

    return COS_XML_HASH_MISMATCH;
}


InstallerService::eResults InstallerService::copyRPX(const std::string &path, const uint8_t *rpx_data, size_t size, const std::string &rpx_hash) {
    std::string rpxSourceFilePath = path + "/code/safe.rpx";
    std::string rpxBackupFilePath = path + "/code/sbac.rpx";
    std::string rpxTargetFilePath = path + "/code/safe.rpx";

    if (!FSUtils::copyFile(rpxSourceFilePath, rpxBackupFilePath)) {
        DEBUG_CONSOLE_LOG("Failed to copy files");
        return FAILED_TO_COPY_FILES;
    }

    std::string srcHash = Utils::hashFile(rpxSourceFilePath);
    std::string dstHash = Utils::hashFile(rpxBackupFilePath);

    if (srcHash != dstHash) {
        ::remove(rpxBackupFilePath.c_str());
        DEBUG_CONSOLE_LOG("Hashes do not match. %s %s", srcHash.c_str(), dstHash.c_str());
        return FAILED_TO_CHECK_HASH_COPIED_FILES;
    }

    if (FSUtils::saveBufferToFile(rpxTargetFilePath.c_str(), (void *) rpx_data, size) < 0) {
        DEBUG_CONSOLE_LOG("Cannot save rpx to file %s", rpxTargetFilePath.c_str());
        return SUCCESS;
    }

    std::string newHash = Utils::hashFile(rpxTargetFilePath);

    if (StringTools::strCompareIC(rpx_hash, newHash)) {
        ::remove(rpxBackupFilePath.c_str());
        DEBUG_CONSOLE_LOG("Successfully patched the safe.rpx");
        return SUCCESS;
    } else {
        DEBUG_CONSOLE_LOG("Hash mismatch! Expected %s but got %s while patching safe.rpx", rpx_hash.c_str(), newHash.c_str());
    }

    FSUtils::copyFile(rpxBackupFilePath, rpxTargetFilePath);

    std::string srcHash2 = Utils::hashFile(rpxTargetFilePath);

    if (srcHash != srcHash2) {
        DEBUG_CONSOLE_LOG("Something went wrong. Failed to restore the safe.rpx. DO NOT RESTART THE SYSTEM until you manually restored the safe.rpx");
        return RPX_HASH_MISMATCH_RESTORE_FAILED;
    }
    ::remove(rpxBackupFilePath.c_str());

    return RPX_HASH_MISMATCH;
}

InstallerService::eResults InstallerService::patchSystemXML(const std::string &path, uint64_t titleId) {
    std::string inputFile = std::string(path + "/system.xml");
    std::string backupFile = std::string(path + "/sbackup.xml");
    std::string finalFile = std::string(path + "/system.xml");

    if (!FSUtils::copyFile(inputFile, backupFile)) {
        DEBUG_CONSOLE_LOG("Failed to copy files");
        return FAILED_TO_COPY_FILES;
    }

    std::string srcHash = Utils::hashFile(inputFile);
    std::string dstHash = Utils::hashFile(backupFile);

    if (srcHash != dstHash) {
        ::remove(backupFile.c_str());
        DEBUG_CONSOLE_LOG("Hashes do not match. %s %s", srcHash.c_str(), dstHash.c_str());
        return FAILED_TO_CHECK_HASH_COPIED_FILES;
    }

    systemXMLInformation *data = nullptr;
    int arrayLength = (sizeof(systemXMLHashInformation) / sizeof(*systemXMLHashInformation));
    for (int i = 0; i < arrayLength; i++) {
        if (systemXMLHashInformation[i].titleId == titleId) {
            data = &systemXMLHashInformation[i];
            break;
        }
    }

    if (data == nullptr) {
        ::remove(backupFile.c_str());
        DEBUG_FUNCTION_LINE("Data was NULL");
        return SYSTEM_XML_INFORMATION_NOT_FOUND;
    }

    pugi::xml_document doc;
    pugi::xml_parse_result resultSystem = doc.load_file(inputFile.c_str());
    if (!resultSystem) {
        ::remove(backupFile.c_str());
        DEBUG_FUNCTION_LINE("Error while parsing %s: %s", inputFile.c_str(), resultSystem.description());
        return SYSTEM_XML_PARSING_FAILED;
    }

    char tmp[18];
    snprintf(tmp, 17, "%016llX", data->titleId);

    doc.child("system").child("default_title_id").first_child().set_value(tmp);
    doc.save_file(finalFile.c_str(), "  ", pugi::format_default, pugi::encoding_utf8);

    std::string newHash = Utils::hashFile(finalFile);

    if (std::string(data->hash) == newHash || std::string(data->hash2) == newHash) {
        ::remove(backupFile.c_str());
        DEBUG_FUNCTION_LINE("Successfully patched the system.xml");
        return SUCCESS;
    } else {
        DEBUG_FUNCTION_LINE("Hash mismatch! Expected %s but got %s while setting default titleID to %s", data->hash, newHash.c_str(), data->titleId);
    }

    FSUtils::copyFile(backupFile, finalFile);

    std::string srcHash2 = Utils::hashFile(finalFile);

    if (srcHash != srcHash2) {
        DEBUG_FUNCTION_LINE("Something went wrong. Failed to restore the system.xml. DO NOT RESTART THE SYSTEM until you manually restored the system.xml");
        return SYSTEM_XML_HASH_MISMATCH_RESTORE_FAILED;
    }

    ::remove(backupFile.c_str());

    return SYSTEM_XML_HASH_MISMATCH;
}
