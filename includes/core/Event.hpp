# ifndef EVENT_HPP
# define EVENT_HPP

# define EVENTTIMEOUT   60000            

# define EPOLLIN_FLAGS  EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP
# define EPOLLOUT_FLAGS EPOLLOUT | EPOLLERR | EPOLLRDHUP | EPOLLHUP
# define EPOLLERR_FLAGS EPOLLERR | EPOLLRDHUP | EPOLLHUP

# include "Socket.hpp"
# include "../extern.hpp"

class Event {
    public:
        bool                        isRegistered( void );

        void                        setEventId(int id);
        int                         getEventId( void ) const;

        void                        setSocketType(std::string type);
        std::string                 getSocketType( void ) const;

        void                        setSocketFileDescriptor(int fd);
        int                         getSocketFileDescriptor( void ) const;

        void                        setEpollFileDescriptor(int fd);
        int                         getEpollFileDescriptor( void ) const;

        void                        setSocket(const Socket& sock);
        Socket                      getSocket( void ) const;

        virtual void                addToEpollInstance(int flags) = 0;
        void                        removeFromEpollInstance( void );

        Event( void );
        virtual ~Event();

    protected:
        bool                        registered;
        int                         id;
        int                         epollFileDescriptor;
        std::string                 socketType;
        Socket                      socketInstance;
};

# endif