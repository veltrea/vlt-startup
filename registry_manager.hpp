#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

class RegistryManager {
public:
  static bool AddToRunRegistry(const std::wstring &name,
                               const std::wstring &targetPath,
                               const std::wstring &args) {
    HKEY hKey;
    std::wstring commandLine = L"\"" + targetPath + L"\" " + args;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
                      KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
      LSTATUS status = RegSetValueExW(
          hKey, name.c_str(), 0, REG_SZ, (BYTE *)commandLine.c_str(),
          (DWORD)((commandLine.length() + 1) * sizeof(wchar_t)));
      RegCloseKey(hKey);

      if (status == ERROR_SUCCESS) {
        // Also set StartupApproved to "Enabled" (02 00 00 00 ...)
        SetStartupStatus(name, true);
        return true;
      }
    }
    return false;
  }

  static bool RemoveFromRunRegistry(const std::wstring &name) {
    HKEY hKey;
    bool success = false;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
                      KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
      if (RegDeleteValueW(hKey, name.c_str()) == ERROR_SUCCESS) {
        success = true;
      }
      RegCloseKey(hKey);
    }

    // Also remove from StartupApproved
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
                      L"\\StartupApproved\\Run",
                      0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
      RegDeleteValueW(hKey, name.c_str());
      RegCloseKey(hKey);
    }

    return success;
  }

  static bool SetStartupStatus(const std::wstring &name, bool enabled) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
                      L"\\StartupApproved\\Run",
                      0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
      BYTE enabledValue[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      BYTE disabledValue[] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Simplified

      LSTATUS status = RegSetValueExW(hKey, name.c_str(), 0, REG_BINARY,
                                      enabled ? enabledValue : disabledValue,
                                      sizeof(enabledValue));
      RegCloseKey(hKey);
      return status == ERROR_SUCCESS;
    }
    return false;
  }

  static void ListItems() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
                      KEY_READ, &hKey) == ERROR_SUCCESS) {
      wchar_t valueName[16383];
      DWORD valueNameLen = 16383;
      DWORD index = 0;

      std::wcout << L"Current Registry Startup Items:" << std::endl;
      while (RegEnumValueW(hKey, index, valueName, &valueNameLen, NULL, NULL,
                           NULL, NULL) == ERROR_SUCCESS) {
        std::wcout << L"  - " << valueName << std::endl;
        valueNameLen = 16383;
        index++;
      }
      RegCloseKey(hKey);
    }
  }
};
