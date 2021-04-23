#pragma once

#include <unordered_map>

#include <treeml/tree.hpp>

namespace curlydoc{

class translator_base{
public:
	typedef std::function<void(const treeml::forest&)> keyword_handler_type;
	typedef std::function<void(const std::string&)> word_handler_type;
private:
	std::unordered_map<std::string, keyword_handler_type> handlers;
	word_handler_type word_handler;
public:
	virtual ~translator_base(){}

	void translate(treeml::forest::const_iterator begin, treeml::forest::const_iterator end);

	void add_keyword(std::string&& keyword, keyword_handler_type&& handler);

	void set_word_handler(word_handler_type&& handler);
};

}
