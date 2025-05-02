# include "../includes/response/Response.hpp"

void    Response::procedRespondProcess( void ) {
    switch (typeOfResponse) {
        case 1:
            respondWithError();
            break ;
        case 2:
            respondByRedirecting();
            break ;
        case 4:
            respondWithDirectoryIndexing();
            break ;
        case 8:
            respondWithRegularResource();
            break;
        case 16:
            respondWithDefaultResponse();
            break;
        case 32:
            respondWithBackEndResponse();
            break ;
        default :
            throw ("Internal Server Error");
    }
}

void    Response::respondByRedirecting( void ) {
    std::string redirectionPhrase;
    
    redirectionPhrase = location.getRedirection().second;
    generatedHeaders  = "HTTP/1.1 " + getStatusPhrase() + "\r\n";
    generatedHeaders += "Server: Mini-Server version 1.0\r\n";
    if ((300 < statusCode && statusCode < 304) || (statusCode == 307) || (statusCode == 308)) {
        generatedHeaders += "Location: " + redirectionPhrase + "\r\n";
        generatedHeaders += "Content-Length: 0\r\n";
        processedData  = generatedHeaders + "\r\n";
    } else {
        generatedHeaders += "Content-Type: text/html; charset=UTF-8\r\n";
        generatedHeaders += "Content-Length: " + size_t_to_string(redirectionPhrase.size()) + "\r\n";
        processedData  = generatedHeaders + "\r\n" + redirectionPhrase;
    }
    serveGeneratedContentToClient();
}

void    Response::respondWithError( void ) {
    std::string defaultErrorPage;
    
    generatedHeaders  = "HTTP/1.1 " + getStatusPhrase() + "\r\n";
    generatedHeaders += "Server: Mini-Server version 1.0\r\n";
    if ((defaultErrorPage = location.getDefaultErrorPage(statusCode)).size()) {
        resourceToServe = location.getRoot() + defaultErrorPage;
        inputFstream.open(resourceToServe.c_str(), std::ios::in);
        if (inputFstream.is_open())
            extractAndServeResourceContent();
        else
            respondWithDefaultResponse();
    } else
        respondWithDefaultResponse();
}

void    Response::respondWithDefaultResponse( void ) {
    generatedHeaders  = "HTTP/1.1 " + getStatusPhrase() + "\r\n";
    generatedHeaders += "Server: Mini-Server version 1.0\r\n";
    generateDefaultPayload();
    generatedHeaders += "Content-Type: text/html; charset=UTF-8\r\n";
    generatedHeaders += "Content-Length: " + size_t_to_string(processedData.size()) + "\r\n";
    processedData  = generatedHeaders + "\r\n" + processedData;
    serveGeneratedContentToClient();
}

void    Response::generateDefaultPayload( void ) {
    processedData  = "<!DOCTYPE html>\n";
    processedData += "<html>\n";
    processedData +=    "<head>\n";
    processedData +=        "<title>" + getStatusPhrase() + "</title>\n";
    processedData +=    "</head>\n";
    processedData +=    "<body>\n";
    processedData +=        "<h1>" + getStatusPhrase() + "</h1>\n";
    processedData +=        "<p>Mini-server version 1.0, default response</p>\n";
    processedData +=    "</body>\n";
    processedData += "</html>\n";
}

void    Response::serveGeneratedContentToClient( void ) {
    ssize_t         writedBytes;
    
    if (!connectionPtr->isRegistered())
        connectionPtr->addToEpollInstance(EPOLLOUT_FLAGS);
    else {
        writedBytes = write(connectionPtr->getSocketFileDescriptor(), processedData.c_str(),processedData.size());
        if (0 < writedBytes) {
            processedData.erase(0, writedBytes);
            processedData.resize(processedData.size());
            currentFunction = &Response::serveGeneratedContentToClient;
        } else if (processedData.size() == 0) {
            connectionPtr->removeFromEpollInstance();
            respondingDone = true;
        }
    }
}

void    Response::respondWithRegularResource( void ) {
    struct stat statBuffer;
    
    generatedHeaders  = "HTTP/1.1 " + getStatusPhrase() + "\r\n";
    generatedHeaders += "Server: Mini-Server version 1.0\r\n";
    if (stat(resourceToServe.c_str(), &statBuffer) == -1)
        throw (404);
    else {
        if (!S_ISREG(statBuffer.st_mode))
            throw (404);
        else {
            generatedHeaders += "Content-Length: " + size_t_to_string(statBuffer.st_size) + "\r\n";
            inputFstream.open(resourceToServe.c_str(), std::ios::in);
            if (!inputFstream.is_open())
                throw (500);
            else {
                sessionManagementCookies();
                processedData = generatedHeaders + "\r\n";
                extractAndServeResourceContent();
            }
        }
    }
}

void    Response::respondWithDirectoryIndexing( void ) {
    struct stat statBuffer;
    
    generatedHeaders  = "HTTP/1.1 " + getStatusPhrase() + "\r\n";
    generatedHeaders += "Server: Mini-Server version 1.0\r\n";
    if (stat(resourceToServe.c_str(), &statBuffer) == -1)
        throw (404);
    else {
        if (!S_ISREG(statBuffer.st_mode))
            throw (404);
        else {
            generatedHeaders += "Content-Length: " + size_t_to_string(statBuffer.st_size) + "\r\n";
            inputFstream.open(resourceToServe.c_str(), std::ios::in);
            if (!inputFstream.is_open())
                throw (500);
            else {
                processedData = generatedHeaders + "\r\n";
                extractAndServeResourceContent();
            }
        }
    }
}

void    Response::respondWithBackEndResponse( void ) {
    struct stat     statBuffer;

    if (stat(backEndResponse.c_str(), &statBuffer) == -1)
        throw (500);
    else {
        if (!statBuffer.st_size)
            throw (500);
        else {
            inputFstream.open(backEndResponse.c_str(), std::ios::in);
            if (!inputFstream.is_open())
                throw (500);
            else {
                responseSize = statBuffer.st_size;
                extractBackEndHeaderSection();
            }
        }
    }
}

void    Response::extractBackEndHeaderSection( void ) {
    size_t pos;
    char buffer[8000];
    std::string headerSection;
    std::streamsize readedBytes;
    
    inputFstream.read(buffer, 8000);
    if ((readedBytes = inputFstream.gcount()) < 0)
        throw (500);
    else {
        if ((readedBytes == 0) || (1600 < processedData.size()))
            convertBackEndHeadersToHTTPHeaders();
        else {
            processedData.append(buffer, readedBytes);
            if ((pos = processedData.find("\r\n\r\n")) == std::string::npos)
                currentFunction = &Response::extractBackEndHeaderSection;
            else {
                headerSection = processedData.substr(0, (pos + 2));
                responseSize  = responseSize - (headerSection.size() + 2);
                processedData.erase(0, (pos + 4));
                extractBackEndHeaders(headerSection);
            }
        }
    }
}

void    Response::extractBackEndHeaders(std::string& headerSection) {
    size_t pos;
    std::vector<std::string> list;
    std::pair<std::string, std::string> tmp;

    while (true) {
        if ((pos = headerSection.find("\r\n")) == std::string::npos)
            break ;
        else {
            list = tokenizer(headerSection.substr(0, pos), ":");
            headerSection.erase(0, (pos + 2));
            if (list.size() == 2) {
                tmp.first = toLowerCaseString(list.front());
                tmp.second = trimString(list.back(), " ");
                backEndHeaders.insert(tmp);
            }
        }
    }
    convertBackEndHeadersToHTTPHeaders();
}

void    Response::convertBackEndHeadersToHTTPHeaders( void ) {
    std::map<std::string, std::string>::iterator it, ed;

    if ((it = backEndHeaders.find("status")) != backEndHeaders.end())
        generatedHeaders = "HTTP/1.1 " + it->second + "\r\n";
    else
        generatedHeaders = "HTTP/1.1 " + getStatusPhrase() + "\r\n";
    if ((it = backEndHeaders.find("server")) == backEndHeaders.end())
        generatedHeaders += "Server: Mini-Server version 1.0\r\n";
    generatedHeaders += "Content-length: " + size_t_to_string(responseSize) + "\r\n";
    it = backEndHeaders.begin(), ed = backEndHeaders.end();
    while (it != ed) {
        if ((it->first != "status") && (it->first != "content-length"))
            generatedHeaders += it->first + ": " + it->second + "\r\n";
        it++;
    }
    processedData = generatedHeaders + "\r\n" + processedData;
    extractAndServeResourceContent();
}

void    Response::extractAndServeResourceContent( void ) {
    std::streamsize     readedBytes;
    char                buffer[80000];
    
    if (!connectionPtr->isRegistered()) {
        connectionPtr->addToEpollInstance(EPOLLOUT_FLAGS);
        currentFunction = &Response::extractAndServeResourceContent;
    } else {
        inputFstream.read(buffer, 80000);
        readedBytes = inputFstream.gcount();
        if (readedBytes < 0)
            throw ("!!!stop responding!!!!");
        else if ((0 < readedBytes) || processedData.size()) {
            processedData.append(buffer, readedBytes);
            serveExtractedDataToClient();
        } else {
            connectionPtr->removeFromEpollInstance();
            respondingDone = true;
        }
    }
}

void    Response::serveExtractedDataToClient( void ) {
    ssize_t         writedBytes;
    
    writedBytes = write(connectionPtr->getSocketFileDescriptor(), processedData.c_str(), processedData.size());
    if (0 < writedBytes) {
        processedData.erase(0, writedBytes);
        if (processedData.size())
            currentFunction = &Response::serveExtractedDataToClient;
        else
            currentFunction = &Response::extractAndServeResourceContent;
        processedData.resize(processedData.size());
    }
}

void    Response::sessionManagementCookies( void ) {
    std::set<std::vector<std::string> > cookies;
    std::set<std::vector<std::string> >::iterator it, ed;

    if (location.isThereAnyCookie()) {
        if (requestHeaders.find("cookie") != requestHeaders.end())
            return ;
        else {
            cookies = location.getCookies();
            it = cookies.begin(), ed = cookies.end();
            while (it != ed) {
                generatedHeaders += "Set-Cookie: " + cookieToHeaderFieldValue(*it) + "\r\n";
                it++;
            }
        }
    } 
}


std::string Response::cookieToHeaderFieldValue(std::vector<std::string> cookie) {
    int sessionId;
    std::string name, result;
    std::vector<std::string>::iterator it, ed;

    if (!cookie.size())
        return (result);
    else {
        if ((name = getCookieName(cookie.front())) == "sessionId") {
            sessionId = connectionPtr->generateUserSessionId();
            cookie.front() = name + "=" + size_t_to_string(size_t(sessionId));
        }
        it = cookie.begin(), ed = cookie.end();
        while (it != ed) {
            result += trimString(*it, " ") + "; ";
            it++;
        }
    }
    return (result);
}

void    Response::resetResponseAttribute( void ) {
    generatedHeaders.clear();
    inputFstream.close();
    currentFunction = &Response::procedRespondProcess;
}

void    Response::operator()(Connection* connection) {
    try {
        connectionPtr = connection;
        (this->*currentFunction)();
        connectionPtr->setLastActivityTime();
    } catch (int code) {
        resetResponseAttribute();
        typeOfResponse = RES_WITH_ERROR;
        statusCode = code;
    } catch (...) {
        typeOfResponse = RES_WITH_ERROR;
        respondingDone = true;
    }
}

bool    Response::isDone( void ) {
    return (respondingDone);
}

void    Response::initiateStatusCodeList( void ) {
    std::string statusCodes;
    std::vector<std::string> list;
    std::vector<std::string>::const_iterator it, ed;
    
    statusCodes += "200 OK,201 Created,204 No Content,205 Reset Content,";
    statusCodes += "300 Multiple Choices,301 Moved Permanently,302 Found, 303 See Others,307 temporary Redirect,308 Permanent Redirect,";
    statusCodes += "400 Bad Request,404 Not Found,405 Method Not Allowed,406 Not Acceptable,408 Request Timeout,411 Length Required,413 Payload Too Large,414 URI Too Long,415 Unsupported Media Type,";
    statusCodes += "500 Internal Server Error,501 Not Implemented,503 Service Unavailable,505 HTTP Version Not Supported";
    (list = tokenizer(statusCodes, ","), it = list.begin(), ed = list.end());
    while (it != ed) {
        statusCodeList.insert(std::pair<short, std::string>(short(string_to_size_t(*it)), *it));
        it++;
    }
}

std::string Response::getStatusPhrase( void ) {
    std::map<short, std::string>::const_iterator it;

    if ((it = statusCodeList.find(statusCode)) == statusCodeList.end())
        throw (500);
    return (it->second);
}

Response::Response(const Request& request): Request(request), respondingDone(false) {
    initiateStatusCodeList();
    currentFunction         = &Response::procedRespondProcess;
}

Response::~Response() {
    // if (typeOfResponse == RES_WITH_BACKE)
    //     remove(backEndResponse.c_str());
}
