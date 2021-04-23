#include "translator_base.hpp"

using namespace curlydoc;

void translator_base::translate(treeml::forest::const_iterator begin, treeml::forest::const_iterator end){
	// TODO:
}

void translator_base::add_keyword(std::string&& keyword, keyword_handler_type&& handler){
	// TODO:
}

void translator_base::set_word_handler(word_handler_type&& handler){
	if(this->word_handler){
		throw std::logic_error("word handler is already set");
	}
	this->word_handler = std::move(handler);
}
