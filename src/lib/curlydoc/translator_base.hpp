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
public:
	virtual ~translator_base(){}

	void translate(treeml::forest::const_iterator begin, treeml::forest::const_iterator end);

	void translate(const treeml::forest& forest){
		this->translate(forest.begin(), forest.end());
	}

	void add_keyword(const std::string& keyword, keyword_handler_type&& handler);

	virtual void handle_space() = 0;
	virtual void handle_word(const std::string& word) = 0;
};

}
