/*
curlydoc - document markup language translator

Copyright (C) 2021 Ivan Gagis <igagis@gmail.com>

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

#include <list>
#include <unordered_map>
#include <vector>

#include <tml/tree_ext.hpp>

namespace curlydoc {

class interpreter {
  std::vector<std::string> file_name_stack;

public:
  using function_type = std::function<tml::forest_ext(const tml::forest_ext &)>;

  class exception : public std::invalid_argument {
  public:
    exception(const std::string &message);
    exception(const std::string &message, const std::string &file,
              const tml::leaf_ext &leaf);
  };

private:
  std::unordered_map<std::string, function_type> functions;

  class context {
    const context *const prev;
    std::unordered_map<std::string, tml::forest_ext> defs;

  public:
    context(const context *const prev = nullptr) : prev(prev) {}

    void add(const std::string &name, tml::forest_ext &&value);

    struct find_result {
      const tml::forest_ext *value;
      const context &ctx;
    };

    find_result try_find(const std::string &name) const;

    const tml::forest_ext &find(const std::string &name) const;
  };

  // NOTE: use std::list to avoid context objects to be moved
  std::list<context> context_stack = {context()};

  context &push_context(const context *prev = nullptr);

  struct bool_state {
    bool flag = false;
    bool true_before_or = false;
  };

  std::vector<bool_state> if_flag_stack = {
      bool_state()}; // initial flag for root scope

  struct if_flag_push {
    interpreter &owner;

    if_flag_push(interpreter &owner) : owner(owner) {
      this->owner.if_flag_stack.emplace_back();
    }

    if_flag_push(const if_flag_push &) = delete;
    if_flag_push &operator=(const if_flag_push &) = delete;

    if_flag_push(if_flag_push &&) = delete;
    if_flag_push &operator=(if_flag_push &&) = delete;

    ~if_flag_push() { this->owner.if_flag_stack.pop_back(); }
  };

  std::unique_ptr<papki::file> file; // for including files

public:
  interpreter(std::unique_ptr<papki::file> file);

  interpreter(const interpreter &) = delete;
  interpreter &operator=(const interpreter &) = delete;

  interpreter(interpreter &&) = delete;
  interpreter &operator=(interpreter &&) = delete;

  virtual ~interpreter() = default;

  tml::forest_ext eval(tml::forest_ext::const_iterator begin,
                       tml::forest_ext::const_iterator end,
                       bool preserve_vars = false);

  tml::forest_ext eval(const tml::forest_ext &forest,
                       bool preserve_vars = false) {
    return this->eval(forest.begin(), forest.end(), preserve_vars);
  }

  tml::forest_ext eval();

  void add_function(const std::string &name, function_type &&func);

  void add_repeater_function(const std::string &name);

  void add_repeater_functions(utki::span<const std::string> names);

private:
  void init_std_lib();
};

} // namespace curlydoc
