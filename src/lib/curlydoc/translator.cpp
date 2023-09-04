/*
curlydoc - document markup language translator

Copyright (C) 2021 Ivan Gagis <igagis@gmail.com>

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

#include "translator.hpp"

#include <utki/util.hpp>

using namespace curlydoc;

namespace {
const std::string double_quote = "\"";
const std::string curly_brace_open = "{";
const std::string curly_brace_close = "}";

const std::string empty_tag = "";
const std::string table_tag = "table";
const std::string cell_tag = "cell";

const std::string list_tag = "list";
const std::string list_item_tag = "li";
} // namespace

translator::translator()
{
	this->add_tag("g", [this](bool space, auto& forest) {
		ASSERT(!forest.empty())
		this->report_space(space);
		this->translate(forest);
	});

	this->add_tag("prm", [](bool space, auto& forest) {
		// ignore
	});

	this->add_tag("dq", [this](bool space, auto& forest) {
		ASSERT(!forest.empty())
		this->report_space(space);
		this->on_word(double_quote);
		this->translate(forest);
		this->on_word(double_quote);
	});

	this->add_tag("cb", [this](bool space, auto& forest) {
		ASSERT(!forest.empty())
		this->report_space(space);
		this->on_word(curly_brace_open);
		this->translate(forest);
		this->on_word(curly_brace_close);
	});

	this->add_tag("p", [this](bool space, auto& forest) {
		this->on_paragraph(forest);
	});

	this->add_tag("b", [this](bool space, auto& forest) {
		this->report_space(space);
		this->on_bold(forest);
	});

	this->add_tag("i", [this](bool space, auto& forest) {
		this->report_space(space);
		this->on_italic(forest);
	});

	this->add_tag("u", [this](bool space, auto& forest) {
		this->report_space(space);
		this->on_underline(forest);
	});

	this->add_tag("s", [this](bool space, auto& forest) {
		this->report_space(space);
		this->on_strikethrough(forest);
	});

	this->add_tag("m", [this](bool space, auto& forest) {
		this->report_space(space);
		this->on_monospace(forest);
	});

	this->add_tag("h1", [this](bool space, auto& forest) {
		this->on_header1(forest);
	});

	this->add_tag("h2", [this](bool space, auto& forest) {
		this->on_header2(forest);
	});

	this->add_tag("h3", [this](bool space, auto& forest) {
		this->on_header3(forest);
	});

	this->add_tag("h4", [this](bool space, auto& forest) {
		this->on_header4(forest);
	});

	this->add_tag("h5", [this](bool space, auto& forest) {
		this->on_header5(forest);
	});

	this->add_tag("h6", [this](bool space, auto& forest) {
		this->on_header6(forest);
	});

	this->add_tag("ins", [this](bool space, auto& forest) {
		this->on_ins(forest);
	});

	this->add_tag("image", [this](bool space, auto& forest) {
		this->report_space(space);
		this->handle_image(forest);
	});

	this->add_tag(table_tag, [this](bool space, auto& forest) {
		this->handle_table(forest);
	});

	this->add_tag(cell_tag, [this](bool space, auto& forest) {
		this->handle_cell(forest);
	});

	this->add_tag(list_tag, [this](bool space, auto& forest) {
		this->handle_list(forest);
	});

	this->add_tag(list_item_tag, [this](bool space, auto& forest) {
		this->handle_list_item(forest);
	});
}

void translator::add_tag(const std::string& tag, handler_type&& func)
{
	auto i = this->handlers.insert(std::make_pair(tag, std::move(func)));
	if (!i.second) {
		throw std::logic_error(std::string("tag '") + tag + "' is already added");
	}
}

const std::string& translator::get_parent_tag() const noexcept
{
	ASSERT(!this->cur_tag.empty())
	for (auto i = std::next(this->cur_tag.rbegin()); i != this->cur_tag.rend(); ++i) {
		if (!i->empty()) {
			// std::cout << "ret = " << *i << '\n';
			return *i;
		}
	}
	return empty_tag;
}

std::vector<std::string> translator::list_tags() const
{
	std::vector<std::string> tags;

	for (const auto& h : this->handlers) {
		if (h.first == "prm") {
			continue; // prm is not a tag, and it is just ignored by translator
		}
		tags.push_back(h.first);
	}

	return tags;
}

void translator::translate(bool space, const treeml::tree_ext& tree)
{
	const auto& tag = tree.value.to_string();

	auto handler_i = this->handlers.find(tag);
	if (handler_i == this->handlers.end()) {
		throw std::invalid_argument(std::string("tag not found: ") + tag);
	}

	this->cur_tag.push_back(tag);
	utki::scope_exit cur_tag_scope_exit([this]() {
		this->cur_tag.pop_back();
	});

	try {
		handler_i->second(space, tree.children);
	} catch (std::exception& e) {
		std::stringstream ss;
		ss << e.what() << " at:" << '\n';
		ss << "    " << tree.value.info.location.line << ":" << tree.value.info.location.offset << ": "
		   << (tag.empty() ? "\"\"" : tag);
		throw std::invalid_argument(ss.str());
	}
}

void translator::translate(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end)
{
	for (auto i = begin; i != end; ++i) {
		bool space = i != begin && i->value.info.flags.get(treeml::flag::space);

		if (i->children.empty()) {
			this->report_space(space);
			this->on_word(i->value.to_string());
			continue;
		}

		this->translate(space, *i);
	}
}

void translator::report_space(bool report)
{
	if (report) {
		this->on_word(" ");
	}
}

bool translator::is_parameters(const treeml::tree_ext& tree) noexcept
{
	return tree == "prm" && !tree.children.empty();
}

void translator::check_parameter(const treeml::tree_ext& tree)
{
	if (tree.children.empty()) {
		throw std::invalid_argument(std::string("no value specified for '") + tree.value.to_string() + "' parameter");
	}
}

void translator::handle_image(const treeml::forest_ext& forest)
{
	image_params params;

	ASSERT(!forest.empty())
	auto i = forest.begin();

	if (is_parameters(*i)) {
		for (const auto& p : i->children) {
			check_parameter(p);

			if (p == "width") {
				params.width = p.children.front().value.to_uint32();
			} else if (p == "height") {
				params.height = p.children.front().value.to_uint32();
			}
		}
		++i;
	}

	if (i == forest.end()) {
		throw std::invalid_argument("image source is not specified");
	}

	params.url = i->value.to_string();

	this->on_image(params, forest);
}

namespace {
translator::align parse_align(const std::string& str)
{
	if (str == "left") {
		return translator::align::left;
	} else if (str == "center") {
		return translator::align::center;
	} else if (str == "right") {
		return translator::align::right;
	}
	throw std::invalid_argument(std::string("unknown alignment keyword: ") + str);
}
} // namespace

namespace {
translator::valign parse_valign(const std::string& str)
{
	if (str == "top") {
		return translator::valign::top;
	} else if (str == "center") {
		return translator::valign::center;
	} else if (str == "bottom") {
		return translator::valign::bottom;
	}
	throw std::invalid_argument(std::string("unknown vertical alignment keyword: ") + str);
}
} // namespace

void translator::handle_table(const treeml::forest_ext& forest)
{
	ASSERT(!forest.empty())
	auto i = forest.begin();

	this->cur_table.emplace_back();
	utki::scope_exit cur_table_scope_exit([this]() {
		this->cur_table.pop_back();
	});

	auto& t = this->cur_table.back();

	if (is_parameters(*i)) {
		for (const auto& p : i->children) {
			check_parameter(p);

			if (p == "cols") {
				t.num_cols = p.children.front().value.to_uint32();
			} else if (p == "border") {
				t.border = p.children.front().value.to_uint32();
			} else if (p == "weight") {
				for (const auto& w : p.children) {
					t.weights.push_back(w.value.to_uint32());
				}
			} else if (p == "align") {
				for (const auto& a : p.children) {
					t.aligns.push_back(parse_align(a.value.to_string()));
				}
			} else if (p == "valign") {
				for (const auto& a : p.children) {
					t.valigns.push_back(parse_valign(a.value.to_string()));
				}
			}
		}
		++i;
	}

	if (t.num_cols == 0) {
		throw std::invalid_argument("table number of columns is not specified");
	}

	if (t.weights.empty()) {
		t.weights.push_back(1);
	}
	while (t.weights.size() < t.num_cols) {
		t.weights.push_back(t.weights.back());
	}

	if (t.aligns.empty()) {
		t.aligns.push_back(align::left);
	}
	while (t.aligns.size() < t.num_cols) {
		t.aligns.push_back(t.aligns.back());
	}

	if (t.valigns.empty()) {
		t.valigns.push_back(valign::top);
	}
	while (t.valigns.size() < t.num_cols) {
		t.valigns.push_back(t.valigns.back());
	}

	this->translate(i, forest.end());

	if (!t.rows.empty()) {
		const auto& last_row = t.rows.back();
		if (last_row.cells.empty() || last_row.span != t.num_cols) {
			throw std::invalid_argument("cells col span values mismatch");
		}
	}

	this->on_table(t, forest);
}

bool translator::table_row::is_free_span(decltype(occupied_cols)::const_iterator iter, size_t span) const noexcept
{
	size_t n = 0;
	for (auto i = iter; n != span; ++i, ++n) {
		if (i == this->occupied_cols.end() || *i) {
			return false;
		}
	}
	return true;
}

bool translator::table_row::is_full() const noexcept
{
	for (const auto& i : this->occupied_cols) {
		if (!i) {
			return false;
		}
	}
	return true;
}

size_t translator::table_row::get_free_col_index(size_t span)
{
	for (auto i = this->occupied_cols.begin(); i != this->occupied_cols.end(); ++i) {
		if (!(*i)) {
			if (this->is_free_span(i, span)) {
				return std::distance(this->occupied_cols.begin(), i);
			}
			break;
		}
	}
	throw std::invalid_argument("cell with requested col span does not fit");
}

void translator::table_row::set_occupied(size_t index, size_t span)
{
	ASSERT(index <= this->occupied_cols.size() - span)
	for (auto i = utki::next(this->occupied_cols.begin(), index);
		 i != utki::next(this->occupied_cols.begin(), index + span);
		 ++i)
	{
		ASSERT(!(*i))
		*i = true;
	}
	this->span += span;
}

void translator::table::push(cell&& c)
{
	ASSERT(this->cur_row <= this->rows.size())

	if (this->cur_row == this->rows.size()) {
		this->rows.emplace_back(this->num_cols);
	}

	auto col_span = c.get_col_span();

	{
		ASSERT(this->cur_row < this->rows.size())
		auto& row = this->rows[this->cur_row];
		c.col_index = row.get_free_col_index(col_span);
	}

	ASSERT(c.get_row_span() >= 1)

	for (size_t i = 0; i != c.get_row_span(); ++i) {
		size_t index = i + this->cur_row;
		ASSERT(index <= this->rows.size())
		if (index == this->rows.size()) {
			this->rows.emplace_back(this->num_cols);
		}

		auto& row = this->rows[index];
		ASSERT(col_span >= 1)
		row.set_occupied(c.col_index, col_span);
	}

	auto& row = this->rows[this->cur_row];

	row.cells.push_back(std::move(c));

	if (row.span > this->num_cols) {
		std::stringstream ss;
		ss << "table number of columns (" << this->num_cols << ") and cells row span (" << row.span
		   << ") mismatch for row #" << (this->cur_row + 1);
		throw std::invalid_argument(ss.str());
	}

	if (row.span == this->num_cols) {
		ASSERT(row.is_full())
		++this->cur_row;
	}
}

void translator::handle_cell(const treeml::forest_ext& forest)
{
	ASSERT(this->cur_tag.size() >= 2)

	if (this->get_parent_tag() != table_tag) {
		throw std::invalid_argument("'cell' tag is only allowed directly inside of 'table' tag");
	}

	cell c;

	ASSERT(!forest.empty())
	auto i = forest.begin();

	if (is_parameters(*i)) {
		for (const auto& p : i->children) {
			check_parameter(p);

			if (p == "span") {
				auto i = p.children.begin();

				c.col_span = i->value.to_uint32();
				++i;
				if (i != p.children.end()) {
					c.row_span = i->value.to_uint32();
				}
			}
		}
		++i;
	}

	if (c.get_col_span() < 1 || c.get_row_span() < 1) {
		throw std::invalid_argument("cell row/col span cannot be 0");
	}

	c.begin = i;
	c.end = forest.end();

	ASSERT(!this->cur_table.empty())
	this->cur_table.back().push(std::move(c));
}

void translator::handle_list(const treeml::forest_ext& forest)
{
	this->cur_list.emplace_back();
	utki::scope_exit cur_list_scope_exit([this]() {
		this->cur_list.pop_back();
	});

	auto& l = this->cur_list.back();

	ASSERT(!forest.empty())
	auto i = forest.begin();

	if (is_parameters(*i)) {
		for (const auto& p : i->children) {
			check_parameter(p);

			if (p == "ordered") {
				l.ordered = p.children.front().value.to_bool();
			}
		}
		++i;
	}

	this->translate(i, forest.end());

	this->on_list(l, forest);
}

void translator::handle_list_item(const treeml::forest_ext& forest)
{
	ASSERT(this->cur_tag.size() >= 2)

	if (this->get_parent_tag() != list_tag) {
		throw std::invalid_argument("'li' tag is only allowed directly inside of 'list' tag");
	}

	ASSERT(!this->cur_list.empty()) // since we have checked the parent tag is 'list'

	this->cur_list.back().items.push_back(forest);
}
