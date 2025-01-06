#pragma once

#include <curlydoc/translator.hpp>

class dummy_translator : public curlydoc::translator{
public:
	void on_word(const std::string& word)override{}
	void on_paragraph(const tml::forest_ext& forest)override{}
	void on_bold(const tml::forest_ext& forest)override{}
	void on_italic(const tml::forest_ext& forest)override{}
	void on_underline(const tml::forest_ext& forest)override{}
	void on_strikethrough(const tml::forest_ext& forest)override{}
	void on_monospace(const tml::forest_ext& forest)override{}
	void on_header1(const tml::forest_ext& forest)override{}
	void on_header2(const tml::forest_ext& forest)override{}
	void on_header3(const tml::forest_ext& forest)override{}
	void on_header4(const tml::forest_ext& forest)override{}
	void on_header5(const tml::forest_ext& forest)override{}
	void on_header6(const tml::forest_ext& forest)override{}
	void on_ins(const tml::forest_ext& forest)override{}

	void on_image(const image_params& params, const tml::forest_ext& forest)override{}
	void on_table(const table& tbl, const tml::forest_ext& forest)override{}
	void on_list(const list& l, const tml::forest_ext& forest)override{}
};
