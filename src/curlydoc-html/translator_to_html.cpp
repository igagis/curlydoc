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

#include "translator_to_html.hpp"

#include <numeric>
#include <ratio>

using namespace curlydoc;

void translator_to_html::on_word(const std::string &word) {
  if (this->no_next_space) {
    this->no_next_space = false;
    if (word == " ") {
      return;
    }
  }
  this->ss << word;
}

void translator_to_html::on_paragraph(const tml::forest_ext &forest) {
  this->ss << '\n' << "<p>";
  this->translate(forest);
  this->ss << "</p>";
}

void translator_to_html::on_bold(const tml::forest_ext &forest) {
  this->ss << "<b>";
  this->translate(forest);
  this->ss << "</b>";
}

void translator_to_html::on_italic(const tml::forest_ext &forest) {
  this->ss << "<i>";
  this->translate(forest);
  this->ss << "</i>";
}

void translator_to_html::on_underline(const tml::forest_ext &forest) {
  this->ss << "<u>";
  this->translate(forest);
  this->ss << "</u>";
}

void translator_to_html::on_strikethrough(const tml::forest_ext &forest) {
  this->ss << "<s>";
  this->translate(forest);
  this->ss << "</s>";
}

void translator_to_html::on_monospace(const tml::forest_ext &forest) {
  this->ss << "<code>";
  this->translate(forest);
  this->ss << "</code>";
}

void translator_to_html::on_header1(const tml::forest_ext &forest) {
  this->ss << '\n' << "<h1>";
  this->translate(forest);
  this->ss << "</h1>";
}

void translator_to_html::on_header2(const tml::forest_ext &forest) {
  this->ss << '\n' << "<h2>";
  this->translate(forest);
  this->ss << "</h2>";
}

void translator_to_html::on_header3(const tml::forest_ext &forest) {
  this->ss << '\n' << "<h3>";
  this->translate(forest);
  this->ss << "</h3>";
}

void translator_to_html::on_header4(const tml::forest_ext &forest) {
  this->ss << '\n' << "<h4>";
  this->translate(forest);
  this->ss << "</h4>";
}

void translator_to_html::on_header5(const tml::forest_ext &forest) {
  this->ss << '\n' << "<h5>";
  this->translate(forest);
  this->ss << "</h5>";
}

void translator_to_html::on_header6(const tml::forest_ext &forest) {
  this->ss << '\n' << "<h6>";
  this->translate(forest);
  this->ss << "</h6>";
}

void translator_to_html::on_ins(const tml::forest_ext &forest) {
  this->ss << '\n' << "<br/>" << '\n';
  this->no_next_space = true;
}

void translator_to_html::on_image(const image_params &params,
                                  const tml::forest_ext &forest) {
  this->ss << "<img src=\"" << params.url << "\"";

  if (params.width) {
    this->ss << " width=\"" << params.width.value() << "\"";
  }

  if (params.height) {
    this->ss << " height=\"" << params.height.value() << "\"";
  }

  this->ss << "/>";
}

namespace {
std::string to_string(translator::align a) {
  switch (a) {
  case translator::align::left:
    return "left";
  case translator::align::right:
    return "right";
  case translator::align::center:
    return "center";
  default:
    ASSERT(false)
    return {};
  }
}
} // namespace

namespace {
std::string to_string(translator::valign a) {
  switch (a) {
  case translator::valign::top:
    return "top";
  case translator::valign::bottom:
    return "bottom";
  case translator::valign::center:
    return "middle";
  default:
    ASSERT(false)
    return {};
  }
}
} // namespace

void translator_to_html::on_table(const table &tbl,
                                  const tml::forest_ext &forest) {
  this->ss << '\n' << "<table width=\"100%\">";

  ASSERT(tbl.weights.size() == tbl.num_cols)
  ASSERT(tbl.aligns.size() == tbl.num_cols)
  ASSERT(tbl.valigns.size() == tbl.num_cols)

  size_t total_weight = std::accumulate(tbl.weights.begin(), tbl.weights.end(),
                                        decltype(tbl.weights)::value_type(0));

  std::vector<size_t> weight_percent;

  for (const auto &w : tbl.weights) {
    weight_percent.push_back(w * std::centi::den / total_weight);
  }

  for (const auto &r : tbl.rows) {
    this->ss << '\n' << "<tr>";

    for (const auto &c : r.cells) {
      std::vector<std::string> style;
      this->ss << '\n' << "<td";
      if (c.col_span) {
        this->ss << " colspan=\"" << c.col_span.value() << '\"';
      } else {
        style.push_back(std::string("width:") +
                        std::to_string(weight_percent[c.col_index]) + "%");
      }
      if (c.row_span) {
        this->ss << " rowspan=\"" << c.row_span.value() << '\"';
      }
      if (tbl.border) {
        style.push_back(std::string("border-width:") +
                        std::to_string(tbl.border.value()) + "px");
      }
      style.push_back(std::string("text-align: ") +
                      to_string(tbl.aligns[c.col_index]));
      style.push_back(std::string("vertical-align: ") +
                      to_string(tbl.valigns[c.col_index]));
      if (!style.empty()) {
        this->ss << " style=\"";
        for (const auto &s : style) {
          this->ss << s << ";";
        }
        this->ss << '\"';
      }
      this->ss << '>';
      this->translate(c.begin, c.end);
      this->ss << "</td>";
    }
    this->ss << '\n' << "</tr>";
  }

  this->ss << '\n' << "</table>";
}

void translator_to_html::on_list(const list &l, const tml::forest_ext &forest) {
  const auto tag = l.ordered ? "ol" : "ul";

  this->ss << '\n' << '<' << tag << '>';

  for (const auto &li : l.items) {
    this->ss << '\n' << "<li>";
    this->translate(li);
    this->ss << "</li>";
  }

  this->ss << '\n' << "</" << tag << '>';
}
