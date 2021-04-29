#pragma once

#include <sstream>

#include <curlydoc/translator.hpp>

namespace curlydoc{

class translator_to_html : public translator{
public:
	std::stringstream ss;

	translator_to_html(std::string&& file_name);

	void handle_word(const std::string& word)override;
	void handle_paragraph(const treeml::tree_ext& tree)override;
	void handle_bold(const treeml::tree_ext& tree)override;
	void handle_italic(const treeml::tree_ext& tree)override;
	void handle_underline(const treeml::tree_ext& tree)override;
	void handle_strikethrough(const treeml::tree_ext& tree)override;

	void handle_header1(const treeml::tree_ext& tree)override;
	void handle_header2(const treeml::tree_ext& tree)override;
	void handle_header3(const treeml::tree_ext& tree)override;
	void handle_header4(const treeml::tree_ext& tree)override;
	void handle_header5(const treeml::tree_ext& tree)override;
	void handle_header6(const treeml::tree_ext& tree)override;
};

}
