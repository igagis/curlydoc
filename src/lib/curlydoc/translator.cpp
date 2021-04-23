#include "translator.hpp"

using namespace curlydoc;

translator::translator(){
	this->add_keyword("p", [this](auto& tokens){
		this->handle_paragraph(tokens);
	});
	
	// TODO:
}
