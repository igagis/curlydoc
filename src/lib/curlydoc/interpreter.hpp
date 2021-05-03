#pragma once

#include <unordered_map>
#include <list>

#include <treeml/tree_ext.hpp>

namespace curlydoc{

class interpreter{
	const std::string file_name;
public:
	typedef std::function<treeml::forest_ext(const treeml::tree_ext&)> function_type;
private:
	std::unordered_map<std::string, function_type> functions;
	
	class context{
		const context* const prev;
		std::unordered_map<std::string, treeml::tree_ext> def;
	public:
		context(const context* const prev = nullptr) : prev(prev){}

		void add(treeml::tree_ext&& var);

		struct find_result{
			const treeml::tree_ext* var;
			const context& ctx;
		};

		find_result find(const std::string& name)const;
	};

	// NOTE: use std::list to avoid context objects to be moved
	std::list<context> context_stack = {context()};

	context& push_context(const context* prev = nullptr);

	// void handle_char(const treeml::tree_ext& tree);
protected:
	// void throw_syntax_error(std::string&& message, const treeml::tree_ext& node);

public:
	interpreter();

	virtual ~interpreter(){}

	treeml::forest_ext eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end);

	treeml::forest_ext eval(const treeml::forest_ext& forest){
		return this->eval(forest.begin(), forest.end());
	}

	void add_function(const std::string& name, function_type&& func);

	// virtual void on_word(const std::string& word) = 0;

	// void on_space(){
	// 	this->on_word(" ");
	// }
};

}
