# include "../includes/request/MultipartParser.hpp"

void    MultipartParser::removeBoundaryWord( void ) {
    std::string     lastBoundaryWord = boundaryWord + "--";

    currentFunction = &MultipartParser::removeBoundaryWord;
    if ((boundaryWord.size() + 2) <= multipartBuffer.size()) {
        if (lastBoundaryWord == multipartBuffer.substr(0, lastBoundaryWord.size()))
            parsingDone = true;
        else if (boundaryWord == multipartBuffer.substr(0, boundaryWord.size())) {
            multipartBuffer.erase(0, boundaryWord.size());
            currentPartHeaders.clear();
            partHeaders.clear();
            extractPartHeaders();
        } else
            throw (400);
    }
}

void    MultipartParser::extractPartHeaders( void ) {
    size_t      pos;

    currentFunction = &MultipartParser::extractPartHeaders;
    if ((pos = multipartBuffer.find("\r\n\r\n", 0)) != std::string::npos) {
        partHeaders = multipartBuffer.substr(2, pos);
        multipartBuffer.erase(0, (pos + 4));
        parsePartHeaders();
        openPartDestinationFile();
        extractPartContent();
    }
}

void    MultipartParser::parsePartHeaders( void ) {
    std::vector<std::string> headerfields;
    std::vector<std::string>::iterator it, ed;

    headerfields = tokenizer(partHeaders, "\n");
    if (!headerfields.size())
        throw (400);
    else {
        it = headerfields.begin(), ed = headerfields.end();
        while (it != ed) {
            trimString(*it, "\r");
            parseSingleHeaderFiled(*it);
            it++;
        }
    }
}

void    MultipartParser::parseSingleHeaderFiled(std::string& headerField) {
    std::vector<std::string> keyValue;
    std::pair<std::string, std::string> element;
    std::map<std::string, std::string>::iterator key;

    keyValue = tokenizer(headerField, ":");
    if (keyValue.size() != 2)
        throw (400);
    else if ((key = currentPartHeaders.find(keyValue.front())) != currentPartHeaders.end())
        throw (400);
    else {
        trimString(keyValue.back(), "; ");
        element = std::pair<std::string, std::string>(keyValue.front(), keyValue.back());
        currentPartHeaders.insert(element);
    }
}

void    MultipartParser::openPartDestinationFile( void ) {
    std::vector<std::string> list;
    std::vector<std::string> values, parameters;
    std::map<std::string, std::string>::iterator it;

    if ((it = currentPartHeaders.find("Content-Disposition")) != currentPartHeaders.end()) {
        values = getValues(currentPartHeaders, "Content-Disposition");
        if ((values.size() != 1) || (*values.begin() != "form-data"))
            throw (400);
        else {
            parameters = getParameters(currentPartHeaders, "Content-Disposition");
            if ((parameters.size() != 2))
                throw (404);
            else {
                list = tokenizer(parameters.front(), "=");
                if ((list.size() != 2) || (list.front() != "name"))
                    throw (400);
                list = tokenizer(parameters.back(), "=");
                if ((list.size() != 2) || (list.front()!= "filename"))
                    throw (400);
            }
            currentPartStream.open(trimString(list.back(), "\" ").c_str(), std::ios::out);
            if (!currentPartStream.is_open())
                throw (500);
        }
    } else
        throw (400);
}

void    MultipartParser::extractPartContent( void ) {
    size_t      pos, toStay;
    std::string extractedData;

    currentFunction = &MultipartParser::extractPartContent;
    if ((pos = multipartBuffer.find(boundaryWord, 0)) != std::string::npos) {
        extractedData = multipartBuffer.substr(0, (pos - 2));
        multipartBuffer.erase(0, pos);
        currentPartStream << extractedData;
        if (currentPartStream.fail())
            throw (500);
        currentPartStream.close();
        removeBoundaryWord();
    } else if ((toStay = (boundaryWord.size() + 2)) < multipartBuffer.size()) {
        toStay = multipartBuffer.size() - toStay;
        extractedData = multipartBuffer.substr(0, toStay);
        multipartBuffer.erase(0, toStay);
        currentPartStream << extractedData;
        if (currentPartStream.fail())
            throw (500);
    }
}

void    MultipartParser::operator()( void ) {
    char        buffer[800000];
    ssize_t     readedBytes;

    readedBytes = read(requestBodyFd, buffer, 800000);
    if (0 < readedBytes) {
        multipartBuffer.append(buffer, readedBytes);
        (this->*currentFunction)();
    } else if (!readedBytes && !parsingDone)
        throw (400);
    else
        throw (500);
}

bool    MultipartParser::isDone( void ) {
    return (parsingDone);
}

MultipartParser::MultipartParser(std::string& boundaryArg, std::string request_body) {
    boundaryWord = boundaryArg;
    parsingDone = false;
    requestBodyFd = open(request_body.c_str(), O_RDONLY);
    if (requestBodyFd < 0)
        throw (500);
    currentFunction = &MultipartParser::removeBoundaryWord;
}

MultipartParser::~MultipartParser( void ) {
    close (requestBodyFd);
}