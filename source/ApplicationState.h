#pragma once

#include <string>
#include <optional>
#include <input/Input.h>
#include "common/common.h"
#include "InstallerService.h"
#include "Menu.h"

class ApplicationState {
public:

    enum eErrorState {
        ERROR_NONE,
        ERROR_IOSUHAX_FAILED,
        ERROR_NO_APP_INSTALLED,
        ERROR_INSTALLER_ERROR,
        ERROR_UNEXPECTED_STATE
    };

    enum eGameState {
        STATE_ERROR,
        STATE_WELCOME_SCREEN,
        STATE_GET_APP_INFORMATION,
        STATE_CHECK_PATCH_POSSIBLE,
        STATE_CHECK_COLDBOOT_STATUS,
        STATE_CHECK_REMOVAL_POSSIBLE,
        STATE_COMPATIBILITY_RESULTS,
        STATE_APP_INCOMPATIBLE,
        STATE_MAIN_MENU,
        STATE_INSTALL_CONFIRM_DIALOG,
        STATE_INSTALL_STARTED,
        STATE_INSTALL_BACKUP,
        STATE_INSTALL_FST,
        STATE_INSTALL_COS,
        STATE_INSTALL_RPX,
        STATE_INSTALL_SUCCESS,
        STATE_REMOVE_CONFIRM_DIALOG,
        STATE_REMOVE_STARTED,
        STATE_REMOVE_COLDBOOT,
        STATE_REMOVE_PAYLOAD_LOADER,
        STATE_REMOVE_SUCCESS,
        STATE_BOOT_MENU,
        STATE_BOOT_SWITCH_SYSMENU,
        STATE_BOOT_SWITCH_PAYLOAD_LOADER,
        STATE_BOOT_SWITCH_SUCCESS,
        STATE_EXIT_SYSMENU,
        STATE_EXIT_SHUTDOWN,
    };

    ApplicationState();

    void setError(eErrorState error);

    void changeState(eGameState newState);

    void render();

    void update(Input *input);

    void checkPatchPossible();

    void checkColdbootStatus();

    void checkRemovalPossible();

    void getAppInformation();

    std::optional<appInformation> appInfo;

    std::string ErrorMessage();

    std::string ErrorDescription();

    InstallerService::eResults installerError = InstallerService::eResults::SUCCESS;

private:
    Menu<eGameState> menu;

    bool fstPatchPossible = false;
    bool cosPatchPossible = false;

    bool removalPossible = false;
    bool installPossible = false;
    bool alreadyInstalledAndUpdated = false;

    bool systemXMLPatchPossible = false;
    bool systemXMLPatchAllowed = false;
    bool systemXMLPatchAllowedButNoRPXCheck = false;
    bool systemXMLAlreadyPatched = false;
    bool systemXMLRestorePossible = false;

    eGameState state = STATE_WELCOME_SCREEN;
    eErrorState error = ERROR_NONE;
    uint64_t coldbootTitleId = 0;
    _gList_t *coldbootTitle = nullptr;
    std::optional<uint64_t> systemMenuTitleId;
    bool fstAlreadyPatched = false;
    bool rpxAlreadyPatched = false;
    bool cosAlreadyPatched = false;
    bool tmdValid = false;
};
