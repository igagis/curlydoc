#include "translator_to_html.hpp"

using namespace curlydoc;

translator_to_html::translator_to_html(){
	// TODO:
}

void translator_to_html::handle_space(){
	this->ss << ' ';
}

void translator_to_html::handle_word(const std::string& word){
	this->ss << word;
}

void translator_to_html::handle_paragraph(const treeml::forest& forest){
	this->ss << "<p>";
	this->translate(forest);
	this->ss << "</p>";
}

void translator_to_html::handle_bold(const treeml::forest& forest){
	this->ss << "<b>";
	this->translate(forest);
	this->ss << "</b>";
}
