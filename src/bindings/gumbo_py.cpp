#include "wrappers.h"

#include <pybind11/pybind11.h>
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

  py::class_<Node>(m, "Node")
    .def_property_readonly("type", &Node::type)
    .def_property_readonly("parent", &Node::parent)
    .def_property_readonly("offset", &Node::offset)
    .def("__str__", &Node::str)
    ;

  py::class_<TagNode, Node>(m, "TagNode");

  py::class_<Document, TagNode>(m, "Document")
    .def_property_readonly("children", &Document::children)
    .def("__iter__", &Document::children)
    ;

  py::class_<Tag, TagNode>(m, "Tag")
    .def_property_readonly("tag_name", &Tag::tag_name)
    .def_property_readonly("attributes", &Tag::attributes)
    .def_property_readonly("children", &Tag::children)
    .def_property_readonly("text", &Tag::text)
    .def("__iter__", &TagNode::children)
    ;

  py::class_<Text, Node>(m, "TextNode");

  py::class_<NodeVector>(m, "NodeVector")
    .def("__iter__", &NodeVector::iter, py::return_value_policy::reference_internal)
    .def("__next__", &NodeVector::next)
    .def("__getitem__", &NodeVector::get_item)
    .def("__len__", &NodeVector::len)
    ;

  py::class_<Output>(m, "Output")
    .def(py::init<std::string>())
    .def_property_readonly("root", &Output::root)
    .def_property_readonly("document", &Output::document)
    ;
}
