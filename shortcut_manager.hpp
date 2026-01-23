#pragma once
#include <iostream>
#include <shldisp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <string>
#include <windows.h>

class ShortcutManager {
public:
  static bool CreateStartupShortcut(const std::wstring &targetPath,
                                    const std::wstring &name,
                                    const std::wstring &args, bool admin) {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
      return false;
    }

    bool success = false;
    IShellLinkW *psl = nullptr;

    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                          IID_IShellLinkW, (LPVOID *)&psl);
    if (SUCCEEDED(hr)) {
      IPersistFile *ppf = nullptr;

      psl->SetPath(targetPath.c_str());
      psl->SetArguments(args.c_str());

      // Set working directory to target path's directory
      wchar_t drive[_MAX_DRIVE];
      wchar_t dir[_MAX_DIR];
      _wsplitpath_s(targetPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR,
                    nullptr, 0, nullptr, 0);
      std::wstring workingDir = std::wstring(drive) + dir;
      psl->SetWorkingDirectory(workingDir.c_str());

      if (admin) {
        IShellLinkDataList *pdl = nullptr;
        if (SUCCEEDED(
                psl->QueryInterface(IID_IShellLinkDataList, (LPVOID *)&pdl))) {
          DWORD flags = 0;
          pdl->GetFlags(&flags);
          flags |= SLDF_RUNAS_USER;
          pdl->SetFlags(flags);
          pdl->Release();
        }
      }

      hr = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);
      if (SUCCEEDED(hr)) {
        wchar_t startupPath[MAX_PATH];
        if (SHGetSpecialFolderPathW(NULL, startupPath, CSIDL_STARTUP, FALSE)) {
          std::wstring lnkPath =
              std::wstring(startupPath) + L"\\" + name + L".lnk";
          hr = ppf->Save(lnkPath.c_str(), TRUE);
          if (SUCCEEDED(hr)) {
            success = true;
          }
        }
        ppf->Release();
      }
      psl->Release();
    }

    CoUninitialize();
    return success;
  }

  static bool RemoveStartupShortcut(const std::wstring &name) {
    wchar_t startupPath[MAX_PATH];
    if (SHGetSpecialFolderPathW(NULL, startupPath, CSIDL_STARTUP, FALSE)) {
      std::wstring lnkPath = std::wstring(startupPath) + L"\\" + name + L".lnk";
      if (PathFileExistsW(lnkPath.c_str())) {
        return DeleteFileW(lnkPath.c_str()) != 0;
      }
    }
    return false;
  }
};
