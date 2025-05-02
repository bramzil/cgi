# include "../includes/core/Core.hpp"

void    Core::clearEventList( void ) {
    std::set<Event*>::iterator it, ed;

    it = eventList.begin(), ed = eventList.end();
    while (it != ed) {
        delete (*it);
        it++;
    }
    eventList.clear();
}

void    Core::removeEvent(Event* event) {
    std::set<Event*>::iterator it;

    if (event->isRegistered())
        event->removeFromEpollInstance();
    if ((it = eventList.find(event)) != eventList.end())
        eventList.erase(it);
    if ((it = unregisteredList.find(event)) != unregisteredList.end())
        unregisteredList.erase(it);
    delete event;
}

void    Core::startListeningProcess( void ) {
    ServerSocket  *socketServer;
    std::vector<Socket>::iterator it, ed;

    it = socketList.begin(), ed = socketList.end();
    while (it != ed) {
        socketServer = new ServerSocket();
        eventList.insert((Event*)socketServer);

        socketServer->setSocketType("serverSocket");
        socketServer->setSocket(*it);
        socketServer->startListening();

        socketServer->setEpollFileDescriptor(epollFileDescriptor);
        socketServer->setEventId(epollFileDescriptor);
        socketServer->addToEpollInstance(EPOLLIN_FLAGS);
        it++;
    }
}

void    Core::connectionReceiver(epoll_event& event) {
    Connection          *connection;
    ServerSocket        *serverSocket;
    
    serverSocket = (ServerSocket*) event.data.ptr;
    connection = new Connection();
    eventList.insert((Event*)connection);



    connection->setLastActivityTime();
    connection->setSocketType("connectionSocket");
    connection->setSocket(serverSocket->getSocket());
    connection->setSocketFileDescriptor(serverSocket->receiveConnection());

    connection->setEpollFileDescriptor(serverSocket->getEpollFileDescriptor());
    connection->setEventId((serverSocket->getEventId()));
    // connection->addToEpollInstance(EPOLLIN_FLAGS);

    // connection->setMessageInstance(new Request(connection->getSocket()));
    addToOrRemoveFromUnregisteredList(connection);
}

void    Core::connectionHandler(epoll_event& event) {
    Connection          *connection;
    Request             *request;
    RequestProcessor    *processor;
    Response            *response;

    std::cout << "\n+++++++++++++++ content +++++++++++++++++\n";
    connection = (Connection*) event.data.ptr;
    if (!connection->getMessageInstance())
        connection->setMessageInstance(new Request(connection->getSocket()));
    else if ((response = dynamic_cast<Response*>(connection->getMessageInstance())))
        responseHandler(response, connection);        
    else if ((processor = dynamic_cast<RequestProcessor*>(connection->getMessageInstance())))
        requestProcessorHandler(processor, connection);
    else if ((request = dynamic_cast<Request*>(connection->getMessageInstance())))
        requestHandler(request, connection);
    else
        throw ("stop running from connectionHandler");
    std::cout << "\n+++++++++++++++++++++++++++++++++++++++++\n";
}

void    Core::requestHandler(Request* request, Connection* connection) {
    (*request)(connection);
    if (request->isDone()) {
        if (request->gettypeOfResponse() & RES_WITH_ERROR) {
            connection->setMessageInstance(new Response(*request));
        } else
            connection->setMessageInstance(new RequestProcessor(*request));
        delete request;
    }
    addToOrRemoveFromUnregisteredList(connection);
}

void    Core::requestProcessorHandler(RequestProcessor* processor, Connection* connection) {
    (*processor)(connection);
    if (processor->isDone()) {
        connection->setMessageInstance(new Response(*processor));
        delete processor;
    }
    addToOrRemoveFromUnregisteredList(connection);
}

void    Core::responseHandler(Response* response, Connection* connection) {
    
    (*response)(connection);
    if (response->isDone()) {
        if ((response->gettypeOfResponse() & RES_WITH_ERROR) || (response->getHeaderValue("connection") != "keep-alive"))
            removeEvent(connection);
        else {
            if (!connection->unprocessedData.size()) {
                connection->setMessageInstance(NULL);
            } else
                connection->setMessageInstance(new Request(connection->getSocket()));
            delete response;
            addToOrRemoveFromUnregisteredList(connection);
        }
    }
}

void    Core::processReadyEvents(int readies) {
    int                 i = 0;
    Event               *event;
    std::string         socketType;

    while (i < readies) {
        if (events[i].events & (EPOLLERR_FLAGS))
            removeEvent((Event*)events[i].data.ptr);
        else {
            event = (Event*) events[i].data.ptr;
            socketType = event->getSocketType();
            if (socketType == "serverSocket")
                connectionReceiver(events[i]);
            else if (socketType == "connectionSocket") {
                connectionHandler(events[i]);
            } else
                throw ("epoll_wait return invalid event");
        }
        i++;
    }
}

void    Core::addToOrRemoveFromUnregisteredList(Event* event) {
    std::set<Event*>::iterator  it;

    if ((it = unregisteredList.find(event)) != unregisteredList.end()) {
        if (event->isRegistered())
            unregisteredList.erase(it);
    } else if (!event->isRegistered())
        unregisteredList.insert(event);
}

void    Core::processUnregisteredEvents( void ) {
    epoll_event event;
    Connection *connection;
    std::set<Event*> eventList;
    std::set<Event*>::iterator it, ed;

    eventList = unregisteredList;
    it = eventList.begin(), ed = eventList.end();
    while (it != ed) {
        if ((connection = dynamic_cast<Connection*>(*it))) {
            event.data.ptr = *it;
            connectionHandler(event);
        }
        it++;
    }
}

void    Core::manageEventsLifeTime( void ) {
    epoll_event event;
    Connection *connection;
    std::set<Event*>::iterator it, ed;

    it = eventList.begin(), ed = eventList.end();
    while (it != ed) {
        if ((connection = dynamic_cast<Connection*>(*it))) {
            event.data.ptr = connection;
            if (connection->getMessageInstance()) {
                if (EVENTTIMEOUT < (getCurrentTime() - connection->getLastActivityTime()))
                    connectionHandler(event);
            } else if ((EVENTTIMEOUT * 3) < (getCurrentTime() - connection->getLastActivityTime()))
                removeEvent(connection);
        }
        it++;
    }
}

void    Core::operator()(std::vector<Socket>& socketLst) {
    int readies, blockTime;
    size_t  lastCheckOfTimeOut;

    socketList = socketLst;
    startListeningProcess();
    lastCheckOfTimeOut = getCurrentTime();
    while (true) {
        blockTime = (unregisteredList.size())? 0 : EVENTTIMEOUT;
        readies = epoll_wait(epollFileDescriptor, events, 500, blockTime);
        if (readies == -1)
            throw ("Failed to get readies list from epoll instance\n");
        else {
            if (unregisteredList.size())
                processUnregisteredEvents();
            if (0 < readies)
                processReadyEvents(readies);
            if (EVENTTIMEOUT < (getCurrentTime() - lastCheckOfTimeOut)) {
                lastCheckOfTimeOut = getCurrentTime();
                manageEventsLifeTime();
            }
            readies = 0;
        }
    }
}

Core::Core( void ) {
    if ((epollFileDescriptor = epoll_create(20)) == -1)
        throw ("Failed to create an epoll instance");
}

Core::~Core() {
    clearEventList();
}
