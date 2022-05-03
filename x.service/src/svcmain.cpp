#include "stdafx.h"

#include "Controller.h"
#include "Enumerator.h"
#include "log.h"
#include <iostream>
#include <codecvt>
#include <locale> 
#include "win32.h"
#include <iomanip>
using namespace std;




using namespace std;

#ifdef _DEBUG
bool SHOW_ERRORS = true;
#else
bool SHOW_ERRORS = false;
#endif
bool opt_svc_all = false;
bool opt_svc_drv = false;
bool opt_svc_win32 = false;
bool opt_wait = false;
bool opt_verbose = false;
bool opt_svc_stop = false;
bool opt_svc_list = false;
bool opt_svc_start = false;
bool opt_svc_delete = false;
bool opt_print_details = false;
bool opt_svc_name = false;
#ifdef _DEBUG
bool dbgout = true;
#else
bool dbgout = false;
#endif
bool opt_svc_set_driver = false;
bool opt_svc_set_permissions = false;
std::wstring service_name;
std::wstring binary_path;
std::wstring invalid_opt;
void InvalidArgument();
void banner();
void usage();
void missingsvcname();



bool ProcessCommandLine(int argc, char *argv[])
{
	int i;
	bool valid = true;
	for (i = 1; i < argc; i++)
	{
		// Is it a switch character?
		if ((argv[i][0] == '-') || (argv[i][0] == '/'))
		{
			if ((argv[i][1] == 'f') || ((argv[i][1] == 'F')))
			{
				const char *name = argv[i+1];
				service_name = stringToWstring(name);
				opt_svc_name = true;
				if (dbgout) {
					std::wcout << "[DBG] Filter name: " << service_name << std::endl;
				}
				continue;
			}
			else if ((argv[i][1] == 'l') || ((argv[i][1] == 'L')))
			{
				opt_svc_list = true;
				const char* svctype = argv[i + 1];
				if (svctype) {
					std::wstring service_type = stringToWstring(svctype);
					if (!service_type.compare(_T("win32"))) {
						opt_svc_win32 = true;
						std::wcout << "[DBG] List Service WIN32" << std::endl;
					}
					else if (!service_type.compare(_T("all"))) {
						opt_svc_all = true;
						std::wcout << "[DBG] List Service ALL" << std::endl;
					}
					else if (!service_type.compare(_T("drv"))) {
						opt_svc_drv = true;
						std::wcout << "[DBG] List Service DRIVERS" << std::endl;
					}
				}
				
	
				if (dbgout) {
					std::wcout << "[DBG] List Service" << std::endl;
				}
				continue;
			}	
			else if ((argv[i][1] == 'e') || ((argv[i][1] == 'E')))
			{
				SHOW_ERRORS = true;
				if (dbgout) {
					std::wcout << "[DBG] SHOW ERRORS" << std::endl;
				}
				continue;
			}
			else if ((argv[i][1] == 'z') || ((argv[i][1] == 'Z')))
			{
				opt_svc_set_driver = true;
				//const char *bin = argv[i+1];
				//binary_path = stringToWstring(bin);
				continue;
			}	
			else if ((argv[i][1] == 'x') || ((argv[i][1] == 'X')))
			{
				opt_svc_set_permissions = true;
				const char* name = argv[i + 1];
				if (!name) { missingsvcname(); return false; }
				service_name = stringToWstring(name);
				opt_svc_name = true;
				if (dbgout) {
					std::wcout << "[DBG] Set Permission on Service: " << service_name << std::endl;
				}
				continue;
			}	

			else if ((argv[i][1] == 's') || ((argv[i][1] == 'S')))
			{
				opt_svc_start = true;
				const char* name = argv[i + 1];
				if (!name) { missingsvcname(); return false;  }
				service_name = stringToWstring(name);
				opt_svc_name = true;
				if (dbgout) {
					std::wcout << "[DBG] Start Service: " << service_name << std::endl;
				}
				continue;
			}	
			else if ((argv[i][1] == 't') || ((argv[i][1] == 'T')))
			{
				opt_svc_stop = true;
				const char* name = argv[i + 1];
				if (!name) { missingsvcname(); return false; }
				service_name = stringToWstring(name);
				opt_svc_name = true;
				if (dbgout) {
					std::wcout << "[DBG] Stop Service: " << service_name << std::endl;
				}
				continue;
			}
			else if ((argv[i][1] == 'd') || ((argv[i][1] == 'D')))
			{
				opt_svc_delete = true;
				const char* name = argv[i + 1];
				if (!name) { missingsvcname(); return false; }
				service_name = stringToWstring(name);
				opt_svc_name = true;
				if (dbgout) {
					std::wcout << "[DBG] Delete Service: " << service_name << std::endl;
				}
				
				continue;
			}			
			else if ((argv[i][1] == 'p') || ((argv[i][1] == 'P')))
			{
				opt_print_details = true;
				continue;
			}
			else if ((argv[i][1] == 'w') || ((argv[i][1] == 'W')))
			{
				opt_wait = true;
				continue;
			}
			else if ((argv[i][1] == 'v') || ((argv[i][1] == 'V')))
			{
				std::wcout << "Verbose Mode: Enabled" << std::endl;
				opt_verbose = true;
				continue;
			}
			else if ((argv[i][1] == 'l') || ((argv[i][1] == 'L')))
			{
				opt_svc_list = true;
				continue;
			}
			else if ((argv[i][1] == 'h') || ((argv[i][1] == 'H')))
			{
				banner();
				usage();
				valid = false;
			}
			else{
				const char *opt = argv[i+1];
				invalid_opt = stringToWstring(opt);
				valid = false;
				InvalidArgument();
			}	
		}
	}
	return valid;
}

void banner() {
	std::wcout << std::endl;
	std::wcout << "xsvc v1.0 - Service Tool" << std::endl;
	std::wcout << "Copyright (C) 2000-2021 Guillaume Plante" << std::endl;
	std::wcout << "Service Tool Suite" << std::endl;
	std::wcout << std::endl;
}


void missingsvcname() {

	banner();
	std::wcout << "Missing service name" << std::endl;
	std::wcout << std::endl;
	usage();
}
void InvalidArgument() {

	banner();
	std::wcout << "invalid argument: " << invalid_opt  <<  std::endl;
	std::wcout << std::endl;
	usage();
}


void usage(){
	std::wcout << "Usage: xsvc.exe [-n service name][-s][-t][-d][-p]" << std::endl;
	std::wcout << "   -p                  Print service status" << std::endl;
	std::wcout << "   -f <name>           Set svc name filter" << std::endl;
	std::wcout << "   -l win32/driver     List services" << std::endl;
	std::wcout << "   -s <name>           Start" << std::endl;
	std::wcout << "   -t <name>           Stop" << std::endl;
	std::wcout << "   -d <name>           Delete" << std::endl;
	std::wcout << "   -v                  Verbose mode" << std::endl;
	std::wcout << "   -z                  Configure Service has driver" << std::endl;	
	std::wcout << "   -x                  Configure Service has driver" << std::endl;	
	std::wcout << "   -w                  Wait for state change (after start/stop/ert...)" << std::endl;
	std::wcout << std::endl;
}

void print_service_details(ServiceConfig const config)
{
	std::wcout << "---------------------" << std::endl;
	std::wcout << " Start name:          " << config.GetStartName() << std::endl;
	std::wcout << " Display name:        " << config.GetDisplayName() << std::endl;
	std::wcout << " Description:         " << config.GetDescription() << std::endl;
	std::wcout << " Type:                " << ServiceTypeToString(config.GetType()) << std::endl;
	std::wcout << " Start type:          " << ServiceStartTypeToString(config.GetStartType()) << std::endl;
	std::wcout << " Error control:       " << ServiceErrorControlToString(config.GetErrorControl()) << std::endl;
	std::wcout << " Binary path:         " << config.GetBinaryPathName() << std::endl;
	std::wcout << " Load ordering group: " << config.GetLoadOrderingGroup() << std::endl;
	std::wcout << " Tag ID:              " << config.GetTagId() << std::endl;
	std::wcout << " Dependencies Num:    " << config.GetDependencies().size() << std::endl;
	std::wcout << " Dependencies:        ";
	if (config.GetDependencies().size() == 0) { std::wcout << "n/a" << std::endl;}
	else {
		for (auto const& d : config.GetDependencies()) std::wcout << d << ", ";
	}
	
	std::wcout << std::endl;
	std::wcout << "---------------------" << std::endl;

};


int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	bool valid = ProcessCommandLine(argc, argv);
	if(!valid){
		return -1;
	}
    
	bool found_svc = false;
	size_t found = 0;
	if (argc < 2) {
		std::wcout << std::endl;
		std::wcout << "Error: Please specify service name and command option" << std::endl;
		usage();
		return 1; 
	}

	

	if(opt_svc_list)
	{
		ServiceType sctype = ServiceType::All;
		if (opt_svc_drv) {
			sctype = ServiceType::Driver;
			std::wcout << "[o] Listing DRIVER services " << std::endl;
		}
		if (opt_svc_win32) {
			sctype = ServiceType::Win32;
			std::wcout << "[o] Listing WIN32 services " << std::endl;
		}
	
		auto services = ServiceEnumerator::EnumerateServices(sctype);
		
		int maxlen0= 0;
		int maxlen1=0;
		int maxlen2= 0;
		for (auto const& s : services)
		{
			auto service = ServiceController{ s.ServiceName };
			int l=s.ServiceName.length();
			if(l>maxlen1){maxlen1 = l;}
			auto config = service.GetServiceConfig();
			l=config.GetDisplayName().length();
			if(l>maxlen2){maxlen2 = l;}

			ServiceStatus st = service.GetStatus();
			ServiceString str = ServiceStatusToString(st);
			l=str.length();
			if(l>maxlen0){maxlen0 = l;}
		}
		int field_1 = maxlen0+3, field_2 = maxlen1+2, field_3 = maxlen2+2;
		wcout<< std::endl <<setw(field_1)<<left<<"Status";
				wcout<<setw(field_2)<<left<<"Name";
				wcout<<setw(field_3)<<left<<"Display Name"<< std::endl;
		ServiceString placeholder;
		ServiceString placeholder_status;
		placeholder_status.insert(0,field_1,'=');
		placeholder.insert(0,field_2,'=');
		wcout<<"\u001b[36m"<< std::endl;
		wcout<< setw(field_1)<<left<<"-------";
				wcout<<setw(field_2)<<left<<"----";
				wcout<<setw(field_3)<<left<<"------------"<< std::endl;	
		wcout<<"\u001b[0m"<< std::endl;			
		
		for (auto const& s : services)
		{
			auto service = ServiceController{ s.ServiceName };
			if(!service.HasValidHandle()){
				if(opt_verbose){
						std::wcout << "[o] ServiceController SERVICE_ALL_ACCESS failed " << std::endl;	
					}
				service = ServiceController{ s.ServiceName, SERVICE_QUERY_CONFIG|SERVICE_QUERY_STATUS  };
				if(!service.HasValidHandle()){
					if(opt_verbose){
						std::wcout << "[o] ServiceController SERVICE_ALL_ACCESS and SERVICE_QUERY_CONFIG failed " << std::endl;	
					}
					continue;
				}
			}
			auto config = service.GetServiceConfig();
			if(opt_svc_name){
				
				found = s.ServiceName.find(service_name);
				if (found != std::string::npos)
				{
					found_svc = true;
					std::wcout << "[o] Found service " << service_name << std::endl;

					auto service = ServiceController{ s.ServiceName };
					if(!service.HasValidHandle()){continue;}
					auto config = service.GetServiceConfig();
					print_service_details(config);	
				}
			}
			else{
				
				ServiceStatus st = service.GetStatus();
				ServiceString str = ServiceStatusToString(st);
				ServiceString strformat = ServiceStatusToFormatString(st);
				wcout<<strformat.c_str();
				wcout<<setw(field_1)<<left<<str.c_str();
				wcout<<setw(field_2)<<left<<s.ServiceName.c_str();
				wcout<<setw(field_3)<<left<<config.GetDisplayName().c_str();
				wcout<<"\u001b[0m"<< std::endl;			
			}	
		}
		

		return 0;
	}

	auto services = ServiceEnumerator::EnumerateServices(ServiceType::All);


	if (opt_svc_set_driver && opt_svc_name)
	{
		for (auto const& s : services) {
			found = s.ServiceName.find(service_name);
			if (found != std::string::npos) {
				found_svc = true;
				std::wcout << "[o] Found service " << service_name << std::endl;
				auto service = ServiceController{ s.ServiceName };
				auto config = service.GetServiceConfig();
				config.SetDriverType("");
				break;
			}
		}
		return 0;
	}
	if (opt_svc_set_permissions && opt_svc_name)
	{
		for (auto const& s : services) {
			char buffer[512];
			found = s.ServiceName.find(service_name);
			if (found != std::string::npos) {
				char* str = new char[255];
				sprintf_s(str, 255, "%ls", s.ServiceName.c_str());

				sprintf_s(buffer, 512, "C:\\ProgramData\\chocolatey\\bin\\accesschk.exe -c \"%s\"", str);
				system(buffer);



				sprintf_s(buffer, 512, "sc sdset \"%s\" D:(A;;CCLCSWLOCRRC;;;AU)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;BA)(A;;CCLCSWRPWPDTLOCRRC;;;SY)(A;;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;S-1-5-21-1782791230-1887111140-533732638-1001)S:(AU;FA;CCDCLCSWRPWPDTLOCRSDRCWDWO;;;WD)", str);
				std::wcout << "Permissions for: " << str << std::endl << buffer << std::endl;
				system(buffer);

				sprintf_s(buffer, 512, "C:\\ProgramData\\chocolatey\\bin\\accesschk.exe -c \"%s\"", str);
				system(buffer);


				found_svc = true;

				auto service = ServiceController{ s.ServiceName };
				auto config = service.GetServiceConfig();

				break;
			}
		}
		return 0;
	}

	std::wcout << "[o] Looking for " << service_name << std::endl;
	for (auto const& s : services)
	{
		if (opt_verbose) {
			std::wcout << "[o] Searching [" << s.ServiceName << "]" << std::endl;
		}
		found = s.ServiceName.find(service_name);
		if (found != std::string::npos)
		{
			found_svc = true;
			std::wcout << "[o] Found service " << s.ServiceName << std::endl;

			auto service = ServiceController{ s.ServiceName };

			ServiceStatus st = service.GetStatus();
			ServiceString str = ServiceStatusToString(st);
			std::wcout << "[o] Service Status is " << str << std::endl;
			auto config = service.GetServiceConfig();
			if(opt_print_details){
				print_service_details(config);	
			}
			bool result = false;
			if(opt_svc_start){
				std::wcout << "[o] Starting Service" << std::endl;
				result = service.Start();
				if (result) { std::wcout << "success" << std::endl; }
				else { std::wcout << "failure" << std::endl; }
				if (!result) {
					std::wstring strError = GetLastMsg();
					printf("Error ( 0x%08lx ): %ls\n", GetLastError(), strError.c_str());
				}
				ServiceStatus st = service.GetStatus();
				ServiceString str = ServiceStatusToString(st);
				std::wcout << "[o] Service Status is " << str << std::endl;
				if (opt_wait) {
					std::wcout << "[o] Waiting for state SERVICE_START_PENDING" << std::endl;
					service.WaitForStatus((ServiceStatus)SERVICE_START_PENDING);
					std::wcout << "[o] Waiting for state SERVICE_RUNNING" << std::endl;
					service.WaitForStatus((ServiceStatus)SERVICE_RUNNING);
				}
			}else if(opt_svc_stop){

				std::wcout << "[o] Stopping Service";
				result = service.Stop();
				if (result) { std::wcout << "success" << std::endl;} else { std::wcout << "failure" << std::endl; }
				ServiceStatus st = service.GetStatus();
				ServiceString str = ServiceStatusToString(st);
				std::wcout << "[o] Service Status is " << str << std::endl;
				if (!result) {
					std::wstring strError = GetLastMsg();
					printf("Error ( 0x%08lx ): %ls\n", GetLastError(), strError.c_str());
				}
				if (opt_wait) {
					std::wcout << "[o] Waiting for state SERVICE_STOP_PENDING" << std::endl;
					service.WaitForStatus((ServiceStatus)SERVICE_STOP_PENDING);
				}
			}else if(opt_svc_delete){
				std::wcout << "[o] Deleting Service" << service_name << std::endl;
				result = service.Delete();
				if (result) { std::wcout << "success" << std::endl; }
				else { std::wcout << "failure" << std::endl; }
				ServiceStatus st = service.GetStatus();
				ServiceString str = ServiceStatusToString(st);
				std::wcout << "[o] Service Status is " << str << std::endl;
				if (!result) {
					std::wstring strError = GetLastMsg();
					printf("Error ( 0x%08lx ): %ls\n", GetLastError(), strError.c_str());
				}
				if (opt_wait) {
					std::wcout << "[o] Waiting for state SERVICE_STOP_PENDING" << std::endl;
					service.WaitForStatus((ServiceStatus)SERVICE_STOP_PENDING);
					std::wcout << "[o] Waiting for state SERVICE_STOPPED" << std::endl;
					service.WaitForStatus((ServiceStatus)SERVICE_STOPPED);
				}
				st = service.GetStatus();
				str = ServiceStatusToString(st);
				std::wcout << "[o] Service Status is " << str << std::endl;
			}
		}
	}
	
	if (!found_svc)
	{
		std::wcout << "[X] ERROR Service " << service_name << " not found."  << std::endl;
	}

	



   return 0;
}

