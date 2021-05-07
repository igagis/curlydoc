#include "translator.hpp"

using namespace curlydoc;

namespace{
const std::string double_quote = "\"";
const std::string curly_brace_open = "{";
const std::string curly_brace_close = "}";

const std::string empty_tag = "";
const std::string table_tag = "table";
const std::string cell_tag = "cell";
}

translator::translator(){
    this->add_tag("", [this](bool space, auto& forest){
        ASSERT(!forest.empty())
        this->report_space(space);
        this->translate(forest);
    });

	this->add_tag("opt", [](bool space, auto& forest){
		// ignore
	});

	this->add_tag("dq", [this](bool space, auto& forest){
		ASSERT(!forest.empty())
		this->report_space(space);
		this->on_word(double_quote);
		this->translate(forest);
		this->on_word(double_quote);
	});

	this->add_tag("cb", [this](bool space, auto& forest){
		ASSERT(!forest.empty())
		this->report_space(space);
		this->on_word(curly_brace_open);
		this->translate(forest);
		this->on_word(curly_brace_close);
	});

	this->add_tag("p", [this](bool space, auto& forest){
		this->on_paragraph(forest);
	});

	{
		auto f = [this](bool space, auto& forest){
            this->report_space(space);
			this->on_bold(forest);
		};
		this->add_tag("*", f);
		this->add_tag("b", f);
	}

	{
		auto f = [this](bool space, auto& forest){
            this->report_space(space);
			this->on_italic(forest);
		};
		this->add_tag("/", f);
		this->add_tag("i", f);
	}

	{
		auto f = [this](bool space, auto& forest){
            this->report_space(space);
			this->on_underline(forest);
		};
		this->add_tag("_", f);
		this->add_tag("u", f);
	}

	{
		auto f = [this](bool space, auto& forest){
            this->report_space(space);
			this->on_strikethrough(forest);
		};
		this->add_tag("~", f);
		this->add_tag("s", f);
	}

	{
		auto f = [this](bool space, auto& forest){
            this->report_space(space);
			this->on_monospace(forest);
		};
		this->add_tag("`", f);
		this->add_tag("m", f);
	}

	{
		auto f = [this](bool space, auto& forest){
			this->on_header1(forest);
		};
		this->add_tag("=", f);
		this->add_tag("h1", f);
	}

	{
		auto f = [this](bool space, auto& forest){
			this->on_header2(forest);
		};
		this->add_tag("==", f);
		this->add_tag("h2", f);
	}

	{
		auto f = [this](bool space, auto& forest){
			this->on_header3(forest);
		};
		this->add_tag("===", f);
		this->add_tag("h3", f);
	}

	{
		auto f = [this](bool space, auto& forest){
			this->on_header4(forest);
		};
		this->add_tag("====", f);
		this->add_tag("h4", f);
	}
	
	{
		auto f = [this](bool space, auto& forest){
			this->on_header5(forest);
		};
		this->add_tag("=====", f);
		this->add_tag("h5", f);
	}

	{
		auto f = [this](bool space, auto& forest){
			this->on_header6(forest);
		};
		this->add_tag("======", f);
		this->add_tag("h6", f);
	}
	
	this->add_tag("image", [this](bool space, auto& forest){
		this->report_space(space);
		this->handle_image(forest);
	});

	this->add_tag(table_tag, [this](bool space, auto& forest){
		this->handle_table(forest);
	});

	{
		auto f = [this](bool space, auto& forest){
			this->handle_cell(forest);
		};
		this->add_tag("|", f);
		this->add_tag("cell", f);
	}
}

void translator::add_tag(const std::string& tag, handler_type&& func){
    auto i = this->handlers.insert(std::make_pair(tag, std::move(func)));
    if(!i.second){
        throw std::logic_error(std::string("tag '") + tag + "' is already added");
    }
}

const std::string& translator::get_parent_tag()const noexcept{
	ASSERT(!this->cur_tag.empty())
	for(auto i = std::next(this->cur_tag.rbegin()); i != this->cur_tag.rend(); ++i){
		if(!i->empty()){
			// std::cout << "ret = " << *i << '\n';
			return *i;
		}
	}
	return empty_tag;
}

std::vector<std::string> translator::list_tags()const{
	std::vector<std::string> tags;

	for(const auto& h : this->handlers){
		if(h.first == "opt"){
			continue; // opt is not a tag, and it is just ignored by translator
		}
		tags.push_back(h.first);
	}

	return tags;
}

void translator::translate(bool space, const treeml::tree_ext& tree){
	const auto& tag = tree.value.to_string();

	auto handler_i = this->handlers.find(tag);
	if(handler_i == this->handlers.end()){
		throw std::invalid_argument(std::string("tag not found: ") + tag);
	}

	this->cur_tag.push_back(tag);
	utki::scope_exit cur_tag_scope_exit([this](){
		this->cur_tag.pop_back();
	});

	try{
		handler_i->second(space, tree.children);
	}catch(std::exception& e){
		std::stringstream ss;
		ss << e.what() << " at:" << '\n';
		ss << "    " << tree.value.info.location.line << ":" << tree.value.info.location.offset << ": " << (tag.empty() ? "\"\"" : tag);
		throw std::invalid_argument(ss.str());
	}
}

void translator::translate(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end){
    for(auto i = begin; i != end; ++i){
        bool space = i != begin && i->value.info.flags.get(treeml::flag::space);

        if(i->children.empty()){
            this->report_space(space);
            this->on_word(i->value.to_string());
            continue;
        }

		this->translate(space, *i);
    }
}

void translator::report_space(bool report){
    if(report){
        this->on_word(" ");
    }
}

bool translator::is_options(const treeml::tree_ext& tree)noexcept{
	return tree == "opt" && !tree.children.empty();
}

void translator::check_option(const treeml::tree_ext& tree){
	if(tree.children.empty()){
		throw std::invalid_argument(std::string("no value specified for '") + tree.value.to_string() + "' parameter");
	}
}

void translator::handle_image(const treeml::forest_ext& forest){
	image_params params;

	ASSERT(!forest.empty())
	auto i = forest.begin();

	if(is_options(*i)){
		for(const auto& p : i->children){
			check_option(p);

			if(p == "width"){
				params.width = p.children.front().value.to_uint32();
			}else if(p == "height"){
				params.height = p.children.front().value.to_uint32();
			}
		}
		++i;
	}

	if(i == forest.end()){
		throw std::invalid_argument("image source is not specified");
	}

	params.url = i->value.to_string();

	this->on_image(params, forest);
}

void translator::handle_table(const treeml::forest_ext& forest){
	ASSERT(!forest.empty())
	auto i = forest.begin();

	this->cur_table.emplace_back();
	utki::scope_exit cur_table_scope_exit([this](){
		this->cur_table.pop_back();
	});

	auto& tbl = this->cur_table.back();

	if(is_options(*i)){
		for(const auto& p : i->children){
			check_option(p);

			if(p == "cols"){
				tbl.num_cols = p.children.front().value.to_uint32();
			}else if(p == "border"){
				tbl.border = p.children.front().value.to_uint32();
			}else if(p == "weight"){
				for(const auto& w : p.children){
					tbl.weights.push_back(w.value.to_uint32());
				}
			}
		}
		++i;
	}

	if(tbl.num_cols == 0){
		throw std::invalid_argument("table number of columns is not specified");
	}

	if(tbl.weights.empty()){
		tbl.weights.push_back(1);
	}

	while(tbl.weights.size() < tbl.num_cols){
		tbl.weights.push_back(tbl.weights.back());
	}

	this->translate(i, forest.end());

	if(!tbl.rows.empty()){
		const auto& last_row = tbl.rows.back();
		if(last_row.cells.empty() || last_row.span != tbl.num_cols){
			throw std::invalid_argument("cells col span values mismatch, table has empty rows");
		}
	}

	this->on_table(tbl, forest);
}

void translator::table::push(cell&& c){
	ASSERT(this->cur_row <= this->rows.size())

	ASSERT(c.get_row_span() >= 1)

	for(size_t i = 0; i != c.get_row_span(); ++i){
		size_t index = i + this->cur_row;
		ASSERT(index <= this->rows.size())
		if(index == this->rows.size()){
			this->rows.emplace_back();
		}

		auto& row = this->rows[index];
		ASSERT(c.get_col_span() >= 1)
		row.span += c.get_col_span();
	}

	ASSERT(this->cur_row < this->rows.size())
	auto& row = this->rows[this->cur_row];

	row.cells.push_back(std::move(c));

	if(row.span > this->num_cols){
		std::stringstream ss;
		ss << "table number of columns (" << this->num_cols << ") and cells row span (" << row.span << ") mismatch";
		throw std::invalid_argument(ss.str());
	}

	if(row.span == this->num_cols){
		++this->cur_row;
	}
}

void translator::handle_cell(const treeml::forest_ext& forest){
	ASSERT(this->cur_tag.size() >= 2)

	if(this->get_parent_tag() != table_tag){
		throw std::invalid_argument("'cell' tag is only allowed directly inside of 'table' tag");
	}

	cell c;

	ASSERT(!forest.empty())
	auto i = forest.begin();

	if(is_options(*i)){
		for(const auto& p : i->children){
			check_option(p);

			if(p == "span"){
				auto i = p.children.begin();

				c.col_span = i->value.to_uint32();
				++i;
				if(i != p.children.end()){
					c.row_span = i->value.to_uint32();
				}
			}
		}
		++i;
	}

	if(c.get_col_span() < 1 || c.get_row_span() < 1){
		throw std::invalid_argument("cell row/col span cannot be 0");
	}

	c.begin = i;
	c.end = forest.end();

	ASSERT(!this->cur_table.empty())
	this->cur_table.back().push(std::move(c));
}
