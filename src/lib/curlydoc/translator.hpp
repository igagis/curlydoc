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

#include <optional>
#include <unordered_map>

#include <tml/tree_ext.hpp>

namespace curlydoc {

class translator {
public:
  using handler_type = std::function<void(bool, const tml::forest_ext &)>;

private:
  std::unordered_map<std::string, handler_type> handlers;

  std::vector<std::string> cur_tag;
  const std::string &get_parent_tag() const noexcept;

  void handle_image(const tml::forest_ext &forest);

  void handle_table(const tml::forest_ext &forest);
  void handle_cell(const tml::forest_ext &forest);

  void handle_list(const tml::forest_ext &forest);
  void handle_list_item(const tml::forest_ext &forest);

  void translate(bool space, const tml::tree_ext &tree);

protected:
  void report_space(bool report);

  static bool is_parameters(const tml::tree_ext &forest) noexcept;
  static void check_parameter(const tml::tree_ext &forest);

public:
  translator();

  translator(const translator &) = delete;
  translator &operator=(const translator &) = delete;

  translator(translator &&) = delete;
  translator &operator=(translator &&) = delete;

  virtual ~translator() = default;

  std::vector<std::string> list_tags() const;

  void add_tag(const std::string &tag, handler_type &&func);

  void translate(tml::forest_ext::const_iterator begin,
                 tml::forest_ext::const_iterator end);

  void translate(const tml::forest_ext &forest) {
    this->translate(forest.begin(), forest.end());
  }

  virtual void on_word(const std::string &word) = 0;

  virtual void on_paragraph(const tml::forest_ext &forest) = 0;

  virtual void on_bold(const tml::forest_ext &forest) = 0;
  virtual void on_italic(const tml::forest_ext &forest) = 0;
  virtual void on_underline(const tml::forest_ext &forest) = 0;
  virtual void on_strikethrough(const tml::forest_ext &forest) = 0;
  virtual void on_monospace(const tml::forest_ext &forest) = 0;

  virtual void on_header1(const tml::forest_ext &forest) = 0;
  virtual void on_header2(const tml::forest_ext &forest) = 0;
  virtual void on_header3(const tml::forest_ext &forest) = 0;
  virtual void on_header4(const tml::forest_ext &forest) = 0;
  virtual void on_header5(const tml::forest_ext &forest) = 0;
  virtual void on_header6(const tml::forest_ext &forest) = 0;

  virtual void on_ins(const tml::forest_ext &forest) = 0;

  struct image_params {
    std::string url;
    std::optional<uint32_t> width;
    std::optional<uint32_t> height;
  };

  virtual void on_image(const image_params &params,
                        const tml::forest_ext &forest) = 0;

  struct cell {
    tml::forest_ext::const_iterator begin;
    tml::forest_ext::const_iterator end;
    std::optional<size_t> col_span;
    std::optional<size_t> row_span;

    size_t col_index{};

    decltype(col_span)::value_type get_col_span() const noexcept {
      return this->col_span.value_or(1);
    }

    decltype(row_span)::value_type get_row_span() const noexcept {
      return this->row_span.value_or(1);
    }
  };

  struct table_row {
    std::vector<cell> cells;

    std::vector<bool> occupied_cols;
    size_t span = 0; // total row span

    table_row(size_t num_cols)
        : occupied_cols(num_cols) // bool values will be initialized to false
    {}

    size_t get_free_col_index(size_t span);

    void set_occupied(size_t index, size_t span);

    bool is_full() const noexcept;

  private:
    bool is_free_span(decltype(occupied_cols)::const_iterator iter,
                      size_t span) const noexcept;
  };

  enum class align { left, center, right };

  enum class valign { top, center, bottom };

  struct table {
    size_t num_cols = 0;
    std::vector<table_row> rows;
    std::optional<unsigned> border;
    std::vector<size_t> weights;
    std::vector<align> aligns;
    std::vector<valign> valigns;

    size_t cur_row = 0;
    void push(cell &&c);
  };

private:
  std::vector<table> cur_table;

public:
  virtual void on_table(const table &tbl, const tml::forest_ext &forest) = 0;

  struct list {
    bool ordered = false;
    std::vector<tml::forest_ext> items;
  };

  virtual void on_list(const list &l, const tml::forest_ext &forest) = 0;

private:
  std::vector<list> cur_list;

public:
};

} // namespace curlydoc
