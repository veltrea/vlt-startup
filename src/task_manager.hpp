#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

class TaskManager {
public:
  static bool AddTask(const std::wstring &name, const std::wstring &targetPath,
                      const std::wstring &args, bool admin) {
    // Build schtasks command
    // /Create /TN "Name" /TR "'Path' args" /SC ONLOGON /RL HIGHEST (if admin)
    // /F

    std::wstring command = L"schtasks /Create /TN \"" + name + L"\"";
    std::wstring taskRun = L"\"" + targetPath + L"\" " + args;
    // Escape quotes for schtasks /TR
    std::wstring escapedRun = L"";
    for (wchar_t c : taskRun) {
      if (c == L'\"')
        escapedRun += L"\\\"";
      else
        escapedRun += c;
    }

    command += L" /TR \"" + escapedRun + L"\" /SC ONLOGON /F";
    if (admin) {
      command += L" /RL HIGHEST";
    }

    // Execute command
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (CreateProcessW(NULL, (LPWSTR)command.c_str(), NULL, NULL, FALSE,
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
      WaitForSingleObject(pi.hProcess, INFINITE);
      DWORD exitCode;
      GetExitCodeProcess(pi.hProcess, &exitCode);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return exitCode == 0;
    }
    return false;
  }

  static bool RemoveTask(const std::wstring &name) {
    std::wstring command = L"schtasks /Delete /TN \"" + name + L"\" /F";

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (CreateProcessW(NULL, (LPWSTR)command.c_str(), NULL, NULL, FALSE,
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
      WaitForSingleObject(pi.hProcess, INFINITE);
      DWORD exitCode;
      GetExitCodeProcess(pi.hProcess, &exitCode);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return exitCode == 0;
    }
    return false;
  }
};
