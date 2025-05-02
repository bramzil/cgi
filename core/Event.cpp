# include "../includes/core/Event.hpp"
# include "../includes/request/Request.hpp"

bool    Event::isRegistered( void ) {
    return (registered);
}

void    Event::setEventId(int parentId) {
    id = (parentId * 10) + getSocketFileDescriptor();
}

int     Event::getEventId( void ) const {
    return (id);
}

void    Event::setSocketType(std::string type) {
    socketType = type;
}

std::string Event::getSocketType( void ) const {
    return (socketType);
}

void    Event::setSocketFileDescriptor(int fd) {
    socketInstance.setFileDescriptor(fd);
}

int     Event::getSocketFileDescriptor( void ) const {
    return (socketInstance.getFileDescriptor());
}

void    Event::setEpollFileDescriptor(int epfd) {
    epollFileDescriptor = epfd;
}

int     Event::getEpollFileDescriptor( void ) const {
    return (epollFileDescriptor);
}

void    Event::setSocket(const Socket& sock) {
    socketInstance = sock;
}

Socket   Event::getSocket( void ) const {
    return (socketInstance);
}

void    Event::removeFromEpollInstance( void ) {

    if (registered) {
        epoll_ctl(epollFileDescriptor, EPOLL_CTL_DEL, socketInstance.getFileDescriptor(), NULL);
        registered = false;
    }
}

Event::Event( void ): registered(false), epollFileDescriptor(-1) {
    // nothing to do for now
}

Event::~Event() {
    removeFromEpollInstance();
}