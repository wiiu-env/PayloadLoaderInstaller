#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <coreinit/debug.h>
#include <nn/ac.h>

#include <whb/proc.h>
#include <whb/log.h>

#include <iosuhax.h>
#include <iosuhax_devoptab.h>
#include "WiiUScreen.h"
#include "logger.h"

void initIOSUHax();

void deInitIOSUHax();

int
hello_thread() {
    int last_tm_sec = -1;
    uint32_t ip = 0;

    WHBLogPrintf("Hello!");

    if (!nn::ac::GetAssignedAddress(&ip)) {
        WHBLogPrintf("GetAssignedAddress failed!");
    }

    WHBLogPrintf("My IP is: %u.%u.%u.%u",
                 (ip >> 24) & 0xFF,
                 (ip >> 16) & 0xFF,
                 (ip >> 8) & 0xFF,
                 (ip >> 0) & 0xFF);

    while (WHBProcIsRunning()) {
        OSCalendarTime tm;
        OSTicksToCalendarTime(OSGetTime(), &tm);

        if (tm.tm_sec != last_tm_sec) {
            WiiUScreen::clearScreen();
            WiiUScreen::drawLine("Hello World from a std::thread!");
            WiiUScreen::drawLinef("%02d/%02d/%04d %02d:%02d:%02d I'm still here.",
                                  tm.tm_mday, tm.tm_mon + 1, tm.tm_year,
                                  tm.tm_hour, tm.tm_min, tm.tm_sec);
            last_tm_sec = tm.tm_sec;
            WiiUScreen::flush();
        }

        OSSleepTicks(OSMillisecondsToTicks(100));
    }

    WHBLogPrintf("Exiting... good bye.");
    OSSleepTicks(OSMillisecondsToTicks(1000));
    return 0;
}

int main(int argc, char **argv) {
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
    auto fsaFd = -1;
    int res = IOSUHAX_Open(nullptr);
    if (res < 0) {
        DEBUG_FUNCTION_LINE("IOSUHAX_open failed");
        OSFatal("IOSUHAX_open failed, please start this installer with an CFW");
    } else {
        sIosuhaxMount = true;
        sFSAFd = IOSUHAX_FSA_Open();
        if (fsaFd < 0) {
            DEBUG_FUNCTION_LINE("IOSUHAX_FSA_Open failed");
        } else {
            mount_fs("storage_slc_installer", fsaFd, nullptr, "/vol/system");
            mount_fs("storage_mlc_installer", fsaFd, nullptr, "/vol/storage_mlc01");
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