#include <windows.h>    
#include <locale>
#include <iostream>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h> 
#include <Winuser.h> 
#pragma comment(lib, "Psapi.lib")

using namespace std;

void createProcess(LPCTSTR applicationName) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcess(
        applicationName,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        cout << "Процес створено успішно!" << endl;
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        cerr << "Помилка процесу створення: " << GetLastError() << endl;
    }
}

void listProcesses() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {
                cout << "ID: " << entry.th32ProcessID << " Name: " << entry.szExeFile << endl;
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
}

void terminateProcess(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess != NULL) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
    else {
        cerr << "Помилка процесу відкриття: " << GetLastError() << endl;
    }
}

void listThreads(DWORD processId) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hThreadSnap, &te32)) {
            do {
                if (te32.th32OwnerProcessID == processId) {
                    cout << "ID потоку: " << te32.th32ThreadID << endl;
                }
            } while (Thread32Next(hThreadSnap, &te32));
        }
        CloseHandle(hThreadSnap);
    }
}

void listModules(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess != NULL) {
        HMODULE hMods[1024];
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                TCHAR szModName[MAX_PATH];
                if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                    wcout << "Назва модуля: " << szModName << endl;
                }
            }
        }
        CloseHandle(hProcess);
    }
}

void terminateAllProcesses() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {
                terminateProcess(entry.th32ProcessID);
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
}

int main() {
    SetConsoleOutputCP(1251);
    setlocale(LC_CTYPE, "ukr");
    while (true) {
        cout << "Виберіть операцію:" << endl;
        cout << "1. Створити процес" << endl;
        cout << "2. Вивести всі процеси" << endl;
        cout << "3. Завершити вибраний процес" << endl;
        cout << "4. Перерахувати всі потоки вибраного процесу" << endl;
        cout << "5. Відобразити всі модулі вибраного процесу" << endl;
        cout << "6. Завершити всі процеси" << endl;
        cout << "0. Вихід" << endl;

        int choice;
        cin >> choice;
        switch (choice) {
        case 1: {
            LPCTSTR applicationName = _T("C:\\Windows\\System32\\notepad.exe");
            createProcess(applicationName);
            break;
        }
        case 2: {
            listProcesses();
            break;
        }
        case 3: {
            cout << "Введіть ідентифікатор процесу, який потрібно завершити: ";
            DWORD processId;
            cin >> processId;
            terminateProcess(processId);
            break;
        }
        case 4: {
            cout << "Введіть ідентифікатор процесу для виведення потоків: ";
            DWORD processId;
            cin >> processId;
            listThreads(processId);
            break;
        }
        case 5: {
            cout << "Введіть ідентифікатор процесу для виведення модулів: ";
            DWORD processId;
            cin >> processId;
            listModules(processId);
            break;
        }
        case 6: {
            terminateAllProcesses();
            break;
        }
        case 0: {
            return 0;
            break;
        }
        default: {
            cerr << "Неправильний вибір, повторіть спробу!" << endl;
            break;
        }
        }
    }

    return 0;
}
