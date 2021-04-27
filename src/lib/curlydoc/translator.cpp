#include "translator.hpp"

using namespace curlydoc;

translator::translator(std::string&& file_name) :
		translator_base(std::move(file_name))
{
	this->add_keyword("p", [this](auto& tree){
		this->handle_paragraph(tree);
	});

	this->add_keyword("b", [this](auto& tree){
		this->handle_bold(tree);
	});
	
	// TODO:
}
