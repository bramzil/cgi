/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 21:56:51 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/23 16:46:08 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"
# include "../extern.hpp"

class Server {
    public:
        void                            displayServerParameters( void ) const; // this must be remove before push.
        
        void                            addServerName(const std::string& str);
        bool                            isMatchServer(const std::string& str) const;
        bool                            isThereCommonName(const Server& srv) const;

        void                            addSocket(sockaddr* socket);
        bool                            setSockets(std::string host, std::string port);
        void                            copyServerSockets(const Server& srv);
        void                            clearSockets( void );
        bool                            isThereCommonSocket(const Server& srv) const;
        bool                            isListenOn(const sockaddr* sock) const;
        const std::vector<sockaddr*>&   getPairs( void ) const;

        void                            addLocation(const Location& loc, size_t& l_nbr);
        bool                            isMatchAnyLocation(const std::string& param) const;
        const Location                  getLocation(const std::string& str) const;

        void                            setMaxBodySize(const size_t& size);
        bool                            isRespectMaxBodySize(size_t size) const;
        bool                            operator==(const Server& x) const;

        Server();
        Server(const Server& x);
        Server&                         operator=(const Server& x);
        virtual ~Server();

    protected:

        size_t                          maxBodySize;
        std::vector<Location>           locations;
        std::vector<std::string>        serverNames;
        std::vector<sockaddr*>          sockets;

};

# endif