# ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include "Event.hpp"
# include "../extern.hpp"

class ServerSocket: public Event {
    public:
        void            startListening( void );
        int             receiveConnection( void );
        void            addToEpollInstance(int flags);

        ServerSocket();
        virtual ~ServerSocket();
};

# endif