/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 13:39:52 by bramzil           #+#    #+#             */
/*   Updated: 2025/02/14 19:06:37 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef HTTPPARSER_HPP
# define HTTPPARSER_HPP

# include "ServParser.hpp"
# include "../extern.hpp"
# include "../core/Socket.hpp"

class HttpParser: public ServParser {
    
    public:
        typedef void                    (HttpParser::*parserPtr)(const std::string&, size_t&);
    
        void                            displaySockets( void ) const;
        void                            parseServer(const std::string& str, size_t& l_nbr);
        void                            addServer(const ServParser& svr, size_t& l_nbr);
        void                            addSocketToList(const sockaddr* sock);
        void                            addServerToSocket(const ServParser& srv, const sockaddr* sock, size_t& l_nbr);

        void                            initiateAttribute( void );
        void                            operator()( void );

        HttpParser(std::vector<Socket>& socketList, std::fstream& is);
        // HttpParser(const HttpParser& x);
        // HttpParser&                     operator=(const HttpParser& x);
        virtual ~HttpParser();

    private:
        std::vector<Socket>&             socketLst;
        std::map<std::string, parserPtr> parserList;
};

# endif