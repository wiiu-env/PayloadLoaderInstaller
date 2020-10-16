#include <coreinit/time.h>
#include <coreinit/debug.h>

#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include <iosuhax.h>
#include <iosuhax_devoptab.h>
#include <string_view>
#include <vector>
#include "WiiUScreen.h"
#include "InstallerService.h"

#include "../build/safe_payload.h"
#include "ApplicationState.h"
#include "VPADInput.h"

constexpr bool strings_equal(char const *a, char const *b) {
    return std::string_view(a) == b;
}

static_assert(strings_equal(RPX_HASH, "2df9282cadcbe3fa86848ade9c67cbff12b72426"), "Built with an untested safe.rpx! Remove this check if you really know what you're doing.");

void initIOSUHax();

void deInitIOSUHax();

int sFSAFd = -1;
bool sIosuhaxMount = false;

int main_loop() {
    DEBUG_FUNCTION_LINE("Creating state");
    ApplicationState state;
    VPadInput input;

    if (sFSAFd < 0 || !sIosuhaxMount) {
        state.setError(ApplicationState::eErrorState::ERROR_IOSUHAX_FAILED);
    }

    DEBUG_FUNCTION_LINE("Entering main loop");
    while (WHBProcIsRunning()) {
        input.update(1280, 720);
        state.update(&input);
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

    main_loop();

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