/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 16:30:56 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/23 16:49:26 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef utilities_HPP
# define utilities_HPP

# include "extern.hpp"
# include "parser/ParserExcep.hpp"

std::vector<std::string>    getParameters(std::map<std::string, std::string> headers, std::string key);
size_t                      string_to_size_t(const std::string& str);
size_t                      hexa_string_to_size_t(const std::string& str);
void                        displaySocketParameter(const sockaddr* socket);
std::string                 getUnit(const std::string& str);
bool                        parseLine(std::string& line, size_t l_nbr);
short                       toCode(const char* code);
std::string                 getCookieName(const std::string& cookie);
void                        initiatHints(addrinfo& hints);
bool                        isAbsolPath(const char* path);
bool                        isPortNumber(std::string& str);
std::string                 size_t_to_string(size_t value);
size_t                      getCurrentTime( void );
std::string&                replaceSetOfCharacters(std::string& str, std::string set, char byThis);
std::string                 getCookieValue(const std::string& fieldValues, std::string name);
bool                        isAnHttpDelimiter(char character);
std::string                 getFileExtention(const std::string& file);
std::vector<std::string>    tokenizer(const std::string& str, const std::string set);
bool                        socketComparer(const sockaddr* a, const sockaddr* b);
size_t                      convertTobyte(const std::string& unit, size_t size);
std::string&                trimString(std::string& str, const std::string& set);
std::string&                trimFromEnd(std::string& str, const std::string& set);
std::string&                trimFromstart(std::string& str, const std::string& set);
std::string                 toLowerCaseString(const std::string &str);
std::string                 toUpperCaseString(const std::string &str);
std::string                 generateRandomName(std::string path);
std::vector<std::string>    getValues(std::map<std::string, std::string> headers, std::string key);


# endif