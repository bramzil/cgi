/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocParser.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 18:55:05 by bramzil           #+#    #+#             */
/*   Updated: 2025/02/14 18:37:39 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef LOCPARSER_HPP
# define LOCPARSER_HPP

# include "../extern.hpp"
# include "../core/Location.hpp"

class LocParser: public Location {

    public:
        typedef void                    (LocParser::*parserPtr)(const std::string&, size_t&);

        void                            parseBody(size_t& l_nbr);
        void                            parseRoot(const std::string&, size_t& l_nbr);
        void                            parseAutoIndex(const std::string&, size_t& l_nbr);
        void                            parseIndex(const std::string&, size_t& l_nbr);
        void                            parseRedirectTo(const std::string&, size_t& l_nbr);
        void                            parseDefaultErrorPage(const std::string&, size_t& l_nbr);
        void                            parseAllowedMethod(const std::string&, size_t& l_nbr);
        void                            parseCGIApplication(const std::string& str, size_t& l_nbr);
        void                            parseCookie(const std::string& str, size_t& l_nbr);

        void                            initiateAttribute( void );
        void                            isGoodConfigured(size_t l_nbr) const;

        void                            parserCaller(std::string& list, size_t& l_nbr);
        void                            operator()(size_t&l_nbr);
        bool                            operator==(const LocParser& x) const;

        LocParser(std::fstream& is);
        LocParser(const LocParser& x);
        LocParser&                      operator=(const LocParser& x);
        virtual ~LocParser();

    
    protected:
        bool                                            done;
        std::fstream&                                   stream;
        std::set<std::string>                           methods;
        std::map<std::string, parserPtr>                parserList;
};

# endif
