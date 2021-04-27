#pragma once

#include <sstream>

#include <curlydoc/translator.hpp>

namespace curlydoc{

class translator_to_html : public translator{
public:
	std::stringstream ss;

	translator_to_html(std::string&& file_name);

	void handle_space()override;
	void handle_word(const std::string& word)override;
	void handle_paragraph(const treeml::tree_ext& tree)override;
	void handle_bold(const treeml::tree_ext& tree)override;
};

}
