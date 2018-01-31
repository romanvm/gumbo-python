/*
 * wrappers.cpp
 *
 *  Created on: 30 ñ³÷. 2018 ð.
 *      Author: Roman
 */

#include "wrappers.h"

#include <iostream>

//#include <algorithm>

namespace gumbo_python {

	const std::array<std::string, 7> node_types = {
				"document",
				"element",
				"text",
				"cdata",
				"comment",
				"whitespace",
				"template"
			};


	HtmlNode NodeVector::next() {
		if (index_ >= vector_->length)
			throw pybind11::stop_iteration();
		HtmlNode node{ static_cast<GumboNode*>(vector_->data[index_]) };
		++index_;
		return node;
	}

	HtmlNode NodeVector::get_item(unsigned int index) {
		if (index >= vector_->length)
			throw pybind11::index_error(std::to_string(index));
		return HtmlNode(static_cast<GumboNode*>(vector_->data[index]));
	}


	HtmlNode::HtmlNode(GumboNode* node) : node_(node) {
		if (node_->type == GUMBO_NODE_ELEMENT || node_->type == GUMBO_NODE_TEMPLATE) {
			tag_name_ = gumbo_normalized_tagname(node_->v.element.tag);
			str_ = std::string(node->v.element.original_tag.data, node->v.element.original_tag.length);
			GumboVector* raw_attributes = &node_->v.element.attributes;
			for (unsigned int i = 0; i < raw_attributes->length; ++i) {
				GumboAttribute* attr = static_cast<GumboAttribute*>(raw_attributes->data[i]);
				attributes_[attr->name] = attr->value;
			}
		} else if (node_->type == GUMBO_NODE_TEXT || node_->type == GUMBO_NODE_WHITESPACE) {
			str_ = node_->v.text.text;
		} else {
			str_ = std::string(node_->v.text.original_text.data, node_->v.text.original_text.length);
		}
	}

	void HtmlNode::check_if_element(const std::string& err_message) {
		if (node_->type != GUMBO_NODE_ELEMENT && node_->type != GUMBO_NODE_TEMPLATE)
			throw NodeTypeError(err_message);
	}

	NodeVector HtmlNode::children() {
		check_if_element("Node type '" + node_type() + "' is not iterable!");
		return NodeVector(&node_->v.element.children);
	}

	bool HtmlNode::has_chidlren() {
		if (node_->type == GUMBO_NODE_ELEMENT || node_->type == GUMBO_NODE_TEMPLATE)
			return node_->v.element.children.length > 0;
		return false;
	}

	std::unordered_map<std::string, std::string>& HtmlNode::attributes() {
		check_if_element("Node type '" + node_type() + "' has no attributes!");
		return attributes_;
	}

	std::string HtmlNode::tag_name() {
		check_if_element("Node type '" + node_type() + "' is not a tag!");
		return tag_name_;
	}

	std::string HtmlNode::text() {
		if (node_->type != GUMBO_NODE_ELEMENT && node_->type != GUMBO_NODE_TEMPLATE)
			return str_;
		else
			throw NodeTypeError("Node '" + node_type() + "' is not a text node!");
	}

	std::string HtmlNode::str() {
		std::cout << "Raw string: '" << str_ << "'" << std::endl;
		return str_;
	}

	std::string HtmlNode::repr() {
		if (node_->type == GUMBO_NODE_ELEMENT || node_->type == GUMBO_NODE_TEMPLATE)
			return "<HtmlNode " + node_type() + "(" + str_ + ")>";
		else
			return "<HtmlNode " + node_type() + "('" + str_ + "')>";
	}

	unsigned int HtmlNode::offset() {
		if (node_->type == GUMBO_NODE_ELEMENT || node_->type == GUMBO_NODE_TEMPLATE)
			return node_->v.element.start_pos.offset;
		else
			return node_->v.text.start_pos.offset;
	}

	void Document::append_node(HtmlNode node, std::vector<HtmlNode>& vect) {
		vect.emplace_back(node);
		if (node.has_chidlren()) {
			NodeVector children = node.children();
			for (unsigned int i = 0; i < children.len(); ++i)
				append_node(children.get_item(i), vect);
		}
	}

	std::vector<HtmlNode> Document::as_vector() {
		std::vector<HtmlNode> vect;
		append_node(root(), vect);
		//std::sort(vect.begin(), vect.end(), comp_offset);
		return vect;
	}
}
