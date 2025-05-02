/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 21:56:51 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/23 16:47:15 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef SERVPARSER_HPP
# define SERVPARSER_HPP

# include "LocParser.hpp"
# include "../extern.hpp"
# include "../core/Server.hpp"

class ServParser: public Server, public LocParser {
    
    public:
        typedef void                        (ServParser::*parserPtr)(const std::string&, size_t&);
    
        void                                parseBody(size_t& l_nbr);
        void                                parseServerName(const std::string& str, size_t& l_nbr);
        void                                parseListen(const std::string& str, size_t& l_nbr);
        void                                parseLocation(const std::string& str, size_t& l_nbr);
        void                                parseMaxBodySize(const std::string& str, size_t& l_nbr);

        void                                initiateAttribute( void );
        void                                isGoodConfigured(size_t l_nbr) const;
        void                                parserCaller(std::string& line, size_t& l_nbr);
        void                                operator()(size_t& l_nbr);
        
        ServParser(std::fstream& is);
        ServParser(const ServParser& x);
        ServParser&                         operator=(const ServParser& x);
        virtual ~ServParser();

    
    protected:
        bool                                done;
        std::vector<std::string>            unites;
        std::map<std::string, parserPtr>    parserList;
};

# endif