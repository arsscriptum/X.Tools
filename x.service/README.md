𝓐𝓡𝓢 𝓢𝓒𝓡𝓘𝓟𝓣𝓤𝓜
-------------------------------------

# x.service

	Usage: svcstart.exe [-n service name][-s][-t][-d][-p]
	   -p          Print service status
	   -n <name>   Service name
	   -l          List services
	   -s          Start
	   -t          Stop
	   -d          Delete
	   -v          Verbose mode
	   -z          Configure Service has driver
	   -x          Configure Service has driver
	   -w          Wait for state change (after start/stop/ert...)

### To list windows services:
    svcm.exe -l win32

### To list driver services:
    svcm.exe -l driver