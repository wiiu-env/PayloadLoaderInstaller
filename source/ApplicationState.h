#pragma once

#include <string>
#include <optional>
#include "common/common.h"
#include "InstallerService.h"
#include "Input.h"

class ApplicationState {
public:

    enum eErrorState {
        ERROR_NONE,
        ERROR_IOSUHAX_FAILED,
        ERROR_NO_APP_INSTALLED,
        ERROR_INSTALLER_ERROR
    };

    enum eGameState {
        STATE_ERROR,
        STATE_WELCOME_SCREEN,
        STATE_GET_APP_INFORMATION,
        STATE_CHECK_PATCH_POSSIBLE,
        STATE_CHECK_PATCH_POSSIBLE_DONE,
        STATE_INSTALL_CHOOSE_COLDBOOT,
        STATE_INSTALL_NO_COLDBOOT_ALLOWED,
        STATE_INSTALL_CONFIRM_DIALOG,
        STATE_INSTALL_STARTED,
        STATE_INSTALL_FST,
        STATE_INSTALL_COS,
        STATE_INSTALL_RPX,
        STATE_INSTALL_SYSTEM_XML,
        STATE_INSTALL_SUCCESS
    };

    ApplicationState();

    void setError(eErrorState error);

    void render();

    void update(Input *input);

    void checkPatchPossible();

    void getAppInformation();

    std::optional<appInformation> appInfo;

    std::string ErrorMessage();

    std::string ErrorDescription();

    void handleError();

    int selectedOption;

    static void printFooter();

    void proccessMenuNavigation(Input *input, int maxOptionValue);

    static bool entrySelected(Input *input);

    bool installColdboot = false;
    InstallerService::eResults installerError = InstallerService::eResults::SUCCESS;

private:
    bool fstPatchPossible = false;
    bool cosPatchPossible = false;
    bool systemXMLPatchPossible = false;

    eGameState state;
    eErrorState error = ERROR_NONE;
    uint64_t coldbootTitleId;
    _gList_t *coldbootTitle;
    bool fstAlreadyPatched;
    bool rpxAlreadyPatched;
    bool cosAlreadyPatched;
    bool tmdValid;
};
