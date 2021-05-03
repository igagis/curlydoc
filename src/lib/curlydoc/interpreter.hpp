#pragma once

#include <unordered_map>
#include <vector>

#include <treeml/tree_ext.hpp>

namespace curlydoc{

class interpreter{
	const std::string file_name;
public:
	typedef std::function<treeml::forest_ext(const treeml::tree_ext&)> function_type;
private:
	std::unordered_map<std::string, function_type> functions;
	
	class context{
	public:
		const context* const prev = nullptr;
		std::unordered_map<std::string, treeml::tree> def;
	};

	// void handle_char(const treeml::tree_ext& tree);
protected:
	// void throw_syntax_error(std::string&& message, const treeml::tree_ext& node);

public:
	interpreter();

	virtual ~interpreter(){}

	treeml::forest_ext eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end, const context* ctx = nullptr);

	treeml::forest_ext eval(const treeml::forest_ext& forest, const context* ctx = nullptr){
		return this->eval(forest.begin(), forest.end(), ctx);
	}

	void add_function(const std::string& name, function_type&& func);

	// virtual void on_word(const std::string& word) = 0;

	// void on_space(){
	// 	this->on_word(" ");
	// }
};

}
