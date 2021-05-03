#include "interpreter.hpp"

#include <utki/util.hpp>

using namespace curlydoc;

// namespace{
// const std::string quote = "\"";
// const std::string curly_brace_open = "{";
// const std::string curly_brace_close = "}";
// }

// interpreter::interpreter(std::string&& file_name) :
// 		file_name(std::move(file_name))
// {
// 	this->add_keyword("", [this](bool space, auto& tree){
// 		if(space){
// 			this->on_space();
// 		}
// 		this->eval(tree.children);
// 	});

// 	this->add_keyword("c", [this](bool space, auto& tree){
// 		if(space){
// 			this->on_space();
// 		}
// 		this->handle_char(tree);
// 	});

// 	this->add_keyword("dq", [this](bool space, auto& tree){
// 		if(space){
// 			this->on_space();
// 		}
// 		this->on_word(quote);
// 		this->eval(tree.children);
// 		this->on_word(quote);
// 	});

// 	this->add_keyword("cb", [this](bool space, auto& tree){
// 		if(space){
// 			this->on_space();
// 		}
// 		this->on_word(curly_brace_open);
// 		this->eval(tree.children);
// 		this->on_word(curly_brace_close);
// 	});
// }

// void interpreter::eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end){
// 	for(auto i = begin; i != end; ++i){
// 		bool space = i != begin && i->value.get_info().flags.get(treeml::flag::space);

// 		const auto& string = i->value.to_string();

// 		if(i->children.empty()){
// 			if(space){
// 				this->on_space();
// 			}
// 			this->on_word(string);
// 			continue;
// 		}

// 		auto h = this->handlers.find(string);
// 		if(h != this->handlers.end()){
// 			ASSERT(h->second)
// 			h->second(space, *i);
// 			continue;
// 		}

// 		// TODO: handle macros

// 		std::stringstream ss;
// 		ss << "unknown keyword encountered: " << string;
// 		throw std::invalid_argument(ss.str());
// 	}
// }

void interpreter::add_function(const std::string& name, function_type&& func){
	auto res = this->functions.insert(std::make_pair(std::move(name), std::move(func)));
	if(!res.second){
		std::stringstream ss;
		ss << "function '" << name << "' is already added";
		throw std::logic_error(ss.str());
	}
}

// void interpreter::throw_syntax_error(std::string&& message, const treeml::tree_ext& node){
// 	std::stringstream ss;
// 	ss << this->file_name << ":" << node.value.get_info().location.line << ":" << node.value.get_info().location.offset << ": error: " << message;
// 	throw std::invalid_argument(ss.str());
// }

// void interpreter::handle_char(const treeml::tree_ext& tree){
// 	if(tree.children.size() != 1){
// 		std::stringstream ss;
// 		ss << "'" << tree.value.to_string() << "' command has " << tree.children.size() << "arguments, expected exactly 1";
// 		throw_syntax_error(ss.str(), tree);
// 	}

// 	const auto& str = tree.children.front().value.to_string();

// 	// TODO: is needed? Add ${nbsp} variables for those?

// 	if(str == "wj"){
// 		this->on_word(u8"\u2060");
// 	}else if(str == "nbsp"){
// 		this->on_word(u8"\u00A0");
// 	}else if(str == "sp"){
// 		this->on_space();
// 	}else{
// 		std::stringstream ss;
// 		ss << "unknown argument to 'c' command: " << str;
// 		throw_syntax_error(ss.str(), tree.children.front());
// 	}
// }

void interpreter::context::add(treeml::tree_ext&& var){
	auto i = this->def.insert(
			std::make_pair(var.value.to_string(), std::move(var))
		);
	if(!i.second){
		// TODO: throw exception
		utki::assert(false, SL);
	}
}

interpreter::context::find_result interpreter::context::find(const std::string& name)const{
	auto i = this->def.find(name);
	if(i == this->def.end()){
		if(this->prev){
			return this->prev->find(name);
		}
		// TODO: throw exception
		utki::assert(false, SL);
	}
	return {
		i->second,
		this
	};
}

interpreter::context& interpreter::push_context(const context* prev){
	if(!prev){
		prev = &this->context_stack.back();
	}
	this->context_stack.push_back(context{prev});
	return this->context_stack.back();
}

interpreter::interpreter(){
	this->add_function("asis", [](const treeml::tree_ext& args){
		return args.children;
	});

	this->add_function("", [this](const treeml::tree_ext& args){
		treeml::forest_ext ret;
		treeml::tree_ext tree(args.value);
		tree.children = this->eval(args.children);
		ret.push_back(std::move(tree));
		return ret;
	});

	this->add_function("def", [this](const treeml::tree_ext& args){
		auto& ctx = this->push_context();

		for(const auto& c : args.children){
			treeml::tree_ext var(c.value);
			var.children = this->eval(c.children);
			ctx.add(std::move(var));
		}
		return treeml::forest_ext();
	});

	this->add_function("$", [this](const treeml::tree_ext& args){
		// eval?
		ASSERT(!this->context_stack.empty())

		if(args.children.size() != 1){
			// TODO: log exception
			utki::assert(false, SL);
		}

		auto v = this->context_stack.back().find(args.children.front().value.to_string());
		return v.var.children;
	});
}

treeml::forest_ext interpreter::eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end){
	treeml::forest_ext ret;

	utki::scope_exit context_stack_scope_exit([this, context_stack_size = this->context_stack.size()](){
		this->context_stack.resize(context_stack_size);
	});

	for(auto i = begin; i != end; ++i){
		if(i->children.empty()){
			ret.push_back(*i);
			continue;
		}

		// search for macro
		{

		}

		// search for function
		{
			auto func_i = this->functions.find(i->value.to_string());
			if(func_i == this->functions.end()){
				// TODO: throw exception
				LOG([&](auto&o){o << "function not found: " << i->value.to_string() << '\n';})
				continue;
			}
			auto res = func_i->second(*i);
			LOG([&](auto&o){o << "res.size() = " << res.size() << '\n';})
			ret.insert(
					ret.end(),
					std::make_move_iterator(res.begin()),
					std::make_move_iterator(res.end())
				);
		}
	}

	return ret;
}
