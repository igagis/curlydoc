#include "interpreter.hpp"

using namespace curlydoc;

namespace{
const std::string quote = "\"";
const std::string curly_brace_open = "{";
const std::string curly_brace_close = "}";
}

interpreter::interpreter(std::string&& file_name) :
		file_name(std::move(file_name))
{
	this->add_keyword("", [this](bool space, auto& tree){
		if(space){
			this->on_space();
		}
		this->eval(tree.children);
	});

	this->add_keyword("c", [this](bool space, auto& tree){
		if(space){
			this->on_space();
		}
		this->handle_char(tree);
	});

	this->add_keyword("q", [this](bool space, auto& tree){
		if(space){
			this->on_space();
		}
		this->on_word(quote);
		this->eval(tree.children);
		this->on_word(quote);
	});

	this->add_keyword("cb", [this](bool space, auto& tree){
		if(space){
			this->on_space();
		}
		this->on_word(curly_brace_open);
		this->eval(tree.children);
		this->on_word(curly_brace_close);
	});
}

void interpreter::eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end){
	for(auto i = begin; i != end; ++i){
		bool space = i != begin && i->value.get_info().flags.get(treeml::flag::space);

		const auto& string = i->value.to_string();

		if(i->children.empty()){
			if(space){
				this->on_space();
			}
			this->on_word(string);
			continue;
		}

		auto h = this->handlers.find(string);
		if(h != this->handlers.end()){
			ASSERT(h->second)
			h->second(space, *i);
			continue;
		}

		// TODO: handle macros

		std::stringstream ss;
		ss << "unknown keyword encountered: " << string;
		throw std::invalid_argument(ss.str());
	}
}

void interpreter::add_keyword(const std::string& keyword, keyword_handler_type&& handler){
	auto res = this->handlers.insert(std::make_pair(std::move(keyword), std::move(handler)));
	if(!res.second){
		std::stringstream ss;
		ss << "keyword '" << keyword << "' is already added";
		throw std::logic_error(ss.str());
	}
}

void interpreter::throw_syntax_error(std::string&& message, const treeml::tree_ext& node){
	std::stringstream ss;
	ss << this->file_name << ":" << node.value.get_info().location.line << ":" << node.value.get_info().location.offset << ": error: " << message;
	throw std::invalid_argument(ss.str());
}

void interpreter::handle_char(const treeml::tree_ext& tree){
	if(tree.children.size() != 1){
		std::stringstream ss;
		ss << "'" << tree.value.to_string() << "' command has " << tree.children.size() << "arguments, expected exactly 1";
		throw_syntax_error(ss.str(), tree);
	}

	const auto& str = tree.children.front().value.to_string();

	// TODO: is needed? Add ${nbsp} variables for those?

	if(str == "wj"){
		this->on_word(u8"\u2060");
	}else if(str == "nbsp"){
		this->on_word(u8"\u00A0");
	}else if(str == "sp"){
		this->on_space();
	}else{
		std::stringstream ss;
		ss << "unknown argument to 'c' command: " << str;
		throw_syntax_error(ss.str(), tree.children.front());
	}
}