#include "../includes/request/CGI.hpp"

void	midToLow(std::string &header) {
	for (size_t	i = 0; i < header.size(); i++) {
		if (header[i] == '-')
			header[i] = '_';
	}
}
void	CGI::formatHeaders(std::map<std::string, std::string> requestHeaders) {
	std::string 				header;
    std::vector<std::string>    list;
	for (std::map<std::string, std::string>::iterator it = requestHeaders.begin(); it != requestHeaders.end(); it++) {
		header = toUpperCaseString(it->first);
		midToLow(header);
		setenv(header.c_str(), it->second.c_str(), 1);
		header += "=" + it->second;
		env_var.push_back(header);
	}
}
void	CGI::fillTheInput(std::string input) {
	// input = generateRandomName("");
	if (input == "") {
		return ;
	}
	fd_in = open(input.c_str(), O_CREAT | O_WRONLY, 0644);
	if (fd_in == -1)
		throw ("Failed to open a file as an input in CGI.");
	write(fd_in, input.c_str(), input.size());
	close(fd_in);
	fd_in = open(input.c_str(), O_RDONLY, 0644);
	if (remove(input.c_str()) != 0)
		throw ("Failed to remove the input file from the CGI.");
}

void	CGI::prepareTheOutput(std::string path) {
	output = generateRandomName(path);
	fd_out = open(output.c_str(), O_CREAT | O_WRONLY, 0644);
	if (fd_out == -1)
		throw ("Failed to open a file as an input in CGI.");

}

void CGI::prepareTheScript(std::string uri) {
    size_t pos;

    pos = uri.rfind('/');
    if (pos == std::string::npos) {
        throw ("Internal Server Error from CGI prepareTheScript");
    }
	scriptName = uri.substr(pos + 1);
}

char** vectorToCharPointer(const std::vector<std::string>& vec) {
    char** result = new char*[vec.size() + 1];

    for (size_t i = 0; i < vec.size(); ++i) {
        result[i] = new char[vec[i].size() + 1];
        strcpy(result[i], vec[i].c_str());
    }
    result[vec.size()] = NULL;
    return result;
}

std::string CGI::getOutputName( void ) {
    return (output);
}

void	CGI::createChildProcess(std::vector<std::string> data) {
	child = fork();
	if (!child) {
        std::string path = data.front();
        if (chdir(path.c_str()) != 0)
            exit (1);
		dup2(fd_out, 2);
		char	**envp = vectorToCharPointer(env_var);
		if (input != "" && dup2(fd_in, 0) == -1)
			exit (1);
		if (dup2(fd_out, 1) == -1)
			exit (1);
        char *const argv[] = {
            (char *)data[2].c_str(),
            (char *)scriptName.c_str(),
            NULL
        };
        if (execve(data[2].c_str(), argv, envp) == -1)
            exit (1);
	} else {
		close(fd_in);
		close(fd_out);
	}
}


CGI::CGI(std::vector<std::string> data, std::map<std::string, std::string> requestHeaders) : exit_status(-1) {
    if (data.size() < 3) {
        throw ("Internal Server Error from CGI constructor");
	}
	formatHeaders(requestHeaders);
	fillTheInput(data.size() > 3 ? data.back() : "");
	prepareTheOutput(data[0]);
	prepareTheScript(data[1]);
	createChildProcess(data);
}

int		CGI::getExitStatus( void ) {
	return (exit_status);
}

void	CGI::operator() ( void ) {
	int	status;

	waitpid(child, &status, WNOHANG);
	if (WIFEXITED(status)) {
		exit_status = WEXITSTATUS(status);
		if (exit_status > 0) {
			throw ("Internal Server Error");
		}
		else
			exit_status = 0;
	}
}

bool	CGI::isDone( void ) {
	// std::cerr << "here" << std::endl;
	if (exit_status > -1)
		return (true);
	return (false);
}

CGI::~CGI() {
	close(fd_in);
	close(fd_out);
}