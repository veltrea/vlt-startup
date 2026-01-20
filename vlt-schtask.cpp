#include "task_manager.hpp"
#include <iostream>
#include <shlwapi.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "shlwapi.lib")

void PrintUsage() {
  std::wcout << L"vlt-schtask v0.1.0 - Task Scheduler Startup Management Tool"
             << std::endl;
  std::wcout << L"Usage:" << std::endl;
  std::wcout << L"  vlt-schtask -a, --add <path> [options]" << std::endl;
  std::wcout << L"  vlt-schtask -r, --remove <name>" << std::endl;
  std::wcout << L"" << std::endl;
  std::wcout << L"Options:" << std::endl;
  std::wcout << L"  --name <string>  Task name" << std::endl;
  std::wcout << L"  --args <string>  Arguments for the program" << std::endl;
  std::wcout
      << L"  --admin          Run with highest privileges (no UAC if admin)"
      << std::endl;
}

int wmain(int argc, wchar_t *argv[]) {
  if (argc < 2) {
    PrintUsage();
    return 1;
  }

  std::wstring action = L"";
  std::wstring target = L"";
  std::wstring name = L"";
  std::wstring args = L"";
  bool admin = false;

  for (int i = 1; i < argc; ++i) {
    std::wstring arg = argv[i];
    if (arg == L"-a" || arg == L"--add") {
      action = L"add";
      if (i + 1 < argc)
        target = argv[++i];
    } else if (arg == L"-r" || arg == L"--remove") {
      action = L"remove";
      if (i + 1 < argc)
        name = argv[++i];
    } else if (arg == L"--name" && i + 1 < argc) {
      name = argv[++i];
    } else if (arg == L"--args" && i + 1 < argc) {
      args = argv[++i];
    } else if (arg == L"--admin") {
      admin = true;
    } else if (target.empty() && action.empty() && arg[0] != L'-') {
      action = L"add";
      target = arg;
    }
  }

  if (action == L"add" && !target.empty()) {
    if (name.empty()) {
      wchar_t *fname = PathFindFileNameW(target.c_str());
      wchar_t buffer[MAX_PATH];
      wcsncpy_s(buffer, fname, _TRUNCATE);
      PathRemoveExtensionW(buffer);
      name = buffer;
    }

    if (TaskManager::AddTask(name, target, args, admin)) {
      std::wcout << L"Task created: " << name << std::endl;
      return 0;
    } else {
      std::wcerr << L"Failed to create task." << std::endl;
      return 1;
    }
  } else if (action == L"remove" && !name.empty()) {
    if (TaskManager::RemoveTask(name)) {
      std::wcout << L"Task removed: " << name << std::endl;
      return 0;
    } else {
      std::wcerr << L"Task not found." << std::endl;
      return 1;
    }
  } else {
    PrintUsage();
    return 1;
  }
}
