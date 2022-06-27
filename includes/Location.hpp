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
		void	set_index(const strs_t& line);
		void	set_cgi(const str_t& line);

		bool				get_autoindex() const;
		const str_t&		get_index() const;
		const str_t&		get_cgi() const;
		const str_t&		get_root() const;
		const redir_t&		get_redir() const;
		const strs_t&		get_allowed() const;

	private:
		Location(const Location&);
		Location&	operator=(const Location&);

		strs_t	_allow; // allowed methods
		str_t	_index; // indexes
		redir_t	_redir; // redirection
		bool	_auto; // autoindex
		str_t	_root; // root
		str_t	_cgi; // cgi
};

const inline str_t&	Location::get_root() const
{ return _root; }

const inline Location::redir_t&	Location::get_redir() const
{ return _redir; }

inline bool	Location::get_autoindex() const
{ return _auto; }

const inline str_t&	Location::get_index() const
{ return _index; }

const inline strs_t&	Location::get_allowed() const
{ return _allow; }

const inline str_t&	Location::get_cgi() const
{ return _cgi; }

inline void	Location::set_root(const str_t& root)
{ _root = root; }

inline void	Location::set_index(const strs_t& line)
{ _index = line[1]; }

inline void	Location::set_autoindex(const str_t& autoindex)
{ _auto = (autoindex == "on" ? 1 : 0); }

inline void	Location::set_cgi(const str_t& cgi)
{ _cgi = cgi; }

std::ostream&	operator<<(std::ostream& out, const Location& loc);
#endif
