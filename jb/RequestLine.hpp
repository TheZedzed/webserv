#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include <iostream>

class RequestLine
{
	private:

		int			_method;	// Enum method GET POST
		std::string _path;		// path wew to search
		std::string _query;		// args after ? 


	public:

		RequestLine() : _method(-1) {}
		RequestLine(const RequestLine& c) :
				_method(c._method), _path(c._path), _query(c._query) {}
		~RequestLine() {}
		RequestLine& operator=(RequestLine a)
		{
			swap(*this, a);
			return *this;
		}
	
		int getMethod() const { return _method; }
		const std::string& getPath() const { return _path; }
		const std::string& getQuery() const { return _query; }

		void setMethod(int method) { _method = method; }
		void setPath(const std::string& path) { _path = path; }
		void setQuery(const std::string& query) { _query = query; }

		// Clear RequestLine object
		void clear()
		{
			_method = -1;
			_path.clear();
			_query.clear();
		}

		// Returns true if no path has been set in RequestLine object
		bool empty() const { return _path.empty(); }

		// Print on stdout RequestLine object
		void print() const
		{
			std::cout << "------ REQUEST LINE ------\nmet: " << _method << ", path: |" << _path
				<< "|, query: |" << _query << "|\n";
		}

		friend void swap(RequestLine& a, RequestLine& b)
		{
			std::swap(a._method, b._method);
			std::swap(a._path, b._path);
			std::swap(a._query, b._query);
		}
		
};

// Print the RequestLine method, path, and query if there is one
inline std::ostream& operator<<(std::ostream& stream, const RequestLine& reqLine)
{
	stream << "Method = " << reqLine.getMethod() << ", path = |" << reqLine.getPath() << "|";
	
	if (!reqLine.getQuery().empty())
		stream << ", query = |" << reqLine.getQuery() << "|";
		
	return stream;
}

#endif