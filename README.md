# X.Tools

3 tools to manage processes and services on Windows.

1. x.processkill : kill a process or processes matching a string passed as argument
1. x.processlist : get a list of processes
1. x.service     : manipulate services

##  x.processkill

	Usage: xpk.exe [-h][-v][-n][-x] [ process name or id ]
	   -v          Verbose mode
	   -h          Help
	   -s          Search mode: kill process with name matching   DANGER *
	   -w          WhatIf: do not actually kill process.   Suggested use with -s.
	   -n          No banner
	   -x code     Exit Code


##  x.processlist

	Usage: xps.exe [-h][-p][-a][-n][-s] [ process name ]
	   -p          Print process executable pats
	   -v          Verbose Show All process even access denied
	   -h          Help
	   -s          Sort
	   -n          No banner

## x.service

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