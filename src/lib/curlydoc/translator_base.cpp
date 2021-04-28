#include "translator_base.hpp"

using namespace curlydoc;

namespace{
const std::string quote = "\"";
}

translator_base::translator_base(std::string&& file_name) :
		file_name(std::move(file_name))
{
	this->add_keyword("c", [this](auto& tree){
		this->handle_char(tree);
	});
}

void translator_base::translate(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end){
	for(auto i = begin; i != end; ++i){
		if(i == begin){
			this->is_first_node = true;
		}else{
			this->is_first_node = false;
		}

		const auto& string = i->value.to_string();

		if(i->children.empty()){
			if(!this->is_first() && i->value.get_info().flags.get(treeml::flag::space)){
				this->handle_space();
			}
			
			if(i->value.get_info().flags.get(treeml::flag::quoted)){
				this->handle_word(quote);
				this->handle_word(string);
				this->handle_word(quote);
			}else{
				this->handle_word(string);
			}
			continue;
		}

		auto h = this->handlers.find(string);
		if(h != this->handlers.end()){
			ASSERT(h->second)
			h->second(*i);
			continue;
		}

		// TODO: handle macros

		std::stringstream ss;
		ss << "unknown keyword encountered: " << string;
		throw std::invalid_argument(ss.str());
	}
}

void translator_base::add_keyword(const std::string& keyword, keyword_handler_type&& handler){
	auto res = this->handlers.insert(std::make_pair(std::move(keyword), std::move(handler)));
	if(!res.second){
		std::stringstream ss;
		ss << "keyword '" << keyword << "' is already added";
		throw std::logic_error(ss.str());
	}
}

void translator_base::throw_syntax_error(std::string&& message, const treeml::tree_ext& node){
	std::stringstream ss;
	ss << this->file_name << ":" << node.value.get_info().location.line << ":" << node.value.get_info().location.offset << ": error: " << message;
	throw std::invalid_argument(ss.str());
}

void translator_base::handle_char(const treeml::tree_ext& tree){
	if(tree.children.size() != 1){
		std::stringstream ss;
		ss << "'" << tree.value.to_string() << "' command has " << tree.children.size() << "arguments, expected exactly 1";
		throw_syntax_error(ss.str(), tree);
	}

	const auto& str = tree.children.front().value.to_string();

	if(str == "wj"){
		this->handle_word(u8"\u2060");
	}else if(str == "nbsp"){
		this->handle_word(u8"\u00A0");
	}else if(str == "sp"){
		this->handle_space();
	}
}