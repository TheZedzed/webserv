#pragma once

# ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Base.hpp"

/* class Location:
** server location block following mandatory part of the subject
** directives:
** allowed_method, redirection, autoindex, root, cgi_pass
*/
class	Location {
	public:
		typedef std::pair<int, str_t>	redir_t;

		Location();
		~Location();

		void	setRoot(const str_t& root);
		void	setAutoIndex(const str_t& autoindex);
		void	setRedirection(const strs_t& line);
		void	setMethod(const strs_t& line);
		void	setCgi(const str_t& line);

		bool				getAuto() const;
		const str_t&		getCgi() const;
		const str_t&		getRoot() const;
		const redir_t&		getRedir() const;
		const strs_t&	getAllow() const;

	private:
		Location(const Location&);
		Location&	operator=(const Location&);

		strs_t	_allow; // allowed methods
		redir_t	_redir; // redirection
		bool	_auto; // autoindex
		str_t	_root; // root
		str_t	_cgi; // cgi
};

std::ostream&	operator<<(std::ostream& out, const Location& loc);
#endif