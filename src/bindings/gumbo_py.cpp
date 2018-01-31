#include <bindings/wrappers.h>

#include <pybind11/stl.h>

using namespace gumbo_python;
namespace py = pybind11;

PYBIND11_MODULE(_gumbo, m) {

	m.attr("GUMBO_NODE_DOCUMENT") = node_types[GUMBO_NODE_DOCUMENT];
	m.attr("GUMBO_NODE_ELEMENT") = node_types[GUMBO_NODE_ELEMENT];
	m.attr("GUMBO_NODE_TEXT") = node_types[GUMBO_NODE_TEXT];
	m.attr("GUMBO_NODE_CDATA") = node_types[GUMBO_NODE_CDATA];
	m.attr("GUMBO_NODE_COMMENT") = node_types[GUMBO_NODE_COMMENT];
	m.attr("GUMBO_NODE_WHITESPACE") = node_types[GUMBO_NODE_WHITESPACE];
	m.attr("GUMBO_NODE_TEMPLATE") = node_types[GUMBO_NODE_TEMPLATE];

	py::register_exception<NodeTypeError>(m, "NodeTypeError", PyExc_TypeError);

	py::class_<NodeVector>(m, "NodeVector")
		.def("__getitem__", &NodeVector::get_item)
		.def("__iter__", &NodeVector::iter, py::return_value_policy::reference_internal)
		.def("__next__", &NodeVector::next)
		.def("__len__", &NodeVector::len)
		;

	py::class_<HtmlNode>(m, "HtmlNode")
		.def_property_readonly("children", &HtmlNode::children)
		.def_property_readonly("has_children", &HtmlNode::has_chidlren)
		.def_property_readonly("type", &HtmlNode::node_type)
		.def_property_readonly("attributes", &HtmlNode::attributes)
		.def_property_readonly("tag_name", &HtmlNode::tag_name)
		.def_property_readonly("offset", &HtmlNode::offset)
		.def_property_readonly("text", &HtmlNode::text)
		.def("__iter__", &HtmlNode::children)
		.def("__str__", &HtmlNode::str)
		.def("__repr__", &HtmlNode::str)
		;

	py::class_<Document>(m, "Document")
		.def(py::init<const std::string &>())
		.def_property_readonly("root" , &Document::root)
		.def("as_list", &Document::as_vector)
		;
}
