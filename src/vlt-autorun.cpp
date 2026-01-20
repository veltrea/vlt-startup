#include "registry_manager.hpp"
#include <iostream>
#include <shlwapi.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "shlwapi.lib")

void PrintUsage() {
  std::wcout << L"vlt-autorun v0.1.0 - Registry Startup Management Tool"
             << std::endl;
  std::wcout << L"Usage:" << std::endl;
  std::wcout << L"  vlt-autorun -a, --add <path> [options]" << std::endl;
  std::wcout << L"  vlt-autorun -r, --remove <name>" << std::endl;
  std::wcout << L"  vlt-autorun -l, --list" << std::endl;
  std::wcout << L"  vlt-autorun -e, --enable <name>" << std::endl;
  std::wcout << L"  vlt-autorun -d, --disable <name>" << std::endl;
  std::wcout << L"" << std::endl;
  std::wcout << L"Options:" << std::endl;
  std::wcout << L"  --name <string>  Registry value name" << std::endl;
  std::wcout << L"  --args <string>  Arguments for the program" << std::endl;
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
    } else if (arg == L"-e" || arg == L"--enable") {
      action = L"enable";
      if (i + 1 < argc)
        name = argv[++i];
    } else if (arg == L"-d" || arg == L"--disable") {
      action = L"disable";
      if (i + 1 < argc)
        name = argv[++i];
    } else if (arg == L"-l" || arg == L"--list") {
      action = L"list";
    } else if (arg == L"--name" && i + 1 < argc) {
      name = argv[++i];
    } else if (arg == L"--args" && i + 1 < argc) {
      args = argv[++i];
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
    if (RegistryManager::AddToRunRegistry(name, target, args)) {
      std::wcout << L"Added to registry: " << name << std::endl;
      return 0;
    }
  } else if (action == L"remove" && !name.empty()) {
    if (RegistryManager::RemoveFromRunRegistry(name)) {
      std::wcout << L"Removed from registry: " << name << std::endl;
      return 0;
    }
  } else if (action == L"list") {
    RegistryManager::ListItems();
    return 0;
  } else if (action == L"enable" && !name.empty()) {
    if (RegistryManager::SetStartupStatus(name, true)) {
      std::wcout << L"Enabled: " << name << std::endl;
      return 0;
    }
  } else if (action == L"disable" && !name.empty()) {
    if (RegistryManager::SetStartupStatus(name, false)) {
      std::wcout << L"Disabled: " << name << std::endl;
      return 0;
    }
  } else {
    PrintUsage();
  }
  return 1;
}
