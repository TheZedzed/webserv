#ifndef BODY_HPP
#define BODY_HPP

class Body
{
	private:

			bool								_recv;		// if line + header receiv
			size_t								_size;		
			long								_maxSize;	
			std::string							_buff;		


	public:
		Body() : _recv(false), _size(), _maxSize(-1) {}
		Body(const Body& c) :
				_recv(c._recv), _size(c._size), _maxSize(c._maxSize), _buff(c._buff) {}
		~Body() {}
		Body& operator=(Body a)
		{
			swap(*this, a);
			return *this;
		}

		const std::string& getBody() const { return _buff; }
		size_t getSize() const { return _size; }
		size_t getMaxSize() const { return _maxSize; }

		void setSize(size_t size) { _size = size; }
		void setMaxSize(long maxSize) { _maxSize = maxSize; }
		void setBuff(std::string const &buf) { _buff = buf; }
		void startReceiving() { _recv = true; }

		bool isReceiving() const { return _recv; }
		
		void clear()
		{
			_recv = false;
			_size = 0;
			_buff.clear();
		}
		
		// Append buffer received from client until content-length octets have been received.
		int recvBuffer(const std::string& buffer, size_t index, size_t lenToRead)
		{
			// maxSize will be evaluated only if before using setMaxSize before, because 
			// default constructor init to -1
			if (_maxSize >= 0 && (_maxSize -= lenToRead) < 0)
				return -1;

			_buff.append(buffer, index, _size);
			_size -= lenToRead;
			return 0;
		}

		friend void swap(Body& a, Body& b)
		{
			std::swap(a._recv, b._recv);
			std::swap(a._size, b._size);
			std::swap(a._maxSize, b._maxSize);
			std::swap(a._buff, b._buff);
		}
		
};

#endif