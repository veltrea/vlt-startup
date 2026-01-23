#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <taskschd.h>
#include <comdef.h>
#include <iomanip>
#include <lmcons.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsuppw.lib")

class TaskManager {
public:
    static bool AddTask(const std::wstring& name, const std::wstring& targetPath,
                        const std::wstring& args, bool admin) {
        
        HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        bool coInitCalled = SUCCEEDED(hr);
        if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
            std::wcerr << L"CoInitializeEx failed: 0x" << std::hex << hr << std::endl;
            return false;
        }

        bool success = false;
        ITaskService* pService = NULL;
        ITaskFolder* pRootFolder = NULL;
        ITaskDefinition* pTask = NULL;
        IRegistrationInfo* pRegInfo = NULL;
        IPrincipal* pPrincipal = NULL;
        ITaskSettings* pSettings = NULL;
        ITriggerCollection* pTriggerCollection = NULL;
        ITrigger* pTrigger = NULL;
        ILogonTrigger* pLogonTrigger = NULL;
        IActionCollection* pActionCollection = NULL;
        IAction* pAction = NULL;
        IExecAction* pExecAction = NULL;
        IRegisteredTask* pRegisteredTask = NULL;

        try {
            hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
            if (FAILED(hr)) throw hr;

            hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
            if (FAILED(hr)) throw hr;

            hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
            if (FAILED(hr)) throw hr;

            // Try to delete existing task, ignore error
            pRootFolder->DeleteTask(_bstr_t(name.c_str()), 0);

            hr = pService->NewTask(0, &pTask);
            if (FAILED(hr)) throw hr;

            hr = pTask->get_RegistrationInfo(&pRegInfo);
            if (SUCCEEDED(hr)) {
                pRegInfo->put_Author(_bstr_t(L"vlt-startup"));
                pRegInfo->Release();
            }

            // Set Principal
            hr = pTask->get_Principal(&pPrincipal);
            if (SUCCEEDED(hr)) {
                pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
                pPrincipal->put_RunLevel(admin ? TASK_RUNLEVEL_HIGHEST : TASK_RUNLEVEL_LUA);
                pPrincipal->Release();
            }

            // Set Settings
            hr = pTask->get_Settings(&pSettings);
            if (SUCCEEDED(hr)) {
                pSettings->put_StartWhenAvailable(VARIANT_TRUE);
                pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
                pSettings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
                pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S")); // No limit
                pSettings->Release();
            }

            // Set Logon Trigger with UserId (Critical for non-admin)
            hr = pTask->get_Triggers(&pTriggerCollection);
            if (SUCCEEDED(hr)) {
                hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
                if (SUCCEEDED(hr)) {
                     hr = pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
                     if (SUCCEEDED(hr)) {
                        wchar_t username[UNLEN + 1];
                        DWORD usernameLen = UNLEN + 1;
                        if (GetUserNameW(username, &usernameLen)) {
                            pLogonTrigger->put_UserId(_bstr_t(username));
                        }
                        pLogonTrigger->Release();
                     }
                     pTrigger->Release();
                }
                pTriggerCollection->Release();
            }

            // Set Actions
            hr = pTask->get_Actions(&pActionCollection);
            if (FAILED(hr)) throw hr;
            hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
            if (FAILED(hr)) throw hr;
            hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
            if (SUCCEEDED(hr)) {
                pExecAction->put_Path(_bstr_t(targetPath.c_str()));
                if (!args.empty()) pExecAction->put_Arguments(_bstr_t(args.c_str()));
                pExecAction->Release();
            }
            pAction->Release();
            pActionCollection->Release();

            // Register Task
            hr = pRootFolder->RegisterTaskDefinition(
                _bstr_t(name.c_str()),
                pTask,
                TASK_CREATE_OR_UPDATE,
                _variant_t(), 
                _variant_t(), 
                TASK_LOGON_INTERACTIVE_TOKEN,
                _variant_t(),
                &pRegisteredTask
            );
            
            if (FAILED(hr)) throw hr;

            success = true;

        } catch (HRESULT errorHr) {
             std::wcerr << L"Task Scheduler API Error: 0x" << std::hex << errorHr << std::endl;
        } catch (...) {
             std::wcerr << L"Unknown Task Scheduler API Exception" << std::endl;
        }

        if (pRegisteredTask) pRegisteredTask->Release();
        if (pTask) pTask->Release();
        if (pRootFolder) pRootFolder->Release();
        if (pService) pService->Release();

        if (coInitCalled) CoUninitialize();
        return success;
    }

    static bool RemoveTask(const std::wstring& name) {
        HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        bool coInitCalled = SUCCEEDED(hr);
        
        ITaskService* pService = NULL;
        ITaskFolder* pRootFolder = NULL;
        bool success = false;
        
        if (SUCCEEDED(CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService))) {
            if (SUCCEEDED(pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t()))) {
                if (SUCCEEDED(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder))) {
                    hr = pRootFolder->DeleteTask(_bstr_t(name.c_str()), 0);
                    success = SUCCEEDED(hr);
                    pRootFolder->Release();
                }
            }
            pService->Release();
        }
        
        if (coInitCalled) CoUninitialize();
        return success;
    }
};
