# ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Event.hpp"
# include "../extern.hpp"

class Request;

class Connection: public Event {
    public:

    
        void                        setLastActivityTime( void );
        size_t                      getLastActivityTime( void ) const;
        
        void                        setMessageInstance(Request* message);
        Request*                    getMessageInstance( void ) const;
        int                         generateUserSessionId( void );
        
        void                        removeUserSessionId( int );
        void                        addToEpollInstance(int flags);

        Connection();
        virtual ~Connection();
        
        std::string                 unprocessedData;

    private:
        short                       registeringFail;
        size_t                      lastActivityTime;
        Request                     *messageInstance;
        std::set<int>               sessionIdentifiers;
};

# endif