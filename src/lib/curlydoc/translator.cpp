#include "translator.hpp"

using namespace curlydoc;

translator::translator(){
	this->add_keyword("p", [this](auto& forest){
		this->handle_paragraph(forest);
	});

	this->add_keyword("b", [this](auto& forest){
		this->handle_bold(forest);
	});
	
	// TODO:
}
