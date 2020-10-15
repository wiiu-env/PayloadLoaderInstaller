#include "GameState.h"
#include "WiiUScreen.h"
#include "Input.h"

void GameState::render() {
    WiiUScreen::clearScreen();
    if (this->state == STATE_ERROR) {
        WiiUScreen::drawLinef("Error: %s", ErrorMessage().c_str());
    } else if (this->state == STATE_GET_APP_INFORMATION) {
        WiiUScreen::drawLine("Getting app information");
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE) {
        WiiUScreen::drawLine("Check if console can be patched.");
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE_DONE) {
        if (this->fstPatchPossibe) {
            WiiUScreen::drawLine("- title.fst can be patched!");
        } else {
            WiiUScreen::drawLine("x title.fst can NOT be patched!");
        }
        if (this->cosPatchPossibe) {
            WiiUScreen::drawLine("- cos.xml can be patched!");
        } else {
            WiiUScreen::drawLine("x cos.xml can NOT be patched!");
        }
        if (this->systemXMLPatchPossibe) {
            WiiUScreen::drawLine("- system.xml can be patched!");
        } else {
            WiiUScreen::drawLine("x system.xml can NOT be patched!");
        }
    }
    WiiUScreen::flipBuffers();
}

Input GameState::getCurrentInput(){

}

void GameState::update(Input input) {
    if (this->state == STATE_ERROR) {
        handleError();
    } else if (this->state == STATE_GET_APP_INFORMATION) {
        getAppInformation();
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE) {
        checkPatchPossible();
    }
}

GameState::GameState() {
    this->state = STATE_GET_APP_INFORMATION;
    DEBUG_FUNCTION_LINE("State has changed to \"STATE_GET_APP_INFORMATION\"");
}

void GameState::checkPatchPossible() {
    DEBUG_FUNCTION_LINE("Check patch possible");
    if(!this->appInfo){
        this->state = STATE_ERROR;
        this->error = ERROR_NO_APP_INSTALLED;
        DEBUG_FUNCTION_LINE("ERROR");
        return;
    }
    DEBUG_FUNCTION_LINE("CHECK FST");
    InstallerService::eResults result;
    this->fstPatchPossibe = ((result = InstallerService::checkFST(this->appInfo->path, this->appInfo->fstHash)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }
    this->cosPatchPossibe = ((result = InstallerService::checkCOS(this->appInfo->path, this->appInfo->cosHash)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }
    this->systemXMLPatchPossibe = ((result = InstallerService::checkSystemXML("storage_slc_installer:/config", this->appInfo->titleId)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }
    this->state = STATE_CHECK_PATCH_POSSIBLE_DONE;

}

void GameState::getAppInformation() {
    DEBUG_FUNCTION_LINE("About to call getInstalledAppInformation");
    this->appInfo = InstallerService::getInstalledAppInformation();
    DEBUG_FUNCTION_LINE("back");
    if (!this->appInfo) {
        DEBUG_FUNCTION_LINE("ERROR =(");
        this->state = STATE_ERROR;
        this->error = ERROR_NO_APP_INSTALLED;
    } else {
        DEBUG_FUNCTION_LINE("WORKED!");
        this->state = STATE_CHECK_PATCH_POSSIBLE;
    }
}

std::string GameState::ErrorMessage() {
    if (this->error == ERROR_NONE) {
        return "NONE";
    } else if (this->error == ERROR_NO_APP_INSTALLED) {
        return "ERROR_NO_APP_INSTALLED";
    }
    return "UNKNOWN_ERROR";
}

void GameState::handleError() {

}
