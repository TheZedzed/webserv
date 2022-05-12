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

		void	set_root(const str_t& root);
		void	set_autoindex(const str_t& autoindex);
		void	set_redirection(const strs_t& line);
		void	set_method(const strs_t& line);
		void	set_cgi(const str_t& line);

		bool				get_autoindex() const;
		const str_t&		get_cgi() const;
		const str_t&		get_root() const;
		const redir_t&		get_redir() const;
		const strs_t&		get_allowed() const;

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