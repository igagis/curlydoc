#pragma once

#include <unordered_map>
#include <vector>

#include <treeml/tree_ext.hpp>

namespace curlydoc{

class interpreter{
	const std::string file_name;
public:
	typedef std::function<void(bool, const treeml::tree_ext&)> keyword_handler_type;
	typedef std::function<void(const std::string&)> word_handler_type;
private:
	std::unordered_map<std::string, keyword_handler_type> handlers;

	std::vector<treeml::tree_ext> def;

	

	void handle_char(const treeml::tree_ext& tree);
protected:
	void throw_syntax_error(std::string&& message, const treeml::tree_ext& node);

public:
	interpreter(std::string&& file_name);

	virtual ~interpreter(){}

	void eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end);

	void eval(const treeml::forest_ext& forest){
		this->eval(forest.begin(), forest.end());
	}

	void add_keyword(const std::string& keyword, keyword_handler_type&& handler);

	virtual void on_word(const std::string& word) = 0;

	void on_space(){
		this->on_word(" ");
	}
};

}
