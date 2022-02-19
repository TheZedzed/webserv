#pragma once

# ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Header.hpp"

/* directive location in server context */
class	Location {
	public:
		typedef std::pair<int, String>	Redirect;

		Location();
		~Location();

		void	setRoot(const String& root);
		void	setAutoIndex(const String& autoindex);
		void	setRedirection(const Array& line);
		void	setMethod(const Array& line);
		void	setCgi(const String& line);

		const String&		getRoot() const;
		const Redirect&		getRedir() const;
		bool				getAuto() const;
		const Array&		getAllow() const;
		const String&		getCgi() const;

	private:
		Location(const Location&);
		Location&	operator=(const Location&);

		Array		_allowed; //Sets allowed methods
		Redirect	_redir; // redirection
		bool		_auto; // autoindex
		String		_root; // root
		String		_cgi; // cgi
};

std::ostream&	operator<<(std::ostream& out, const Location& loc);
#endif