#pragma once

#include "translator_base.hpp"

namespace curlydoc{

class translator : public translator_base{
public:
	translator(std::string&& file_name);

	virtual void handle_paragraph(const treeml::tree_ext& tree) = 0;

	virtual void handle_bold(const treeml::tree_ext& tree) = 0;

	// TODO: add basic virtual functions
};

}
