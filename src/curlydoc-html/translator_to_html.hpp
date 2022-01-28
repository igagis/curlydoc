/*
curlydoc - document markup language translator

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <sstream>

#include <curlydoc/translator.hpp>

namespace curlydoc{

class translator_to_html : public translator{
public:
	std::stringstream ss;

	translator_to_html();

	void on_word(const std::string& word)override;
	void on_paragraph(const treeml::forest_ext& forest)override;
	void on_bold(const treeml::forest_ext& forest)override;
	void on_italic(const treeml::forest_ext& forest)override;
	void on_underline(const treeml::forest_ext& forest)override;
	void on_strikethrough(const treeml::forest_ext& forest)override;
	void on_monospace(const treeml::forest_ext& forest)override;

	void on_header1(const treeml::forest_ext& forest)override;
	void on_header2(const treeml::forest_ext& forest)override;
	void on_header3(const treeml::forest_ext& forest)override;
	void on_header4(const treeml::forest_ext& forest)override;
	void on_header5(const treeml::forest_ext& forest)override;
	void on_header6(const treeml::forest_ext& forest)override;

	void on_image(const image_params& params, const treeml::forest_ext& forest)override;

	void on_table(const table& tbl, const treeml::forest_ext& forest)override;

	void on_list(const list& l, const treeml::forest_ext& forest)override;
};

}
