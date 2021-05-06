#include "translator.hpp"

using namespace curlydoc;

namespace{
const std::string double_quote = "\"";
const std::string curly_brace_open = "{";
const std::string curly_brace_close = "}";
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

	this->add_tag("table", [this](bool space, auto& forest){
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

std::vector<std::string> translator::list_tags()const{
	std::vector<std::string> tags;

	for(const auto& h : this->handlers){
		if(h.first.empty() || h.first == "opt"){
			continue;
		}
		tags.push_back(h.first);
	}

	return tags;
}

void translator::translate(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end){
    for(auto i = begin; i != end; ++i){
        bool space = i != begin && i->value.info.flags.get(treeml::flag::space);

        if(i->children.empty()){
            this->report_space(space);
            this->on_word(i->value.to_string());
            continue;
        }

        auto handler_i = this->handlers.find(i->value.to_string());
        if(handler_i == this->handlers.end()){
            throw std::invalid_argument(std::string("tag not found: ") + i->value.to_string());
        }

        handler_i->second(space, i->children);
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
	table_params params;

	ASSERT(!forest.empty())
	auto i = forest.begin();

	if(is_options(*i)){
		for(const auto& p : i->children){
			check_option(p);

			if(p == "cols"){
				params.num_cols = p.children.front().value.to_uint32();
			}
		}
	}

	if(params.num_cols == 0){
		throw std::invalid_argument("table number of columns is not specified");
	}

	this->on_table(params, forest);
}

void translator::handle_cell(const treeml::forest_ext& forest){
	cell_params params;

	ASSERT(!forest.empty())
	auto i = forest.begin();

	if(is_options(*i)){
		for(const auto& p : i->children){
			check_option(p);

			if(p == "span"){
				auto i = p.children.begin();

				params.row_span = i->value.to_uint32();
				++i;
				if(i != p.children.end()){
					params.col_span = i->value.to_uint32();
				}
			}
		}
	}

	this->on_cell(params, forest);
}
