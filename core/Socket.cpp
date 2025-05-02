# include "../includes/core/Socket.hpp"

void Socket::setFileDescriptor(int fd) {
    fileDescriptor = fd;
}

int Socket::getFileDescriptor( void ) const {
    return (fileDescriptor);
}

void Socket::setPair(const sockaddr* sock) {
    sockaddr_in *in_ptr;
    sockaddr_in6 *in6_ptr;

    if (addressPort != sock) {
        delete addressPort;
        if (sock->sa_family == AF_INET) {
            in_ptr = new sockaddr_in, *in_ptr = *((sockaddr_in*) sock);
            addressPort = (sockaddr*) in_ptr;
        } else if (sock->sa_family == AF_INET6) {
            in6_ptr = new sockaddr_in6, *in6_ptr = *((sockaddr_in6*) sock);
            addressPort = (sockaddr*) in6_ptr;
        }
        else
            addressPort = NULL;
    }
}

const sockaddr* Socket::getPair( void ) const {
    return (addressPort);
}

void Socket::addServer(const Server& srv, size_t& l_nbr) {
    std::vector<Server>::iterator it, ed;

    if (!serverLst.size())
        serverLst.push_back(Server(srv));
    else {
        it = serverLst.begin(), ed = serverLst.end();
        while (it != ed) {
            if (*it == srv)
                throw (ParserExcep("server", "Have conflect with other server", l_nbr)) ;
            else if ((it + 1) == ed)
                serverLst.push_back(Server(srv));
            it++;
        }
    }
}


Server Socket::getServer(std::string name) const {
    std::vector<Server>::const_iterator it, ed;

    it = serverLst.begin(), ed = serverLst.end();
    while (it != ed) {
        if (it->isMatchServer(name))
            return (*it);
        it++;
    }
    return (serverLst.front());
}

std::vector<Server> Socket::getServers( void ) const {
    return (serverLst);
}

Socket::Socket(): fileDescriptor(-1), addressPort(NULL) {
    // nothing to do for now.
}

Socket::Socket(const sockaddr* sock): fileDescriptor(-1), addressPort(NULL) {
    setPair(sock);
}

Socket::Socket(const Socket& x): fileDescriptor(-1), addressPort(NULL) {
    *this = x;
}

Socket& Socket::operator=(const Socket& x) {
    if (this != &x) {
        setPair(x.addressPort);
        serverLst   = x.serverLst;
    }
    return (*this);
}

Socket::~Socket() {
    delete (addressPort);
    close (fileDescriptor);
}