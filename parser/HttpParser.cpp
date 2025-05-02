/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 13:47:31 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/23 16:51:42 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/parser/HttpParser.hpp"

void    HttpParser::displaySockets( void ) const {

    std::vector<Server> server_list;
    std::vector<Socket>::const_iterator so_it, so_ed;
    std::vector<Server>::const_iterator sr_it, sr_ed;

    std::cout << "==================  sockets ===============" << '\n';
    so_it = socketLst.begin(), so_ed = socketLst.end();
    while (so_it != so_ed) {
        std::cout << "==================  socket  ===============" << '\n';
        displaySocketParameter(so_it->getPair());
        std::cout << "================== servers ================" << '\n';
        server_list = so_it->getServers();
        sr_it = server_list.begin(), sr_ed = server_list.end();
        while (sr_it != sr_ed) {
            sr_it->displayServerParameters();
            sr_it++;
        }
        so_it++;
    } 
    std::cout << "===========================================" << '\n';
    std::cout << "===========================================" << '\n';
}

void    HttpParser::parseServer(const std::string& str, size_t& l_nbr) {
    size_t start = l_nbr;
    ServParser server(stream);
    std::vector<std::string> list;

    list = tokenizer(str, " ");
    if (list.size() != 2)
        throw (ParserExcep(list.front(), "Invalid number of parameter", l_nbr));
    else if (list.back() != "{") 
        throw (ParserExcep(list.front(), "block not opened correctly", l_nbr));
    else {
        server = *this;
        server(l_nbr);
        addServer(server, start);
    }
}

void    HttpParser::addServer(const ServParser& srv, size_t& l_nbr) {
    std::vector<sockaddr*> pairs;
    std::vector<sockaddr*>::iterator so_it, so_ed;

    pairs = srv.getPairs();
    so_it = pairs.begin(), so_ed = pairs.end();
    while (so_it != so_ed) {
        addSocketToList(*so_it);
        addServerToSocket(srv, (*so_it), l_nbr);
        so_it++;
    }
}

void    HttpParser::addSocketToList(const sockaddr* sock) {
    std::vector<Socket>::iterator it, ed;

    if (!socketLst.size())
        socketLst.push_back(Socket(sock));
    else {
        it = socketLst.begin(), ed = socketLst.end();
        while (it != ed) {
            if (socketComparer(it->getPair(), sock))
                break ;
            else if ((it + 1) == ed)
                socketLst.push_back(Socket(sock));
            it++;
        }
    }
}

void    HttpParser::addServerToSocket(const ServParser& srv, const sockaddr* sock, size_t& l_nbr) {
    std::vector<Socket>::iterator it, ed;

    it = socketLst.begin(), ed = socketLst.end();
    while (it != ed) {
        if (socketComparer(it->getPair(), sock))
            it->addServer(srv, l_nbr);
        it++;
    }
}

void    HttpParser::operator()( void ) {
    size_t  l_nbr = 1;
    std::string line;
    std::vector<std::string> list;
    std::map<std::string, parserPtr>::iterator it;

    while (!stream.eof()) {
        std::getline(stream, line);
        parseLine(line, l_nbr);
        list = tokenizer(line, " ");
        if (list.size()) {
            if ((it = parserList.find(list.front())) != parserList.end())
                (this->*(it->second))(trimString(line, "; "), l_nbr);
            else
                throw (ParserExcep(list.front(), "Not allowed in this contetxt", l_nbr));
        }
        l_nbr++;
    }
    if (!socketLst.size())
        throw (ParserExcep("http", "Must contain at least one server", l_nbr));
    displaySockets();
}

void    HttpParser::initiateAttribute( void ) {
    parserList.insert(std::pair<std::string, parserPtr>("listen", &HttpParser::parseListen));
    parserList.insert(std::pair<std::string, parserPtr>("server", &HttpParser::parseServer));
}

HttpParser::HttpParser(std::vector<Socket>& socketList, std::fstream& is): ServParser(is), socketLst(socketList) {
    initiateAttribute();
}

HttpParser::~HttpParser() {
    //no thing to clear
}