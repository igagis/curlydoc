#pragma once

#include "translator_base.hpp"

namespace curlydoc{

class translator : public translator_base{
public:
	translator();

	virtual void handle_paragraph(const treeml::forest& tokens) = 0;

	virtual void handle_bold(const treeml::forest& forest) = 0;

	// TODO: add basic virtual functions
};

}
