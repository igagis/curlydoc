#pragma once

#include "translator_base.hpp"

namespace curlydoc{

class translator : public translator_base{
public:
	translator(std::string&& file_name);

	virtual void handle_paragraph(const treeml::tree_ext& tree) = 0;

	virtual void handle_bold(const treeml::tree_ext& tree) = 0;
	virtual void handle_italic(const treeml::tree_ext& tree) = 0;
	virtual void handle_underline(const treeml::tree_ext& tree) = 0;
	virtual void handle_strikethrough(const treeml::tree_ext& tree) = 0;

	virtual void handle_header1(const treeml::tree_ext& tree) = 0;
	virtual void handle_header2(const treeml::tree_ext& tree) = 0;
	virtual void handle_header3(const treeml::tree_ext& tree) = 0;
	virtual void handle_header4(const treeml::tree_ext& tree) = 0;
	virtual void handle_header5(const treeml::tree_ext& tree) = 0;
	virtual void handle_header6(const treeml::tree_ext& tree) = 0;

	// TODO: add basic virtual functions
};

}
