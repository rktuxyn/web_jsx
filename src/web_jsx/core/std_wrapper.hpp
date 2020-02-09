/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#if defined(_MSC_VER)
#pragma once
#endif//!_MSC_VER
#if !defined(_std_wrapper_h)
#	define _std_wrapper_h
#	include <iostream>
#	include <sstream> // std::stringstream
#	include <regex>
#	include <cstdlib>
#	include <string>
#	include <list>
#	include <map>
//December 01, 2017, 01:48 AM
namespace std {
	template<class BidirIt,
		class Traits,
		class CharT,
		class UnaryFunction>
		std::basic_string<CharT>
		__regex_replacematch(BidirIt first, BidirIt last,
			const std::basic_regex<CharT, Traits>& re,
			UnaryFunction f, size_t count = 0
		) {
		std::basic_string<CharT> str;
		{
			typename std::match_results<BidirIt>::difference_type
				positionOfLastMatch = 0;
			auto endOfLastMatch = first;
			size_t increment = 0;
			std::sregex_iterator begin(first, last, re), end;
			bool find = std::all_of(begin, end, [&](const std::match_results<BidirIt>& match) {
				auto positionOfThisMatch = match.position(0);
				auto diff = positionOfThisMatch - positionOfLastMatch;
				auto startOfThisMatch = endOfLastMatch;
				std::advance(startOfThisMatch, diff);
				str.append(endOfLastMatch, startOfThisMatch);
				str.append(f(match));
				auto lengthOfMatch = match.length(0);
				positionOfLastMatch = positionOfThisMatch + lengthOfMatch;
				endOfLastMatch = startOfThisMatch;
				if (count != 0) {
					increment++;
					if (increment >= count) {
						return false;
					}
				}
				std::advance(endOfLastMatch, lengthOfMatch);
				return true;
			});
			if (find == true)
				str.append(endOfLastMatch, last);
		}
		return str;
	}
	//std::regex_replace
	template<class _RxTraits,
		class _Elem,
		class _Traits1,
		class _Alloc1>
		void __regex_replace(
			basic_string<_Elem, _Traits1, _Alloc1>& _Str,
			const basic_regex<_Elem, _RxTraits>& _Re,
			const _Elem *_Ptr,
			regex_constants::match_flag_type _Flgs = regex_constants::match_default/*search and replace, string result, string target, NTBS format*/) {
		_Str = std::regex_replace(_Str, _Re, _Ptr, _Flgs);
		return;
	};
	template<class BidirIt,
		class Traits,
		class CharT,
		class UnaryFunction>
		std::basic_string<CharT>
		__regex_core(BidirIt first, BidirIt last,
			const std::basic_regex<CharT, Traits>& re,
			UnaryFunction f, size_t count = 0
		) {
		std::basic_string<CharT> str;
		{
			std::sregex_iterator next(first, last, re);
			std::sregex_iterator end;
			size_t increment = 0;
			while (next != end) {
				std::smatch match = *next;
				str.append(f(match));
				if (count != 0) {
					increment++;
					if (increment >= count) {
						break;
					}
				}
				next++;
			}
		}
		return str;
	}
	template<class Traits,
		class CharT,
		class UnaryFunction>
		std::basic_string<CharT>
		regex__replacematch(const std::basic_string<CharT>& s,
			const std::basic_regex<CharT, Traits>& re,
			UnaryFunction f, size_t count = 0
		) {
		return __regex_replacematch(s.cbegin(), s.cend(), re, f, count);
	}
#define REGEX_REPLACE_MATCH regex__replacematch
	template<class BidirIt,
		class Traits,
		class CharT>
		std::basic_string<CharT>
		__regex_macth_str(BidirIt first, BidirIt last,
			const std::basic_regex<CharT, Traits>& re
		) {
		std::basic_string<CharT> tmp("INVALID");//
		std::sregex_iterator begin(first, last, re), end;
		std::all_of(begin, end, [&](const std::match_results<BidirIt>& match) {
			tmp = match.str(1);
			return false;
		});
		return tmp;
	};
	template<class Traits,
		class CharT>
		std::basic_string<CharT>
		regex__matchstr(const std::basic_string<CharT>& str,
			const std::basic_regex<CharT, Traits>& re
		) {
		if (str.empty()) return "INVALID";
		std::basic_string<CharT> tmp("INVALID");//
		__regex_replacematch(str.cbegin(), str.cend(), re, [&tmp](const std::smatch& m) {
			tmp = m.str(1);
			return std::basic_string<CharT>("__NOP__");
		}, 1);
		//return __regex_macth_str(str.cbegin(), str.cend(), re);
		return tmp;
	}
#define REGEX_MATCH_STR regex__matchstr
	template<class Traits,
		class CharT>
		void
		regex__str(const std::basic_string<CharT>& str,
			const std::basic_regex<CharT, Traits>& re,
			std::basic_string<CharT>& result
		) {
		if (str.empty()) return;
		__regex_replacematch(str.cbegin(), str.cend(), re, [&](const std::smatch& m) {
			result = m.str(1);
			return std::basic_string<CharT>("__NOP__");
		}, 1);
		//return __regex_macth_str(str.cbegin(), str.cend(), re);
		return;
	}
	template<class Traits,
		class CharT>
		std::basic_string<CharT>
		regex__matchstr_all(const std::basic_string<CharT>& str,
			const std::basic_regex<CharT, Traits>& re, int count = 0
		) {
		if (str.empty()) return "INVALID";
		std::sregex_iterator next(str.begin(), str.end(), re);
		std::sregex_iterator end;
		if (count != 0) {
			while (next != end) {
				count--; next++;
				if (count <= 0)break;
			}
			if (count <= 0) {
				return "EXISTS";
			}
			return "INVALID";
		}
		std::stringstream body_stream;

		while (next != end) {
			std::smatch match = *next;
			body_stream << match.str();
			next++;
		}
		//std::basic_string<CharT> tmp(body_stream.str());//
		//std::stringstream().swap(body_stream);
		return body_stream.str();// __regex_macth_str(str.cbegin(), str.cend(), re);
	};
	template<class Traits,
		class CharT>
		void
		regex__matchlist(std::list<std::string>&ml, const std::basic_string<CharT>& str,
			const std::basic_regex<CharT, Traits>& re
		) {
			{
				if (str.empty())return;
				std::sregex_iterator next(str.begin(), str.end(), re);
				std::sregex_iterator end;
				while (next != end) {
					std::smatch match = *next;
					ml.push_back(match.str());
					next++;
				}
			}
			return;
	};
#define REGEX_MATCH_LIST regex__matchlist
	/**============================================================================*/
	template<class Traits,
		class CharT, class BidirIt>
		void
		regex__match_list(std::list<std::string>& result, BidirIt irFirst, BidirIt irLast,
			const std::basic_regex<CharT, Traits>& re
		) {
			{
				std::sregex_iterator next(irFirst, irLast, re);
				std::sregex_iterator end;
				while (next != end) {
					std::smatch match = *next;
					result.push_back(match.str());
					next++;
				}
			}
	};
#define REGEX_MATCH_LIST_N regex__match_list
	/*=============================================================================*/
	template<class Traits,
		class CharT>
		std::basic_string<CharT>
		regex__matchstrreplaceall(const std::basic_string<CharT>& str,
			const std::basic_regex<CharT, Traits>& re, std::string repstr
		) {
		return std::regex_replace(str, re, repstr);
	}
#define REGEX_REPLACE_ALL regex__matchstrreplaceall
	/*================================================*/
	template<class Traits,
		class CharT, class DataRefStr>
		std::basic_string<CharT>
		regex__match_str_replace_all(DataRefStr& str,
			const std::basic_regex<CharT, Traits>& re, std::string repstr
		) {
		return std::regex_replace(str, re, repstr);
	}
#define REGEX_REPLACE_ALL_N regex__match_str_replace_all
	/*=================================================*/
	template<class BidirIt,
		class Traits, class CharT>
		size_t
		__regex_ismatch(BidirIt first, BidirIt last,
			const std::basic_regex<CharT, Traits>& re
		) {
		size_t increment = 0;
		std::sregex_iterator begin(first, last, re), end;
		std::all_of(begin, end, [&](const std::match_results<BidirIt>& match) {
			increment++;
			return false;
		});
		return increment;
	}
	template<class Traits,
		class CharT>
		size_t
		regex__ismatch(const std::basic_string<CharT>& str,
			const std::basic_regex<CharT, Traits>& re
		) {
		size_t count = 0;
		//if (str == NULL)return 0;
		if (str.empty())return count;
		//return __regex_ismatch(str.cbegin(), str.cend(), re);
		std::sregex_iterator next(str.begin(), str.end(), re);
		//_STD sregex_iterator end;
		std::sregex_iterator end;

		while (next != end) {
			count++;
			break;
		}
		//if (next->length() > 0)return 1;
		return count;

	}
#define REGEX_IS_MATCH regex__ismatch
	/*=================================================*/
	template<class Traits,
		class CharT, class BidirIt>
		size_t
		regex__is_match(BidirIt irFirst, BidirIt irLast,
			const std::basic_regex<CharT, Traits>& re
		) {
		size_t count = 0;
		{
			std::sregex_iterator next(irFirst, irLast, re);
			std::sregex_iterator end;
			while (next != end) {
				count++;
				break;
			}
		}
		return count;

	}
#define REGEX_IS_MATCH_N regex__is_match
	/*=================================================*/
	template<class CharT>
	std::string
		_trim(std::basic_string<CharT>& str) {
		if (str.empty())return str;
		size_t first = str.find_first_not_of(' ');
		size_t last = str.find_last_not_of(' ');
		return std::string(str.substr(first, (last - first + 1)));
	}
#define STR_TRIM _trim
	/**=================================================*/
	template<class Traits,
		class CharT>
		void
		_map_split(std::map<int, std::string>&result, std::basic_string<CharT>& str,
			const std::basic_regex<CharT, Traits>& rgx
		) {
		{
			if (str.empty())return;
			int count = 0;
			std::sregex_token_iterator iter(str.begin(), str.end(), rgx, -1);
			for (std::sregex_token_iterator end; iter != end; ++iter) {
				result[count] = iter->str();
				count++;
			}
		}
		return;
	};
#define STR_MAP_SPLIT _map_split
	/**=====================================================*/
	template<class Traits,
		class CharT, class BidirIt>
		std::map<int, std::string>
		_map_split_n(BidirIt irFirst, BidirIt irLast,
			const std::basic_regex<CharT, Traits>& rgx
		) {
		std::map<int, std::string> result;
		{
			int count = 0;
			std::sregex_token_iterator iter(irFirst, irLast, rgx, -1);
			for (std::sregex_token_iterator end; iter != end; ++iter) {
				result[count] = iter->str();
				count++;
			}
		}
		return result;
	}
#define STR_MAP_SPLIT_N _map_split_n
	/**=====================================================*/
	template<class CharT>
	std::basic_string<CharT>
		_split(std::basic_string<CharT> str,
			const std::basic_string<CharT>& delimiter
		) {
		if (str.empty())return str;

		return str.substr(0, str.find(delimiter));
	}
#define STR_SPLIT _split
	/**======================================================*/
	template<class DataRefStr>
	std::string
		_split_n(DataRefStr str,
			const std::string& delimiter
		) {
		return str->substr(0, str->find(delimiter));
	}
#define STR_SPLIT_N _split_n
	/**======================================================*/
	template<class BidirIt>
	void __reverse(BidirIt first, BidirIt last) {
		while ((first != last) && (first != --last)) {
			std::iter_swap(first++, last);
		}
	};
	// char b 'a' char c 'a'  if (b ==c) 
	//5:34 AM 12/5/2018
	//solution for const char* key and const char* value compaire
	//pdf_generator.cpp ==> int pdf_ext::pdf_generator::update_map_ke
	template <typename Iterator>
	class const_cmp_iter {
		Iterator _begin;
		Iterator _end;
		Iterator _cur;
	public:
		explicit const_cmp_iter(Iterator begin, Iterator end)
			: _begin(begin), _end(end) {
			_cur = _begin;
		}

		bool has_next() const {
			return (_cur != _end);
		}
		Iterator end() {
			return _end;
		};
		Iterator find(const char* key) {
			for (; _cur != _end; ++_cur) {
				if (strcmp(_cur->first, key) == 0) {
					break;
				}
			}
			return _cur;
		};
		void operator++(int dummy) {
			if (!has_next())
				throw std::out_of_range("Out of range.");
			++_cur;
		}
	};
} // namespace std
#endif// !STD_EXTEND_REGX
