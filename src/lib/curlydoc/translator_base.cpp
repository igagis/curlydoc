#include "translator_base.hpp"

using namespace curlydoc;

void translator_base::translate(treeml::forest::const_iterator begin, treeml::forest::const_iterator end){
	for(auto i = begin; i != end; ++i){
		if(i != begin){
			// TODO: if word starts with space or ends with space, no need to handle space
			this->handle_space();
		}

		const auto& string = i->value.to_string();

		if(i->children.empty()){
			this->handle_word(string);
			continue;
		}

		auto h = this->handlers.find(string);
		if(h != this->handlers.end()){
			ASSERT(h->second)
			h->second(i->children);
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
