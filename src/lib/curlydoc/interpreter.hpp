#pragma once

#include <unordered_map>
#include <list>
#include <vector>

#include <treeml/tree_ext.hpp>

namespace curlydoc{

class interpreter{
	std::vector<std::string> file_name_stack;

public:
	typedef std::function<treeml::forest_ext(const treeml::forest_ext&)> function_type;

	class exception : public std::invalid_argument{
	public:
		exception(const std::string& message);
		exception(const std::string& message, const std::string& file, const treeml::leaf_ext& leaf);
	};

private:
	std::unordered_map<std::string, function_type> functions;
	
	class context{
		const context* const prev;
		std::unordered_map<std::string, treeml::forest_ext> def;
	public:
		context(const context* const prev = nullptr) : prev(prev){}

		void add(const std::string& name, treeml::forest_ext&& value);

		struct find_result{
			const treeml::forest_ext* value;
			const context& ctx;
		};

		find_result find(const std::string& name)const;
	};

	// NOTE: use std::list to avoid context objects to be moved
	std::list<context> context_stack = {context()};

	context& push_context(const context* prev = nullptr);

	std::vector<bool> if_flag_stack = {false}; // initial flag for root scope
public:
	interpreter(std::string&& file_name);

	virtual ~interpreter(){}

	treeml::forest_ext eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end);

	treeml::forest_ext eval(const treeml::forest_ext& forest){
		return this->eval(forest.begin(), forest.end());
	}

	void add_function(const std::string& name, function_type&& func);
};

}
