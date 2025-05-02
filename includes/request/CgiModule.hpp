# ifndef CGIMODULE_HPP
# define CGIMODULE_HPP

# include "../extern.hpp"
# include "../utilities.hpp"

class CgiModule {
    public:

        void    runCgiScript( void );
        void    createChildProcess( void );
        void    duplicateChildInputOutput(void);
        void    setCgiParamters(std::map<std::string, std::string>& headers);

        bool    isDone( void );

        CgiModule(std::vector<std::string> requirements);
        virtual ~CgiModule();

    private:
        char**                              arguments;
        char**                              envVariables;
        int                                 fds[2];
        pid_t                               childPid;
        int                                 exitStatus;
        std::vector<std::string>            requirements;
        std::vector<std::string>            requestHeaders;

};

# endif