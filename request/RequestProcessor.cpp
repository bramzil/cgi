# include "../includes/request/RequestProcessor.hpp"

void    RequestProcessor::startProcessing( void ) {
    std::string requestMethod(requestLine.getRequestMethod());

    if (!location.getParameter().size())
        throw (404);
    else {
        if (location.isThereAnyRedirection()) {
            typeOfResponse = RES_BY_REDIREC;
            processingDone = true;
        } else if (!location.isAllowedMethod(requestMethod))
            throw (405);
        else{
            if (requestMethod == "GET")
                processGetRequest();
            else if (requestMethod == "POST")
                processPostRequest();
            else if (requestMethod == "DELETE")
                processDeleteRequest();
            else
                throw (501);
        }
    }
}

std::string RequestProcessor::variablesExpander(const std::string& path) const {
    size_t start, end;
    std::string left, right;
    std::string result(path), cookies, variable;
    std::map<std::string, std::string>::const_iterator it;

    start = end = 0;
    if ((it = requestHeaders.find("cookie")) != requestHeaders.end())
        cookies = it->second;
    else
        cookies = getDefaultCookies(location.getCookies());
    while (start != std::string::npos) {
        if ((start = result.find('$', start)) != std::string::npos) {
            left = result.substr(0, start);
            end = result.find('/', (start + 1));
            right = result.substr(end);
            variable = result.substr((start + 1), (end - start - 1));
            result = left + getCookieValue(cookies, variable) + right;
            start = 0;
        }
        left = right = "";
    }
    return (result);
}

std::string RequestProcessor::getDefaultCookies(const std::set<std::vector<std::string> >& defaultCookies) const {
    std::string result;
    std::set<std::vector<std::string> >::const_iterator it, ed;

    it = defaultCookies.begin(), ed = defaultCookies.end();
    while (it != ed) {
        result += it->front() + "; ";
        it++;
    }
    return (result);
}

/*the following section for GET request processing */

void    RequestProcessor::processGetRequest( void ) {
    struct stat statBuffer;
    std::string index(location.getIndex());
    std::string target(requestLine.getRequestURI());
    std::string root(variablesExpander(location.getRoot()));
    
    if (target == location.getParameter())
        resourceToServe = root + target + index;
    else
        resourceToServe = root + target;
    if (stat(resourceToServe.c_str(), &statBuffer) == -1)
        throw (404);
    else {
        if (location.isThereAnyBackendApplication()) {
            if (!S_ISREG(statBuffer.st_mode))
                throw (404);
            else
                cgiInstantiator();
        } else {
            if (S_ISREG(statBuffer.st_mode)) {
                typeOfResponse = RES_WITH_REGUL;
                statusCode = 200;
                processingDone = true;
            } else if (S_ISDIR(statBuffer.st_mode))
                generateDirectoryHTMLIndexing();
            else
                throw (404);
        }
    }
}

void    RequestProcessor::generateDirectoryHTMLIndexing( void ) {
    DIR *directory;
    std::fstream outStream;

    if ((directory = opendir(resourceToServe.c_str())) == NULL)
        throw (500);
    else {
        outStream.open(resourceToServe.c_str(), std::ios::out);
        if (!outStream.is_open())
            throw (500);
        else {
            generateDirectoryHTMLContent(directory);
            outStream << processedData;
            if (outStream.fail())
                throw (500);
            else {
                typeOfResponse = RES_WITH_DIREC;
                statusCode = 200;
                processingDone = true;
            }
            outStream.close();
        }
        closedir(directory);
    }
}

void    RequestProcessor::generateDirectoryHTMLContent(DIR*  directory) {
    struct dirent *entry;
    std::string resource = requestLine.getRequestURI();
    
    if (resource.at(resource.size() - 1) != '/')
        resource  += "/";
    processedData += "<!DOCTYPE html>\n";
    processedData += "<html>\n";
    processedData +=      "<head>\n";
    processedData +=          "<title>" + resource + "</title>\n";
    processedData +=      "</head>\n";
    processedData +=      "<body>\n";
    processedData +=          "<h2>Index of " + resource + "</h2>\n";
    processedData +=          "<ul>\n";
                                    while ((entry = readdir(directory)))
                                        processedData += std::string("<li><a href=\"") + resource + entry->d_name + "\">" + entry->d_name + "</a></li>\n";
    processedData +=          "</ul>\n";
    processedData +=      "</body>\n";
    processedData += "</html>\n";
}

/*the following section for POST request processing */

void    RequestProcessor::processPostRequest( void ) {
    std::vector<std::string> values;
    std::string target(requestLine.getRequestURI());
    std::map<std::string, std::string>::const_iterator it;
    std::string root(variablesExpander(location.getRoot()));

    requestBodyFile = generateRandomName(root + target);
    requestBodyStream.open(requestBodyFile.c_str(), std::ios::out);
    if (!requestBodyStream.is_open())
        throw (400);
    else if ((it = requestHeaders.find("transfer-encoding")) != requestHeaders.end()) {
        values = getValues(requestHeaders, "transfer-encoding");
        if ((values.size() != 1) || (values.front() != "chunked"))
            throw (400);
        getChunkSize();
    } else if ((it = requestHeaders.find("content-length")) != requestHeaders.end()) {
        contentLength = string_to_size_t(it->second.c_str());
        readRequestBody();
    } else
        throw (400);
}

void   RequestProcessor::getChunkSize( void ) {
    size_t                  pos;
    std::stringstream       iss;
    std::string             hexaString;

    if ((pos = connectionPtr->unprocessedData.find("\r\n")) != std::string::npos) {
        hexaString = connectionPtr->unprocessedData.substr(0, pos);
        connectionPtr->unprocessedData.erase(0, pos + 2);
        if (!(chunkSize = hexa_string_to_size_t(hexaString))) {
            requestBodyStream.close();
            parceRequestBody();
        } else
            extractChunk();
    } else {
        currentFunction = &RequestProcessor::getChunkSize;
        moreData = true;
    }
}

void    RequestProcessor::extractChunk( void ) {
    std::string     extractedData;

    extractedData = connectionPtr->unprocessedData.substr(0, chunkSize);
    connectionPtr->unprocessedData.erase(0, extractedData.size());
    if (extractedData.size()) {
        chunkSize -= extractedData.size();
        requestBodyStream << extractedData;
        if (requestBodyStream.fail())
            throw (500);
        else if (!chunkSize)
            removeChunkLimiter();
    } if (chunkSize) {
        currentFunction = &RequestProcessor::extractChunk;
        moreData = true;
    }
}

void    RequestProcessor::removeChunkLimiter( void ) {
    if (2 <= connectionPtr->unprocessedData.size()) {
        if (connectionPtr->unprocessedData.substr(0, 2) != "\r\n")
            throw (400);
        else
            connectionPtr->unprocessedData.erase(0, 2);
        getChunkSize();
    } else {
        currentFunction = &RequestProcessor::removeChunkLimiter;
        moreData = true;
    }
}

void    RequestProcessor::readRequestBody( void ) {
    std::string     extractedData;

    extractedData = connectionPtr->unprocessedData.substr(0, contentLength);
    connectionPtr->unprocessedData.erase(0, extractedData.size());
    if (extractedData.size()) {
        contentLength -= extractedData.size();
        requestBodyStream << extractedData;
        if (requestBodyStream.fail())
            throw (500);
    } if (contentLength == 0) {
        requestBodyStream.close();
        parceRequestBody();
    } else {
        currentFunction = &RequestProcessor::readRequestBody;
        moreData = true;
    }
}

void    RequestProcessor::parceRequestBody( void ) {
    if (location.isThereAnyBackendApplication())
        cgiInstantiator();
    else if (formData) {
        multipartInstance = new MultipartParser(boundary, requestBodyFile);
        if (!multipartInstance)
            throw (500);
        else
            multipartMonitor();
    } else {
        requestBodyFile.clear();
        typeOfResponse   = RES_WITH_DEFAU;
        statusCode       = 201;
        processingDone   = true;
    }
}

void    RequestProcessor::multipartMonitor( void ) {
    (*multipartInstance)();
    if (!multipartInstance->isDone())
        currentFunction = &RequestProcessor::multipartMonitor;
    else {
        typeOfResponse  = RES_WITH_DEFAU;
        statusCode      = 201;
        processingDone  = true;
    }
}

/*the following section for cgi functions*/

std::string RequestProcessor::extractScriptName( void ) {
    size_t pos;
    std::string result;
    std::string requestUri(requestLine.getRequestURI());

    if ((pos = requestUri.rfind('/')) != std::string::npos)
        return (requestUri.substr((pos + 1)));
    return (requestUri);
}

std::string RequestProcessor::getCGIExecutablePath( void ) {
    std::string extention;
    std::string cgiExecutablePath;

    extention = getFileExtention(requestLine.getRequestURI());
    if (!extention.size())
        throw (404);
    else {
        cgiExecutablePath = location.getCGIApplication(extention);
        if (!cgiExecutablePath.size())
            throw (404);
    }
    return (cgiExecutablePath);
}

void    RequestProcessor::cgiInstantiator( void ) {
    std::vector<std::string> cgiRequirementList;
    std::string parameter(location.getParameter());
    std::string root(variablesExpander(location.getRoot()));

    backEndResponse = generateRandomName(root + parameter);

    cgiRequirementList.push_back(getCGIExecutablePath());
    cgiRequirementList.push_back(extractScriptName());
    cgiRequirementList.push_back(root + parameter);
    cgiRequirementList.push_back(backEndResponse);

    if (requestLine.getRequestMethod() == "POST")
        cgiRequirementList.push_back(requestBodyFile);

    cgiApplicationInstance = new CgiModule(cgiRequirementList);
    if (cgiApplicationInstance) {
        cgiApplicationInstance->setCgiParamters(requestHeaders);
        cgiApplicationInstance->createChildProcess();
        cgiMonitor();
    }
    else
        throw (500);
}

void    RequestProcessor::cgiMonitor( void ) {
    if (cgiApplicationInstance->isDone()) {
        typeOfResponse  = RES_WITH_BACKE;
        statusCode      = 200;
        processingDone  = true;
    } else
        currentFunction = &RequestProcessor::cgiMonitor;
}


/*the following section for class special functions*/

void    RequestProcessor::operator() (Connection* connection) {
    try {
        connectionPtr = connection;
        if (EVENTTIMEOUT < (getCurrentTime() - connectionPtr->getLastActivityTime()))
            throw (408);
        else {
            if (moreData)
                getMoreDataOfRequestBody();
            else if (!moreData)
                (this->*currentFunction)();
            connectionPtr->setLastActivityTime();
        }
    } catch (int code) {
        typeOfResponse = RES_WITH_ERROR;
        statusCode = code;
        processingDone = true;
    } catch (...) {
        typeOfResponse = RES_WITH_ERROR;
        statusCode = 500;
        processingDone = true;
    }
}

bool    RequestProcessor::isDone( void ) {
    return (processingDone);
}

RequestProcessor::RequestProcessor(Request& request) : Request(request) {
    std::map<std::string, std::string>::const_iterator it;
    
    (cgiApplicationInstance = NULL, multipartInstance = NULL);
    (processingDone = false, chunkSize = 0);
    if ((it = requestHeaders.find("host")) == requestHeaders.end())
        throw ("request misparser");
    server = socket.getServer(it->second);
    if ((it = requestHeaders.find("target")) == requestHeaders.end())
        throw ("request misparser");
    location = server.getLocation(it->second);
    currentFunction = &RequestProcessor::startProcessing;
}

RequestProcessor::~RequestProcessor() {
    delete cgiApplicationInstance;
    delete multipartInstance;
    remove(requestBodyFile.c_str());
}

/*the following section for DELETE request processing */

void    RequestProcessor::deleteFile(const std::string& filePath) {
    if (remove(filePath.c_str()) == 0)
        std::cout << "Deleted file: " << filePath << std::endl;
    else
        std::cerr << "Error deleting file: " << filePath << std::endl;
    processingDone = true;
}

void    RequestProcessor::deleteDirectory(const std::string& dirPath) {
    DIR* dir = opendir(dirPath.c_str());
    if (dir == NULL) {
        std::cerr << "Error opening directory: " << dirPath << std::endl;
        throw (500);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string fileName = entry->d_name;

        if (fileName == "." || fileName == "..")
            continue;

        std::string fullPath = dirPath + "/" + fileName;

        if (entry->d_type == DT_DIR)
            deleteDirectory(fullPath);
        else if (entry->d_type == DT_REG)
            deleteFile(fullPath);
    }

    closedir(dir);
    processingDone = true;
    if (rmdir(dirPath.c_str()) == 0)
        std::cout << "Deleted directory: " << dirPath << std::endl;
    else {
        std::cerr << "Error deleting directory: " << dirPath << std::endl;
        throw (500);
    }
}

void    RequestProcessor::deleteResource(const std::string& path) {
    struct stat statBuf;
    if (stat(path.c_str(), &statBuf) != 0) {
        std::cerr << "Error accessing resource: " << path << std::endl;
        throw (400);
    }

    if (S_ISDIR(statBuf.st_mode))
        deleteDirectory(path);
    else if (S_ISREG(statBuf.st_mode))
        deleteFile(path);
}

void    RequestProcessor::processDeleteRequest( void ) {
    std::cout << location.getRoot() + requestLine.getRequestURI() << std::endl;
    if (requestLine.getRequestURI() == "/")
        throw (403);
    deleteResource(location.getRoot() + requestLine.getRequestURI());
    processingDone = true;
}