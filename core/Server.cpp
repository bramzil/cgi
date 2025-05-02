/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 21:34:22 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/26 19:12:03 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/core/Server.hpp"

void Server::displayServerParameters( void ) const {
    std::cout << "================== server =================\n";

    std::cout << "================== names  =================\n";
    if (serverNames.size()) {
        std::vector<std::string>::const_iterator it, ed;
        it = serverNames.begin(), ed = serverNames.end();
        while (it != ed) {
            std::cout << " name          : " << *it << '\n';
            it++;
        }
    }
    std::cout << "================= sockets =================\n";
    if (sockets.size()) {
        std::vector<sockaddr*>::const_iterator it, ed;
        it = sockets.begin(), ed = sockets.end();
        while (it != ed) {
            displaySocketParameter(*it);
            it++;
        }
    }
    std::cout << "=========== maxRequesBodysize  ============\n";
        std::cout << " max body size : " << maxBodySize << '\n';
    std::cout << "=============== locations  ================\n";
    if (locations.size()) {
        std::vector<Location>::const_iterator it, ed;
        it = locations.begin(), ed = locations.end();
        while (it != ed) {
            it->displayLocationParameters();
            it++;
        }
    }
    std::cout << "===========================================\n";
}


void Server::addServerName(const std::string& str) {
    std::vector<std::string>::iterator it, ed;

    if (!serverNames.size())
        serverNames.push_back(str);
    else {
        it = serverNames.begin(), ed = serverNames.end();
        while (it != ed) {
            if (*it == str)
                break ;
            else if ((it + 1) == ed)
                serverNames.push_back(str);
            it++;
        }
    }
}

bool Server::isThereCommonName(const Server& srv) const {
    std::vector<std::string>::const_iterator n_it, n_ed;

    n_it = serverNames.begin(), n_ed = serverNames.end();
    while ((n_it != n_ed)) {
        if (srv.isMatchServer(*n_it))
            return (true);
        n_it++;
    }
    return (false);
}

bool Server::isMatchServer(const std::string& str) const {
    std::vector<std::string>::const_iterator it, ed;

    it = serverNames.begin(), ed = serverNames.end();
    while (it != ed) {
        if (*it == str)
            return (true);
        it++;
    }
    return (false);
}

void Server::addSocket(sockaddr* socket) {
    sockaddr_in *ptr;
    sockaddr_in6 *ptr6;
    
    if (!isListenOn(socket)) {
        if (socket->sa_family == AF_INET) {
            ptr = new sockaddr_in, *ptr = *((sockaddr_in*) socket);
            sockets.push_back((sockaddr*) ptr);
        } else if (socket->sa_family == AF_INET6) {
            ptr6 = new sockaddr_in6, *ptr6 = *((sockaddr_in6*) socket);
            sockets.push_back((sockaddr*) ptr6);
        }
    }
}

bool Server::setSockets(std::string host, std::string port) {
    addrinfo hints, *result, *head;

    initiatHints(hints), result = NULL;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
        return (false);
    else {
        head = result;
        while (result) {
            addSocket(result->ai_addr);
            result = result->ai_next;
        }
        freeaddrinfo(head);
    }
    return (true);
}

void Server::copyServerSockets(const Server& srv) {
    sockaddr_in *in_ptr;
    sockaddr_in6 *in6_ptr;
    std::vector<sockaddr*>::const_iterator it, ed;

    clearSockets();
    it = srv.sockets.begin(), ed = srv.sockets.end();
    while (it != ed) {
        if ((*it)->sa_family == AF_INET) {
            in_ptr = new sockaddr_in, *in_ptr = *((sockaddr_in*) *it);
            sockets.push_back((sockaddr*) in_ptr);
        } else if ((*it)->sa_family == AF_INET6) {
            in6_ptr = new sockaddr_in6, *in6_ptr = *((sockaddr_in6*) *it);
            sockets.push_back((sockaddr*) in6_ptr);
        }
        it++;
    }
}

void Server::clearSockets( void ) {
    std::vector<sockaddr*>::iterator it, ed;

    it = sockets.begin(), ed = sockets.end();
    while (it != ed) {
        delete *it;
        it++;
    }
    sockets.clear();
}

bool Server::isThereCommonSocket(const Server& srv) const {
    std::vector<sockaddr*>::const_iterator it, ed;

    it = sockets.begin(), ed = sockets.end();
    while (it != ed) {
        if (srv.isListenOn(*it))
            return (true);
        it++;
    }
    return (false);  
}

bool Server::isListenOn(const sockaddr* sock) const {
    std::vector<sockaddr*>::const_iterator it, ed;

    it = sockets.begin(), ed = sockets.end();
    while (it != ed) {
        if (socketComparer(*it, sock))
            return (true);
        it++;
    }
    return (false);
}

const std::vector<sockaddr*>& Server::getPairs( void ) const {
    return (sockets);
}

void Server::addLocation(const Location& loc, size_t& l_nbr) {
    (void)l_nbr;
    std::vector<Location>::iterator it, ed;

    if (!locations.size())
        locations.push_back(loc);
    else {
        it = locations.begin(), ed = locations.end();
        while (it != ed) {
            if (*it == loc)
                *it = loc;
            else if ((it + 1) == ed)
                locations.push_back(loc);
            it++;
        }
    }
}

const Location Server::getLocation(const std::string& str) const {
    bool flag;
    Location loc;
    size_t  newMatchLength, oldMatchLength;
    std::vector<Location>::const_iterator it, ed;

    newMatchLength = oldMatchLength = flag = 0;
    ed = locations.end(), it = locations.begin();
    while (it != ed) {
        if ((newMatchLength = it->isMatchParameter(str))) {
            if (oldMatchLength < newMatchLength) {
                oldMatchLength = newMatchLength;
                flag = true;
                loc = *it;
            }
        }
        it++;
    }
    return (loc);
}

bool Server::isMatchAnyLocation(const std::string& param) const {
    std::vector<Location>::const_iterator it, ed;

    it = locations.begin(), ed = locations.end();
    while (it != ed) {
        if (it->isMatchParameter(param))
            return (true);
        it++;
    }
    return (false);
}

void Server::setMaxBodySize(const size_t& size) {
    maxBodySize = size;
}

bool Server::isRespectMaxBodySize(size_t size) const {
    if (maxBodySize >= size)
        return (true);
    return (false);
}


bool Server::operator==(const Server& x) const {
    return (isThereCommonName(x) && isThereCommonSocket(x));
}

Server::Server(): maxBodySize(2000000) {
    // do nothing for this time.
}

Server::Server(const Server& x): maxBodySize(20000000) {
    *this = x;
}

Server& Server::operator=(const Server& x) {
    if (this != &x) {
        copyServerSockets(x);
        locations   = x.locations;
        serverNames = x.serverNames;
        maxBodySize = x.maxBodySize;
    }
    return (*this);
}

Server::~Server() {
    std::vector<sockaddr*>::iterator it, ed;

    it = sockets.begin(), ed = sockets.end();
    while (it != ed) {
        delete (*it);
        it++;
    }
 }