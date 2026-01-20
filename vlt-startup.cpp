#include "shortcut_manager.hpp"
#include <iostream>
#include <shlwapi.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "shlwapi.lib")

void PrintUsage() {
  std::wcout << L"vlt-startup v0.1.0 - Startup Folder Management Tool"
             << std::endl;
  std::wcout << L"Usage:" << std::endl;
  std::wcout << L"  vlt-startup -a, --add <path> [options]" << std::endl;
  std::wcout << L"  vlt-startup -r, --remove <name>" << std::endl;
  std::wcout << L"" << std::endl;
  std::wcout << L"Options:" << std::endl;
  std::wcout << L"  --name <string>  Shortcut name (default: filename)"
             << std::endl;
  std::wcout << L"  --args <string>  Arguments for the program" << std::endl;
  std::wcout << L"  --admin          Run as administrator" << std::endl;
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

    if (ShortcutManager::CreateStartupShortcut(target, name, args, admin)) {
      std::wcout << L"Shortcut created: " << name << std::endl;
      return 0;
    } else {
      std::wcerr << L"Failed to create shortcut." << std::endl;
      return 1;
    }
  } else if (action == L"remove" && !name.empty()) {
    if (ShortcutManager::RemoveStartupShortcut(name)) {
      std::wcout << L"Shortcut removed: " << name << std::endl;
      return 0;
    } else {
      std::wcerr << L"Shortcut not found." << std::endl;
      return 1;
    }
  } else {
    PrintUsage();
    return 1;
  }
}
