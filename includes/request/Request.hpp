/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:39:45 by abbaraka          #+#    #+#             */
/*   Updated: 2025/04/26 18:24:04 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "RequestLine.hpp"
# include "../extern.hpp"
# include "../core/Socket.hpp"
# include "../core/Connection.hpp"

# define	RES_UNSPECIFIE					0
# define	RES_WITH_ERROR					1
# define	RES_BY_REDIREC					2
# define	RES_WITH_DIREC					4
# define	RES_WITH_REGUL					8
# define	RES_WITH_DEFAU					16
# define	RES_WITH_BACKE					32

typedef	struct s_host {
	std::string								address;
	long									port;
}											host_t;

class Request {
	protected:		
		Socket                              socket;
		Server                              server;
		Location                            location;
		Connection							*connectionPtr;
		int									maxCharHeaders;

		bool								moreData;
		std::string                         processedData;
		
		bool								headersDone;
		RequestLine							requestLine;
		host_t								host;
		std::map<std::string, std::string>  requestHeaders;
		
		bool								formData;
		std::string							boundary;
		
		short                               statusCode;
		short                               typeOfResponse;
		std::string                         backEndResponse;
		std::string							resourceToServe;

	public:
		// bool								requestInitiated() const;
		void								setHost();
		void								checkDuplicatedHeader(std::string &header);
		void    							setContentType(std::string &value);
		// bool								validMIME(std::string input);
		
		void								parseHeaders(std::string input);
		void								parseRequestLine(std::string line);
		void								parseRequest();
		std::string							getHeaderValue(std::string name) const;
		
		short								gettypeOfResponse( void ) const;
		void								getMoreDataOfRequestBody( void );
		bool								isDone();
		void								operator() (Connection *connection);

		Request(const Request& x);
		Request(const Socket& sock);
		Request& operator=(const Request& x);
		virtual ~Request();	
	};
