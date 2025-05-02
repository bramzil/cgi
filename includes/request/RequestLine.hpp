/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/14 22:39:41 by abbaraka          #+#    #+#             */
/*   Updated: 2025/04/23 17:11:41 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "../extern.hpp"
# include "../utilities.hpp"

# define CRLF "\r\n"

class RequestLine
{
	private:
		std::string							method;
		std::string							uri;
		std::string							version;
		std::vector<std::string>			methods;
		std::string							params;

	public:
		RequestLine();
		virtual ~RequestLine();

		void    	checkMethod(std::string method);
		void		setRequestLine(std::string line);
		std::string	getRequestLine() const;
		std::string	getRequestURI() const;
		std::string	getRequestMethod() const;
		bool		checkIfInit() const;
		void    	checkParams(std::string &uri);
		std::string getRequestParams() const;
};