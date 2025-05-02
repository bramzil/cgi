/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocParser.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:05:18 by bramzil           #+#    #+#             */
/*   Updated: 2025/02/14 18:27:22 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/parser/LocParser.hpp"

void    LocParser::parseRoot(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list;

    list = tokenizer(str, " ");
    if (list.size() != 2)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else {
        if (isAbsolPath(trimString(list.back(), "'\"").c_str()))
            setRoot(list.back());
        else
            throw (ParserExcep(list.front(), "Invalid path to root", l_nbr));
    }
}

void    LocParser::parseIndex(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list;

    list = tokenizer(str, " ");
    if (list.size() != 2)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else
        setIndex(trimString(list.back(), "'\""));
}

void    LocParser::parseAutoIndex(const std::string& str, size_t& l_nbr) {
    std::string value;
    std::vector<std::string> list;

    list = tokenizer(str, " ");
    if (list.size() == 2){
        value = toLowerCaseString(list.back());
        if ((value != "on") && (value != "off"))
            throw (ParserExcep(list.front(), "Invalid value", l_nbr));
        setAutoIndex(value);
    } else
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
}

void    LocParser::parseAllowedMethod(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list;
    std::vector<std::string>::iterator it, ed;

    list = tokenizer(str, " ");
    if (list.size() < 2)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else {
        it = list.begin(), ed = list.end();
        while (++it != ed) {
            addAllowedMethod(trimString(*it, "'\""));
        }
    }
}

void    LocParser::parseRedirectTo(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list;
    std::pair<short, std::string> tmp;

    list = tokenizer(str, " ");
    if (list.size() != 3)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else {
        tmp.first = string_to_size_t(list.at(1));
        if ((tmp.first < 300) || (399 < tmp.first))
            throw (ParserExcep(list.front(), "Invalid code", l_nbr));
        else {
            tmp.second = trimString(list.back(), "'\"");
            setRedirection(tmp);
        }
    }
}

void    LocParser::parseDefaultErrorPage(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list;
    std::pair<short, std::string> tmp;

    list = tokenizer(str, " ");
    if (list.size() != 3)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else {
        tmp.first = string_to_size_t(list.at(1));
        if ((tmp.first < 400) || (599 <= tmp.first))
            throw (ParserExcep(list.front(), "Invalid status code", l_nbr));
        else {
            tmp.second = trimString(list.back(), "'\"");
            addDefaultErrorPage(tmp);
        }
    }
}

void    LocParser::parseCGIApplication(const std::string& str, size_t& l_nbr) {
    std::vector<std::string> list;

    list = tokenizer(str, " ");
    if (list.size() != 3)
        throw (ParserExcep(list.front(), "Invalid number of arguments", l_nbr));
    else
        setCGIApplication(trimString(list.at(1), "'\""), trimString(list.at(2), "'\""));
}

void    LocParser::parseCookie(const std::string& str, size_t& l_nbr) {
    size_t pos;
    std::string tmp(str);
    std::vector<std::string> list;
    std::vector<std::string> cookie;
    std::vector<std::string>::iterator it, ed;

    if ((pos = tmp.find("cookie_to_set")) == std::string::npos)
        throw (ParserExcep(list.front(), "Invalid Number of argument", l_nbr));
    else {
        tmp = tmp.substr((pos + 14));
        list = tokenizer(tmp, ";");
        it = list.begin(), ed = list.end();
        while (it != ed) {
            cookie.push_back(*it);
            it++;
        }
        addCookie(cookie);
    }
}

void    LocParser::isGoodConfigured(size_t l_nbr) const {
    if (!parameter.size())
        throw (ParserExcep("location", "Must have its unique parameter", l_nbr));
    else if (!root.size())
        throw (ParserExcep("location", "Must have a root from where to serve content", l_nbr));
    else if (!done)
        throw (ParserExcep("location", "Left unclosed", l_nbr));
}

void    LocParser::parserCaller(std::string& line, size_t& l_nbr) {
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

void    LocParser::operator()(size_t& l_nbr) {
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

void    LocParser::initiateAttribute( void ) {
    parserList.insert(std::pair<std::string, parserPtr>("auto_index", &LocParser::parseAutoIndex));
    parserList.insert(std::pair<std::string, parserPtr>("redirect_to", &LocParser::parseRedirectTo));
    parserList.insert(std::pair<std::string, parserPtr>("index", &LocParser::parseIndex));
    parserList.insert(std::pair<std::string, parserPtr>("root", &LocParser::parseRoot));
    parserList.insert(std::pair<std::string, parserPtr>("cgi_application", &LocParser::parseCGIApplication));
    parserList.insert(std::pair<std::string, parserPtr>("default_error_page", &LocParser::parseDefaultErrorPage));
    parserList.insert(std::pair<std::string, parserPtr>("allowed_method", &LocParser::parseAllowedMethod));
    parserList.insert(std::pair<std::string, parserPtr>("cookie_to_set", &LocParser::parseCookie));
}

LocParser::LocParser(std::fstream& is): Location(), done(false), stream(is) {
    initiateAttribute();
}

LocParser::LocParser(const LocParser& x): Location(), done(false), stream(x.stream) {
    initiateAttribute();
    *this = x;
}

LocParser&  LocParser::operator=(const LocParser& x) {
    if (this != &x)
        Location::operator=(x);
    return (*this);
}

bool    LocParser::operator==(const LocParser& x) const {
    if (parameter == x.parameter)
        return (true);
    return (false);
}

LocParser::~LocParser() {
    // nothing to do for this time.
}
