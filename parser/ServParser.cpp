

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 21:34:22 by bramzil           #+#    #+#             */
/*   Updated: 2025/02/14 19:17:42 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/parser/ServParser.hpp"

void    ServParser::parseServerName(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list;
    std::vector<std::string>::iterator it, ed;

    list = tokenizer(str, " ");
    if (list.size() < 2)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else {
        it = list.begin(), ed = list.end();
        while (++it != ed) {
            addServerName(trimString(*it, "'\""));
        }
    }
}

void    ServParser::parseListen(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list, tmp;
    std::vector<std::string>::iterator it, ed;

    list = tokenizer(str, " "), it = list.begin(), ed = list.end();
    if (list.size() < 2)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    while (++it != ed) {
        tmp = tokenizer(*it, ":");
        if (!tmp.size() || (2 < tmp.size()))
            throw (ParserExcep(list.front(), "Invalid format of argument", l_nbr));
        else {
            trimString(tmp.front(), "[]");
            if (2 == tmp.size()) {
                if (!setSockets(tmp.front(), tmp.back()))
                    throw (ParserExcep(list.front(), "Invalid argument", l_nbr));
            } else if (isPortNumber(tmp.front())) {
                if (!setSockets("localhost", tmp.front()))
                    throw (ParserExcep(list.front(), "Invalid port", l_nbr));
            } else if (!setSockets(tmp.front(), "8080"))
                throw (ParserExcep(list.front(), "Invalid host", l_nbr));
        }
    }
}

void    ServParser::parseLocation(const std::string& str, size_t& l_nbr) {
    size_t  start = l_nbr;
    LocParser loc(stream);
    std::vector<std::string> list;

    list = tokenizer(str, " ");
    if (list.size() != 3)
        throw (ParserExcep(list.front(), "Invalid number of parameter", l_nbr));
    else if (list.back() != "{") 
        throw (ParserExcep(list.front(), "block not opened correctly", l_nbr));
    else {
        loc = *this;
        loc.setParameter(trimString(list.at(1), "'\""));
        loc(l_nbr);
        addLocation(loc, start);
    }
}

void    ServParser::parseMaxBodySize(const std::string& str, size_t& l_nbr) {
    size_t size;
    std::string unit;
    std::vector<std::string> list;
    
    list = tokenizer(str, " ");
    if ((list.size() < 2) || (3 < list.size()))
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else {
        size = string_to_size_t(list.at(1));
        if (list.size() == 3) {
            unit = toLowerCaseString(trimString(list.at(2), "'\""));
            setMaxBodySize(convertTobyte(unit, size));
        } else
            setMaxBodySize(size);
    }
}

void    ServParser::isGoodConfigured(size_t l_nbr) const {
    if (!serverNames.size())
        throw (ParserExcep("server", "Must have a server_name", l_nbr));
    else if (!sockets.size())
        throw (ParserExcep("server", "Must have a socket to listen to", l_nbr));
    else if (!locations.size())
        throw (ParserExcep("server", "Must have a location to serve from", l_nbr));
    else if (!done)
        throw (ParserExcep("server", "Left unclosed", l_nbr));
}

void    ServParser::parserCaller(std::string& line, size_t& l_nbr) {
    std::vector<std::string> list;
    std::map<std::string, parserPtr>::iterator it;

    list = tokenizer(line, " ");
    if (!list.size())
        return ;
    else if ((list.size() == 1) && (list.front() == "}"))
        done = true;
    else if ((it = parserList.find(list.front())) != parserList.end())
        (this->*(it->second))(trimString(line, "; "), l_nbr);
    else
        throw (ParserExcep(list.front(), "Not allowed in this contetxt", l_nbr));
}

void    ServParser::operator()(size_t& l_nbr) {
    size_t start;
    std::string line;

    start = l_nbr;
    while (!stream.eof() && !done) {
        l_nbr++;
        std::getline(stream, line);
        parseLine(trimString(line, " "), l_nbr);
        if (!line.size())
            continue ;
        else
            parserCaller(line, l_nbr);
    }
    isGoodConfigured(start);
}

void    ServParser::initiateAttribute( void ) {
    parserList.insert(std::pair<std::string, parserPtr>("auto_index", &ServParser::parseAutoIndex));
    parserList.insert(std::pair<std::string, parserPtr>("index", &ServParser::parseIndex));
    parserList.insert(std::pair<std::string, parserPtr>("root", &ServParser::parseRoot));
    parserList.insert(std::pair<std::string, parserPtr>("default_error_page", &ServParser::parseDefaultErrorPage));
    parserList.insert(std::pair<std::string, parserPtr>("allowed_method", &ServParser::parseAllowedMethod));
    parserList.insert(std::pair<std::string, parserPtr>("listen", &ServParser::parseListen));
    parserList.insert(std::pair<std::string, parserPtr>("server_name", &ServParser::parseServerName));
    parserList.insert(std::pair<std::string, parserPtr>("max_body_size", &ServParser::parseMaxBodySize));
    parserList.insert(std::pair<std::string, parserPtr>("location", &ServParser::parseLocation));
}

ServParser::ServParser(std::fstream& is): Server(), LocParser(is), done(false) {
    initiateAttribute();   
}

ServParser::ServParser(const ServParser& x): Server(), LocParser(x.stream) {
    initiateAttribute();
    *this = x;
}

ServParser& ServParser::operator=(const ServParser& x) {
    if (this != &x) {
        Server::operator=(x);
        done    = x.done;
    }
    return (*this);
}

ServParser::~ServParser() {
    // nothing to do for this time.
}