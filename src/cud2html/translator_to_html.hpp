#pragma once

#include <sstream>

#include <curlydoc/translator.hpp>

namespace curlydoc{

class translator_to_html : public translator{
public:
	std::stringstream ss;

	translator_to_html(std::string&& file_name);

	void on_word(const std::string& word)override;
	void on_paragraph(const treeml::tree_ext& tree)override;
	void on_bold(const treeml::tree_ext& tree)override;
	void on_italic(const treeml::tree_ext& tree)override;
	void on_underline(const treeml::tree_ext& tree)override;
	void on_strikethrough(const treeml::tree_ext& tree)override;
	void on_monospace(const treeml::tree_ext& tree)override;

	void on_header1(const treeml::tree_ext& tree)override;
	void on_header2(const treeml::tree_ext& tree)override;
	void on_header3(const treeml::tree_ext& tree)override;
	void on_header4(const treeml::tree_ext& tree)override;
	void on_header5(const treeml::tree_ext& tree)override;
	void on_header6(const treeml::tree_ext& tree)override;
};

}
