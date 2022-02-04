
//==============================================================================
//
//     main.cpp
//
//============================================================================
//  Copyright (C) Guilaume Plante 2020 <cybercastor@icloud.com>
//==============================================================================


#include "stdafx.h"

#include "ServiceController.h"
#include "ServiceEnumerator.h"

#include <iostream>
#include <codecvt>
#include <locale> 
#include "windows-api-ex.h"
#include "cmdline.h"
#include "Shlwapi.h"

#include <codecvt>
#include <locale>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <regex>
#include <filesystem>

#pragma message( "Compiling " __FILE__ )
#pragma message( "Last modified on " __TIMESTAMP__ )

using namespace std;


#define DOMAINLOCAL  TEXT("desktop-lk1v00r")
#define ADMIN_PASS  TEXT("MaMemoireEstMaCle7955")
#define ADMIN_USER  TEXT("Sysop")
#define COMMAND_POWERSHELL5  TEXT("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe")
#define COMMAND_PWSH  TEXT("C:\\\My_Apps\\Powershell\\7\\pwsh.exe")
#define COMMAND_MSWT  TEXT("C:\\Users\\radic\\AppData\\Local\\Microsoft\\WindowsApps\\wt.exe")
int main(int argc, TCHAR** argv)
{

#ifdef UNICODE
	const char ** argn = (const char **)C::Convert::allocate_argn(argc, argv);
#else
	char ** argn = argv;
#endif // UNICODE

#ifdef UNICODE
#define STD_OUT std::wcout
#else
#define STD_OUT std::cout
#endif

	CmdLineUtil::getInstance()->initializeCmdlineParser(argc, argn);

	CmdlineParser *inputParser = CmdLineUtil::getInstance()->getInputParser();

	CmdlineOption cmdlineOptionHelp({    "-h", "--help"    }, "display this help");
	CmdlineOption cmdlineOptionVerbose({ "-v", "--verbose" }, "verbose output");
	CmdlineOption cmdlineOptionElevate({ "-e", "--elevate" }, "elevete account privileges to administrator when launching application");
	CmdlineOption cmdlineOptionListServices({ "-l", "--list" }, "list the Services currently running");
	CmdlineOption cmdlineOptionServiceFilter({ "-s", "--service" }, "service filter");
	CmdlineOption cmdlineOptionServiceInfo({ "-i", "--info" }, "service info");
	CmdlineOption cmdlineOptionChangePermission({ "-p", "--permission" }, "change permission");
	CmdlineOption cmdlineOptionNoBanner({ "-nb", "--nobanner" }, "change permission");

	inputParser->addOption(cmdlineOptionHelp);
	inputParser->addOption(cmdlineOptionVerbose);
	inputParser->addOption(cmdlineOptionListServices);
	inputParser->addOption(cmdlineOptionElevate);
	inputParser->addOption(cmdlineOptionServiceFilter);
	inputParser->addOption(cmdlineOptionChangePermission);
	inputParser->addOption(cmdlineOptionServiceInfo);
	inputParser->addOption(cmdlineOptionNoBanner);
	bool optHelp = inputParser->isSet(cmdlineOptionHelp);
	bool optVerbose = inputParser->isSet(cmdlineOptionVerbose);
	bool listAllServices = inputParser->isSet(cmdlineOptionListServices);
	bool optElevate = inputParser->isSet(cmdlineOptionElevate);
	bool optNoBanner = inputParser->isSet(cmdlineOptionNoBanner);
	bool changeServicePermissions = inputParser->isSet(cmdlineOptionChangePermission);
	bool printStatus = inputParser->isSet(cmdlineOptionServiceInfo);;
	bool sendSignal = false;
	bool filter = false;

	if (optNoBanner == false) {
		CmdLineUtil::getInstance()->printTitle();
	}
	
#ifdef UNICODE
	std::wstring service_filter = "";
#else
	std::string service_filter = "";
#endif

	if (inputParser->cmdOptionExists("-s")) {
		filter = true;
		service_filter = inputParser->getCmdOption("-s");
	}else if (inputParser->cmdOptionExists("--service")) {
		filter = true;
		service_filter = inputParser->getCmdOption("--service");
	}
	

	int signal_id = 0;

	if (optElevate){
		if (C::Process::IsRunAsAdministrator()){
			std::cout << "The applicaiton is already running with admin privileges" << std::endl;
		}
		else{
			C::Process::ElevateNow();
		}
	}
	auto services = ServiceEnumerator::EnumerateServices();
	if (listAllServices)
	{
		for (auto const& s : services)
		{
			if (filter)
			{
				size_t found = s.ServiceName.find(service_filter);
				if (found != std::string::npos)
				{

					STD_OUT << "Name:    " << s.ServiceName << std::endl
						<< "Display: " << s.DisplayName << std::endl
						<< "Status:  " << ServiceStatusToString(static_cast<ServiceStatus>(s.Status.dwCurrentState)) << std::endl
						<< "--------------------------" << std::endl;
				}
			}
			else
			{
				STD_OUT << "Name:    " << s.ServiceName << std::endl
					<< "Display: " << s.DisplayName << std::endl
					<< "Status:  " << ServiceStatusToString(static_cast<ServiceStatus>(s.Status.dwCurrentState)) << std::endl
					<< "--------------------------" << std::endl;


				auto service = ServiceController{ s.ServiceName };
				auto config = service.GetServiceConfig();
				service.Stop();
			}
		}
	}

	if (changeServicePermissions)
	{
		for (auto const& s : services)
		{
			char buffer[512];

			if (filter)
			{
				size_t found = s.ServiceName.find(service_filter);
				if (found != std::string::npos)
				{
					char* str = new char[255];
#ifdef UNICODE
					__SNPRINTF(str, 255, "%ls", s.ServiceName.c_str());
					__SNPRINTF(buffer, 512, "sc sdset \"%ls\" D:(A;;CCLCSWLOCRRC;;;AU)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;BA)(A;;CCLCSWRPWPDTLOCRRC;;;SY)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;S-1-5-21-1782791230-1887111140-533732638-1001)S:(AU;FA;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;WD)", str);
#else
					__SNPRINTF(str, 255, "%s", s.ServiceName.c_str());
					__SNPRINTF(buffer, 512, "sc sdset \"%s\" D:(A;;CCLCSWLOCRRC;;;AU)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;BA)(A;;CCLCSWRPWPDTLOCRRC;;;SY)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;S-1-5-21-1782791230-1887111140-533732638-1001)S:(AU;FA;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;WD)", str);
#endif
					STD_OUT << "Permissions for: " << str << std::endl << buffer << std::endl;
					system(buffer);
				}
			}
			else
			{
				char* str = new char[255];
#ifdef UNICODE
				__SNPRINTF(str, 255, "%ls", s.ServiceName.c_str());
				__SNPRINTF(buffer, 512, "sc sdset \"%s\" D:(A;;CCLCSWLOCRRC;;;AU)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;BA)(A;;CCLCSWRPWPDTLOCRRC;;;SY)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;S-1-5-21-1782791230-1887111140-533732638-1001)S:(AU;FA;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;WD)", str);
#else
				__SNPRINTF(str, 255, "%s", s.ServiceName.c_str());
				__SNPRINTF(buffer, 512, "sc sdset \"%s\" D:(A;;CCLCSWLOCRRC;;;AU)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;BA)(A;;CCLCSWRPWPDTLOCRRC;;;SY)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;S-1-5-21-1782791230-1887111140-533732638-1001)S:(AU;FA;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;WD)", str);
#endif
				STD_OUT << "Permissions for: " << str << std::endl << buffer << std::endl;
				system(buffer);
			}
		}
	}

	if (printStatus)
	{
		auto print_config = [](ServiceConfig const config) {
			STD_OUT << "---------------------" << std::endl;
			STD_OUT << "Start name:          " << config.GetStartName() << std::endl;
			STD_OUT << "Display name:        " << config.GetDisplayName() << std::endl;
			STD_OUT << "Description:         " << config.GetDescription() << std::endl;
			STD_OUT << "Type:                " << ServiceTypeToString(config.GetType()) << std::endl;
			STD_OUT << "Start type:          " << ServiceStartTypeToString(config.GetStartType()) << std::endl;
			STD_OUT << "Error control:       " << ServiceErrorControlToString(config.GetErrorControl()) << std::endl;
			STD_OUT << "Binary path:         " << config.GetBinaryPathName() << std::endl;
			STD_OUT << "Load ordering group: " << config.GetLoadOrderingGroup() << std::endl;
			STD_OUT << "Tag ID:              " << config.GetTagId() << std::endl;
			STD_OUT << "Dependencies:        ";
			for (auto const& d : config.GetDependencies()) STD_OUT << d << ", ";
			STD_OUT << std::endl;
			STD_OUT << "---------------------" << std::endl;

		};

		for (auto const& s : services)
		{
			if (filter)
			{
				size_t found = s.ServiceName.find(service_filter);
				if (found != std::string::npos)
				{
					auto service = ServiceController{ s.ServiceName };
					auto config = service.GetServiceConfig();

					print_config(config);
					auto description = config.GetDescription();
					STD_OUT << "Description: " << description << std::endl;
				}
			}
			else
			{
				auto service = ServiceController{ s.ServiceName };
				auto config = service.GetServiceConfig();

				print_config(config);
				auto description = config.GetDescription();
				STD_OUT << "Description: " << description << std::endl;
			}
		}
	}

	return 0;
}
