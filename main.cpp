#include <iostream>
#include "osplatformutil.h"

#ifdef I_OS_WIN

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "advapi32.lib")
#define SVCNAME TEXT("SvcName")

void SvcInstall() {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    // Get a handle to the SCM database.

    schSCManager = OpenSCManager(
            NULL,                    // local computer
            NULL,                    // ServicesActive database
            SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Create the service

    schService = CreateService(
            schSCManager,              // SCM database
            SVCNAME,                   // name of service
            SVCNAME,                   // service name to display
            SERVICE_ALL_ACCESS,        // desired access
            SERVICE_WIN32_OWN_PROCESS, // service type
            SERVICE_AUTO_START,      // start type
            SERVICE_ERROR_NORMAL,      // error control type
            szPath,                    // path to service's binary
            NULL,                      // no load ordering group
            NULL,                      // no tag identifier
            NULL,                      // no dependencies
            NULL,                      // LocalSystem account
            NULL);                     // no password

    if (schService == NULL) {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    } else printf("Service installed successfully\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

#endif

#ifdef I_OS_UNIX

#include <fstream>
#include <cstdlib>
void SvcInstall() {
    std::ofstream ofs("/lib/systemd/system/SvcName.service");
    ofs << "[Unit]" << std::endl
        << "After=network.target" << std::endl
        << "[Service]" << std::endl
        << "ExecStart=/usr/sbin/SvcName" << std::endl
        << "ExecReload=/bin/kill -HUP $MAINPID" << std::endl
        << "Type=simple" << std::endl
        << "KillMode=control-group" << std::endl
        << "[Install]" << std::endl
        << "WantedBy=multi-user.target" << std::endl;
    ofs.close();
    system("chmod 754 /lib/systemd/system/SvcName.service");
    system("systemctl daemon-reload");
    system("systemctl enable SvcName.service");
}

#endif

int main() {
    SvcInstall();
    std::cout << "Hello, World!\a" << std::endl;
    system("pause");
    return 0;
}