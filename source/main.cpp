#include <coreinit/debug.h>

#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/proc.h>

#include "InstallerService.h"
#include "utils/WiiUScreen.h"
#include <input/CombinedInput.h>
#include <input/VPADInput.h>
#include <input/WPADInput.h>
#include <iosuhax.h>
#include <iosuhax_devoptab.h>
#include <string_view>

#include "../build/safe_payload.h"
#include "ApplicationState.h"

constexpr bool strings_equal(char const *a, char const *b) {
    return std::string_view(a) == b;
}

static_assert(strings_equal(RPX_HASH, "1736574cf6c949557aed0c817eb1927e35a9b820"), "Built with an untested root.rpx! Remove this check if you really know what you're doing.");

void initIOSUHax();

void deInitIOSUHax();

int sFSAFd         = -1;
bool sIosuhaxMount = false;

int main_loop() {
    DEBUG_FUNCTION_LINE("Creating state");
    ApplicationState state;
    CombinedInput baseInput;
    VPadInput vpadInput;
    WPADInput wpadInputs[4] = {
            WPAD_CHAN_0,
            WPAD_CHAN_1,
            WPAD_CHAN_2,
            WPAD_CHAN_3};

    if (sFSAFd < 0 || !sIosuhaxMount) {
        state.setError(ApplicationState::eErrorState::ERROR_IOSUHAX_FAILED);
    }

    DEBUG_FUNCTION_LINE("Entering main loop");
    while (WHBProcIsRunning()) {
        baseInput.reset();
        if (vpadInput.update(1280, 720)) {
            baseInput.combine(vpadInput);
        }
        for (auto &wpadInput : wpadInputs) {
            if (wpadInput.update(1280, 720)) {
                baseInput.combine(wpadInput);
            }
        }
        baseInput.process();
        state.update(&baseInput);
        state.render();
    }

    return 0;
}

int main(int argc, char **argv) {
    WHBLogUdpInit();
    DEBUG_FUNCTION_LINE("Hello from PayloadLoader Installer!");
    WHBProcInit();
    WiiUScreen::Init();

    initIOSUHax();

    WPADInput::init();

    main_loop();

    WPADInput::close();

    deInitIOSUHax();

    WiiUScreen::DeInit();
    WHBProcShutdown();

    return 0;
}

void initIOSUHax() {
    sIosuhaxMount = false;
    int res       = IOSUHAX_Open(nullptr);
    if (res < 0) {
        DEBUG_FUNCTION_LINE("IOSUHAX_open failed");
        OSFatal("IOSUHAX_open failed, please start this installer with an CFW");
    } else {
        sIosuhaxMount = true;
        sFSAFd        = IOSUHAX_FSA_Open();
        if (sFSAFd < 0) {
            DEBUG_FUNCTION_LINE("IOSUHAX_FSA_Open failed");
        } else {
            mount_fs("storage_slc_installer", sFSAFd, nullptr, "/vol/system");
            mount_fs("storage_mlc_installer", sFSAFd, nullptr, "/vol/storage_mlc01");
        }
        DEBUG_FUNCTION_LINE("IOSUHAX done");
    }
}

void deInitIOSUHax() {
    if (sIosuhaxMount) {
        unmount_fs("storage_slc_installer");
        unmount_fs("storage_mlc_installer");
        if (sFSAFd >= 0) {
            IOSUHAX_FSA_Close(sFSAFd);
        }
        IOSUHAX_Close();
    }
}