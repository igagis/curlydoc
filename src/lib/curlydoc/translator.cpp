#include "translator.hpp"

using namespace curlydoc;

translator::translator(std::string&& file_name) :
		translator_base(std::move(file_name))
{
	this->add_keyword("p", [this](auto& tree){
		this->handle_paragraph(tree);
	});

	this->add_keyword("*", [this](auto& tree){
		this->handle_bold(tree);
	});
	this->add_keyword("b", [this](auto& tree){
		this->handle_bold(tree);
	});

	this->add_keyword("/", [this](auto& tree){
		this->handle_italic(tree);
	});
	this->add_keyword("i", [this](auto& tree){
		this->handle_italic(tree);
	});

	this->add_keyword("_", [this](auto& tree){
		this->handle_underline(tree);
	});
	this->add_keyword("u", [this](auto& tree){
		this->handle_underline(tree);
	});

	this->add_keyword("~", [this](auto& tree){
		this->handle_strikethrough(tree);
	});
	this->add_keyword("s", [this](auto& tree){
		this->handle_strikethrough(tree);
	});

	this->add_keyword("=", [this](auto& tree){
		this->handle_header1(tree);
	});
	this->add_keyword("h1", [this](auto& tree){
		this->handle_header1(tree);
	});

	this->add_keyword("==", [this](auto& tree){
		this->handle_header2(tree);
	});
	this->add_keyword("h2", [this](auto& tree){
		this->handle_header2(tree);
	});

	this->add_keyword("===", [this](auto& tree){
		this->handle_header3(tree);
	});
	this->add_keyword("h3", [this](auto& tree){
		this->handle_header3(tree);
	});

	this->add_keyword("====", [this](auto& tree){
		this->handle_header4(tree);
	});
	this->add_keyword("h4", [this](auto& tree){
		this->handle_header4(tree);
	});
	
	this->add_keyword("=====", [this](auto& tree){
		this->handle_header5(tree);
	});
	this->add_keyword("h5", [this](auto& tree){
		this->handle_header5(tree);
	});

	this->add_keyword("======", [this](auto& tree){
		this->handle_header6(tree);
	});
	this->add_keyword("h6", [this](auto& tree){
		this->handle_header6(tree);
	});

	// TODO:
}
