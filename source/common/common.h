#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION        "v0.1"

enum SYSTEM_XML_DEFAULT_TITLE_ID {
    WII_U_MENU_EUR,
    WII_U_MENU_USA,
    WII_U_MENU_JAP,
    HEALTH_SAFETY_EUR,
    HEALTH_SAFETY_USA,
    HEALTH_SAFETY_JAP
};

typedef struct systemXMLInformation {
    SYSTEM_XML_DEFAULT_TITLE_ID type;
    uint64_t titleId;
    char hash[41];
    char hash2[41];
} systemXMLInformation;

typedef struct compatApps {
    uint64_t titleId;
    const char *appName;
    bool onTheWiiU;
    char path[255];
    char fstHash[41];
    char cosHash[41];
} appInformation;

#ifdef __cplusplus
}
#endif