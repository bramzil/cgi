# ifndef CORE_HPP
# define CORE_HPP

# include "../extern.hpp"
# include "Event.hpp"
# include "Connection.hpp"
# include "ServerSocket.hpp"

# include "../request/Request.hpp"
# include "../request/RequestProcessor.hpp"
# include "../response/Response.hpp"

class Core {
    public:
        void                            clearEventList( void );
        void                            removeEvent(Event* event);
        // void                            eventReceiver(epoll_event& event);

        // void                            eventHandler(epoll_event& event);
        void                            requestHandler(Request* request, Connection* connection);
        void                            requestProcessorHandler(RequestProcessor* processor, Connection* connection);
        void                            responseHandler(Response* response, Connection* connection);

        void                            startListeningProcess( void );
        void                            connectionReceiver(epoll_event& event);
        void                            connectionHandler(epoll_event& event);
        void                            processReadyEvents(int readies);

        void                            addToOrRemoveFromUnregisteredList(Event* event);
        void                            processUnregisteredEvents( void );
        void                            manageEventsLifeTime( void );



        void                            operator()(std::vector<Socket>& socketListArg);

        Core();
        ~Core();

    private:
        int                             epollFileDescriptor;
        epoll_event                     events[500];
        std::vector<Socket>             socketList;
        std::set<Event*>                unregisteredList;
        std::set<Event*>                eventList;
};

# endif