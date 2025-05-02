/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 22:30:40 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/23 16:43:11 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "includes/utilities.hpp"

void initiatHints(addrinfo& hints) {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;
}

size_t  getCurrentTime( void ) {
    timeval         time;
    
    gettimeofday(&time, NULL);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

std::string getFileExtention(const std::string& file) {
    size_t pos;

    if ((pos = file.rfind('.', (file.size() - 1))) != std::string::npos)
        return (file.substr(pos));
    return (std::string(""));
}

/*the following section for header section parsing functions*/

bool    isAbsolPath(const char* path) {
    if (!path || (*path != '/'))
        return (false);
    path++;
    while (*path) {
        if ((*path == '/') && (*(path - 1) == '/'))
            return (false);
        path++;
    }
    return (true);
}

std::vector<std::string> getValues(std::map<std::string, std::string> headers, std::string key) {
    size_t pos;
    std::string section;
    std::vector<std::string> values;
    std::map<std::string, std::string>::const_iterator  it;

    if (((it = headers.find(key)) != headers.end())) {
        if ((pos = it->second.find(';')) != std::string::npos)
            section = it->second.substr(0, pos);
        else
            section = it->second;
        values = tokenizer(section, ", ");
    }
    return (values);
}

std::vector<std::string> getParameters(std::map<std::string, std::string> headers, std::string key) {
    size_t pos;
    std::string section;
    std::vector<std::string> parameters;
    std::map<std::string, std::string>::const_iterator  it;

    if (((it = headers.find(key)) != headers.end())) {
        if ((pos = it->second.find(';')) != std::string::npos) {
            section = it->second.substr((pos + 1));
            parameters = tokenizer(section, "; ");
        }
    }
    return (parameters);
}

/*the following section for socket address (sockaddr) functions*/


bool    isPortNumber(std::string& str) {
    std::string::iterator it, ed;

    it = str.begin(), ed = str.end();
    while (it != ed) {
        if (!std::isdigit(*it))
            return (false);
        it++;
    }
    return (true);
}

void    displaySocketParameter(const sockaddr* socket) {
    sockaddr_in *in_sock;
    sockaddr_in6 *in6_sock;
    char buf[INET6_ADDRSTRLEN];

    if(socket->sa_family == AF_INET) {
        in_sock = (sockaddr_in*) socket;
        inet_ntop(AF_INET, &in_sock->sin_addr, buf, INET6_ADDRSTRLEN);
        std::cout << " socket port: " << ntohs(in_sock->sin_port);
    }
    else if (socket->sa_family == AF_INET6) {
        in6_sock = (sockaddr_in6*) socket;
        inet_ntop(AF_INET6, &in6_sock->sin6_addr, buf, INET6_ADDRSTRLEN);
        std::cout << " socket port: " << ntohs(in6_sock->sin6_port);
    }
    std::cout << " socket address: " << buf << '\n';
}


bool    socketComparer(const sockaddr* a, const sockaddr* b) {
    int i = 0;

    if (a->sa_family != b->sa_family)
        return (false);
    else if (a->sa_family == AF_INET) {
        sockaddr_in *ptr1, *ptr2;
        ptr1 = (sockaddr_in*) a, ptr2 = (sockaddr_in*) b;
        if (ptr1->sin_addr.s_addr != ptr2->sin_addr.s_addr)
            return (false);
        else if (ptr1->sin_port != ptr2->sin_port)
            return (false);
    } else if (a->sa_family == AF_INET6) {
        sockaddr_in6 *ptr1, *ptr2;
        ptr1 = (sockaddr_in6*) a, ptr2 = (sockaddr_in6*) b;
        if (ptr1->sin6_port != ptr2->sin6_port)
            return (false);
        while (i < 16) {
            if (ptr1->sin6_addr.s6_addr[i] != ptr2->sin6_addr.s6_addr[i])
                return (false);
            i++;
        }
    } else
        return (false);
    return (true);
}

/*the following section for converting between numeric and string functions*/

std::string size_t_to_string(size_t value) {
    std::string result;
    std::stringstream iss;

    (iss << value, iss >> result);
    if (iss.fail())
        throw (500);
    return (result);
}

size_t  string_to_size_t(const std::string& str) {
    size_t  result;
    std::stringstream iss;

    (iss << str, iss >> result);
    if (iss.fail())
        throw (500);
    return (result);
}

size_t  hexa_string_to_size_t(const std::string& str) {
    size_t  result;
    std::stringstream iss;

    (iss << std::hex << str, iss >> result);
    if (iss.fail())
        throw (400);
    return (result);
}

size_t  convertTobyte(const std::string& unit, size_t size) {
    size_t      result;

    if ((unit == "k") || (unit == "kb"))
        result = size * 1000;
    else if ((unit == "m") || (unit == "mb"))
        result = size * 1000000;
    else if ((unit == "g") || (unit == "gb"))
        result = size * 1000000000;
    else
        result = size;
    return (result);
}

/*the following section for string functions*/

std::string toLowerCaseString(const std::string& str) {
    std::string result = str;
    std::string::iterator it, ed;

    it = result.begin(), ed = result.end();
    while (it != ed) {
        if (isupper(*it))
            *it = (*it + 32);
        it++;
    }
    return (result);
}

std::string toUpperCaseString(const std::string &str) {
    std::string lower = str;
    for (size_t i = 0; i < str.size(); i++) {
        lower[i] = toupper(str[i]);
    }
    return (lower);
}

std::string generateRandomName(std::string path) {
    static bool initialized = false;
    if (!initialized) {
        std::srand(static_cast<unsigned int>(std::time(0)));
        initialized = true;
    }

    int randomNumber = std::rand();

    std::ostringstream oss;
    oss << "Name_" << randomNumber;
    std::string pathname = path + oss.str();
    struct stat buf;
    if (stat((path).c_str(), &buf) == -1)
        throw (404);
    while (stat((pathname).c_str(), &buf) != -1) {
        oss.clear();
        randomNumber = std::rand();
        oss << "Name_" << randomNumber;
        pathname = path + oss.str();
    }
    return (pathname);
}

std::string&    trimFromEnd(std::string& str, const std::string& set) {
    std::string::iterator it, ed;

    it = str.begin(), ed = str.end();
    while (it != ed) {
        if (set.find(*(ed - 1)) != std::string::npos)
            str.erase((ed - 1));
        else
            break ;
        ed--;
    }
    return (str);
}

std::string&    trimFromStart(std::string& str, const std::string& set) {
    std::string::iterator it, ed;

    it = str.begin(), ed = str.end();
    while (it != ed) {
        if (set.find(*it) != std::string::npos)
            str.erase(it);
        else
            break ;
        it++;
    }
    return (str);
}

std::string&    trimString(std::string& str, const std::string& set) {
    trimFromStart(str, set), trimFromEnd(str, set);
    return (str);
}

std::string&    replaceSetOfCharacters(std::string& str, std::string set, char byThis) {
    size_t  current = 0, size = str.size();

    while (current < size) {
        if (set.find(str[current]) != std::string::npos)
            str[current] = byThis;
        current++;
    }
    return (str);
}

/*the following section for tokenizer functions*/

static char getCloser(char opener) {
    std::map<char, char> groupers;
    std::map<char, char>::iterator it;

    groupers.insert(std::pair<char, char>( '"', '"' ));
    groupers.insert(std::pair<char, char>( '(', ')' ));
    groupers.insert(std::pair<char, char>( '[', ']' ));
    groupers.insert(std::pair<char, char>('\'', '\''));
    if ((it = groupers.find(opener)) == groupers.end())
        throw ("Unsupported groupers from getCloser");
    return (it->second);
}

static std::string extractToken(const std::string& str, std::string& set, size_t& current) {
    char closer;
    bool flag = false;
    size_t size = str.size(), begin = current;
    std::string openers("'\"(["), token;

    while (current < size) {
        if (flag && (str[current] == closer))
            flag = false;
        else if (!flag && (openers.find(str[current]) != std::string::npos)) {
            closer = getCloser(str[current]);
            flag = true;
        } else if (!flag && (set.find(str[current]) != std::string::npos))
            break ;
        current++;
    }
    return (str.substr(begin, (current - begin)));
}


std::vector<std::string> tokenizer(const std::string& str, std::string set) {
    std::string token;
    std::vector<std::string> result;
    size_t current = 0, size = str.size();
    
    while (current < size) {
        if (set.find(str[current]) == std::string::npos) {
            token = extractToken(str, set, current);
            result.push_back(token);
        }
        current++;
    }
    return(result);
}


/*the following section for line parser functions*/


static bool isJustAllowedCharacters(const std::string& line) {
    std::string::const_iterator it, ed;

    it = line.begin(), ed = line.end();
    while (it != ed) {
        if (!std::isprint(*it))
            return (false);
        it++;
    }
    return (true);
}

bool parseLine(std::string& line, size_t l_nbr) {
    size_t pos;
    std::vector<std::string> list;

    if ((pos = line.find('#', 0)) != std::string::npos)
        line = line.substr(0, pos);
    if (line.size()) {
        if (!isJustAllowedCharacters(line))
            throw (ParserExcep("line", "accept just printable characters", l_nbr));
        else {
            list = tokenizer(replaceSetOfCharacters(line, "\t", ' '), " ");
            if (!list.size())
                return (true);
            else if ((list.back() != ";") && (list.back() != "{") && (list.back() != "}"))
                throw (ParserExcep("line", "Not ended correctly", l_nbr));
        }
    }
    return (true);
}

/*the following section is for cookies functions*/

std::string getCookieName(const std::string& cookie) {
    size_t pos;
    std::string name(cookie);

    if ((pos = cookie.find('=')) != std::string::npos)
        name = cookie.substr(0, pos);
    return (name);
}

std::string getCookieValue(const std::string& cookie) {
    size_t pos;
    std::string value(cookie);

    if ((pos = cookie.find('=')) != std::string::npos)
        value = cookie.substr((pos + 1));
    return (value);
}

std::string getCookieValue(const std::string& fieldValues, std::string name) {
    std::string value;
    std::vector<std::string> list;
    std::vector<std::string>::iterator it, ed;

    list = tokenizer(fieldValues, ";");
    if (list.size()) {
        it = list.begin(), ed = list.end();
        while (it != ed) {
            if (getCookieName(*it) == name) {
                value = getCookieValue(*it);
                break ;
            }
            it++;
        }
    }
    return (value);
}