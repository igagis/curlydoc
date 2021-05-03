#include "translator.hpp"

using namespace curlydoc;

// translator::translator(std::string&& file_name) :
// 		interpreter(std::move(file_name))
// {
// 	this->add_keyword("p", [this](bool space, auto& tree){
// 		this->on_paragraph(tree);
// 	});

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			if(space){
// 				this->on_space();
// 			}
// 			this->on_bold(tree);
// 		};
// 		this->add_keyword("*", f);
// 		this->add_keyword("b", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			if(space){
// 				this->on_space();
// 			}
// 			this->on_italic(tree);
// 		};
// 		this->add_keyword("/", f);
// 		this->add_keyword("i", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			if(space){
// 				this->on_space();
// 			}
// 			this->on_underline(tree);
// 		};
// 		this->add_keyword("_", f);
// 		this->add_keyword("u", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			if(space){
// 				this->on_space();
// 			}
// 			this->on_strikethrough(tree);
// 		};
// 		this->add_keyword("~", f);
// 		this->add_keyword("s", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			if(space){
// 				this->on_space();
// 			}
// 			this->on_monospace(tree);
// 		};
// 		this->add_keyword("`", f);
// 		this->add_keyword("m", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			this->on_header1(tree);
// 		};
// 		this->add_keyword("=", f);
// 		this->add_keyword("h1", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			this->on_header2(tree);
// 		};
// 		this->add_keyword("==", f);
// 		this->add_keyword("h2", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			this->on_header3(tree);
// 		};
// 		this->add_keyword("===", f);
// 		this->add_keyword("h3", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			this->on_header4(tree);
// 		};
// 		this->add_keyword("====", f);
// 		this->add_keyword("h4", f);
// 	}
	
// 	{
// 		auto f = [this](bool space, auto& tree){
// 			this->on_header5(tree);
// 		};
// 		this->add_keyword("=====", f);
// 		this->add_keyword("h5", f);
// 	}

// 	{
// 		auto f = [this](bool space, auto& tree){
// 			this->on_header6(tree);
// 		};
// 		this->add_keyword("======", f);
// 		this->add_keyword("h6", f);
// 	}
	
// 	// TODO:
// }
