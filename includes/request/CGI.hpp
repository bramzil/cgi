#pragma once

# include "../extern.hpp"
# include "../utilities.hpp"

class CGI
{
	private:
		std::vector<std::string> env_var;
		std::string	buffer;
		std::string	output;
		std::string	input;
		std::string	scriptName;
		int			fd_in;
		int			fd_out;
		int			exit_status;
		pid_t		child;

		
	public:
		CGI(std::vector<std::string> data, std::map<std::string, std::string> requestHeaders);
		void	operator() ( void );
		virtual	~CGI();

		void	formatHeaders(std::map<std::string, std::string> requestHeaders);
		void	fillTheInput(std::string input);
		void	prepareTheOutput(std::string path);
		void	createChildProcess(std::vector<std::string> data);
		void	prepareTheScript(std::string uri);
		int		getExitStatus( void );
		bool	isDone( void );
        std::string getOutputName( void );
};