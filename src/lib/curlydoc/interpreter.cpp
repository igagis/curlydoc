#include "interpreter.hpp"

#include <utki/util.hpp>

using namespace curlydoc;

interpreter::exception::exception(const std::string& message) :
		std::invalid_argument(message + " at:")
{}

interpreter::exception::exception(const std::string& message, const std::string& file, const treeml::leaf_ext& leaf) :
		std::invalid_argument([&](){
			const auto& l = leaf.info.location;
			std::stringstream ss;
			ss << message << '\n';
			ss << "    " << file << ":" << l.line << ":" << l.offset << ": " << leaf.to_string();
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

void interpreter::add_repeater_function(const std::string& name){
	this->add_function(name, nullptr);
}

void interpreter::add_repeater_functions(utki::span<const std::string> names){
	for(const auto& n : names){
		if(n.empty()){
			continue; // empty repeater is already registered
		}
		this->add_repeater_function(n);
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

interpreter::context::find_result interpreter::context::try_find(const std::string& name)const{
	auto i = this->def.find(name);
	if(i == this->def.end()){
		if(this->prev){
			return this->prev->try_find(name);
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

const treeml::forest_ext& interpreter::context::find(const std::string& name)const{
	auto v = this->try_find(name);

	if(!v.value){
		throw exception(std::string("variable '") + name +"' not found");
	}

	return *v.value;
}

interpreter::context& interpreter::push_context(const context* prev){
	if(!prev){
		prev = &this->context_stack.back();
	}
	this->context_stack.push_back(context{prev});
	return this->context_stack.back();
}

interpreter::interpreter(std::unique_ptr<papki::file> file) :
		file_name_stack{"unknown"},
		file(std::move(file))
{
	this->add_repeater_function("");

	this->add_function("asis", [](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		return args;
	});

	this->add_function("map", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		treeml::forest_ext ret;

		for(const auto& a : args){
			ret.push_back(treeml::tree_ext(a.value, this->eval(a.children)));
		}

		return ret;
	});

	this->add_function("opt", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		treeml::forest_ext ret;
		ret.emplace_back("opt");

		for(const auto& a : args){
			ret.back().children.push_back(treeml::tree_ext(a.value, this->eval(a.children)));
		}

		return ret;
	});

	this->add_function("def", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

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
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		ASSERT(!this->context_stack.empty())

		auto res = this->eval(args);

		if(res.empty()){
			throw exception("variable name is not given");
		}
		if(res.size() != 1){
			throw exception("more than one variable name given");
		}

		const auto& name = res.front().value.to_string();

		const auto& val = this->context_stack.back().find(name);

		return val;
	});

	this->add_function("for", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		auto iter_name = args[0].value.to_string();
		auto iter_values = this->eval(args[0].children);

		treeml::forest_ext ret;

		for(const auto& i : iter_values){
			auto& ctx = this->push_context();
			utki::scope_exit context_scope_exit([this](){
				this->context_stack.pop_back();
			});

			try{
				ctx.add(iter_name, {i});
			}catch(exception&){
				ASSERT(false)
			}

			auto output = this->eval(std::next(args.begin()), args.end());

			ret.insert(ret.end(), std::make_move_iterator(output.begin()), std::make_move_iterator(output.end()));
		}

		return ret;
	});

	this->add_function("if", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		bool flag;
		{
			if_flag_push if_flag_push(*this);
			flag = !this->eval(args).empty();
		}

		auto& bs = this->if_flag_stack.back();
		bs.flag = flag;
		bs.true_before_or = false;

		return treeml::forest_ext();
	});

	this->add_function("then", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		auto& bs = this->if_flag_stack.back();

		if(!bs.flag){
			return treeml::forest_ext();
		}

		if_flag_push if_flag_push(*this);

		return this->eval(args);
	});

	this->add_function("else", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		auto& bs = this->if_flag_stack.back();

		if(bs.flag){
			return treeml::forest_ext();
		}

		if_flag_push if_flag_push(*this);

		return this->eval(args);
	});

	this->add_function("and", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		{
			auto& bs = this->if_flag_stack.back();

			if(!bs.flag || bs.true_before_or){
				return treeml::forest_ext();
			}
		}

		bool flag;
		{
			if_flag_push if_flag_push(*this);
			flag = !this->eval(args).empty();
		}

		this->if_flag_stack.back().flag = flag;

		return treeml::forest_ext();
	});

	this->add_function("or", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		{
			auto& bs = this->if_flag_stack.back();

			if(bs.flag){
				bs.true_before_or = true;
				return treeml::forest_ext();
			}
		}

		bool flag;
		{
			if_flag_push if_flag_push(*this);
			flag = !this->eval(args).empty();
		}

		this->if_flag_stack.back().flag = flag;

		return treeml::forest_ext();
	});

	this->add_function("not", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		if(this->eval(args).empty()){
			return treeml::forest_ext{{"true"}};
		}

		return treeml::forest_ext();
	});

	this->add_function("include", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		if(!this->file){
			throw exception("include is not supported");
		}

		this->file->set_path(args.front().value.to_string());

		this->file_name_stack.push_back(this->file->path());
		utki::scope_exit file_name_stack_scope_exit([this](){
			this->file_name_stack.pop_back();
		});

		return this->eval(
				treeml::read_ext(*this->file),
				true
			);
	});

	this->add_function("size", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		// TODO: optimize for the case of size{${v}}, since variables are stored evaluated, no need to copy variable contents via eval()

		auto res = this->eval(args);

		return treeml::forest_ext{{std::to_string(res.size())}};
	});

	this->add_function("at", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		// TODO: optimize for the case of at{some_expr ${v}}, since variables are stored evaluated, no need to copy variable contents via eval()

		auto res = this->eval(args);

		if(res.empty()){
			throw exception("no index argument is given to 'at' function");
		}

		int64_t index = res.front().value.to_int64();

		int64_t size = res.size() - 1;

		if(index < 0){
			index = size  + index;
		}

		if(index < 0 || size <= index){
			std::stringstream ss;
			ss << "array index (" << index << ") out of bounds (" << size << ")";
			throw exception(ss.str());
		}

		return treeml::forest_ext{res[index + 1]};
	});

	this->add_function("get", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		// TODO: optimize for the case of get{some_expr ${v}}, since variables are stored evaluated, no need to copy variable contents via eval()

		auto res = this->eval(args);

		if(res.empty()){
			throw exception("no key argument is given to 'get' function");
		}

		const auto& key = res.front().value.to_string();

		for(auto i = std::next(res.begin()); i != res.end(); ++i){
			if(i->value == key){
				return i->children;
			}
		}

		std::stringstream ss;
		ss << "key (" << key << ") not found";
		throw exception(ss.str());
	});

	this->add_function("is_word", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		auto res = this->eval(args);

		if(res.size() == 1 && res.front().children.empty()){
			return treeml::forest_ext{{"true"}};
		}
		return treeml::forest_ext();
	});

	this->add_function("val", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		treeml::forest_ext ret;

		auto evaled = this->eval(args);

		if(evaled.size() == 1){
			ret.push_back(treeml::tree_ext(evaled.front().value));
		}else if(!evaled.empty()){
			throw exception("more than one value passed to 'val' function");
		}

		return ret;
	});

	this->add_function("args", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		auto evaled = this->eval(args);

		if(evaled.size() == 1){
			return evaled.front().children;
		}else if(!evaled.empty()){
			throw exception("more than one value passed to 'val' function");
		}

		return treeml::forest_ext();
	});

	this->add_function("eq", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		auto res = this->eval(args);

		if(res.size() != 2){
			throw exception("'eq' function requires exactly 2 arguments");
		}

		if(res.front() == res.back()){
			return treeml::forest_ext{{"true"}};
		}
		return treeml::forest_ext();
	});

	this->add_function("gt", [this](const treeml::forest_ext& args){
		ASSERT(!args.empty()) // if there are no arguments, then it is not a function call

		auto res = this->eval(args);

		if(res.size() != 2){
			throw exception("'eq' function requires exactly 2 arguments");
		}

		if(res.front().value.to_int64() > res.back().value.to_int64()){
			return treeml::forest_ext{{"true"}};
		}
		return treeml::forest_ext();
	});
}

treeml::forest_ext interpreter::eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end, bool preserve_vars){
	treeml::forest_ext ret;

	utki::scope_exit context_stack_scope_exit([this, context_stack_size = this->context_stack.size(), preserve_vars](){
		if(!preserve_vars){
			this->context_stack.resize(context_stack_size);
		}
	});

	for(auto i = begin; i != end; ++i){
		try{
			if(i->children.empty()){
				ret.push_back(*i);
				continue;
			}

			treeml::forest_ext output;

			// search for macro
			auto v = this->context_stack.back().try_find(i->value.to_string());
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

				if(func_i->second){
					output = func_i->second(i->children);

					if(!output.empty()){
						output.front().value.info.flags.set(
								treeml::flag::space,
								i->value.info.flags.get(treeml::flag::space)
							);
					}
				}else{
					// repeater function, which means the tag remains as is, but its children are evaluated
					output.emplace_back(i->value);
					output.back().children = this->eval(i->children);
				}
			}

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

treeml::forest_ext interpreter::eval(){
	if(!this->file){
		throw std::logic_error("no file interface provided");
	}

	auto forest = treeml::read_ext(*this->file);

	this->file_name_stack.push_back(this->file->path());
	utki::scope_exit file_name_stack_scope_exit([this](){
		this->file_name_stack.pop_back();
	});

	return this->eval(forest);
}
