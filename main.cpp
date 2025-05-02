# include                               <cstring>
# include                               <fcntl.h>
# include                              <unistd.h>
# include                              <stdlib.h>
# include                            <sys/time.h>
# include                           <sys/epoll.h>
# include                "includes/core/Core.hpp"
# include        "includes/parser/HttpParser.hpp"
# include "includes/request/RequestProcessor.hpp"

void    configFileParser(std::vector<Socket>& socketList, char* path) {
    std::fstream is;

    if (!path)
        throw ("Null path passed to configFileParser");
    is.open(path, std::ios::in);
    if (is.is_open()) {
        try {
            HttpParser httpObj(socketList, is);
            httpObj();
        } catch (std::exception& ex) {
            std::cerr << ex.what() << "\n";
            throw ("!!!!!! Stop parsing process !!!!!!!");
        }
    } else
        throw ("Failed to open config file");
}

int main(int ac, char** av) {
    Core  mainServer;
    std::vector<Socket> socketList;

    try {
        if (ac != 2)
            throw ("Invalid number of arguments");
        else {
            configFileParser(socketList, av[1]);
            mainServer(socketList);
        }
    } catch(char const* ex) {
        std::cerr << ex << '\n';
    } catch (...) {
        std::cout << "unexpected exception";
    }
    return (0);
}