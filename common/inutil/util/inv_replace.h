#ifndef __INV_REPLACE_H
#define __INV_REPLACE_H

#include <hash_map>

namespace inv
{

template <typename CH>
class INV_Replacer_impl
{
	typedef INV_Replacer_impl<CH> this_type;
	typedef __gnu_cxx::hash_map<CH, this_type *> ChildsT;
	typedef std::basic_string<CH> string_type;
	typedef typename string_type::size_type size_type;

public:

	INV_Replacer_impl(){}

	INV_Replacer_impl(const INV_Replacer_impl& r): _old(r._old), _new(r._new)
	{
		copy_deap_from(r._childs);
	}

	~INV_Replacer_impl()
	{
		clear();
	}

	INV_Replacer_impl& operator=(const INV_Replacer_impl& r)
	{
		if(this == &r)
		{
			_old = r._old;
			_new = r._new;
			copy_deap_from(r._childs);
		}
		return *this;
	}

	void clear()
	{
		for(typename ChildsT::iterator i = _childs.begin(); i != _childs.end(); ++i)
		{
			delete i->second;
		}
		_childs.clear();
		_old.clear();
		_new.clear();
	}

	string_type replace(const string_type& s) const
	{
		ResultPos wi;
		bool r = find_words(s, 0, wi);
		if(!r)
		{
			return s;
		}

		string_type sb;
		sb.reserve(s.size());
		int writen = 0;
		for(; r && wi.idx < (int)s.size(); ){
			sb.append(s, writen, wi.idx - writen);
			sb.append(wi.msr->_new);
			writen = wi.idx + wi.msr->_old.size();
			r = find_words(s, writen, wi);
		}
		sb.append(s, writen, s.size() - writen);
		return sb;
	}

	bool exist_words(const string_type& s) const
	{
		ResultPos fi;
		return find_words(s, 0, fi);
	}

	int count_words(const string_type& s) const
	{
		ResultPos wi;
		bool r = find_words(s, 0, wi);
		if(!r)
		{
			return 0;
		}

		int writen = 0;
		int count = 0;
		for(; r && wi.idx < (int)s.size(); ){
			++count;
			writen = wi.idx + wi.msr->_old.size();
			r = find_words(s, writen, wi);
		}
		return count;
	}

	this_type& add(const string_type& oldWords, const string_type& newWords)
	{
		add(oldWords, 0, newWords);
		return *this;
	}

private:
	ChildsT			_childs;
	string_type		_old;
	string_type		_new;

	struct ResultPos
	{
		const this_type * msr;
		int idx;
	};

private:
	void copy_deap_from(const ChildsT& from)
	{
		clear();
		for(typename ChildsT::const_iterator i = from.begin(); i != from.end(); ++i)
		{
			_childs[i->first] = new this_type(*i->second);
		}
	}

	bool compare_words(const string_type& s, int idx, ResultPos& fi) const
	{
		if(!_old.empty() && _childs.size() == 0)
		{
			fi.msr = this;
			fi.idx = idx - _old.size();
			return true;
		}
		
		if(idx >= (int)s.size())
		{
			return false;
		}

		CH c = s[idx];
		typename ChildsT::const_iterator it = _childs.find(c);
		if(it == _childs.end())
		{
			return false;
		}
		this_type * ti = it->second;
		
		bool r = ti->compare_words(s, idx + 1, fi);
		
		if(!r && !ti->_old.empty()){
			fi.msr = ti;
			fi.idx = idx + 1 - ti->_old.size();
			return true;
		}

		return r;
	}

	bool find_words(const string_type& s, int idx, ResultPos& fi) const
	{
		for(int i = idx; i < (int)s.size(); ++i){
			bool r = compare_words(s, i, fi);
			if(r){
				return true;
			}
		}
		return false;
	}

	void add(const string_type& oldWords, int idx, const string_type& newWords)
	{
		if((int)oldWords.size() == idx){
			_new = newWords;
			_old = oldWords;
			return;
		}
		
		CH next_ch = oldWords[idx];
		this_type *& ti = _childs[next_ch];
		if(ti == NULL){
			ti = new this_type();
		}
		
		ti->add(oldWords, idx + 1, newWords);
	}
};

typedef INV_Replacer_impl<char> INV_Replacer;

}

#endif

/*
int main()
{
    
	inv::INV_Replacer msr;
	msr.add("中文", "你好");
	msr.add("测试", "他好");
	
	string ss = "123456789中文1111111测试";
	std::string s = msr.replace(ss);

	cout << s << endl;

    return 0;
}
*/

