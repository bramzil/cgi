/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:05:18 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/23 16:50:56 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/core/Location.hpp"

void    Location::displayLocationParameters( void ) const {
    /*
        this function must not be pushed with the project
        it is just for debuging, so we need to don't
        forget this.
    */

    std::cout << "================ location =================" << '\n';

    std::cout << " parameter     : " << parameter << '\n';

    std::cout << " root          : " << root << '\n';

    std::cout << " auto_index    : " << autoIndex << '\n';

    std::cout << " default page  : " << index << '\n';

    std::cout << " return code   : " << redirection.first << \
        " description: " << redirection.second << '\n';

    std::cout << "================= methods =================" << '\n';
    if (allowedMethods.size()) {
        std::set<std::string>::const_iterator it, ed;
        it = allowedMethods.begin(), ed = allowedMethods.end();
        while (it != ed) {
            std::cout << " method        : " << *it << '\n';
            it++;
        }
    }

    std::cout << "=============== error_Pages ===============" << '\n';
    if (defaultErrorPages.size()) {
        std::map<short, std::string>::const_iterator it, ed;
        it = defaultErrorPages.begin(), ed = defaultErrorPages.end();
        while (it != ed) {
            std::cout << " code          : " << it->first << " path: " << it->second << '\n';
            it++;
        }
    }
    std::cout << "===========================================" << '\n';
}

size_t  Location::isMatchParameter(std::string str) const {
    if (str.find(parameter.c_str(), 0, parameter.size()) != std::string::npos)
        return (parameter.size());
    return (0);
}

void Location::setParameter(std::string str) {
    if (str.size() && (str.at(str.size() - 1) != '/'))
        parameter = trimFromEnd(str, "/ ");
    else
        parameter = str;
}

std::string Location::getParameter( void ) const {
    return (parameter);
}

void    Location::setRoot(std::string str) {
    if (str.size() && (str.at(str.size() - 1) != '/'))
        root = str + "/";
    else
        root = str;
}

std::string Location::getRoot( void ) const {
    return (std::string(root));
}

void    Location::setIndex(std::string str) {
    index = str;
}

std::string Location::getIndex( void ) const {
    return (index);
}

void    Location::setAutoIndex(std::string str) {
    autoIndex = toLowerCaseString(str);
}

bool    Location::isAutoIndex( void ) const {
    if (autoIndex == std::string("on"))
        return (true);
    return (false);
}

bool    Location::isThereAnyRedirection( void ) const {
    return (redirection.first);
}

void    Location::setRedirection(std::pair<short, std::string>& pair) {
    redirection = pair;
}

std::pair<short, std::string> Location::getRedirection( void ) const {
    return (redirection);
}

bool    Location::isAllowedMethod(std::string str) const {
    std::set<std::string>::const_iterator it, ed;

    if ((it = allowedMethods.find(str)) != allowedMethods.end())
        return (true);
    return (false);
}

void    Location::addAllowedMethod(std::string str) {
    allowedMethods.insert(str);
}

void    Location::addDefaultErrorPage(std::pair<short, std::string>& pair) {
    defaultErrorPages.insert(pair);
}

std::string Location::getDefaultErrorPage(short code) const {
    std::map<short, std::string>::const_iterator it;

    if ((it = defaultErrorPages.find(code)) != defaultErrorPages.end())
        return (it->second);
    return (std::string(""));
}

bool    Location::isThereAnyBackendApplication( void ) const {
    return (cgiApllications.size());
}

void    Location::setCGIApplication(std::string extention, std::string executablePath) {
    cgiApllications.insert(std::pair<std::string, std::string>(extention, executablePath));
}

std::string Location::getCGIApplication(std::string extention) const {
    std::map<std::string, std::string>::const_iterator it;

    if ((it = cgiApllications.find(extention)) != cgiApllications.end())
        return (it->second);
    return ("");
}

bool    Location::isThereAnyCookie( void ) const {
    return (cookies.size());
}

void    Location::addCookie(std::vector<std::string> cookie) {
    cookies.insert(cookie);
}

std::set<std::vector<std::string> > Location::getCookies( void ) const {
    return (cookies);
}

Location::Location() {
    redirection = std::pair<short, std::string>(0, "");
}

Location::Location(const Location& x) {
    *this = x;
}

const Location& Location::operator=(const Location& x) {
    parameter           = x.parameter;
    root                = x.root;
    index               = x.index;
    autoIndex           = x.autoIndex;
    allowedMethods      = x.allowedMethods;
    defaultErrorPages   = x.defaultErrorPages;
    redirection         = x.redirection;
    cgiApllications     = x.cgiApllications;
    cookies             = x.cookies;
    return (*this);
}

bool Location::operator==(const Location& x) const {
    if (parameter == x.parameter)
        return (true);
    return (false);
}

Location::~Location() {
    // nothing to do for this time.
}
