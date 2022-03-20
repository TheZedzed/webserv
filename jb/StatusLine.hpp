#ifndef STATUSLINE_HPP
#define STATUSLINE_HPP

#include "iostream"

class StatusLine
{
	private:

		int			_code;		// return code
		std::string _reason;	
		std::string _addInfos;	

	public:

		StatusLine() : _code(-1), _reason() {}
		StatusLine(int code, const char* reason, const std::string& addInfos = "") :
			_code(code), _reason(reason), _addInfos(addInfos) {}
		StatusLine(const StatusLine& c) :
				_code(c._code), _reason(c._reason), _addInfos(c._addInfos) {}
		~StatusLine() {}
		StatusLine& operator=(StatusLine a)
		{
			swap(*this, a);
			return *this;
		}

		void setCode(int code)								{ _code = code; }
		void setReason(const std::string& reason)			{ _reason = reason; }
		void setAdditionalInfo(const std::string& addInfos)	{ _addInfos  = addInfos; }

		int getCode() const { return _code; }
		const std::string& getReason() const { return _reason; }
		const std::string& getAdditionalInfo() const { return _addInfos; }


		void clear()
		{
			_code = -1;
			_reason.clear();
			_addInfos.clear();
		}
		
		// print status
		void print() const
		{
			std::cout << "------ REQUEST LINE ------\ncode: " << _code << ", reason: |" << _reason
				<< "|, add info: |" << _addInfos << "|\n";
		}

		friend void swap(StatusLine& a, StatusLine& b)
		{
			std::swap(a._code, b._code);
			std::swap(a._reason, b._reason);
			std::swap(a._addInfos, b._addInfos);
		}

inline std::ostream& operator<<(std::ostream& stream, const StatusLine& staLine)
{
	stream << staLine.getCode() << ": " << staLine.getReason();
	
	if (!staLine.getAdditionalInfo().empty())
		stream << ": " << staLine.getAdditionalInfo();
		
	return stream;
}

#endif