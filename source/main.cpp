#include <coreinit/time.h>
#include <coreinit/debug.h>

#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include <iosuhax.h>
#include <iosuhax_devoptab.h>
#include <string_view>
#include "WiiUScreen.h"
#include "utils/logger.h"
#include "InstallerService.h"

#include "../build/safe_payload.h"
#include "GameState.h"

constexpr bool strings_equal(char const *a, char const *b) {
    return std::string_view(a) == b;
}

static_assert(strings_equal(RPX_HASH, "6ce36d8838cab58a0f90f381119b12aca009974b"), "Built with an untested safe.rpx! Remove this check if you really know what you're doing.");

void initIOSUHax();

void deInitIOSUHax();


int hello_thread() {
    DEBUG_FUNCTION_LINE("Creating state");
    GameState state;

    DEBUG_FUNCTION_LINE("Entering main loop");
    while (WHBProcIsRunning()) {
        state.update();
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

    hello_thread();

    deInitIOSUHax();

    WiiUScreen::DeInit();
    WHBProcShutdown();

    return 0;
}

int sFSAFd = -1;
bool sIosuhaxMount = false;

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