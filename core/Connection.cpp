# include "../includes/request/Request.hpp"
# include "../includes/core/Connection.hpp"

void    Connection::removeUserSessionId(int id) {
    std::set<int>::iterator it;

    if ((it = sessionIdentifiers.find(id)) != sessionIdentifiers.end())
        sessionIdentifiers.erase(it);
}

void    Connection::setLastActivityTime( void ) {
    lastActivityTime = getCurrentTime();
}

size_t  Connection::getLastActivityTime( void ) const {
    return (lastActivityTime);
}

void    Connection::setMessageInstance(Request* instance) {
    messageInstance = instance;
}

Request*   Connection::getMessageInstance( void ) const {
    return (messageInstance);
}

int     Connection::generateUserSessionId( void ) {
    int id, min = 0, max = 10000;   

    while (true) {
        id = (((getEventId() * 100) + min) * 1000) + max;
        if (sessionIdentifiers.find(id) == sessionIdentifiers.end()) {
            sessionIdentifiers.insert(id);
            break ;
        } else if (min == max)
            throw (500);
        min++, max--;
    }
    return (id);
}

void    Connection::addToEpollInstance(int flags) {
    epoll_event      event;

    event.events = flags, event.data.ptr = this;
    if (registered)
        epoll_ctl(epollFileDescriptor, EPOLL_CTL_DEL, socketInstance.getFileDescriptor(), NULL);
    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, socketInstance.getFileDescriptor(), &event) != -1) {
        registered = true;
        registeringFail = 0;
    } else if (5 < registeringFail)
        throw ("fail to add a connection fd to epoll instance");
    else
        registeringFail++;
}

Connection::Connection(): Event() {
    lastActivityTime    = size_t(-1);
    messageInstance     = NULL;
    registeringFail     = 0;
}

Connection::~Connection() {
    delete messageInstance;
}