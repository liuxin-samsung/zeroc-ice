// **********************************************************************
//
// Copyright (c) 2003-2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <IcePack/Activator.h>
#include <IcePack/Admin.h>
#include <IcePack/Internal.h>
#include <IcePack/TraceLevels.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>


using namespace std;
using namespace Ice;
using namespace IcePack;

namespace IcePack
{

class TerminationListenerThread : public IceUtil::Thread
{
public:

    TerminationListenerThread(Activator& activator) :
	_activator(activator)
    {
    }

    virtual 
    void run()
    {
	_activator.runTerminationListener();
    }

private:
    
    Activator& _activator;
};

}

#define ICE_STRING(X) #X

namespace
{

#ifndef _WIN32
//
// Helper function for async-signal safe error reporting
//
void
reportChildError(int err, int fd, const char* cannot, const char* name)
{
    //
    // Send any errors to the parent process, using the write
    // end of the pipe.
    //
    char msg[500];
    strcpy(msg, cannot);
    strcat(msg, " `");
    strcat(msg, name);
    strcat(msg, "':  ");
    strcat(msg, strerror(err));
    write(fd, msg, strlen(msg));
    close(fd);

    //
    // _exit instead of exit to avoid interferences with the parent
    // process.
    //
    _exit(EXIT_FAILURE);
}

#endif

#ifndef _WIN32
string
signalToString(int signal)
{
    switch(signal)
    {
	case SIGHUP:
	{
	    return ICE_STRING(SIGHUP);
	}
	case SIGINT:
	{
	    return ICE_STRING(SIGINT);
	}
	case SIGQUIT:
	{
	    return ICE_STRING(SIGQUIT);
	}
	case SIGILL:
	{
	    return ICE_STRING(SIGILL);
	}
	case SIGTRAP:
	{
	    return ICE_STRING(SIGTRAP);
	}
	case SIGABRT:
	{
	    return ICE_STRING(SIGABRT);
	}
	case SIGBUS:
	{
	    return ICE_STRING(SIGBUS);
	}
	case SIGFPE:
	{
	    return ICE_STRING(SIGFPE);
	}
	case SIGKILL:
	{
	    return ICE_STRING(SIGKILL);
	}
	case SIGUSR1:
	{
	    return ICE_STRING(SIGUSR1);
	}
	case SIGSEGV:
	{
	    return ICE_STRING(SIGSEGV);
	}
	case SIGPIPE:
	{
	    return ICE_STRING(SIGPIPE);
	}
	case SIGALRM:
	{
	    return ICE_STRING(SIGALRM);
	}
	case SIGTERM:
	{
	    return ICE_STRING(SIGTERM);
	}
	default:
	{
	    ostringstream os;
	    os << "signal " << signal;
	    return os.str();
	}
    }
#endif
}

int
stringToSignal(const string& str)
{
#ifdef _WIN32
    throw BadSignalException();
#else

    if(str == ICE_STRING(SIGHUP))
    {
	return SIGHUP;
    }
    else if(str ==  ICE_STRING(SIGINT))
    {
	return SIGINT;
    }
    else if(str == ICE_STRING(SIGQUIT))
    {
	return SIGQUIT;
    }
    else if(str == ICE_STRING(SIGILL))
    {
	return SIGILL;
    }
    else if(str == ICE_STRING(SIGTRAP))
    {
	return SIGTRAP;
    }
    else if(str == ICE_STRING(SIGABRT))
    {
	return SIGABRT;
    }
    else if(str == ICE_STRING(SIGBUS))
    {
	return SIGBUS;
    }
    else if(str == ICE_STRING(SIGFPE))
    {
	return SIGFPE;
    }
    else if(str == ICE_STRING(SIGKILL))
    {
	return SIGKILL;
    }
    else if(str == ICE_STRING(SIGUSR1))
    {
	return SIGUSR1;
    }
    else if(str == ICE_STRING(SIGSEGV))
    {
	return SIGSEGV;
    }
    else if(str == ICE_STRING(SIGUSR2))
    {
	return SIGUSR2;
    }
    else if(str == ICE_STRING(SIGPIPE))
    {
	return SIGPIPE;
    }
    else if(str == ICE_STRING(SIGALRM))
    {
	return SIGALRM;
    }
    else if(str == ICE_STRING(SIGTERM))
    {
	return SIGTERM;
    }
    else
    {
	if(str != "")
	{
	    char* end;
	    long int signal = strtol(str.c_str(), &end, 10);
	    if(*end == '\0' && signal > 0 && signal < 64)
	    {
		return static_cast<int>(signal);
	    }
	}
	throw BadSignalException();
    }
}
#endif

}

Activator::Activator(const TraceLevelsPtr& traceLevels, const PropertiesPtr& properties) :
    _traceLevels(traceLevels),
    _properties(properties),
    _deactivating(false)
{
#ifdef _WIN32
    _hIntr = CreateEvent(
        NULL,  // Security attributes
        TRUE,  // Manual reset
        FALSE, // Initial state is nonsignaled
        NULL   // Unnamed
    );

    if(_hIntr == NULL)
    {
	SyscallException ex(__FILE__, __LINE__);
	ex.error = getSystemErrno();
	throw ex;
    }
#else
    int fds[2];
    if(pipe(fds) != 0)
    {
	SyscallException ex(__FILE__, __LINE__);
	ex.error = getSystemErrno();
	throw ex;
    }
    _fdIntrRead = fds[0];
    _fdIntrWrite = fds[1];
    int flags = fcntl(_fdIntrRead, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(_fdIntrRead, F_SETFL, flags);
#endif

    _outputDir = _properties->getProperty("IcePack.Node.Output");
    _redirectErrToOut = (_properties->getPropertyAsInt("IcePack.Node.RedirectErrToOut") > 0);

    //
    // Parse the properties override property.
    //
    string props = _properties->getProperty("IcePack.Node.PropertiesOverride");
    if(!props.empty())
    {
	string::size_type end = 0;
	while(end != string::npos)
	{
	    const string delim = " \t\r\n";
		
	    string::size_type beg = props.find_first_not_of(delim, end);
	    if(beg == string::npos)
	    {
		break;
	    }
		
	    end = props.find_first_of(delim, beg);
	    string arg;
	    if(end == string::npos)
	    {
		arg = props.substr(beg);
	    }
	    else
	    {
		arg = props.substr(beg, end - beg);
	    }
	    if(arg.find("--") != 0)
	    {
		arg = "--" + arg;
	    }
	    _propertiesOverride.push_back(arg);
	}
    }    
}

Activator::~Activator()
{
    assert(!_thread);
    
#ifdef _WIN32
    if(_hIntr != NULL)
    {
        CloseHandle(_hIntr);
    }
#else
    close(_fdIntrRead);
    close(_fdIntrWrite);
#endif
}

bool
Activator::activate(const string& name,
		    const string& exePath,
		    const string& pwdPath,
		    const Ice::StringSeq& options,
		    const Ice::StringSeq& envs,
		    const ServerPrx& server)
{
    IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);

    if(_deactivating)
    {
	return false;
    }

    string path = exePath;
    if(path.empty())
    {
	return false;
    }

    string pwd = pwdPath;

#ifdef _WIN32
    //
    // Get the absolute pathname of the executable.
    //
    char absbuf[_MAX_PATH];
    char* filePart;
    if(SearchPath(NULL, path.c_str(), ".exe", _MAX_PATH, absbuf, &filePart) == 0)
    {
        Error out(_traceLevels->logger);
        out << "cannot convert `" << path << "' into an absolute path";
        return false;
    }
    path = absbuf;

    //
    // Get the absolute pathname of the working directory.
    //
    if(!pwd.empty())
    {
        if(_fullpath(absbuf, pwd.c_str(), _MAX_PATH) == NULL)
        {
            Error out(_traceLevels->logger);
            out << "cannot convert `" << pwd << "' into an absolute path";
            return false;
        }
        pwd = absbuf;
    }
#else
    //
    // Normalize the pathname a bit.
    //
    {
	string::size_type pos;
	while((pos = path.find("//")) != string::npos)
	{
	    path.erase(pos, 1);
	}
	while((pos = path.find("/./")) != string::npos)
	{
	    path.erase(pos, 2);
	}
    }

    //
    // Normalize the path to the working directory.
    //
    if(!pwd.empty())
    {
	string::size_type pos;
	while((pos = pwd.find("//")) != string::npos)
	{
	    pwd.erase(pos, 1);
	}
	while((pos = pwd.find("/./")) != string::npos)
	{
	    pwd.erase(pos, 2);
	}
    }
#endif

    //
    // Setup arguments.
    //
    StringSeq args;
    args.push_back(path);
    args.insert(args.end(), options.begin(), options.end());
    args.insert(args.end(), _propertiesOverride.begin(), _propertiesOverride.end());
    args.push_back("--Ice.Default.Locator=" + _properties->getProperty("Ice.Default.Locator"));
    args.push_back("--Ice.ServerId=" + name);

    if(_traceLevels->activator > 1)
    {
	Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
	out << "activating server `" << name << "'";
	if(_traceLevels->activator > 2)
	{
	    out << "\n";
	    out << "path = " << path << "\n";
	    out << "pwd = " << pwd << "\n";
	    out << "args = ";

	    StringSeq::const_iterator p = args.begin();
            ++p;
	    for(StringSeq::const_iterator q = p; q != args.end(); ++q)
	    {
		out << " " << *q;
	    }
	}
    }

    //
    // Activate and create.
    //
#ifdef _WIN32
    //
    // Compose command line.
    //
    string cmd;
    StringSeq::const_iterator p;
    for(p = args.begin(); p != args.end(); ++p)
    {
        if(p != args.begin())
        {
            cmd.push_back(' ');
        }
        //
        // Enclose arguments containing spaces in double quotes.
        //
        if((*p).find(' ') != string::npos)
        {
            cmd.push_back('"');
            cmd.append(*p);
            cmd.push_back('"');
        }
        else
        {
            cmd.append(*p);
        }
    }

    const char* dir;
    if(!pwd.empty())
    {
        dir = pwd.c_str();
    }
    else
    {
        dir = NULL;
    }

    //
    // Make a copy of the command line.
    //
    char* cmdbuf = strdup(cmd.c_str());

    //
    // Create the environment block for the child process. We start with the environment
    // of this process, and then merge environment variables from the server description.
    //
    const char* env = NULL;
    string envbuf;
    if(!envs.empty())
    {
        map<string, string> envMap;
        LPVOID parentEnv = GetEnvironmentStrings();
        const char* var = reinterpret_cast<const char*>(parentEnv);
        if(*var == '=')
        {
            //
            // The environment block may start with some information about the
            // current drive and working directory. This is indicated by a leading
            // '=' character, so we skip to the first '\0' byte.
            //
            while(*var)
                var++;
            var++;
        }
        while(*var)
        {
            string s(var);
            string::size_type pos = s.find('=');
            if(pos != string::npos)
            {
                envMap.insert(map<string, string>::value_type(s.substr(0, pos), s.substr(pos + 1)));
            }
            var += s.size();
            var++; // Skip the '\0' byte
        }
        FreeEnvironmentStrings(static_cast<char*>(parentEnv));
        for(p = envs.begin(); p != envs.end(); ++p)
        {
            string s = *p;
            string::size_type pos = s.find('=');
            if(pos != string::npos)
            {
                envMap.insert(map<string, string>::value_type(s.substr(0, pos), s.substr(pos + 1)));
            }
        }
        for(map<string, string>::const_iterator q = envMap.begin(); q != envMap.end(); ++q)
        {
            envbuf.append(q->first);
            envbuf.push_back('=');
            envbuf.append(q->second);
            envbuf.push_back('\0');
        }
        envbuf.push_back('\0');
        env = envbuf.c_str();
    }

    Process process;

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if(_outputDir.size() > 0)
    {
	string outFile = _outputDir + "/" + name + ".out";

	SECURITY_ATTRIBUTES sec = { 0 };
	sec.nLength = sizeof(sec);
	sec.bInheritHandle = true;
	
	process.outHandle = CreateFile(outFile.c_str(),
				       GENERIC_WRITE, 
				       FILE_SHARE_READ, 
				       &sec,
				       OPEN_ALWAYS,
				       FILE_ATTRIBUTE_NORMAL,
				       NULL);
	if(process.outHandle == INVALID_HANDLE_VALUE)
	{
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	}
        //
        // NOTE: INVALID_SET_FILE_POINTER is not defined in VC6.
        //
	//if(SetFilePointer(process.outHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
	if(SetFilePointer(process.outHandle, 0, NULL, FILE_END) == (DWORD)-1)
	{
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	}
	
	if(_redirectErrToOut)
	{
	    process.errHandle = process.outHandle;
	}
	else
	{
	    string errFile = _outputDir + "/" + name + ".err";

	    process.errHandle = CreateFile(errFile.c_str(),
					   GENERIC_WRITE, 
					   FILE_SHARE_READ, 
					   &sec,
					   OPEN_ALWAYS,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);
	    if(process.errHandle == INVALID_HANDLE_VALUE)
	    {
		SyscallException ex(__FILE__, __LINE__);
		ex.error = getSystemErrno();
		throw ex;
	    }
            //
            // NOTE: INVALID_SET_FILE_POINTER is not defined in VC6.
            //
	    //if(SetFilePointer(process.errHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
	    if(SetFilePointer(process.errHandle, 0, NULL, FILE_END) == (DWORD)-1)
	    {
		SyscallException ex(__FILE__, __LINE__);
		ex.error = getSystemErrno();
		throw ex;
	    }

	}

	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	if(si.hStdInput == INVALID_HANDLE_VALUE)
	{
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	}
	si.hStdOutput = process.outHandle;
	si.hStdError = process.errHandle;
	si.dwFlags = STARTF_USESTDHANDLES;
    }
    else
    {
	process.outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if(process.outHandle == INVALID_HANDLE_VALUE)
	{
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	}
	if(_redirectErrToOut)
	{
	    //
	    // Note: very partial implementation, since we don't pass this 
	    // info to the child
	    //
	    process.errHandle = process.outHandle;
	}
	else
	{
	    process.errHandle = GetStdHandle(STD_ERROR_HANDLE);
	    if(process.errHandle == INVALID_HANDLE_VALUE)
	    {
		SyscallException ex(__FILE__, __LINE__);
		ex.error = getSystemErrno();
		throw ex;
	    }
	}
	//
	// Note: we don't set si.hStdOutput or si.hStdError, and use
	// Windows default behavior.
	//
    }

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    BOOL b = CreateProcess(
        NULL,                     // Executable
        cmdbuf,                   // Command line
        NULL,                     // Process attributes
        NULL,                     // Thread attributes
        _outputDir.size() > 0,    // Inherit handles only when redirecting (Temporary, should always be FALSE)
        CREATE_NEW_PROCESS_GROUP, // Process creation flags
        (LPVOID)env,              // Process environment
        dir,                      // Current directory
        &si,                      // Startup info
        &pi                       // Process info
    );

    free(cmdbuf);

    if(!b)
    {
        SyscallException ex(__FILE__, __LINE__);
        ex.error = getSystemErrno();
        throw ex;
    }

    //
    // Caller is responsible for closing handles in PROCESS_INFORMATION. We don't need to
    // keep the thread handle, so we close it now. The process handle will be closed later.
    //
    CloseHandle(pi.hThread);

    
    process.pid = pi.dwProcessId;
    process.hnd = pi.hProcess;
    process.server = server;
    _processes.insert(make_pair(name, process));
    
    setInterrupt();

    if(_traceLevels->activator > 0)
    {
        Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
        out << "activated server `" << name << "' (pid = " << pi.dwProcessId << ")";
    }
#else
    int fds[2];
    if(pipe(fds) != 0)
    {
	SyscallException ex(__FILE__, __LINE__);
	ex.error = getSystemErrno();
	throw ex;
    }

    //
    // Convert to standard argc/argv.
    //
    int argc = static_cast<int>(args.size());
    char** argv = static_cast<char**>(malloc((argc + 1) * sizeof(char*)));
    int i = 0;
    for(StringSeq::const_iterator p = args.begin(); p != args.end(); ++p, ++i)
    {
	assert(i < argc);
	argv[i] = strdup(p->c_str());
    }
    assert(i == argc);
    argv[argc] = 0;
    
    int envCount = envs.size();
    char** envArray = new char*[envCount];
    i = 0;
    for(StringSeq::const_iterator q = envs.begin(); q != envs.end(); ++q)
    {
	envArray[i++] = strdup(q->c_str());
    }

    //
    // stdout and stderr redirection
    //
    int flags = O_WRONLY | O_APPEND | O_CREAT;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   
    int outFd;;
    string outFile;
    int errFd;
    string errFile;

    if(_outputDir.size() == 0)
    {
	outFd = STDOUT_FILENO;
	errFd = _redirectErrToOut ? outFd : STDERR_FILENO;
    }
    else
    {
	outFile = _outputDir + "/" + name + ".out";
	outFd = open(outFile.c_str(), flags, mode);
	if(outFd < 0)
	{
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	}
	
	if(_redirectErrToOut)
	{
	    errFile = outFile;
	    errFd = outFd;
	}
	else
	{
	    errFile = _outputDir + "/" + name + ".err";
	    errFd = open(errFile.c_str(), flags, mode);
	    
	    if(errFd < 0)
	    {
		SyscallException ex(__FILE__, __LINE__);
		ex.error = getSystemErrno();
		throw ex;
	    }   
	}
    }
    
    const char* outFileCStr = outFile.c_str();
    const char* errFileCStr = errFile.c_str();

    //
    // Current directory
    //
    const char* pwdCStr = pwd.c_str();


    pid_t pid = fork();
    if(pid == -1)
    {
	SyscallException ex(__FILE__, __LINE__);
	ex.error = getSystemErrno();
	throw ex;
    }

    if(pid == 0) // Child process.
    {
	//
	// Until exec, we can only use async-signal safe functions
	//

#ifdef __linux
	//
	// Create a process group for this child, to be able to send 
	// a signal to all the thread-processes with killpg
	//
	setpgrp();
#endif

	//
	// stdout and stderr redirection
	//
	if(_outputDir.size() > 0)
	{
	    if(dup2(outFd, STDOUT_FILENO) != STDOUT_FILENO)
	    {
		reportChildError(errno, fds[1], "cannot associate stdout with opened file",  outFileCStr);
	    }
	    
	    if(dup2(errFd, STDERR_FILENO) != STDERR_FILENO)
	    {
		reportChildError(errno, fds[1], "cannot associate stderr with opened file",  errFileCStr);
	    }
	}

	//
	// Close all file descriptors, except for standard input,
	// standard output, standard error, and the write side
	// of the newly created pipe.
	//
	int maxFd = static_cast<int>(sysconf(_SC_OPEN_MAX));
	for(int fd = 3; fd < maxFd; ++fd)
	{
	    if(fd != fds[1])
	    {
		close(fd);
	    }
	}

	for(i = 0; i < envCount; i++)
	{
	    if(putenv(envArray[i]) != 0)
	    {
		reportChildError(errno, fds[1], "cannot set environment variable",  envArray[i]); 
	    }
	}
	//
	// Each env is leaked on purpose ... see man putenv().
	//
	delete[] envArray;

	//
	// Change working directory.
	//
	if(strlen(pwdCStr) != 0)
	{
	    if(chdir(pwdCStr) == -1)
	    {
		reportChildError(errno, fds[1], "cannot change working directory to",  pwdCStr);
	    }
	}	

	if(execvp(argv[0], argv) == -1)
	{
	    reportChildError(errno, fds[1], "cannot execute",  argv[0]);
	}
    }
    else // Parent process.
    {
	close(fds[1]);

	for(i = 0; i < envCount; ++i)
	{
	    free(envArray[i]);
	}
	delete[] envArray;

	Process process;
	process.pid = pid;
	process.pipeFd = fds[0];
	process.outFd = outFd;
	process.errFd = errFd;
	process.server = server;
	_processes.insert(make_pair(name, process));
	
	int flags = fcntl(process.pipeFd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(process.pipeFd, F_SETFL, flags);

	setInterrupt();

	if(_traceLevels->activator > 0)
	{
	    Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
	    out << "activated server `" << name << "' (pid = " << pid << ")";
	}
    }
#endif

    return true;
}

void
Activator::deactivate(const string& name, const Ice::ProcessPrx& process)
{
#ifdef _WIN32
    Ice::Int pid = getServerPid(name);
    if(pid == 0)
    {
	//
	// Server is already deactivated.
	//
	return;
    }
#endif 

    //
    // Try to shut down the server gracefully using the process proxy.
    //
    if(process)
    {
	if(_traceLevels->activator > 1)
	{
	    Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
	    out << "deactivating `" << name << "' using process proxy";
	}
	try
	{
	    process->shutdown();
	    return;
	}
	catch(const Ice::LocalException& ex)
	{
	    Ice::Warning out(_traceLevels->logger);
	    out << "exception occurred while deactivating `" << name << "' using process proxy:\n" << ex;
	}
    }

    if(_traceLevels->activator > 1)
    {
        Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
        out << "no process proxy, deactivating `" << name << "' using signal";
    }

#ifdef _WIN32
    //
    // Generate a Ctrl+Break event on the child.
    //
    if(GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pid))
    {
        if(_traceLevels->activator > 1)
        {
            Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
            out << "sent Ctrl+Break to server `" << name << "' (pid = " << pid << ")";
        }
    }
    else
    {
        SyscallException ex(__FILE__, __LINE__);
        ex.error = getSystemErrno();
        throw ex;
    }
#else
    //
    // Send a SIGTERM to the process.
    //
    sendSignal(name, SIGTERM);
    
#endif
}

void
Activator::kill(const string& name)
{
#ifdef _WIN32
    Ice::Int pid = getServerPid(name);
    if(pid == 0)
    {
	//
	// Server is already deactivated.
	//
	return;
    }

    HANDLE hnd = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if(hnd == NULL)
    {
        SyscallException ex(__FILE__, __LINE__);
        ex.error = getSystemErrno();
        throw ex;
    }

    BOOL b = TerminateProcess(hnd, 1);

    CloseHandle(hnd);

    if(!b)
    {
        SyscallException ex(__FILE__, __LINE__);
        ex.error = getSystemErrno();
        throw ex;
    }

    if(_traceLevels->activator > 1)
    {
	Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
	out << "terminating server `" << name << "' (pid = " << pid << ")";
    }

#else
    sendSignal(name, SIGKILL);
#endif
}


void
Activator::sendSignal(const string& name, const string& signal)
{
    sendSignal(name, stringToSignal(signal));
}
void
Activator::sendSignal(const string& name, int signal)
{
#ifdef _WIN32
    //
    // TODO: Win32 implementation?
    //
    throw BadSignalException();

#else
    Ice::Int pid = getServerPid(name);
    if(pid == 0)
    {
	//
	// Server is already deactivated.
	//
	return;
    }

#ifdef __linux
    // Use process groups on Linux instead of processes
    int ret = ::killpg(static_cast<pid_t>(pid), signal);
#else
    int ret = ::kill(static_cast<pid_t>(pid), signal);
#endif
    if(ret != 0 && getSystemErrno() != ESRCH)
    {
	SyscallException ex(__FILE__, __LINE__);
	ex.error = getSystemErrno();
	throw ex;
    }

    if(_traceLevels->activator > 1)
    {
	Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
	out << "sent " << signalToString(signal) << " to server `" << name << "' (pid = " << pid << ")";
    }
#endif
}

void 
Activator::writeMessage(const string& name, const string& message, Ice::Int fd)
{
    assert(fd == 1 || fd == 2);
    
    string msg = message + "\n";

#ifdef _WIN32
    HANDLE handle = 0;
#else
    int actualFd = -1;
#endif
    {
	IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);

	map<string, Process>::const_iterator p = _processes.find(name);
	if(p == _processes.end())
	{
	    return;
	}
	
#ifdef _WIN32
	handle = (fd == 1 ? p->second.outHandle : p->second.errHandle);
#else
	actualFd = (fd == 1 ? p->second.outFd : p->second.errFd);
#endif
    }

#ifdef _WIN32
    if(handle != 0)
    {
	DWORD written = 0;
	if(!WriteFile(handle, msg.c_str(), msg.size(), &written, NULL))
	{
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	} 
	assert(written == msg.size());
    }
#else
    if(actualFd > 0)
    {
	ssize_t written = write(actualFd, msg.c_str(), msg.size());
	if(written == -1)
	{
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	}
	assert(written == static_cast<ssize_t>(msg.size()));
    }
#endif
}
   

Ice::Int
Activator::getServerPid(const string& name)
{
    IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);

    map<string, Process>::const_iterator p = _processes.find(name);
    if(p == _processes.end())
    {
	return 0;
    }

    return static_cast<Ice::Int>(p->second.pid);
}

void
Activator::start()
{
    //
    // Create and start the termination listener thread.
    //
    _thread = new TerminationListenerThread(*this);
    _thread->start();
}

void
Activator::waitForShutdown()
{
    IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);
    while(!_deactivating)
    {
	wait();
    }
}

void
Activator::shutdown()
{
    IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);
    //
    // Deactivation has been initiated. Set _deactivating to true to 
    // prevent activation of new processes. This will also cause the
    // termination listener thread to stop when there are no more
    // active processes.
    //
    _deactivating = true;
    setInterrupt();
    notifyAll();
}

void
Activator::destroy()
{
    {
	IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);
	assert(_deactivating);
    }

    //
    // Deactivate all the processes.
    //
    deactivateAll();

    //
    // Join the termination listener thread. This thread terminates
    // when there's no more processes and when _deactivating is set to
    // true.
    //
    _thread->getThreadControl().join();
    _thread = 0;
}

void
Activator::runTerminationListener()
{
    try
    {
	terminationListener();
    }
    catch(const Exception& ex)
    {
	Error out(_traceLevels->logger);
	out << "exception in process termination listener:\n" << ex;
    }
    catch(...)
    {
	Error out(_traceLevels->logger);
	out << "unknown exception in process termination listener";
    }
}

void
Activator::deactivateAll()
{
    //
    // Stop all active processes.
    //
    map<string, Process> processes;
    {
	IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);
	processes = _processes;
    }

    for(map<string, Process>::iterator p = processes.begin(); p != processes.end(); ++p)
    {
	//
	// Stop the server. The listener thread should detect the
	// process deactivation and remove it from the activator's
	// list of active processes.
	//
	try
	{
	    p->second.server->stop();
	}
	catch(const ObjectNotExistException&)
	{
	    //
	    // Expected if the server was in the process of being destroyed.
	    //
	}
    }
}

void
Activator::terminationListener()
{
#ifdef _WIN32
    while(true)
    {
        vector<HANDLE> handles;

        //
        // Lock while we collect the process handles.
        //
        {
            IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);

            for(map<string, Process>::iterator p = _processes.begin(); p != _processes.end(); ++p)
            {
                handles.push_back(p->second.hnd);
            }
        }

        handles.push_back(_hIntr);

        //
        // Wait for a child to terminate, or the interrupt event to be signaled.
        //
        DWORD ret = WaitForMultipleObjects(handles.size(), &handles[0], FALSE, INFINITE);
        if(ret == WAIT_FAILED)
        {
            SyscallException ex(__FILE__, __LINE__);
            ex.error = getSystemErrno();
            throw ex;
        }

        vector<HANDLE>::size_type pos = ret - WAIT_OBJECT_0;
        assert(pos < handles.size());
        HANDLE hnd = handles[pos];

        IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);

        if(hnd == _hIntr)
        {
            clearInterrupt();
        }
        else
        {
            for(map<string, Process>::iterator p = _processes.begin(); p != _processes.end(); ++p)
            {
                if(p->second.hnd == hnd)
                {
                    if(_traceLevels->activator > 0)
                    {
                        Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
                        out << "detected termination of server `" << p->first << "'";
                    }

                    try
                    {
                        p->second.server->terminated();
                    }
                    catch(const Ice::LocalException& ex)
                    {
                        Ice::Warning out(_traceLevels->logger);
                        out << "unexpected exception raised by server `" << p->first << "' termination:\n" << ex;
                    }
                            
		    CloseHandle(hnd);
		    if(_outputDir.size() > 0)
		    {
			//
			// STDOUT and STDERR should not be closed
			//
			CloseHandle(p->second.outHandle);
			if(!_redirectErrToOut)
			{
			    CloseHandle(p->second.errHandle);
			}
		    }
                    _processes.erase(p);
                    break;
                }
            }
        }

        if(_deactivating && _processes.empty())
        {
            return;
        }
    }
#else
    while(true)
    {
	fd_set fdSet;
	int maxFd = _fdIntrRead;
	FD_ZERO(&fdSet);
	FD_SET(_fdIntrRead, &fdSet);
	
	{
	    IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);

	    for(map<string, Process>::iterator p = _processes.begin(); p != _processes.end(); ++p)
	    {
		int fd = p->second.pipeFd;
		FD_SET(fd, &fdSet);
		if(maxFd < fd)
		{
		    maxFd = fd;
		}
	    }
	}
	
    repeatSelect:
	int ret = ::select(maxFd + 1, &fdSet, 0, 0, 0);
	assert(ret != 0);
	
	if(ret == -1)
	{
#ifdef EPROTO
	    if(errno == EINTR || errno == EPROTO)
	    {
		goto repeatSelect;
	    }
#else
	    if(errno == EINTR)
	    {
		goto repeatSelect;
	    }
#endif
	    
	    SyscallException ex(__FILE__, __LINE__);
	    ex.error = getSystemErrno();
	    throw ex;
	}
	
	{
	    IceUtil::Monitor< IceUtil::Mutex>::Lock sync(*this);
	    
	    if(FD_ISSET(_fdIntrRead, &fdSet))
	    {
		clearInterrupt();

		if(_deactivating && _processes.empty())
                {
                    return;
                }
	    }
	    
	    map<string, Process>::iterator p = _processes.begin();
	    while(p != _processes.end())
	    {
		int fd = p->second.pipeFd;
		if(!FD_ISSET(fd, &fdSet))   
		{
		    ++p;
		    continue;
		}

		char s[16];
		ssize_t rs;
		string message;

		//
		// Read the message over the pipe.
		//
		while((rs = read(fd, &s, 16)) > 0)
		{
		    message.append(s, rs);
		}

		if(rs == -1)
		{
		    if(errno != EAGAIN || message.empty())
		    {
			SyscallException ex(__FILE__, __LINE__);
			ex.error = getSystemErrno();
			throw ex;
		    }

		    ++p;
		}
		else if(rs == 0)
		{    
		    //
		    // If the pipe was closed, the process has terminated.
		    //

		    if(_traceLevels->activator > 0)
		    {
			Ice::Trace out(_traceLevels->logger, _traceLevels->activatorCat);
                        out << "detected termination of server `" << p->first << "'";
		    }

		    try
		    {
			p->second.server->terminated();
		    }
		    catch(const Ice::LocalException& ex)
		    {
			Ice::Warning out(_traceLevels->logger);
			out << "unexpected exception raised by server `" << p->first << "' termination:\n" << ex;
		    }
			    
		    if(p->second.errFd != p->second.outFd && p->second.errFd != STDERR_FILENO)
		    {
			close(p->second.errFd);
		    }
		    if(p->second.outFd != STDOUT_FILENO)
		    {
			close(p->second.outFd);
		    }
		    close(p->second.pipeFd);
		    _processes.erase(p++);

		    //
		    // We are deactivating and there's no more active processes. We can now 
		    // end this loop
		    //
		    if(_deactivating && _processes.empty())
		    {
			return;
		    }
		}

		//
		// Log the received message.
		//
		if(!message.empty())
		{
		    Error out(_traceLevels->logger);
		    out << message;
		}
	    }
	}
    }
#endif
}

void
Activator::clearInterrupt()
{
#ifdef _WIN32
    ResetEvent(_hIntr);
#else
    char c;
    while(read(_fdIntrRead, &c, 1) == 1)
        ;
#endif
}

void
Activator::setInterrupt()
{
#ifdef _WIN32
    SetEvent(_hIntr);
#else
    char c = 0;
    write(_fdIntrWrite, &c, 1);
#endif
}