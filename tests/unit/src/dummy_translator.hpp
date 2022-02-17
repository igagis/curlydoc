#pragma once

#include <curlydoc/translator.hpp>

class dummy_translator : public curlydoc::translator{
public:
	void on_word(const std::string& word)override{}
	void on_paragraph(const treeml::forest_ext& forest)override{}
	void on_bold(const treeml::forest_ext& forest)override{}
	void on_italic(const treeml::forest_ext& forest)override{}
	void on_underline(const treeml::forest_ext& forest)override{}
	void on_strikethrough(const treeml::forest_ext& forest)override{}
	void on_monospace(const treeml::forest_ext& forest)override{}
	void on_header1(const treeml::forest_ext& forest)override{}
	void on_header2(const treeml::forest_ext& forest)override{}
	void on_header3(const treeml::forest_ext& forest)override{}
	void on_header4(const treeml::forest_ext& forest)override{}
	void on_header5(const treeml::forest_ext& forest)override{}
	void on_header6(const treeml::forest_ext& forest)override{}
	void on_ins(const tml::forest_ext& forest)override{}

	void on_image(const image_params& params, const treeml::forest_ext& forest)override{}
	void on_table(const table& tbl, const treeml::forest_ext& forest)override{}
	void on_list(const list& l, const treeml::forest_ext& forest)override{}
};
