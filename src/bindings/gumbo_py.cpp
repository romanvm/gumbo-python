#include "wrappers.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace gumbo_python;
namespace py = pybind11;

PYBIND11_MODULE(_gumbo, m) {

  m.attr("__all__") = std::vector<std::string>{
    "GUMBO_NODE_DOCUMENT",
    "GUMBO_NODE_ELEMENT",
    "GUMBO_NODE_TEXT",
    "GUMBO_NODE_CDATA",
    "GUMBO_NODE_COMMENT",
    "GUMBO_NODE_WHITESPACE",
    "GUMBO_NODE_TEMPLATE",
    "parse"
  };

  py::enum_<GumboNodeType>(m, "GumboNodeType", py::arithmetic(), "Gumbo node types")
    .value("GUMBO_NODE_DOCUMENT", GumboNodeType::GUMBO_NODE_DOCUMENT)
    .value("GUMBO_NODE_ELEMENT", GumboNodeType::GUMBO_NODE_ELEMENT)
    .value("GUMBO_NODE_TEXT", GumboNodeType::GUMBO_NODE_TEXT)
    .value("GUMBO_NODE_CDATA", GumboNodeType::GUMBO_NODE_CDATA)
    .value("GUMBO_NODE_COMMENT", GumboNodeType::GUMBO_NODE_COMMENT)
    .value("GUMBO_NODE_WHITESPACE", GumboNodeType::GUMBO_NODE_WHITESPACE)
    .value("GUMBO_NODE_TEMPLATE", GumboNodeType::GUMBO_NODE_TEMPLATE)
    .export_values();

  py::class_<NodeVector>(m, "NodeVector")
    .def("__iter__", &NodeVector::iter, py::return_value_policy::reference_internal)
    .def("__next__", &NodeVector::next)
    .def("__getitem__", &NodeVector::get_item)
    .def("__len__", &NodeVector::len)
    ;

  py::class_<AttributeMap>(m, "AttributeMap")
    .def("get", &AttributeMap::get)
    .def("as_dict", &AttributeMap::as_dict)
    .def("__getitem__", &AttributeMap::get_item)
    .def("__contains__", &AttributeMap::contains)
    .def("__len__", &AttributeMap::len)
    ;

  py::class_<Node>(m, "Node")
    .def_property_readonly("parent", &Node::parent)
    .def_property_readonly("is_tag", &Node::is_tag)
    .def_property_readonly("type", &Node::type)
    .def_property_readonly("offset", &Node::offset)
    .def("__str__", &Node::str)
    ;

  py::class_<TagNode, Node>(m, "TagNode")
    .def_property_readonly("is_tag", &TagNode::is_tag)
    .def_property_readonly("children", &TagNode::children)
    ;

  py::class_<Document, TagNode>(m, "Document")
    .def_property_readonly("doctype", &Document::doctype)
    //.def_property_readonly("children", &Document::children)
    .def("__str__", &Document::str)
    ;

  py::class_<Tag, TagNode>(m, "Tag")
    .def_property_readonly("tag_name", &Tag::tag_name)
    .def_property_readonly("attributes", &Tag::attributes)
    //.def_property_readonly("children", &Tag::children)
    .def("__str__", &Tag::str)
    ;

  py::class_<Text, Node>(m, "TextNode")
    .def_property_readonly("text", &Text::text)
    .def("__str__", &Text::str)
    ;

  py::class_<Output>(m, "Output")
    .def_property_readonly("root", &Output::root)
    .def_property_readonly("document", &Output::document)
    ;

  m.def("parse", &parse);
}
