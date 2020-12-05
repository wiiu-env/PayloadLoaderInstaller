#include <coreinit/debug.h>

#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include <iosuhax.h>
#include <iosuhax_devoptab.h>
#include <string_view>
#include "WiiUScreen.h"
#include "InstallerService.h"

#include "ApplicationState.h"
#include "WPADInput.h"
#include "../build/safe_payload.h"

constexpr bool strings_equal(char const *a, char const *b) {
    return std::string_view(a) == b;
}

static_assert(strings_equal(RPX_HASH, "9ab80503f82c7e1403e6f966346c825a89cc613f"), "Built with an untested root.rpx! Remove this check if you really know what you're doing.");

void initIOSUHax();

void deInitIOSUHax();

int sFSAFd = -1;
bool sIosuhaxMount = false;

int main_loop() {
    DEBUG_FUNCTION_LINE("Creating state");
    ApplicationState state;
    VPadInput vpadInput;
    WPADInput wpadInputs[4] = {
        WPAD_CHAN_0,
        WPAD_CHAN_1,
        WPAD_CHAN_2,
        WPAD_CHAN_3
    };

    if (sFSAFd < 0 || !sIosuhaxMount) {
        state.setError(ApplicationState::eErrorState::ERROR_IOSUHAX_FAILED);
    }

    DEBUG_FUNCTION_LINE("Entering main loop");
    while (WHBProcIsRunning()) {
        vpadInput.update(1280, 720);
        for (int i = 0; i < 4; i++) {
            wpadInputs[i].update(1280, 720);
            vpadInput.combine(wpadInputs[i]);
        }
        state.update(&vpadInput);
        state.render();
    }

    return 0;
}

int main(int argc, char **argv) {
    WHBLogUdpInit();
    DEBUG_FUNCTION_LINE("Hello from Aroma Installer!");
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
    int res = IOSUHAX_Open(nullptr);
    if (res < 0) {
        DEBUG_FUNCTION_LINE("IOSUHAX_open failed");
        OSFatal("IOSUHAX_open failed, please start this installer with an CFW");
    } else {
        sIosuhaxMount = true;
        sFSAFd = IOSUHAX_FSA_Open();
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