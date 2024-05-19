
//==============================================================================
//
//     main.cpp
//
//============================================================================
//  Copyright (C) Guilaume Plante 2020 <cybercastor@icloud.com>
//==============================================================================

#include "stdafx.h"

#include <codecvt>
#include <locale>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <regex>
#include <filesystem>
#include <algorithm>
#include <array>
#include <iostream>
#include <iomanip>

#include "windows-api-ex.h"
#include "cmdline.h"
#include "Shlwapi.h"
#include "phantom.h"
#include "uac_bypass.h"
#include "ps_enum.h"
#include "psinfo.h"
#include "psutils.h"

using namespace std;

#pragma message( "Compiling " __FILE__ )
#pragma message( "Last modified on " __TIMESTAMP__ )

void banner() {
	std::wcout << std::endl;
	COUTC("pl v2.1 - Process List Tool\n");
	COUTC("Built on %s\n", __TIMESTAMP__);
	COUTC("Copyright (C) 2000-2021 Guillaume Plante\n");
	COUTC("Process/Service Tool Suite\n");
	std::wcout << std::endl;
}
void usage(){
	COUTCS("Usage: pl.exe [-h][-p][-a][-n][-s] [ process name ]\n");
	COUTCS("   -p          Print process executable pats\n");
	COUTCS("   -v          Verbose Show All process even access denied\n");
	COUTCS("   -h          Help\n");
	COUTCS("   -s          Sort\n");
	COUTCS("   -n          No banner\n");
	std::wcout << std::endl;
}

 
std::string ConstructCommand(const std::string& executablePath, const std::vector<std::string>& arguments) {
	std::ostringstream command;
	command << "& \"" << executablePath << "\"";
	for (const auto& arg : arguments) {
		command << " \"" << arg << "\"";
	}
	return command.str();
}
 
template <class T, size_t N>
void sort(std::array<T, N> &arr) {
    std::sort(std::begin(arr), std::end(arr));
    //std::sort(std::begin(arr), std::end(arr), std::greater<double>{});
}
DWORD* sort6(PDWORD* v, int size)
{
    double start = std::clock();
    DWORD* table = new DWORD[size];
    for (DWORD i = 0; i < size; ++i)
    {
        table[i] = 0;
    }
    PDWORD end = *v + size;
    for (DWORD* vi = *v; vi < end; ++vi)
    {
        ++table[*vi];
    }
    PDWORD cur = *v;
    for (DWORD i = 0; i < size; ++i)
    {
        DWORD count = table[i];
        for (DWORD j = 0; j < count; ++j)
        {
            *(cur++) = i;
        }
    }
    //std::cout << "count sort with pointers over char array took " << (std::clock() - start) / CLOCKS_PER_SEC << " s" << std::endl;
    return table;
}

bool ArgsToString(int args_count, char** args) {
  if (args_count <= 1) return false;
  if (args_count == 2) return true;
  for (int i = 2; i < args_count; ++i) {
    char* cursor = args[i];
    while (*cursor) --cursor;
    *cursor = ' ';
  }
  return true;
}

#define DOMAINLOCAL  TEXT("desktop-lk1v00r")
#define ADMIN_PASS  TEXT("MaMemoireEstMaCle7955")
#define ADMIN_USER  TEXT("Sysop")
#define COMMAND_POWERSHELL5  TEXT("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe")
#define COMMAND_PWSH  TEXT("C:\\\My_Apps\\Powershell\\7\\pwsh.exe")
#define COMMAND_MSWT  TEXT("C:\\Users\\radic\\AppData\\Local\\Microsoft\\WindowsApps\\wt.exe")
int main(int argc, TCHAR **argv, TCHAR envp)
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
	CmdlineOption cmdlineOptionPath({ "-p", "--path" }, "print process path");
	CmdlineOption cmdlineOptionPINFO({ "-i", "--info" }, "print process cpu usage and memory usage");
	CmdlineOption cmdlineOptionNoBanner({ "-n", "--nobanner" }, "no banner");
	CmdlineOption cmdlineOptionSort({ "-s", "--sort" }, "sort");
	CmdlineOption cmdlineOptionAdmin({ "-a", "--admin" }, "admin mode");


	inputParser->addOption(cmdlineOptionHelp);
	inputParser->addOption(cmdlineOptionVerbose);
	inputParser->addOption(cmdlineOptionPath); 
	inputParser->addOption(cmdlineOptionPINFO); 
	inputParser->addOption(cmdlineOptionSort); 
	inputParser->addOption(cmdlineOptionNoBanner); 
	inputParser->addOption(cmdlineOptionAdmin); 


	bool optHelp = inputParser->isSet(cmdlineOptionHelp);
	bool optVerbose = inputParser->isSet(cmdlineOptionVerbose);
	bool optPsPath = inputParser->isSet(cmdlineOptionPath);
	bool optPsInfo = inputParser->isSet(cmdlineOptionPINFO);
	bool optSort = inputParser->isSet(cmdlineOptionSort);
	bool optNoBanner = inputParser->isSet(cmdlineOptionNoBanner);
	bool optAdmin = inputParser->isSet(cmdlineOptionAdmin);

	std::string current_exec_name = argv[0]; // Name of the current exec program
  	std::vector<std::string> all_args;

	std::string commandLineStr= "";
	for (int i=1;i<argc;i++) commandLineStr.append(std::string(argv[i]).append(" "));


	if(optHelp){
		usage();
		return 0;
	}
	if(optNoBanner){
		bool privEnabled = C::Process::EnableDebugPrivilege();
		printf("EnableDebugPrivilege: %d",privEnabled?1:0);
	}
	
	
	
	if (optAdmin) {
		if (C::Process::IsRunAsAdministrator() == false) {
			string szCommand;
			std::vector<std::string> arguments;
			

			for (int i = 0; i < argc; i++) {
				if (i == 0) {
					
					szCommand = argv[i];
				}
				else {
					string a = argv[i];
					if (a != "-a") {
						arguments.push_back(a);
					}
					
				}
			}
			string psCommand = ConstructCommand(szCommand, arguments);
			printf("ps command: %s\n", psCommand.c_str());
			string encodedCommand = C::Convert::base64::encode64(psCommand);
			C::Process::RunPowerShellCmdAsAdministrator(psCommand);
			return 0;
			
		}
	}
	
	

	std::unordered_map<DWORD, std::string> ProcessList;

	std::unordered_map<DWORD, ProcessInfo*> ProcessInfoList;
	TCHAR searchPsName[MAX_PATH + 1];
	DWORD bufferSize = MAX_PATH;
	TCHAR processname[MAX_PATH + 1], * stop;
	TCHAR fileExt[MAX_PATH + 1];
	TCHAR fileDir[MAX_PATH + 1];
	TCHAR fileName[MAX_PATH + 1];
	DWORD* processes, lpProcesses[QUITE_LARGE_NB_PROCESSES], bytes, processId;
	SIZE_T nbProcesses;
	HANDLE hProcess;
	processes = lpProcesses;
	TCHAR* processIdentifier = nullptr;
	bool shouldCheckUserDefined = false;
	bool foundProcess = false;
	std::vector<std::string> ListedProcess;
	ProcessInfo *pinfo = nullptr;
	for (int x = 1; x < argc; x++) {
		processIdentifier = argv[x];
		
		if (isalpha(*processIdentifier)){
			_tcscpy(searchPsName,processIdentifier);
			shouldCheckUserDefined = true;
			//printf("Looking for specific : %s\n", processIdentifier);
		}
	}

	if(optPsInfo){
		optPsPath = true;
	}

	nbProcesses = C::Process::FillProcessesList(&processes, sizeof(lpProcesses));
	if (!nbProcesses) {
		ConsoleTrace("ERROR : Could not enumerate process list\n");
		return -1;
	}

	for (int i = 0; i < nbProcesses; i++) {
		DWORD pid = processes[i];
		ProcessInfo * pinfo = new ProcessInfo(pid);
		ProcessInfoList[processes[i]] = pinfo;
	}


	if(optSort){
		std::vector<DWORD> v;
		//printf("[sorting] %d elements...\n",nbProcesses);
		for (int i = 0; i < nbProcesses; i++) {
			v.push_back(processes[i]);
		}
    	sort(v.begin(), v.end());
	
		for (int i = 0; i < nbProcesses; i++) {
			if(optVerbose){
				printf("[sorting] %d/%d %5d %5d", i,(int)nbProcesses,processes[i],v[i]);	
			}
			
			processes[i] = v[i];
		}
	}
	

	//COUTC("PROCESS LIST\n");
	//if(optPsInfo){
	//	COUTC("[ PID ]\tNAME\tCPU\t\tPATH");COUTBB("\n");
	//}
	//else if(optPsPath){
	//	COUTC("[ PID ]\tCPU\tNAME\t\tPATH");COUTBB("\n");
	//}else{
	//	COUTC("[ PID ]\tCPU\tNAME");COUTBB("\n");
	//}

	EndOfLineEscapeTag FormatId{ ANSI_TEXT_COLOR_BLUE_BRIGHT, ANSI_TEXT_COLOR_RESET };
	EndOfLineEscapeTag FormatName{ ANSI_TEXT_COLOR_WHITE, ANSI_TEXT_COLOR_RESET };
	EndOfLineEscapeTag FormatPath{ ANSI_TEXT_COLOR_BLACK_BRIGHT, ANSI_TEXT_COLOR_RESET };
	PrintProcessListTitle(optPsPath);
	int denied = 0;
	int listed = 0;
	int totalProcesListed=0;
	int ret = 0;
	for (int i = 0; i < nbProcesses; i++) {

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
		if (hProcess && C::Process::ProcessIdToName(processes[i], processname, bufferSize)) {
			listed++;
			std::string processnameDouble = std::regex_replace(processname, std::regex(R"(\\)"), R"(\\)");
			decomposePath(processnameDouble.c_str(), fileDir, fileName, fileExt);
			ProcessList[processes[i]] = fileName;
			if(shouldCheckUserDefined){
				if(!strcmp(searchPsName,fileName)){
					PrintProcessPropertiesOneLine(hProcess, fileName, processname, optPsPath);
					foundProcess = true;
					totalProcesListed++;
				}
			}
			else{
				PrintProcessPropertiesOneLine(hProcess, fileName, processname, optPsPath);
				totalProcesListed++;
			}
		}
		else {
			denied++;
		}

	}

	if(shouldCheckUserDefined && !foundProcess){
		printf("Error: cannot find process \"%s\".\n",searchPsName);
		ret=-1;
	}
	printf("Detected %d total processes. listed %d (%d denied)", (int)nbProcesses,totalProcesListed,denied);


	return ret;
}
