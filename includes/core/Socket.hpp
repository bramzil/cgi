# ifndef SOCKET_HPP
# define SOCKET_HPP

# include "Server.hpp"
# include "../extern.hpp"

class Socket {
    public:
        void                            setFileDescriptor(int fd);
        int                             getFileDescriptor( void ) const;
        void                            setPair(const sockaddr* sock);
        const sockaddr*                 getPair( void ) const;
        void                            addServer(const Server& srv, size_t& l_nbr);
        std::vector<Server>             getServers( void ) const;
        Server                          getServer(std::string name) const;

        Socket();
        Socket(const sockaddr* sock);
        Socket(const Socket& x);
        Socket&                         operator=(const Socket& x);
        virtual ~Socket();

    private:

        int                             fileDescriptor;
        sockaddr                        *addressPort;
        std::vector<Server>             serverLst;
};

# endif