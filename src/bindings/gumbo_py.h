#pragma once

#include <gumbo/gumbo.h>

#include <pybind11/pybind11.h>

#include <string>
#include <exception>
#include <unordered_map>

namespace gumbo_python {

	std::unordered_map<int,std::string> node_types{
		{GUMBO_NODE_DOCUMENT, "document"},
		{GUMBO_NODE_ELEMENT, "element"},
		{GUMBO_NODE_TEXT, "text"},
		{GUMBO_NODE_CDATA, "cdata"},
		{GUMBO_NODE_COMMENT, "comment"},
		{GUMBO_NODE_WHITESPACE, "whitespace"},
		{GUMBO_NODE_TEMPLATE, "template"}
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

	public:
		NodeWrapper(GumboNode* node) : node_(node) {}

		NodeVectorWrapper iter() {
			if (node_->type != GUMBO_NODE_ELEMENT || node_->type != GUMBO_NODE_TEMPLATE)
				throw TypeError("Node type '" + node_type() + "' is not iterable!");
			return NodeVectorWrapper(node_->v.element.children);
		}

		std::string node_type() { return node_types[node_->type]; }
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
