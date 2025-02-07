/*
 * UniqueId.hpp
 *
 *  Created on: 10 déc. 2014
 *      Author: pierre
 */

#ifndef UNIQUEID_HPP_
#define UNIQUEID_HPP_

#include <ostream>
#include <functional>


#define UNIQUE_ID(name, type)\
	struct name##_tag{};\
	typedef UniqueId_t<type,name##_tag > name##_id;



//---implementation---

template< typename T, typename Tag>
struct UniqueId_t{
	typedef T data_t;
	typedef UniqueId_t<T,Tag> id_t;

	constexpr UniqueId_t() =default;
	UniqueId_t(id_t&&)     =default;

	         constexpr UniqueId_t(const id_t &t):data(t.data){}
	explicit constexpr UniqueId_t(const T&t):data(t){};


	id_t& operator=(const id_t&a)=default;

	constexpr operator T() const {return data;}

    //set & get
	const T&  get()const{return data;}
	template<typename X>  void set(const X & x){data=x;}


	//Do construct, set and assign from others IDs
	template< typename T2, typename Tag2> void      set       (const UniqueId_t<T2,Tag2>&)=delete;//Do set from others IDs
	template< typename T2, typename Tag2> constexpr UniqueId_t(const UniqueId_t<T2,Tag2>&)=delete;//Do construct from others IDs
	template< typename T2, typename Tag2> id_t&     operator= (const UniqueId_t<T2,Tag2>&)=delete;//Do not assign from others IDs

	//Do not construct and assign from others types (but set can be used instead)
	template< typename X> constexpr UniqueId_t(const X &a)=delete;//Do not assign from others types
	template< typename X> id_t&      operator=(const X &a)=delete;//Do not assign from others types

	//direct acces to internal data
	//know what you do when you use it
	T data;


private:

	template< typename T2, typename Tag2> friend bool operator < (const UniqueId_t<T2,Tag2>& a, const UniqueId_t<T2,Tag2>&b);
	template< typename T2, typename Tag2> friend bool operator > (const UniqueId_t<T2,Tag2>& a, const UniqueId_t<T2,Tag2>&b);
	template< typename T2, typename Tag2> friend bool operator <=(const UniqueId_t<T2,Tag2>& a, const UniqueId_t<T2,Tag2>&b);
	template< typename T2, typename Tag2> friend bool operator >=(const UniqueId_t<T2,Tag2>& a, const UniqueId_t<T2,Tag2>&b);
	template< typename T2, typename Tag2> friend bool operator ==(const UniqueId_t<T2,Tag2>& a, const UniqueId_t<T2,Tag2>&b);
	template< typename T2, typename Tag2> friend bool operator !=(const UniqueId_t<T2,Tag2>& a, const UniqueId_t<T2,Tag2>&b);


	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2> & operator ++(UniqueId_t<T2,Tag2> &a);
	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2> & operator --(UniqueId_t<T2,Tag2> &a);
	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2>   operator ++(UniqueId_t<T2,Tag2> a, int);
	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2>   operator --(UniqueId_t<T2,Tag2> a, int);

	template< typename T2, typename Tag2> friend std::ostream& operator << (std::ostream &, const UniqueId_t<T2,Tag2>& );
	template< typename T2, typename Tag2> friend std::istream& operator >> (std::istream &, const UniqueId_t<T2,Tag2>& );

	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2> operator +(const UniqueId_t<T2,Tag2> a,const UniqueId_t<T,Tag> b);
	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2> operator -(const UniqueId_t<T2,Tag2> a,const UniqueId_t<T,Tag> b);
	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2> operator *(const UniqueId_t<T2,Tag2> a,const UniqueId_t<T,Tag> b);
	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2> operator /(const UniqueId_t<T2,Tag2> a,const UniqueId_t<T,Tag> b);

	template< typename T2, typename Tag2> friend UniqueId_t<T2,Tag2> operator -(UniqueId_t<T2,Tag2> a);

};

//https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B#Arithmetic_operators
//< > <= >= == !=
template< typename T, typename Tag>
bool operator < (const UniqueId_t<T,Tag>& a, const UniqueId_t<T,Tag>&b){return a.data< b.data;}

template< typename T, typename Tag>
bool operator > (const UniqueId_t<T,Tag>& a, const UniqueId_t<T,Tag>&b){return a.data> b.data;}

template< typename T, typename Tag>
bool operator <=(const UniqueId_t<T,Tag>& a, const UniqueId_t<T,Tag>&b){return a.data<=b.data;}

template< typename T, typename Tag>
bool operator >=(const UniqueId_t<T,Tag>& a, const UniqueId_t<T,Tag>&b){return a.data>=b.data;}

template< typename T, typename Tag>
bool operator ==(const UniqueId_t<T,Tag>& a, const UniqueId_t<T,Tag>&b){return a.data==b.data;}

template< typename T, typename Tag>
bool operator !=(const UniqueId_t<T,Tag>& a, const UniqueId_t<T,Tag>&b){return a.data!=b.data;}


//++, --, -
template< typename T, typename Tag>
UniqueId_t<T,Tag> & operator ++(UniqueId_t<T,Tag> &a){++(a.data);return a;}

template< typename T, typename Tag>
UniqueId_t<T,Tag> & operator --(UniqueId_t<T,Tag> &a){--(a.data);return a;}

template< typename T, typename Tag>
UniqueId_t<T,Tag> operator ++(UniqueId_t<T,Tag> a, int){UniqueId_t<T,Tag> r=a;++a;return r;}

template< typename T, typename Tag>
UniqueId_t<T,Tag> operator --(UniqueId_t<T,Tag> a, int){UniqueId_t<T,Tag> r=a;--a;return r;}

template< typename T, typename Tag>
UniqueId_t<T,Tag> operator -(UniqueId_t<T,Tag> a){return UniqueId_t<T,Tag>(-a.data);}


//<< >>
template< typename T, typename Tag>
std::ostream& operator << (std::ostream &out, const UniqueId_t<T,Tag>&a ){out << a.data; return out;}

template< typename T, typename Tag>
std::istream& operator >> (std::istream &in , const UniqueId_t<T,Tag>&a ){in >> a.data; return in;}


//+ - * /
template< typename T, typename Tag>
UniqueId_t<T,Tag> operator +(const UniqueId_t<T,Tag> a,const UniqueId_t<T,Tag> b) {return UniqueId_t<T,Tag>(a.data + b.data);}

template< typename T, typename Tag>
UniqueId_t<T,Tag> operator -(const UniqueId_t<T,Tag> a,const UniqueId_t<T,Tag> b) {return UniqueId_t<T,Tag>(a.data - b.data);}

template< typename T, typename Tag>
UniqueId_t<T,Tag> operator *(const UniqueId_t<T,Tag> a,const UniqueId_t<T,Tag> b) {return UniqueId_t<T,Tag>(a.data * b.data);}

template< typename T, typename Tag>
UniqueId_t<T,Tag> operator /(const UniqueId_t<T,Tag> a,const UniqueId_t<T,Tag> b) {return UniqueId_t<T,Tag>(a.data / b.data);}



namespace std {
template< typename T, typename Tag>

	struct hash<UniqueId_t<T,Tag> >{

		size_t operator()(const UniqueId_t<T,Tag>& a ) const {
			typedef typename UniqueId_t<T,Tag>::data_t data_t;
			return std::hash<data_t>()(a.get());
		}
	};
};





#endif /* UNIQUEID_HPP_ */
