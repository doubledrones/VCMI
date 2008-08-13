#pragma once
#include "../global.h"
#include <string>
#include <vector>
#include <set>

#include <boost/type_traits/is_fundamental.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_pointer.hpp> 
#include  <boost/type_traits/remove_pointer.hpp>

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/identity.hpp>

const int version = 63;
class CConnection;

namespace mpl = boost::mpl;

namespace boost
{
	namespace asio
	{
		namespace ip
		{
			class tcp;
		}
		class io_service;

		template <typename Protocol> class stream_socket_service;
		template <typename Protocol,typename StreamSocketService>
		class basic_stream_socket;

		template <typename Protocol> class socket_acceptor_service;
		template <typename Protocol,typename SocketAcceptorService>
		class basic_socket_acceptor;
	}
	class mutex;
};

enum SerializationLvl
{
	Wrong=0,
	Primitive,
	Pointer,
	Serializable
};

template<typename T>
struct SerializationLevel
{    
	typedef mpl::integral_c_tag tag;
	typedef
		typename mpl::eval_if<
			boost::is_fundamental<T>,
			mpl::int_<Primitive>,
		//else
		typename mpl::eval_if<
			boost::is_class<T>,
			mpl::int_<Serializable>,
		//else
		typename mpl::eval_if<
			boost::is_array<T>,
			mpl::int_<Primitive>,
		//else
		typename mpl::eval_if<
			boost::is_pointer<T>,
			mpl::int_<Pointer>,
		//else
		typename mpl::eval_if<
			boost::is_enum<T>,
			mpl::int_<Primitive>,
		//else
			mpl::int_<Wrong>
		>
		>
		>
		>
		>::type type;
	static const int value = SerializationLevel::type::value;
};

template <typename Serializer> class DLL_EXPORT COSer
{
public:
	bool saving;
	COSer(){saving=true;};
    Serializer * This()
	{
		return static_cast<Serializer*>(this);
	}

	template<class T>
	Serializer & operator<<(const T &t)
	{
		this->This()->save(t);
		return * this->This();
	}
	
	template<class T>
	COSer & operator&(T & t){
		return * this->This() << t;
	}
};
template <typename Serializer> class DLL_EXPORT CISer
{
public:
	bool saving;
	CISer(){saving = false;};
    Serializer * This()
	{
		return static_cast<Serializer*>(this);
	}

	template<class T>
	Serializer & operator>>(T &t)
	{
		this->This()->load(t);
		return * this->This();
	}
	
	template<class T>
	CISer & operator&(T & t){
		return * this->This() >> t;
	}
};

template<typename Ser,typename T>
struct SavePrimitive
{
	static void invoke(Ser &s, const T &data)
	{
		s.savePrimitive(data);
	}
};
template<typename Ser,typename T>
struct SaveSerializable
{
	static void invoke(Ser &s, const T &data)
	{
		s.saveSerializable(data);
	}
};
template<typename Ser,typename T>
struct LoadPrimitive
{
	static void invoke(Ser &s, T &data)
	{
		s.loadPrimitive(data);
	}
};
template<typename Ser,typename T>
struct SavePointer
{
	static void invoke(Ser &s, const T &data)
	{
		s.savePointer(data);
	}
};
template<typename Ser,typename T>
struct LoadPointer
{
	static void invoke(Ser &s, T &data)
	{
		s.loadPointer(data);
	}
};
template<typename Ser,typename T>
struct LoadSerializable
{
	static void invoke(Ser &s, T &data)
	{
		s.loadSerializable(data);
	}
};

template<typename Ser,typename T>
struct SaveWrong
{
	static void invoke(Ser &s, const T &data)
	{
		throw std::exception("Wrong save serialization call!");
	}
};
template<typename Ser,typename T>
struct LoadWrong
{
	static void invoke(Ser &s, const T &data)
	{
		throw std::exception("Wrong load serialization call!");
	}
};


class DLL_EXPORT CConnection
	:public CISer<CConnection>, public COSer<CConnection>
{


	std::ostream &out;
	CConnection(void);
	void init();
public:
	boost::mutex *rmx, *wmx; // read/write mutexes

	template <typename T>
	void savePrimitive(const T &data)
	{
		write(&data,sizeof(data));
	}
	template <typename T>
	void loadPrimitive(T &data)
	{
		read(&data,sizeof(data));
	}

	
	
	template <typename T>
	void saveSerializable(const T &data)
	{
		const_cast<T&>(data).serialize(*static_cast<COSer<CConnection>*>(this),version);
	}
	template <typename T>
	void loadSerializable(T &data)
	{
		data.serialize(*static_cast<CISer<CConnection>*>(this),version);
	}	
	template <typename T>
	void savePointer(const T &data)
	{
		*this << *data;
	}
	template <typename T>
	void loadPointer(T &data)
	{
		std::cout<<"Allocating memory for pointer!"<<std::endl;
		typedef typename boost::remove_pointer<T>::type npT;
		data = new npT;
		*this >> *data;
	}
	template <typename T>
	void saveSerializable(const std::vector<T> &data)
	{
		boost::uint32_t length = data.size();
		*this << length;
		for(ui32 i=0;i<length;i++)
			*this << data[i];
	}
	template <typename T>
	void loadSerializable(std::vector<T> &data)
	{
		boost::uint32_t length;
		*this >> length;
		data.resize(length);
		for(ui32 i=0;i<length;i++)
			*this >> data[i];
	}
	
	template <typename T>
	void saveSerializable(const std::set<T> &data)
	{
		std::set<T> &d = const_cast<std::set<T> &>(data);
		boost::uint32_t length = d.size();
		*this << length;
		for(typename std::set<T>::iterator i=d.begin();i!=d.end();i++)
			*this << *i;
	}
	template <typename T>
	void loadSerializable(std::set<T> &data)
	{
		boost::uint32_t length;
		*this >> length;
		T ins;
		for(ui32 i=0;i<length;i++)
		{
			*this >> ins;
			data.insert(ins);
		}
	}
	
	template <typename T1, typename T2>
	void saveSerializable(const std::pair<T1,T2> &data)
	{
		*this << data.first << data.second;
	}
	template <typename T1, typename T2>
	void loadSerializable(std::pair<T1,T2> &data)
	{
		*this >> data.first >> data.second;
	}
	
	template <typename T1, typename T2>
	void saveSerializable(const std::map<T1,T2> &data)
	{
		*this << ui32(data.size());
		for(typename std::map<T1,T2>::const_iterator i=data.begin();i!=data.end();i++)
			*this << i->first << i->second;
	}
	template <typename T1, typename T2>
	void loadSerializable(std::map<T1,T2> &data)
	{
		ui32 length;
		*this >> length;
		T1 t;
		for(int i=0;i<length;i++)
		{
			*this >> t;
			*this >> data[t];
		}
	}
	template <typename T>
	void save(const T &data)
	{
		typedef 
			//if
			typename mpl::eval_if< mpl::equal_to<SerializationLevel<T>,mpl::int_<Primitive> >,
				mpl::identity<SavePrimitive<CConnection,T> >,
			//else if
			typename mpl::eval_if<mpl::equal_to<SerializationLevel<T>,mpl::int_<Pointer> >,
				mpl::identity<SavePointer<CConnection,T> >,
			//else if
			typename mpl::eval_if<mpl::equal_to<SerializationLevel<T>,mpl::int_<Serializable> >,
				mpl::identity<SaveSerializable<CConnection,T> >,
			//else
				mpl::identity<SaveWrong<CConnection,T> >
			>
			>
			>::type typex;
		typex::invoke(*this, data);
	}

	template <typename T>
	void load(T &data)
	{
		typedef 
			//if
			typename mpl::eval_if< mpl::equal_to<SerializationLevel<T>,mpl::int_<Primitive> >,
				mpl::identity<LoadPrimitive<CConnection,T> >,
			//else if
			typename mpl::eval_if<mpl::equal_to<SerializationLevel<T>,mpl::int_<Pointer> >,
				mpl::identity<LoadPointer<CConnection,T> >,
			//else if
			typename mpl::eval_if<mpl::equal_to<SerializationLevel<T>,mpl::int_<Serializable> >,
				mpl::identity<LoadSerializable<CConnection,T> >,
			//else
				mpl::identity<LoadWrong<CConnection,T> >
			>
			>
			>::type typex;
		typex::invoke(*this, data);
	}

	boost::asio::basic_stream_socket < boost::asio::ip::tcp , boost::asio::stream_socket_service<boost::asio::ip::tcp>  > * socket;
	bool logging;
	bool connected;
	bool myEndianess, contactEndianess; //true if little endian, if ednianess is different we'll have to revert recieved multi-byte vars
    boost::asio::io_service *io_service;
	std::string name; //who uses this connection

	CConnection
		(std::string host, std::string port, std::string Name, std::ostream & Out);
	CConnection
		(boost::asio::basic_socket_acceptor<boost::asio::ip::tcp, boost::asio::socket_acceptor_service<boost::asio::ip::tcp> > * acceptor, 
		boost::asio::io_service *Io_service, std::string Name, std::ostream & Out);
	CConnection
		(boost::asio::basic_stream_socket < boost::asio::ip::tcp , boost::asio::stream_socket_service<boost::asio::ip::tcp>  > * Socket, 
		std::string Name, std::ostream & Out); //use immediately after accepting connection into socket
	int write(const void * data, unsigned size);
	int read(void * data, unsigned size);
	int readLine(void * data, unsigned maxSize);
	~CConnection(void);
};

template<> DLL_EXPORT 
void CConnection::saveSerializable<std::string>(const std::string &data);
template <>DLL_EXPORT 
void CConnection::loadSerializable<std::string>(std::string &data);

