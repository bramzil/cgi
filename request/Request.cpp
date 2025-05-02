/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:39:48 by abbaraka          #+#    #+#             */
/*   Updated: 2025/04/26 01:50:03 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utilities.hpp"
#include "../includes/request/Request.hpp"

Request::Request(const Socket& sock) : socket(sock), maxCharHeaders(16000) {
    typeOfResponse  = RES_UNSPECIFIE;
    connectionPtr   = NULL;
    moreData        = false;
    headersDone     = false;
    formData        = false;
}

Request::Request(const Request& x): moreData(false) {
    connectionPtr   = NULL;
    *this = x;
}

Request& Request::operator=(const Request& x) {
    if (this != &x) {
        socket                  = x.socket;
        server                  = x.server;
        location                = x.location;

        headersDone             = x.headersDone;
        requestLine             = x.requestLine;
        host                    = x.host;
        requestHeaders          = x.requestHeaders;

        formData                = x.formData;
        boundary                = x.boundary;

        statusCode              = x.statusCode;
        typeOfResponse          = x.typeOfResponse;
        backEndResponse         = x.backEndResponse;
        resourceToServe         = x.resourceToServe;
    }
    return (*this);
}

bool Request::isDone() {
    if (headersDone)
        return (true);
    return (false);
}

void    Request::getMoreDataOfRequestBody( void ) {
    char    buffer[8000];
    ssize_t readedBytes;

    if (!connectionPtr->isRegistered())
        connectionPtr->addToEpollInstance(EPOLLIN_FLAGS);
    else {
        readedBytes = read(connectionPtr->getSocketFileDescriptor(), buffer, 8000);
        if (0 < readedBytes) {
            connectionPtr->unprocessedData.resize(connectionPtr->unprocessedData.size());
            connectionPtr->unprocessedData.append(buffer, readedBytes);
            moreData = false;
            connectionPtr->removeFromEpollInstance();
        }
    }
}

short   Request::gettypeOfResponse( void ) const {
    return (typeOfResponse);
}

std::string Request::getHeaderValue(std::string name) const {
    std::map<std::string, std::string>::const_iterator it;

    if ((it = requestHeaders.find(name)) != requestHeaders.end())
        return (it->second);
    return ("");
}

void Request::operator()(Connection *connection) {
    try {
        connectionPtr = connection;
        if (EVENTTIMEOUT < (getCurrentTime() - connectionPtr->getLastActivityTime()))
            throw (408);
        else {
            if (moreData)
                getMoreDataOfRequestBody();
            else if (!moreData)
                parseRequest();
            connectionPtr->setLastActivityTime();
        }
    } catch (int code) {
        typeOfResponse = RES_WITH_ERROR;
        statusCode = code;
        headersDone = true;
    } catch (...) {
        typeOfResponse = RES_WITH_ERROR;
        statusCode = 500;
        headersDone = true;
    }
}

Request::~Request() {}

bool containsWhitespaceOrNotAlpha(const std::string& str) {
    for (size_t i = 0; i < str.size(); ++i) {
        if (isspace(str[i]) || (str[i] != '-' && !(str[i] >= 'a' && str[i] <= 'z')))
            return (true);
    }
    return (false);
}

std::string &stringTrimer(std::string &str, const std::string &set) {
    std::string::iterator it, ed;

    it = str.begin(), ed = str.end();
    while (it != ed) {
        if (set.find(*it) != std::string::npos)
            (str.erase(it), it = str.begin(), ed = str.end());
        else if (set.find(*(ed - 1)) != std::string::npos)
            (str.erase(ed - 1), it = str.begin(), ed = str.end());
        else
            break;
    }
    return (str);
}

void Request::setHost() {
    for (std::map<std::string, std::string>::iterator it = requestHeaders.begin(); it != requestHeaders.end(); it++) {
        if (it->first == "host") {
            size_t colon_pos = it->second.find(":");
            if (colon_pos != std::string::npos) {
                host.address = it->second.substr(0, colon_pos);
                host.port = atoi(it->second.substr(colon_pos + 1).c_str());
            }
            else
                host.address = it->second;
        }
    }
}

void Request::parseRequestLine(std::string line) {
    requestLine.setRequestLine(line);
}

void Request::checkDuplicatedHeader(std::string &header)
{
    std::string criticalHeaders[4] = {"content-length", "transfer-encoding", "host", "authorization"};
    for (size_t i = 0; i < 4; i++) {
        if (header == criticalHeaders[i]) {
            for (std::map<std::string, std::string>::iterator it = requestHeaders.begin(); it != requestHeaders.end(); it++) {
                if (it->first == header)
                    throw (400);
            }
        }
    }
}

void Request::setContentType(std::string &value)
{
    std::vector<std::string> list;

    list = tokenizer(value, "; ");
    if (!list.size())
        throw(400);
    else if (list.size() == 2 && list.front() == "multipart/form-data") {
        formData = true;
        list = tokenizer(list.back(), "=");
        list.front() = toLowerCaseString(list.front());
        if (list.size() == 2 && list.front() == "boundary")
        {
            std::string set = "\"";
            boundary = "--" + stringTrimer(list.back(), set);
        }
    }
}

void	Request::parseHeaders(std::string line) {
	size_t colon_pos = line.find(":");

    maxCharHeaders -= line.size();
	if (colon_pos != std::string::npos) {
		std::string key = toLowerCaseString(line.substr(0, colon_pos));
        if (containsWhitespaceOrNotAlpha(key))
            throw (400);
        checkDuplicatedHeader(key);
		std::string value = line.substr(colon_pos + 1);
            
		value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find(CRLF));
        trimString(value, " \t");
        if (key == "transfer-encoding")
            value = toLowerCaseString(value);
        if (key == "content-type") {
            value = toLowerCaseString(value);
            setContentType(value);
        }
        if (maxCharHeaders < 0 || line.size() > 8000)
            throw (400);
		requestHeaders[key] = value;
	} else
        throw (400);
}

void Request::parseRequest() {
    std::string line;

    while (!headersDone) {
        if (!headersDone) {
            size_t crlf_pos = connectionPtr->unprocessedData.find(CRLF);
            if (crlf_pos == std::string::npos) {
                moreData = true;
                return;
            }
            line = connectionPtr->unprocessedData.substr(0, crlf_pos + 2);
            connectionPtr->unprocessedData.erase(0, crlf_pos + 2);
            if (!requestLine.checkIfInit())
                parseRequestLine(line);
            else if (!line.empty() && line == CRLF && !requestHeaders.empty()) {
                headersDone = true;
                requestHeaders["method"] = requestLine.getRequestMethod();
                requestHeaders["target"] = requestLine.getRequestURI();
                requestHeaders["query-string"] = requestLine.getRequestParams();
                if (requestHeaders.find("connection") == requestHeaders.end())
                    requestHeaders["connection"] = "keep-alive";
                setHost();
                if (host.address.empty())
                    throw(400);
            } else
                parseHeaders(line);
        }
    }
}
