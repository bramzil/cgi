/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:39:54 by abbaraka          #+#    #+#             */
/*   Updated: 2025/04/23 16:52:36 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/request/RequestLine.hpp"

RequestLine::RequestLine() {
	std::string methodsStrs[8] = {"GET", "POST", "DELETE"};
    for (size_t i = 0; i < 8; i++) {
        methods.push_back(methodsStrs[i]);
    }
}

RequestLine::~RequestLine() {}



std::string decode(const std::string& encodedURI) {
    std::string decodedURI;
    for (size_t i = 0; i < encodedURI.length(); ++i) {
        if (encodedURI[i] == '%') {
            if (i + 2 < encodedURI.length()) {
                std::string hex = encodedURI.substr(i + 1, 2);
                unsigned int value;
                std::istringstream(hex) >> std::hex >> value;
                decodedURI += static_cast<char>(value);
                i += 2;
            } else {
                decodedURI += '%';
            }
        } else {
            decodedURI += encodedURI[i];
        }
    }
    return (decodedURI);
}



void    RequestLine::checkMethod(std::string method) {
    for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++) {
        if (*it == method)
           return ;
    }
    throw (501);
}

void    RequestLine::checkParams(std::string &uri) {
    size_t paramStart = uri.find("?");
    if (paramStart != std::string::npos) {
        std::string paramStr = uri.substr(paramStart + 1);
        std::vector<std::string> list = tokenizer(trimString(paramStr, "?"), "?");
        if (list.size() > 1) {
            std::cout << list.front() << std::endl;
            throw (400);
        }
        params = decode(list.front());
        uri = uri.substr(0, paramStart);
    }
}

void	RequestLine::setRequestLine(std::string line) {
    if (!isalpha(line[0]))
		throw (400);

	std::istringstream	iss(line);

	if (line.find(CRLF) == std::string::npos)
		throw (400);
	if (line.find("\t\v\f") != std::string::npos)
		throw (400);
    iss >> method >> uri >> version;
    checkMethod(method);
    if (uri[0] != '/')
        throw (400);
    if (uri.size() > 8000)
        throw (414);
    checkParams(uri);
    uri = decode(uri);
    if (version != "HTTP/1.1")
		throw (505);
}

std::string	RequestLine::getRequestLine() const {
    std::string reqLine;
    reqLine = method + " " + uri + " " + version;
    return (reqLine);
}

bool	RequestLine::checkIfInit() const {
    if (!version.empty())
        return (true);
    return (false);
}

std::string RequestLine::getRequestURI() const {
    return (uri);
}

std::string RequestLine::getRequestMethod() const {
    return (method);
}

std::string RequestLine::getRequestParams() const {
    return (params);
}