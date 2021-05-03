#include "interpreter.hpp"

#include <utki/util.hpp>

using namespace curlydoc;

interpreter::exception::exception(const std::string& message, const std::string& file, const treeml::leaf_ext& leaf) :
		std::invalid_argument([&](){
			const auto& l = leaf.get_info().location;
			std::stringstream ss;
			ss << message << '\n';
			ss << "  " << file << ":" << l.line << ":" << l.offset << ": " << leaf.to_string();
			return ss.str();
		}())
{}

void interpreter::add_function(const std::string& name, function_type&& func){
	auto res = this->functions.insert(std::make_pair(std::move(name), std::move(func)));
	if(!res.second){
		std::stringstream ss;
		ss << "function '" << name << "' is already added";
		throw std::logic_error(ss.str());
	}
}

void interpreter::context::add(const std::string& name, treeml::forest_ext&& value){
	auto i = this->def.insert(
			std::make_pair(name, std::move(value))
		);
	if(!i.second){
		throw exception("variable name already exists in this context");
	}
}

interpreter::context::find_result interpreter::context::find(const std::string& name)const{
	auto i = this->def.find(name);
	if(i == this->def.end()){
		if(this->prev){
			return this->prev->find(name);
		}
		return {
			nullptr,
			*this
		};
	}
	ASSERT(this->prev)
	return {
		&i->second,
		*this->prev
	};
}

interpreter::context& interpreter::push_context(const context* prev){
	if(!prev){
		prev = &this->context_stack.back();
	}
	this->context_stack.push_back(context{prev});
	return this->context_stack.back();
}

interpreter::interpreter(std::string&& file_name) :
		file_name_stack{file_name}
{
	this->add_function("asis", [](const treeml::forest_ext& args){
		return args;
	});

	this->add_function("", [this](const treeml::forest_ext& args){
		treeml::forest_ext ret;
		treeml::tree_ext t(std::string(""));
		t.children = this->eval(args);
		ret.push_back(std::move(t));
		return ret;
	});

	this->add_function("def", [this](const treeml::forest_ext& args){
		auto& ctx = this->push_context();

		for(const auto& c : args){
			try{
				ctx.add(c.value.to_string(), this->eval(c.children));
			}catch(exception& e){
				throw exception(e.what(), this->file_name_stack.back(), c.value);
			}
		}
		return treeml::forest_ext();
	});

	this->add_function("$", [this](const treeml::forest_ext& args){
		ASSERT(!this->context_stack.empty())

		if(args.size() != 1){
			throw exception("$ expects 1 or 2 arguments");
		}

		const auto& name = args.front().value.to_string();

		auto v = this->context_stack.back().find(name);

		if(!v.value){
			throw exception(std::string("variable '") + name +"' not found");
		}

		return *v.value;
	});
}

treeml::forest_ext interpreter::eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end){
	treeml::forest_ext ret;

	utki::scope_exit context_stack_scope_exit([this, context_stack_size = this->context_stack.size()](){
		this->context_stack.resize(context_stack_size);
	});

	for(auto i = begin; i != end; ++i){
		try{
			if(i->children.empty()){
				ret.push_back(*i);
				continue;
			}

			treeml::forest_ext output;

			// search for macro
			auto v = this->context_stack.back().find(i->value.to_string());
			if(v.value){
				auto args = this->eval(i->children);

				auto& ctx = this->push_context(&v.ctx);
				utki::scope_exit macro_context_scope_exit([this](){
					this->context_stack.pop_back();
				});

				try{
					ctx.add("@", std::move(args));
				}catch(exception&){
					ASSERT(false)
				}

				output = this->eval(*v.value);
			}else{
				// search for function

				auto func_i = this->functions.find(i->value.to_string());
				if(func_i == this->functions.end()){
					throw exception(std::string("function/macro '") + i->value.to_string() + "' not found");
				}
				output = func_i->second(i->children);
			}

			LOG([&](auto&o){o << "output.size() = " << output.size() << '\n';})
			ret.insert(
					ret.end(),
					std::make_move_iterator(output.begin()),
					std::make_move_iterator(output.end())
				);
		}catch(exception& e){
			throw exception(e.what(), this->file_name_stack.back(), i->value);
		}
	}

	return ret;
}
