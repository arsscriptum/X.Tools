
//==============================================================================
//
//     main.cpp
//
//============================================================================
//  Copyright (C) Guilaume Plante 2020 <cybercastor@icloud.com>
//==============================================================================



#include "stdafx.h"
#include "win32.h"
#include "cmdline.h"
#include "Shlwapi.h"
#include "log.h"
#include <codecvt>
#include <locale>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <regex>
#include <filesystem>
#pragma message( "Compiling " __FILE__ )
#pragma message( "Last modified on " __TIMESTAMP__ )



void banner() {
	std::wcout << std::endl;
	COUTC("xpk v2.1 - Process Kill Tool\n");
	COUTC("Built on %s\n", __TIMESTAMP__);
	COUTC("Copyright (C) 2000-2021 Guillaume Plante\n");
	COUTC("Process/Service Tool Suite\n");
	std::wcout << std::endl;
}
void usage(){
	COUTCS("Usage: xpk.exe [-h][-v][-n][-x] [ process name or id ]\n");
	COUTCS("   -v          Verbose mode\n");
	COUTCS("   -h          Help\n");
	COUTCSNR("   -s          Search mode: kill process with name matching");
	COUTRS("  DANGER *");
	COUTCSNR("   -w          WhatIf: do not actually kill process.");
	COUTY("  Suggested use with -s.");
	COUTCS("   -n          No banner\n");
	COUTCS("   -x code     Exit Code\n");	
	std::wcout << std::endl;
}

int main(int argc, TCHAR** argv, TCHAR envp)
{

#ifdef UNICODE
	const char** argn = (const char**)C::Convert::allocate_argn(argc, argv);
#else
	char** argn = argv;
#endif // UNICODE

	CmdLineUtil::getInstance()->initializeCmdlineParser(argc, argn);

	CmdlineParser* inputParser = CmdLineUtil::getInstance()->getInputParser();

	CmdlineOption cmdlineOptionHelp({ "-h", "--help" }, "display this help");
	CmdlineOption cmdlineOptionVerbose({ "-v", "--verbose" }, "verbose output");
	CmdlineOption cmdlineOptionXcode({ "-x", "--exit" }, "exit code");
	CmdlineOption cmdlineOptionNoBanner({ "-n", "--nobanner" }, "no banner");
	CmdlineOption cmdlineOptionSearch({ "-s", "--search" }, "search");
	CmdlineOption cmdlineOptionWhatIf({ "-w", "--whatif" }, "whatif");

	inputParser->addOption(cmdlineOptionHelp);
	inputParser->addOption(cmdlineOptionVerbose);
	inputParser->addOption(cmdlineOptionXcode); 
	inputParser->addOption(cmdlineOptionNoBanner); 
	inputParser->addOption(cmdlineOptionWhatIf); 
	inputParser->addOption(cmdlineOptionSearch); 


	bool optHelp = inputParser->isSet(cmdlineOptionHelp);
	bool optVerbose = inputParser->isSet(cmdlineOptionVerbose);
	bool optXcode= inputParser->isSet(cmdlineOptionXcode);
	bool optNoBanner = inputParser->isSet(cmdlineOptionNoBanner);
	bool optWhatIf = inputParser->isSet(cmdlineOptionWhatIf);
	bool optSearch = inputParser->isSet(cmdlineOptionSearch);
	if(optNoBanner == false){
		banner();
	}
	if(optHelp){
		usage();
		return 0;
	}

	bool privEnabled = C::Process::EnableRequiredPrivileges();
	if(optVerbose){
		COUTY("[i] privEnabled: %d",privEnabled?1:0);
	}

	DWORD bufferSize = MAX_PATH;
	TCHAR processname[MAX_PATH + 1], * stop;
	TCHAR fileExt[MAX_PATH + 1];
	TCHAR fileDir[MAX_PATH + 1];
	TCHAR fileName[MAX_PATH + 1];
	std::unordered_map<DWORD, std::string> ProcessList;
	DWORD aProcesses[QUITE_LARGE_NB_PROCESSES];
	memset(aProcesses, 0, QUITE_LARGE_NB_PROCESSES);
	HANDLE hProcess;
	DWORD Res = 0;
	TCHAR szName[MAX_PATH];

	std::string pName;
	memset(szName, 0, MAX_PATH);
	memset(fileExt, 0, MAX_PATH);
	memset(fileDir, 0, MAX_PATH);
	memset(fileName, 0, MAX_PATH);

	DWORD nb_processes = C::Process::FillProcessesList(aProcesses, QUITE_LARGE_NB_PROCESSES);

	int processedKilled = 0;
	if (!nb_processes) {
		COUTRS("ERROR : Could not enumerate process list\n");
		return -1;
	}
	bool shouldCheckUserDefined = false;
	bool psok = false;
	std::vector<std::string> ListedProcess;
	for (int x = 1; x < argc; x++) {
		TCHAR* processIdentifier = argv[x];
		
		if(isalpha(*processIdentifier)){
				ListedProcess.push_back(processIdentifier);
				psok = true;
				shouldCheckUserDefined = true;
				COUTYY("Searching this process name: ");
				COUTBM("%s\n\n", processIdentifier);
				printf("\n");
				if(optSearch){
					COUTYRL("Search: All Matching Names");
					printf("\n");
				}
			
		} else if(isdigit(*processIdentifier)){
			psok = true;
		}
	}
	if (!psok) {
		COUTY("ERROR : no process specified");
		return -1;
	}
	UINT uExitCode = 1;
	int denied = 0;
	int listed = 0;
	for (int i = 0; i < nb_processes; i++) {
		DWORD dwDesiredAccess = PROCESS_TERMINATE;
		BOOL  bInheritHandle = FALSE;
		HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, aProcesses[i]);
		if (hProcess && C::Process::ProcessIdToName(aProcesses[i], processname, bufferSize)) {

			listed++;
			std::string processnameDouble = std::regex_replace(processname, std::regex(R"(\\)"), R"(\\)");
			decomposePath(processnameDouble.c_str(), fileDir, fileName, fileExt);

			ProcessList[aProcesses[i]] = fileName;
			if(optVerbose){
				printf("%s (pid %d) located process...\n",processname,aProcesses[i]); 
			}			
			if(shouldCheckUserDefined){
				std::string currentProcessString = fileName;
				for (const std::string& ups : ListedProcess) {
					bool found = false;
					if(optSearch){
						found = (strstr(currentProcessString.c_str(),ups.c_str()));	
					}else{
						found = currentProcessString == ups;
					}
					
					if (found){
						BOOL killed = false;
						if(optWhatIf){
							killed = true;
							C::Process::ProcessIdToName(aProcesses[i], processname, bufferSize);
							COUTGG("[WHAT IF] ");	
							COUTMM("%s",processname); 
							COUTBM(" (pid %d) ", aProcesses[i]);
							COUTGG(" NOT terminated\n");
							printf("\n");
						}else{
								killed = C::Process::TerminateProcess(aProcesses[i], uExitCode);
						}
						
						if (!killed) {
								pName = ProcessList[aProcesses[i]];
								C::Process::ProcessIdToName(aProcesses[i], processname, bufferSize);	
								DWORD errId = GetLastError();
								String strError = GetErrorMessage(errId);
								COUTYRL("%s (pid %d) ERROR [%#08x] %s\n", processname, aProcesses[i], errId, strError.c_str());
								printf("\n");
						}
						else{
							if(!optWhatIf){
								C::Process::ProcessIdToName(aProcesses[i], processname, bufferSize);	
								COUTYY("%s",processname); 
								COUTBM(" (pid %d) ", aProcesses[i]);
								COUTMM(" terminated\n");
								processedKilled++;
								printf("\n");
							}
						}
					}
				}
			}
			
		}else{
			if(optVerbose){
				denied++;
				COUTY("%d : permission denied",aProcesses[i]);
					
			}
		}
	}
	
	for (int x = 1; x < argc; x++) {
		TCHAR* processIdentifier = argv[x];
		DWORD processId = _tcstoul(processIdentifier, &stop, 0);
		
		if (processId) {

			BOOL killed = false;
			if(optWhatIf){
				killed = true;
				COUTY("WHAT IF Kill pid %d\n", processId);
			}else{
				killed  = C::Process::TerminateProcess(processId, uExitCode);
			}
			
			if (!killed) {
				pName = ProcessList[processId];
				DWORD errId = GetLastError();
				String strError = GetErrorMessage(errId);
				COUTRS("%s - %s (pid %d) ERROR [%#08x] %s\n", argv[0], pName.c_str(), processId, errId, strError.c_str());
			}
			else {
				COUTRS("%s (pid %d) terminated\n",  pName.c_str(), processId);
				processedKilled++;
			}
		}
	}

	COUTRS("Terminated: %d.\n", processedKilled);
	if(optVerbose){
		COUTY("%d denied, listed %d",denied, listed); 	
	}		
	return 0;
}