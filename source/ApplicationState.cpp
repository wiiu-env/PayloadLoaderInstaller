#include "ApplicationState.h"
#include "WiiUScreen.h"
#include "ScreenUtils.h"
#include "StringTools.h"
#include "../build/safe_payload.h"
#include <sysapp/launch.h>
#include <iosuhax.h>

extern "C" void OSShutdown();

void ApplicationState::changeState(eGameState newState) {
    this->state = newState;

    menu.clear();
    if (this->state == STATE_ERROR) {
        menu.addText("The installation failed:");
        menu.addText();
        menu.addText("Error:       " + ErrorMessage());
        menu.addText("Description: " + ErrorDescription());
        menu.addText();
        menu.addText();
        menu.addOption("Press A to return to the Wii U Menu.", STATE_EXIT_SYSMENU);
    } else if (this->state == STATE_WELCOME_SCREEN) {
        menu.addText("Welcome to the Aroma Installer!");
        menu.addText("Do you want to check if an installation is possible?");
        menu.addText();
        menu.addOption("Check", STATE_GET_APP_INFORMATION);
        menu.addOption("Exit", STATE_EXIT_SYSMENU);
    } else if (this->state == STATE_GET_APP_INFORMATION) {
        menu.addText("Getting app information");
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE) {
        menu.addText("Check if console can be patched.");
    } else if (this->state == STATE_CHECK_COLDBOOT_STATUS) {
        menu.addText("Check if coldboot can be enabled.");
    } else if (this->state == STATE_CHECK_REMOVAL_POSSIBLE) {
        menu.addText("Check if Aroma can be removed.");
    } else if (this->state == STATE_APP_INCOMPATIBLE) {
        menu.addText("Sorry, Aroma cannot be safely installed to:");
        menu.addText(std::string(appInfo->appName));
        menu.addText();
        menu.addText("Additional informations:");
        auto showCheckResult = [&] (const std::string &name, bool canPatch, bool patched) {
            if (patched) {
                menu.addText("[ X ] " + name + " is already patched!");
            } else if (canPatch) {
                menu.addText("[ X ] " + name + " can be patched!");
            } else {
                menu.addText("[   ] " + name + " can NOT be patched!");
            }
        };
        if (this->tmdValid) {
            menu.addText("[ X ] title.tmd is valid!");
        } else {
            menu.addText("[   ] title.tmd is NOT valid!");
        }
        showCheckResult("title.fst", this->fstPatchPossible, this->fstAlreadyPatched);
        showCheckResult("cos.xml", this->cosPatchPossible, this->cosAlreadyPatched);
        showCheckResult("safe.rpx", true, this->rpxAlreadyPatched);
        menu.addText();
        menu.addOption("Exit", STATE_EXIT_SYSMENU);
    } else if (this->state == STATE_MAIN_MENU) {
        menu.addText("Aroma " + std::string(this->alreadyInstalledAndUpdated ? "is" : "can be") + " installed to:");
        menu.addText(std::string(appInfo->appName));
        menu.addText();
        menu.addOption("Install / Update", STATE_INSTALL_CONFIRM_DIALOG);
        menu.addOption("Boot options", STATE_BOOT_MENU);
        if (this->removalPossible) {
            menu.addOption("Remove", STATE_REMOVE_CONFIRM_DIALOG);
        }
        menu.addOption("Exit", STATE_EXIT_SYSMENU);
    } else if (this->state == STATE_INSTALL_CONFIRM_DIALOG) {
        if (this->alreadyInstalledAndUpdated) {
            menu.addText("Everything is already up to date.");
            menu.addText();
            menu.addOption("Back", STATE_MAIN_MENU);
        } else {
            if (this->coldbootTitleId == this->appInfo->titleId) {
                menu.addText("Before you can install/update Aroma you need to change");
                menu.addText("the coldboot title back to Wii U Menu");
                menu.addText();
                menu.addOption("Back", STATE_MAIN_MENU);

            } else {
                menu.addText("Are you REALLY sure you want to install Aroma?");
                menu.addText("Installing could permanently damage your console");
                menu.addText();
                menu.addText("After the installation the following app will turn into");
                menu.addText("a payload.elf loader. Loading it without a sd card will");
                menu.addText("ALWAYS open the Wii U Menu");
                menu.addText("- " + std::string(appInfo->appName));
                menu.addText();
                menu.addOption("Back", STATE_MAIN_MENU);
                menu.addOption("Install", STATE_INSTALL_STARTED);
            }
        }
    } else if (this->state == STATE_INSTALL_STARTED) {
        menu.addText("Installing...");
    } else if (this->state == STATE_INSTALL_BACKUP) {
        menu.addText("... backing up files");
    } else if (this->state == STATE_INSTALL_FST) {
        menu.addText("... patching title.fst");
    } else if (this->state == STATE_INSTALL_COS) {
        menu.addText("... patching cos.xml");
    } else if (this->state == STATE_INSTALL_RPX) {
        menu.addText("... install safe.rpx");
    } else if (this->state == STATE_INSTALL_SUCCESS) {
        menu.addText("Aroma was successfully installed");
        menu.addText();
        menu.addOption("Press A to shutdown the console", STATE_EXIT_SHUTDOWN);
    } else if (this->state == STATE_REMOVE_CONFIRM_DIALOG) {
        if (this->systemXMLAlreadyPatched) {
            menu.addText("Before you can remove Aroma you need to switch");
            menu.addText("the system boot title back to the Wii U Menu");
            menu.addText();
            menu.addOption("Back", STATE_MAIN_MENU);
        } else {
            menu.addText("Are you REALLY sure you want to remove Aroma?");
            menu.addText();
            menu.addOption("Back", STATE_MAIN_MENU);
            menu.addOption("Remove", STATE_REMOVE_STARTED);
        }
    } else if (this->state == STATE_REMOVE_STARTED) {
        menu.addText("Removing...");
    } else if (this->state == STATE_REMOVE_COLDBOOT) {
        menu.addText("... remove system.xml coldboot patches");
    } else if (this->state == STATE_REMOVE_AROMA) {
        menu.addText("... remove Aroma application patches");
    } else if (this->state == STATE_REMOVE_SUCCESS) {
        menu.addText("Aroma was successfully removed");
        menu.addText();
        menu.addOption("Press A to shutdown the console", STATE_EXIT_SHUTDOWN);
    } else if (this->state == STATE_BOOT_MENU) {
        menu.addText("System is currently booting into: ");
        std::string titleId = StringTools::strfmt("%ll016X", this->coldbootTitleId);
        std::string titleName = this->coldbootTitle ?
            std::string(this->coldbootTitle->name) : "Unknown title";
        menu.addText(titleId + " (" + titleName + ")");
        menu.addText();
        if (this->systemXMLRestorePossible && this->systemXMLAlreadyPatched) {
            menu.addText("If you have modified the Wii U Menu this could make");
            menu.addText("your console unusable");
            menu.addText();

            menu.addOption("Switch back to Wii U Menu", STATE_BOOT_SWITCH_SYSMENU);
        } else if (this->systemXMLPatchAllowed) {
            menu.addOption("Switch to Aroma", STATE_BOOT_SWITCH_AROMA);
        } else if (this->systemXMLPatchAllowedButNoRPXCheck) {
            menu.addText("Your RPX is not as expected. You probably");
            menu.addText("need to update or re-install Aroma first.");
            menu.addText();
            menu.addOption("Back", STATE_MAIN_MENU);
        } else if (this->systemXMLPatchPossible) {
            menu.addText("To change the system boot title to Aroma, you need to");
            menu.addText("launch this installer from an already running Aroma");
            menu.addText("instance, in order to verify that the installation");
            menu.addText("is working properly.");
            menu.addText();
            menu.addText("After installing Aroma, reboot the console, open the");
            menu.addText("Health & Safety app and relaunch the Aroma installer.");
            menu.addText();
        } else {
            menu.addText("Sorry, your system.xml file has not yet been tested");
            menu.addText("with this tool. Boot options cannot be modified.");
            menu.addText();
        }
        menu.addOption("Back", STATE_MAIN_MENU);
    } else if (this->state == STATE_BOOT_SWITCH_AROMA) {
        menu.addText("Changing system.xml to boot " + std::string(this->appInfo->appName) + " ...");
    } else if (this->state == STATE_BOOT_SWITCH_SYSMENU) {
        menu.addText("Changing system.xml to boot System Menu ...");
    } else if (this->state == STATE_BOOT_SWITCH_SUCCESS) {
        menu.addText("Boot title successfully updated!");
        menu.addText();
        menu.addOption("Press A to shutdown the console", STATE_EXIT_SHUTDOWN);
    }

    this->state = newState;
}

void ApplicationState::render() {
    menu.render();
}

void ApplicationState::update(Input *input) {
    if (this->state == STATE_ERROR) {
        OSEnableHomeButtonMenu(true);
    } else if (this->state == STATE_GET_APP_INFORMATION) {
        getAppInformation();
    } else if (this->state == STATE_CHECK_PATCH_POSSIBLE) {
        checkPatchPossible();
    } else if (this->state == STATE_CHECK_COLDBOOT_STATUS) {
        checkColdbootStatus();
    } else if (this->state == STATE_CHECK_REMOVAL_POSSIBLE) {
        checkRemovalPossible();
    } else if (this->state == STATE_COMPATIBILITY_RESULTS) {
        if (this->installPossible) {
            changeState(STATE_MAIN_MENU);
        } else {
            changeState(STATE_APP_INCOMPATIBLE);
        }
    } else if (this->state == STATE_INSTALL_STARTED) {
        OSEnableHomeButtonMenu(false);
        changeState(STATE_INSTALL_BACKUP);
    } else if (this->state == STATE_INSTALL_BACKUP) {
        auto result = InstallerService::backupAppFiles(this->appInfo->path);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_INSTALL_FST);
        }
    } else if (this->state == STATE_INSTALL_FST) {
        auto result = (this->fstAlreadyPatched) ? InstallerService::SUCCESS :
            InstallerService::patchFST(this->appInfo->path, this->appInfo->fstHash);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_INSTALL_COS);
        }
    } else if (this->state == STATE_INSTALL_COS) {
        auto result = (this->cosAlreadyPatched) ? InstallerService::SUCCESS :
            InstallerService::patchCOS(this->appInfo->path, this->appInfo->cosHash);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_INSTALL_RPX);
        }
    } else if (this->state == STATE_INSTALL_RPX) {
        auto result = InstallerService::copyRPX(this->appInfo->path, root_rpx, root_rpx_size, RPX_HASH);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_INSTALL_SUCCESS);
        }
    } else if (this->state == STATE_REMOVE_STARTED) {
        OSEnableHomeButtonMenu(false);

        if (this->systemXMLAlreadyPatched) {
            // It's only possible to remove aroma when it's not coldbooting into aroma.
            //    changeState(STATE_REMOVE_COLDBOOT);
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_REMOVE_AROMA);
        }
    } else if (this->state == STATE_REMOVE_COLDBOOT) {
        auto result = InstallerService::setBootTitle(*this->systemMenuTitleId);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_REMOVE_AROMA);
        }
    } else if (this->state == STATE_REMOVE_AROMA) {
        auto result = InstallerService::restoreAppFiles(this->appInfo->path);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_REMOVE_SUCCESS);
        }
    } else if (this->state == STATE_BOOT_SWITCH_SYSMENU) {
        OSEnableHomeButtonMenu(false);
        auto result = InstallerService::setBootTitle(*this->systemMenuTitleId);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_BOOT_SWITCH_SUCCESS);
        }
    } else if (this->state == STATE_BOOT_SWITCH_AROMA) {
        OSEnableHomeButtonMenu(false);
        auto result = InstallerService::setBootTitle(this->appInfo->titleId);
        if (result != InstallerService::SUCCESS) {
            this->installerError = result;
            setError(ERROR_INSTALLER_ERROR);
        } else {
            changeState(STATE_BOOT_SWITCH_SUCCESS);
        }
    } else if (this->state == STATE_EXIT_SYSMENU) {
        SYSLaunchMenu();
    } else if (this->state == STATE_EXIT_SHUTDOWN) {
        OSShutdown();
    }

    menu.update(input);
}

ApplicationState::ApplicationState() {
    menu.setOptionsCallback([this](auto &&newState) { changeState(std::forward<decltype(newState)>(newState)); });
    menu.setHeader("Aroma Installer");
    menu.setFooter("By Maschell");

    changeState(STATE_WELCOME_SCREEN);
    DEBUG_FUNCTION_LINE("State has changed to \"STATE_WELCOME_SCREEN\"");
}

void ApplicationState::checkPatchPossible() {
    DEBUG_FUNCTION_LINE("Check patch possible");

    this->fstAlreadyPatched = (InstallerService::checkFSTAlreadyValid(this->appInfo->path, this->appInfo->fstHash) == InstallerService::SUCCESS);
    this->rpxAlreadyPatched = (InstallerService::checkRPXAlreadyValid(this->appInfo->path, RPX_HASH) == InstallerService::SUCCESS);
    this->cosAlreadyPatched = (InstallerService::checkCOSAlreadyValid(this->appInfo->path, this->appInfo->cosHash) == InstallerService::SUCCESS);
    this->tmdValid = (InstallerService::checkTMDValid(this->appInfo->path, this->appInfo->tmdHash, this->appInfo->tmdWithCertHash) == InstallerService::SUCCESS);

    InstallerService::eResults result;

    this->fstPatchPossible = ((result = InstallerService::checkFST(this->appInfo->path, this->appInfo->fstHash)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }
    this->cosPatchPossible = ((result = InstallerService::checkCOS(this->appInfo->path, this->appInfo->cosHash)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }

    this->installPossible = this->fstPatchPossible && this->cosPatchPossible && this->tmdValid;
    this->alreadyInstalledAndUpdated = this->fstAlreadyPatched && this->cosAlreadyPatched && this->tmdValid && this->rpxAlreadyPatched;

    changeState(STATE_CHECK_COLDBOOT_STATUS);
}

void ApplicationState::checkColdbootStatus() {
    DEBUG_FUNCTION_LINE("Check coldboot status");

    // Read the current coldboot title from the system.xml
    this->coldbootTitleId = InstallerService::getColdbootTitleId("storage_slc_installer:/config");

    // Try getting more information about the current coldboot title.
    this->coldbootTitle = nullptr;
    for (int i = 0; GameList[i].tid != 0; i++) {
        if (GameList[i].tid == this->coldbootTitleId) {
            this->coldbootTitle = &GameList[i];
            break;
        }
    }

    InstallerService::eResults result;

    this->systemMenuTitleId = InstallerService::getSystemMenuTitleId();

    // Check if setting the title id to H&S results in a hash we are expecting
    this->systemXMLPatchPossible = ((result = InstallerService::checkSystemXML("storage_slc_installer:/config", this->appInfo->titleId)) == InstallerService::SUCCESS);
    if (result != InstallerService::SUCCESS) {
        DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
    }

    if (this->systemMenuTitleId) {
        // Check if setting the title id back to Wii U menu results in a hash we are expecting
        this->systemXMLRestorePossible = ((result = InstallerService::checkSystemXML("storage_slc_installer:/config", *this->systemMenuTitleId)) == InstallerService::SUCCESS);
        if (result != InstallerService::SUCCESS) {
            DEBUG_FUNCTION_LINE("ERROR: %s", InstallerService::ErrorMessage(result).c_str());
        }
    } else {
        this->systemXMLRestorePossible = false;
    }

    if (this->systemMenuTitleId) {
        // If we are not booting into the Wii U menu, we know it's already patched.
        this->systemXMLAlreadyPatched = (this->coldbootTitleId != *this->systemMenuTitleId);
    } else {
        // If we for some fail to get the "systemMenuTitleId" we can still if the system.xml is patched
        // by comparing with the H&S title id
        this->systemXMLAlreadyPatched = (this->coldbootTitleId == this->appInfo->titleId);
    }

    this->systemXMLPatchAllowed = this->systemXMLPatchPossible && this->alreadyInstalledAndUpdated && InstallerService::isColdBootAllowed();
    this->systemXMLPatchAllowedButNoRPXCheck = this->systemXMLPatchPossible && this->fstAlreadyPatched && this->cosAlreadyPatched && this->tmdValid && InstallerService::isColdBootAllowed();

    changeState(STATE_CHECK_REMOVAL_POSSIBLE);
}

void ApplicationState::checkRemovalPossible() {
    DEBUG_FUNCTION_LINE("Check removal possible");

    // We can only restore if a restore of the system.xml is possible or it isn't patched at all.
    this->removalPossible = !this->systemXMLAlreadyPatched || this->systemXMLRestorePossible;

    // And we can only install if we have a backup.
    if (this->removalPossible) {
        this->removalPossible &= InstallerService::isBackupAvailable(this->appInfo->path);
    }
    changeState(STATE_COMPATIBILITY_RESULTS);
}

void ApplicationState::getAppInformation() {
    DEBUG_FUNCTION_LINE("About to call getInstalledAppInformation");
    this->appInfo = InstallerService::getInstalledAppInformation();
    if (!this->appInfo) {
        DEBUG_FUNCTION_LINE("ERROR =(");
        setError(ERROR_NO_APP_INSTALLED);
    } else {
        DEBUG_FUNCTION_LINE("WORKED!");
        changeState(STATE_CHECK_PATCH_POSSIBLE);
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
    } else if (this->error == ERROR_UNEXPECTED_STATE) {
        return "ERROR_UNEXPECTED_STATE";
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
    } else if (this->error == ERROR_UNEXPECTED_STATE) {
        return "ERROR_UNEXPECTED_STATE";
    }
    return "UNKNOWN_ERROR";
}

void ApplicationState::setError(eErrorState err) {
    this->error = err;
    OSEnableHomeButtonMenu(true);
    changeState(STATE_ERROR);
}
