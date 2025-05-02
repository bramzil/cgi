/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 18:55:05 by bramzil           #+#    #+#             */
/*   Updated: 2025/04/23 17:11:41 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../extern.hpp"
# include "../utilities.hpp"
# include "../parser/ParserExcep.hpp"

class Location {
    public:
        void                                displayLocationParameters( void ) const;    

        size_t                              isMatchParameter(std::string str) const;
        void                                setParameter(std::string str);
        std::string                         getParameter( void ) const; 

        void                                setRoot(std::string str);
        std::string                         getRoot(void) const;    

        void                                setAutoIndex(std::string str);
        bool                                isAutoIndex(void) const;    

        void                                setIndex(std::string str);
        std::string                         getIndex(void) const;   

        bool                                isThereAnyRedirection( void ) const;
        void                                setRedirection(std::pair<short, std::string>& pair);
        std::pair<short, std::string>       getRedirection( void ) const;   

	    void                                addDefaultErrorPage(std::pair<short, std::string>& tmp);
        std::string                         getDefaultErrorPage(short code) const;  

        bool                                isAllowedMethod(std::string str) const;
        void                                addAllowedMethod(std::string str);

        bool                                isThereAnyBackendApplication( void ) const;
        void                                setCGIApplication(std::string extention, std::string executablePath);
        std::string                         getCGIApplication(std::string extention) const; 

        bool                                isThereAnyCookie( void ) const;
        void                                addCookie(std::vector<std::string> cookie);
        std::set<std::vector<std::string> > getCookies( void ) const;

        bool                                operator==(const Location& x) const;

        Location();
        Location(const Location& x);
        const Location&                     operator=(const Location& x);
        virtual ~Location();

    protected:
        std::string                         root;
        std::string                         index;
        std::string                         autoIndex;
        std::string                         parameter;
        std::set<std::string>               allowedMethods;
        std::pair<short, std::string>       redirection;
        std::map<std::string, std::string>  cgiApllications;
        std::map<short, std::string>        defaultErrorPages;

        std::set<std::vector<std::string> > cookies;

};

# endif
