/*
curlydoc - document markup language translator

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

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
		std::unordered_map<std::string, treeml::forest_ext> defs;
	public:
		context(const context* const prev = nullptr) : prev(prev){}

		void add(const std::string& name, treeml::forest_ext&& value);

		struct find_result{
			const treeml::forest_ext* value;
			const context& ctx;
		};

		find_result try_find(const std::string& name)const;

		const treeml::forest_ext& find(const std::string& name)const;
	};

	// NOTE: use std::list to avoid context objects to be moved
	std::list<context> context_stack = {context()};

	context& push_context(const context* prev = nullptr);

	struct bool_state{
		bool flag = false;
		bool true_before_or = false;
	};

	std::vector<bool_state> if_flag_stack = {bool_state()}; // initial flag for root scope

	struct if_flag_push{
		interpreter& owner;
		if_flag_push(interpreter& owner) : owner(owner){
			this->owner.if_flag_stack.emplace_back();
		}
		~if_flag_push(){
			this->owner.if_flag_stack.pop_back();
		}
	};

	std::unique_ptr<papki::file> file; // for including files
public:
	interpreter(std::unique_ptr<papki::file> file);

	virtual ~interpreter() = default;

	treeml::forest_ext eval(treeml::forest_ext::const_iterator begin, treeml::forest_ext::const_iterator end, bool preserve_vars = false);

	treeml::forest_ext eval(const treeml::forest_ext& forest, bool preserve_vars = false){
		return this->eval(forest.begin(), forest.end(), preserve_vars);
	}

	treeml::forest_ext eval();

	void add_function(const std::string& name, function_type&& func);

	void add_repeater_function(const std::string& name);

	void add_repeater_functions(utki::span<const std::string> names);

private:
	void init_std_lib();
};

}
