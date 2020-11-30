#include "ApplicationState.h"
#include "WiiUScreen.h"
#include "ScreenUtils.h"
#include "../build/safe_payload.h"
#include <sysapp/launch.h>
#include <iosuhax.h>

extern "C" void OSShutdown();

void ApplicationState::render() {
    WiiUScreen::clearScreen();
    WiiUScreen::drawLine("Aroma Installer");
    WiiUScreen::drawLine("==================");
    WiiUScreen::drawLine("");

    if (this->state == STATE_ERROR) {
        WiiUScreen::drawLine("The installation failed:");
        WiiUScreen::drawLine();
        WiiUScreen::drawLinef("Error:       %s", ErrorMessage().c_str());
        WiiUScreen::drawLinef("Description: %s", ErrorDescription().c_str());
        WiiUScreen::drawLine();
        WiiUScreen::drawLine();
        WiiUScreen::drawLine("Press A to return to the Wii U Menu.");
    } else if (this->state == STATE_WELCOME_SCREEN) {
        WiiUScreen::drawLine("Welcome to the Aroma Installer!");
        WiiUScreen::drawLine("Do you want to check if an installation is possible?");
        WiiUScreen::drawLine("");
        if (this->selectedOption == 0) {
            WiiUScreen::drawLine("> Check             Exit");
        } else if (this->selectedOption == 1) {
            WiiUScreen::drawLine("  Check           > Exit");
        }
    } else if (this->state == STATE_GET_APP_INFORMATION) {
        WiiUScreen::drawLine("Getting app information");
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE) {
        WiiUScreen::drawLine("Check if console can be patched.");
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE_DONE) {
        WiiUScreen::drawLinef("Compatible title:");
        WiiUScreen::drawLinef("%s", appInfo->appName);
        WiiUScreen::drawLine();

        if (this->fstAlreadyPatched) {
            WiiUScreen::drawLine("[ X ] title.fst is already patched!");
        } else if (this->fstPatchPossible) {
            WiiUScreen::drawLine("[ X ] title.fst can be patched!");
        } else {
            WiiUScreen::drawLine("[   ] title.fst can NOT be patched!");
        }
        if (this->cosAlreadyPatched) {
            WiiUScreen::drawLine("[ X ] cos.xml is already patched!");
        } else if (this->cosPatchPossible) {
            WiiUScreen::drawLine("[ X ] cos.xml can be patched!");
        } else {
            WiiUScreen::drawLine("[   ] cos.xml can NOT be patched!");
        }
        if (this->rpxAlreadyPatched) {
            WiiUScreen::drawLine("[ X ] safe.rpx is already patched!");
        } else {
            WiiUScreen::drawLine("[ X ] safe.rpx can to be patched!");
        }

        WiiUScreen::drawLine();
        WiiUScreen::drawLinef("System is booting into: ");
        if (this->coldbootTitle == nullptr) {
            WiiUScreen::drawLinef("%ll016X (Unknown title)", this->coldbootTitleId);
        } else {
            WiiUScreen::drawLinef("%ll016X (%s)", this->coldbootTitle->tid, this->coldbootTitle->name);
        }

        WiiUScreen::drawLine();

        if (!this->fstPatchPossible || !this->cosPatchPossible) {
            WiiUScreen::drawLine("A safe installation of Aroma can not be provided.");
            WiiUScreen::drawLine();
            WiiUScreen::drawLine("Press A to return to the Wii U Menu");
        } else {
            WiiUScreen::drawLine("Do you want to install Aroma?");
            WiiUScreen::drawLine("");
            if (this->selectedOption == 0) {
                WiiUScreen::drawLine("> Install             Exit");
            } else if (this->selectedOption == 1) {
                WiiUScreen::drawLine("  Install           > Exit");
            }
        }
    } else if (this->state == STATE_INSTALL_CHOOSE_COLDBOOT) {
        WiiUScreen::drawLine("Select your installation type:");
        WiiUScreen::drawLine();
        WiiUScreen::drawLine("[Coldboot] Aroma will launch directly after booting the console.");
        WiiUScreen::drawLine("[No Coldboot] Aroma will need to be launched manually.");
        WiiUScreen::drawLine("");
        if (this->selectedOption == 0) {
            WiiUScreen::drawLine("> Back               Coldboot                   No Coldboot");
        } else if (this->selectedOption == 1) {
            WiiUScreen::drawLine("  Back             > Coldboot                   No Coldboot");
        } else if (this->selectedOption == 2) {
            WiiUScreen::drawLine("  Back               Coldboot                 > No Coldboot");
        }
    } else if (this->state == STATE_INSTALL_NO_COLDBOOT_ALLOWED) {
        WiiUScreen::drawLine("Note: To install Aroma as coldboot you need to run this installer");
        WiiUScreen::drawLine("from an already running Aroma instance (and not the browser)");
        WiiUScreen::drawLine("After the installation has finished, reboot the console, open the");
        WiiUScreen::drawLine("Health & Safety app and run the Aroma installer.");
        WiiUScreen::drawLine();
        if (this->selectedOption == 0) {
            WiiUScreen::drawLine("> Back               Install without Coldboot");
        } else if (this->selectedOption == 1) {
            WiiUScreen::drawLine("> Back             > Install without Coldboot");
        }
    } else if (this->state == STATE_INSTALL_CONFIRM_DIALOG) {
        WiiUScreen::drawLine("Are you REALLY sure you want to install Aroma?");
        WiiUScreen::drawLine("Installing could permanently damage your console");
        WiiUScreen::drawLine();
        WiiUScreen::drawLine("After the installation you can NOT longer use:");
        WiiUScreen::drawLinef("- %s", appInfo->appName);
        WiiUScreen::drawLine();
        WiiUScreen::drawLine("Selected installation type:");
        if (this->installColdboot) {
            WiiUScreen::drawLine("- Coldboot");
        } else {
            WiiUScreen::drawLine("- No Coldboot");
        }

        WiiUScreen::drawLine();
        WiiUScreen::drawLine();
        if (this->selectedOption == 0) {
            WiiUScreen::drawLine("> Back               Install");
        } else if (this->selectedOption == 1) {
            WiiUScreen::drawLine("  Back             > Install");
        }
    } else if (this->state == STATE_INSTALL_STARTED) {
        WiiUScreen::drawLine("Installing...");
    } else if (this->state == STATE_INSTALL_BACKUP) {
        WiiUScreen::drawLine("... backing up files");
    } else if (this->state == STATE_INSTALL_FST) {
        WiiUScreen::drawLine("... patching title.fst");
    } else if (this->state == STATE_INSTALL_COS) {
        WiiUScreen::drawLine("... patching cos.xml");
    } else if (this->state == STATE_INSTALL_SYSTEM_XML) {
        WiiUScreen::drawLine("... patching system.xml");
    } else if (this->state == STATE_INSTALL_RPX) {
        WiiUScreen::drawLine("... install safe.rpx");
    } else if (this->state == STATE_INSTALL_SUCCESS) {
        WiiUScreen::drawLine("Aroma was successfully installed");
        WiiUScreen::drawLine();
        WiiUScreen::drawLine("Press A to shutdown the console");
    }
    printFooter();
    WiiUScreen::flipBuffers();
}

void ApplicationState::update(Input *input) {
    if (this->state == STATE_ERROR) {
        OSEnableHomeButtonMenu(true);
        if (entrySelected(input)) {
            SYSLaunchMenu();
        }
    } else if (this->state == STATE_WELCOME_SCREEN) {
        proccessMenuNavigation(input, 2);
        if (entrySelected(input)) {
            if (this->selectedOption == 0) {
                this->state = STATE_GET_APP_INFORMATION;
            } else {
                SYSLaunchMenu();
            }
            this->selectedOption = 0;
            return;
        }
    } else if (this->state == STATE_GET_APP_INFORMATION) {
        getAppInformation();
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE) {
        checkPatchPossible();
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE_DONE) {
        if (this->fstPatchPossible && this->cosPatchPossible) {
            proccessMenuNavigation(input, 2);
            if (entrySelected(input)) {
                if (this->selectedOption == 0) {
                    if (systemXMLPatchPossible) {
                        this->state = STATE_INSTALL_CHOOSE_COLDBOOT;
                        this->installColdboot = false;
                    } else {
                        this->state = STATE_INSTALL_CONFIRM_DIALOG;
                    }
                } else {
                    SYSLaunchMenu();
                }
                this->selectedOption = 0;
                return;
            }
        } else {
            if (entrySelected(input)) {
                SYSLaunchMenu();
            }
        }
    } else if (this->state == STATE_INSTALL_CHOOSE_COLDBOOT) {

        if (!InstallerService::isColdBootAllowed()) {
            this->installColdboot = false;
            this->state = STATE_INSTALL_NO_COLDBOOT_ALLOWED;
            return;
        }

        proccessMenuNavigation(input, 3);
        if (entrySelected(input)) {
            if (this->selectedOption == 0) { // Back
                this->state = STATE_CHECK_PATCH_POSSIBLE_DONE;
            } else {
                if (selectedOption == 1) { // Install with coldboot
                    this->installColdboot = true;
                }
                this->state = STATE_INSTALL_CONFIRM_DIALOG;
            }
            this->selectedOption = 0;
            return;
        }
    } else if (this->state == STATE_INSTALL_NO_COLDBOOT_ALLOWED) {
        proccessMenuNavigation(input, 2);
        if (entrySelected(input)) {
            if (this->selectedOption == 0) {
                this->state = STATE_CHECK_PATCH_POSSIBLE_DONE;
            } else {
                this->state = STATE_INSTALL_CONFIRM_DIALOG;
            }
            this->selectedOption = 0;
        }
    } else if (this->state == STATE_INSTALL_CONFIRM_DIALOG) {
        proccessMenuNavigation(input, 2);
        if (entrySelected(input)) {
            if (this->selectedOption == 0) {
                this->state = STATE_CHECK_PATCH_POSSIBLE_DONE;
            } else {
                this->state = STATE_INSTALL_STARTED;
                OSEnableHomeButtonMenu(false);
            }
            this->selectedOption = 0;
            return;
        }
    } else if (this->state == STATE_INSTALL_STARTED) {
        this->state = STATE_INSTALL_BACKUP;
    } else if (this->state == STATE_INSTALL_BACKUP) {
        auto result = InstallerService::backupAppFiles(this->appInfo->path);
        if (result != InstallerService::SUCCESS) {
            setError(ERROR_INSTALLER_ERROR);
            this->installerError = result;
        } else {
            this->state = STATE_INSTALL_FST;
        }
    } else if (this->state == STATE_INSTALL_FST) {
        auto result = InstallerService::patchFST(this->appInfo->path, this->appInfo->fstHash);
        if (result != InstallerService::SUCCESS) {
            setError(ERROR_INSTALLER_ERROR);
            this->installerError = result;
        } else {
            this->state = STATE_INSTALL_COS;
        }
    } else if (this->state == STATE_INSTALL_COS) {
        auto result = InstallerService::patchCOS(this->appInfo->path, this->appInfo->cosHash);
        if (result != InstallerService::SUCCESS) {
            setError(ERROR_INSTALLER_ERROR);
            this->installerError = result;
        } else {
            this->state = STATE_INSTALL_RPX;
        }
    } else if (this->state == STATE_INSTALL_RPX) {
        auto result = InstallerService::copyRPX(this->appInfo->path, root_rpx, root_rpx_size, RPX_HASH);
        if (result != InstallerService::SUCCESS) {
            setError(ERROR_INSTALLER_ERROR);
            this->installerError = result;
        } else {
            if (this->installColdboot) {
                this->state = STATE_INSTALL_SYSTEM_XML;
            } else {
                this->state = STATE_INSTALL_SUCCESS;
            }
        }
    } else if (this->state == STATE_INSTALL_SYSTEM_XML) {
        auto result = InstallerService::patchSystemXML("storage_slc_installer:/config", this->appInfo->titleId);
        if (result != InstallerService::SUCCESS) {
            setError(ERROR_INSTALLER_ERROR);
            this->installerError = result;
        } else {
            auto fsaFd = IOSUHAX_FSA_Open();
            if (fsaFd >= 0) {
                if (IOSUHAX_FSA_FlushVolume(fsaFd, "/vol/storage_mlc01") == 0) {
                    DEBUG_FUNCTION_LINE("Flushed mlc");
                }
                if (IOSUHAX_FSA_FlushVolume(fsaFd, "/vol/system") == 0) {
                    DEBUG_FUNCTION_LINE("Flushed slc");
                }
                IOSUHAX_FSA_Close(fsaFd);
            } else {
                DEBUG_FUNCTION_LINE("Failed to open fsa");
            }
            this->state = STATE_INSTALL_SUCCESS;
        }
    } else if (this->state == STATE_INSTALL_SUCCESS) {
        if (entrySelected(input)) {
            OSShutdown();
        }
    }
}

ApplicationState::ApplicationState() {
    this->state = STATE_WELCOME_SCREEN;
    this->selectedOption = 0;
    DEBUG_FUNCTION_LINE("State has changed to \"STATE_WELCOME_SCREEN\"");
}

void ApplicationState::checkPatchPossible() {
    DEBUG_FUNCTION_LINE("Check patch possible");
    if (!this->appInfo) {
        this->state = STATE_ERROR;
        this->error = ERROR_NO_APP_INSTALLED;
        DEBUG_FUNCTION_LINE("ERROR");
        return;
    }

    this->coldbootTitleId = InstallerService::getColdbootTitleId("storage_slc_installer:/config");

    this->coldbootTitle = nullptr;
    for (int i = 0; GameList[i].tid != 0; i++) {
        if (GameList[i].tid == this->coldbootTitleId) {
            this->coldbootTitle = &GameList[i];
            break;
        }
    }

    DEBUG_FUNCTION_LINE("CHECK FST");

    InstallerService::eResults result;

    this->fstAlreadyPatched = ((result = InstallerService::checkFSTAlreadyValid(this->appInfo->path, this->appInfo->fstHash)) == InstallerService::SUCCESS);
    this->rpxAlreadyPatched = ((result = InstallerService::checkRPXAlreadyValid(this->appInfo->path, RPX_HASH)) == InstallerService::SUCCESS);
    this->cosAlreadyPatched = ((result = InstallerService::checkCOSAlreadyValid(this->appInfo->path, this->appInfo->cosHash)) == InstallerService::SUCCESS);
    this->tmdValid = ((result = InstallerService::checkTMDValid(this->appInfo->path, this->appInfo->tmdHash)) == InstallerService::SUCCESS);

    this->fstPatchPossible = ((result = InstallerService::checkFST(this->appInfo->path, this->appInfo->fstHash)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }
    this->cosPatchPossible = ((result = InstallerService::checkCOS(this->appInfo->path, this->appInfo->cosHash)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }
    this->systemXMLPatchPossible = ((result = InstallerService::checkSystemXML("storage_slc_installer:/config", this->appInfo->titleId)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }
    this->state = STATE_CHECK_PATCH_POSSIBLE_DONE;

}

void ApplicationState::getAppInformation() {
    DEBUG_FUNCTION_LINE("About to call getInstalledAppInformation");
    this->appInfo = InstallerService::getInstalledAppInformation();
    if (!this->appInfo) {
        DEBUG_FUNCTION_LINE("ERROR =(");
        this->state = STATE_ERROR;
        this->error = ERROR_NO_APP_INSTALLED;
    } else {
        DEBUG_FUNCTION_LINE("WORKED!");
        this->state = STATE_CHECK_PATCH_POSSIBLE;
    }
}

std::string ApplicationState::ErrorMessage() {
    if (this->error == ERROR_NONE) {
        return "NONE";
    } else if (this->error == ERROR_NO_APP_INSTALLED) {
        return "ERROR_NO_APP_INSTALLED";
    } else if (this->error == ERROR_IOSUHAX_FAILED) {
        return "ERROR_IOSUHAX_FAILED";
    } else if (this->error == ERROR_INSTALLER_ERROR) {
        return InstallerService::ErrorMessage(this->installerError);
    }
    return "UNKNOWN_ERROR";
}

std::string ApplicationState::ErrorDescription() {
    if (this->error == ERROR_NONE) {
        return "-";
    } else if (this->error == ERROR_NO_APP_INSTALLED) {
        return "No compatible application is installed. A safe installation is not possible.";
    } else if (this->error == ERROR_INSTALLER_ERROR) {
        return InstallerService::ErrorDescription(this->installerError);
    } else if (this->error == ERROR_IOSUHAX_FAILED) {
        return "Failed to init IOSUHAX.";
    }
    return "UNKNOWN_ERROR";
}

void ApplicationState::setError(eErrorState err) {
    this->state = STATE_ERROR;
    this->error = err;
    OSEnableHomeButtonMenu(true);
}

void ApplicationState::handleError() {

}

void ApplicationState::printFooter() {
    ScreenUtils::printTextOnScreen(CONSOLE_SCREEN_TV, 0, 27, "By Maschell");
    ScreenUtils::printTextOnScreen(CONSOLE_SCREEN_DRC, 0, 17, "By Maschell");
}

void ApplicationState::proccessMenuNavigation(Input *input, int maxOptionValue) {
    if (input->data.buttons_d & Input::BUTTON_LEFT) {
        this->selectedOption--;
    } else if (input->data.buttons_d & Input::BUTTON_RIGHT) {
        this->selectedOption++;
    }
    if (this->selectedOption < 0) {
        this->selectedOption = maxOptionValue;
    } else if (this->selectedOption >= maxOptionValue) {
        this->selectedOption = 0;
    }
}

bool ApplicationState::entrySelected(Input *input) {
    return input->data.buttons_d & Input::BUTTON_A;
}
