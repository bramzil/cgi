# include "../includes/request/CgiModule.hpp"

void    CgiModule::setCgiParamters(std::map<std::string, std::string>& headers) {
    size_t i = 0;
    std::string headerField;
    std::map<std::string, std::string>::const_iterator it, ed;

    it = headers.begin(), ed = headers.end();
    envVariables = new char*[headers.size() + 1];
    while (it != ed) {
        headerField = toUpperCaseString(it->first);
        headerField = replaceSetOfCharacters(headerField, "-", '_');
        headerField = headerField + "=" + it->second;
        envVariables[i] = new char[headerField.size() + 1];
        memcpy(envVariables[i], headerField.c_str(), headerField.size());
        envVariables[i][headerField.size()] = '\0';
        it++, i++;
    }
    envVariables[i] = NULL;
}

void    CgiModule::duplicateChildInputOutput( void ) {
    if ((fds[0] != -1) && (dup2(fds[0], 0) == -1))
        throw ("du2 failed to dup child input");
    else if ((dup2(fds[1], 1) == -1) || (dup2(fds[1], 2) == -1))
        throw ("du2 failed to dup child output");
    else
        close (fds[0]), close (fds[1]);
}

void    CgiModule::runCgiScript( void ) {
    try {
        if (chdir(requirements[2].c_str()) == -1)
            throw ("chdir failed to change directoryt");
        else {
            duplicateChildInputOutput();
            if (execve(arguments[0], arguments, envVariables) == -1)
                throw ("execve failed to execute script");
        }
    } catch (...) {
        exit(1);
    }
}

void    CgiModule::createChildProcess( void ) {
    if ((fds[1] = open(requirements[3].c_str(), O_CREAT | O_WRONLY, 0644)) == -1)
        throw ("open failed to open request body file");
    else if (4 < requirements.size()){
        if ((fds[0] = open(requirements[4].c_str(), O_RDONLY)) == -1)
            throw ("open failed to open request body file");
    }
    if ((childPid = fork()) == -1)
        throw ("fork failed to create a child");
    else if (childPid == 0)
        runCgiScript();
    else
        close (fds[1]), fds[1] = -1;
}

bool    CgiModule::isDone( void ) {
    int status;

    if (childPid == waitpid(childPid, &status, WNOHANG)) {
        if (WIFSIGNALED(status)) 
            throw ("child was signaled");
        else
            return (true);
    }
    return (false);
}

CgiModule::CgiModule(std::vector<std::string> parameters) {
    envVariables        = NULL;
    if (parameters.size() < 4)
        throw ("stop running from CgiModule constructor");
    else {
        fds[0] = fds[1] = -1;
        requirements    = parameters;
        arguments       = new char*[3];
        arguments[0]    = new char[requirements[0].size() + 1];
        memcpy(arguments[0], requirements[0].c_str(), requirements[0].size());
        arguments[0][requirements[0].size()] = '\0';
        arguments[1]    = new char[requirements[1].size() + 1];
        memcpy(arguments[1], requirements[1].c_str(), requirements[1].size());
        arguments[1][requirements[1].size()] = '\0';
        arguments[2]    = NULL;
    }
}

CgiModule::~CgiModule() {
    if (fds[0] != -1)
        close (fds[0]);
    if (fds[1] != -1)
        close (fds[1]);
    if (!isDone())
        kill(childPid, 9);
    delete [] envVariables;
}