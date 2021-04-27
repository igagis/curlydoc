#pragma once

#include "translator_base.hpp"

namespace curlydoc{

class translator : public translator_base{
public:
	translator(std::string&& file_name);

	virtual void handle_paragraph(const treeml::forest_ext& tokens) = 0;

	virtual void handle_bold(const treeml::forest_ext& forest) = 0;

	// TODO: add basic virtual functions
};

}
