#include "translator_to_html.hpp"

#include <optional>

using namespace curlydoc;

translator_to_html::translator_to_html(){
	// TODO: ?
}

void translator_to_html::on_word(const std::string& word){
	this->ss << word;
}

void translator_to_html::on_paragraph(const treeml::forest_ext& forest){
	this->ss << '\n' << "<p>";
	this->translate(forest);
	this->ss << "</p>";
}

void translator_to_html::on_bold(const treeml::forest_ext& forest){
	this->ss << "<b>";
	this->translate(forest);
	this->ss << "</b>";
}

void translator_to_html::on_italic(const treeml::forest_ext& forest){
	this->ss << "<i>";
	this->translate(forest);
	this->ss << "</i>";
}

void translator_to_html::on_underline(const treeml::forest_ext& forest){
	this->ss << "<u>";
	this->translate(forest);
	this->ss << "</u>";
}

void translator_to_html::on_strikethrough(const treeml::forest_ext& forest){
	this->ss << "<s>";
	this->translate(forest);
	this->ss << "</s>";
}

void translator_to_html::on_monospace(const treeml::forest_ext& forest){
	this->ss << "<code>";
	this->translate(forest);
	this->ss << "</code>";
}

void translator_to_html::on_header1(const treeml::forest_ext& forest){
	this->ss << '\n' << "<h1>";
	this->translate(forest);
	this->ss << "</h1>";
}

void translator_to_html::on_header2(const treeml::forest_ext& forest){
	this->ss << '\n' << "<h2>";
	this->translate(forest);
	this->ss << "</h2>";
}

void translator_to_html::on_header3(const treeml::forest_ext& forest){
	this->ss << '\n' << "<h3>";
	this->translate(forest);
	this->ss << "</h3>";
}

void translator_to_html::on_header4(const treeml::forest_ext& forest){
	this->ss << '\n' << "<h4>";
	this->translate(forest);
	this->ss << "</h4>";
}

void translator_to_html::on_header5(const treeml::forest_ext& forest){
	this->ss << '\n' << "<h5>";
	this->translate(forest);
	this->ss << "</h5>";
}

void translator_to_html::on_header6(const treeml::forest_ext& forest){
	this->ss << '\n' << "<h6>";
	this->translate(forest);
	this->ss << "</h6>";
}

void translator_to_html::on_image(const image_params& params, const treeml::forest_ext& forest){
	this->ss << "<img src=\"" << params.url << "\"";

	if(params.width){
		this->ss << " width=\"" << params.width.value() << "\"";
	}

	if(params.height){
		this->ss << " height=\"" << params.height.value() << "\"";
	}

	this->ss << "/>";
}

void translator_to_html::on_table(const table& tbl, const treeml::forest_ext& forest){
	this->ss << '\n' << "<table width=\"100%\">";

	bool is_first_row = true;
	for(const auto& r : tbl.rows){
		this->ss << '\n' << "<tr>";

		const char* td = is_first_row ? "th" : "td";
		is_first_row = false;

		for(const auto& c : r.cells){
			this->ss << '\n' << '<' << td;
			const auto& col_span = std::get<0>(c.span);
			const auto& row_span = std::get<1>(c.span);
			ASSERT(col_span >= 1)
			ASSERT(row_span >= 1)
			if(col_span != 1){
				this->ss << " colspan=\"" << col_span << '\"';
			}
			if(row_span != 1){
				this->ss << " rowspan=\"" << row_span << '\"';
			}
			this->ss << '>';
			this->translate(c.begin, c.end);
			this->ss << "</" << td << '>';
		}
		this->ss << '\n' << "</tr>";
	}

	this->ss << '\n' << "</table>";
}
