# include "../includes/core/ServerSocket.hpp"

int     ServerSocket::receiveConnection( void ) {
    int             fd;

    if ((fd = accept(socketInstance.getFileDescriptor(), NULL, NULL)) == -1)
        throw ("Failed to accept a connection\n");
    else if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        close (fd);
        throw ("Failed to set file descriptor on non blocking mode\n");
    }
    return (fd);
}

void    ServerSocket::startListening( void ) {
    int     optionValue = 1;

    socketInstance.setFileDescriptor(socket(socketInstance.getPair()->sa_family, SOCK_STREAM, 0));
    if (socketInstance.getFileDescriptor() == -1)
        throw ("Failed to create socket");
    else if (setsockopt(socketInstance.getFileDescriptor(), SOL_SOCKET, SO_REUSEADDR, &optionValue, 4) == -1)
        throw ("Failed to set socket option");
    else if (bind(socketInstance.getFileDescriptor(), socketInstance.getPair(), sizeof(sockaddr_in6)) == -1)
        throw ("Failed to bind a socket");
    else if (listen(socketInstance.getFileDescriptor(), 20) == -1)
        throw ("Failed to listen on socket");
}

void    ServerSocket::addToEpollInstance(int flags) {
    epoll_event      event;

    event.events = flags, event.data.ptr = this;
    if (registered)
        epoll_ctl(epollFileDescriptor, EPOLL_CTL_DEL, socketInstance.getFileDescriptor(), NULL);
    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, socketInstance.getFileDescriptor(), &event) != -1)
        registered = true;
    else
        throw ("Failed to add a Server Socket to epoll instance");
}

ServerSocket::ServerSocket() {
    // nothing to do for moment
}

ServerSocket::~ServerSocket() {
    // nothing to do for now
}