#pragma once

#include <gumbo/gumbo.h>

#include <pybind11/pybind11.h>

#include <string>
#include <vector>
#include <exception>
#include <unordered_map>
#include <array>

namespace gumbo_python {

	std::array<std::string, 7> node_types{
		"document",
		"element",
		"text",
		"cdata",
		"comment",
		"whitespace",
		"template"
	};

	class TypeError : public std::exception {};

	class NodeWrapper;

	class NodeVectorWrapper {
	private:
		GumboVector* vector_;
		unsigned int index_;

	public:
		NodeVectorWrapper(GumboVector* vector) : vector_(vector), index_(0) {}

		NodeWrapper next() {
			if (index_ >= vector_->length)
				throw pybind11::stop_iteration();
			NodeWrapper node = static_cast<GumboNode*>(vector_->data[index_]);
			++index_;
			return node;
		}

		NodeWrapper get_item(unsigned int index) {
			if (index >= vector_->length)
				throw pybind11::index_error(std::to_string(index));
			return NodeWrapper(vector_->data[index]);
		}

		unsigned int len() { return vector_->length; }
	};


	class NodeWrapper {
	private:
		GumboNode* node_;
		std::string tag_name_;
		std::unordered_map<std::string, std::string> attributes_;
		std::string text_;

		void check_if_element(const std::string& err_message) {
			if (node_->type != GUMBO_NODE_ELEMENT || node_->type != GUMBO_NODE_TEMPLATE)
				throw TypeError(err_message);
		}

	public:
		NodeWrapper(GumboNode* node) : node_(node) {
			if (node_->type == GUMBO_NODE_ELEMENT || node_->type == GUMBO_NODE_TEMPLATE) {
				tag_name_ = gumbo_normalized_tagname(node_->v.element.tag);
				GumboVector* raw_attributes = node_->v.element.attributes;
				for (unsigned int i = 0; i < raw_attributes->length; ++i) {
					GumboAttribute* attr = static_cast<GumboAttribute*>(raw_attributes[i]);
					attributes_[attr->name] = attr->value;
				}
			}
			else if (node_->type == GUMBO_NODE_TEXT || node_->type == GUMBO_NODE_CDATA ||
					node_->type == GUMBO_NODE_COMMENT || node_->type == GUMBO_NODE_WHITESPACE) {
				text_ = node_->v.text.text;
			}
		}

		NodeVectorWrapper iter() {
			check_if_element("Node type '" + node_type() + "' is not iterable!");
			return NodeVectorWrapper(node_->v.element.children);
		}

		std::string node_type() { return node_types[node_->type]; }

		std::unordered_map<std::string, std::string>& attributes() {
			check_if_element("Node type '" + node_type() + "' has no attributes!");
			return attributes_;
		}

		std::string tag_name() {
			check_if_element("Node type '" + node_type() + "' is not a tag!");
			return tag_name_;
		}

		std::string text() {
			if (node_->type == GUMBO_NODE_TEXT || node_->type == GUMBO_NODE_CDATA ||
					node_->type == GUMBO_NODE_COMMENT || node_->type == GUMBO_NODE_WHITESPACE)
				return text_;
			else
				throw TypeError("Node '" + node_type() + "' is not a text node!");
		}

		unsigned int offset() {
			if (node_->type == GUMBO_NODE_ELEMENT || node_->type == GUMBO_NODE_TEMPLATE)
				return node_->v.element.start_pos.offset;
			else
				return node_->v.text.start_pos.offset;
			throw TypeError("Node type '" + node_type() + "' does not have offset!");
		}
	};


	class parse_html {
	private:
		GumboOutput* output_;

	public:
		parse_html(const std::string& html) : output_(nullptr) {
			output_ = gumbo_parse(html.c_str());
		}

		~parse_html() {
			if (output_ != nullptr) {
				gumbo_destroy_output(&kGumboDefaultOptions, output_);
			}
		}

		NodeWrapper enter() {
			return NodeWrapper(output_->root);
		}

		bool exit(pybind11::object t, pybind11::object v, pybind11::object tb) {
			gumbo_destroy_output(&kGumboDefaultOptions, output_);
			output_ = nullptr;
			return false;
		}
	};
}
