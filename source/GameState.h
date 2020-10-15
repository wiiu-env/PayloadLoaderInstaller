#pragma once

#include <string>
#include <optional>
#include "common/common.h"
#include "InstallerService.h"

class GameState {

    enum eGameState {
        STATE_ERROR,
        STATE_GET_APP_INFORMATION,
        STATE_CHECK_PATCH_POSSIBLE,
        STATE_CHECK_PATCH_POSSIBLE_DONE
    };

    enum eErrorState {
        ERROR_NONE,
        ERROR_NO_APP_INSTALLED
    };

private:
    bool fstPatchPossibe = false;
    bool cosPatchPossibe = false;
    bool systemXMLPatchPossibe = false;

    eGameState state;
    eErrorState error = ERROR_NONE;
public:
    GameState();

    void render();

    void update();


    void checkPatchPossible();

    void getAppInformation();

    std::optional<appInformation> appInfo;

    std::string ErrorMessage();

    void handleError();
};
