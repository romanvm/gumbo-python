from bs4 import BeautifulSoup, Tag, NavigableString, CData, Comment
from bs4.element import Doctype
from . import _gumbo

__all__ = ['get_soup']


def _add_document(soup, node, soup_listing):
    if node.has_doctype:
        doctype = Doctype.for_name_and_ids(
            node.doctype,
            node.public_identifier,
            node.system_identifier
        )
        doctype.offset = node.offset
        soup.object_was_parsed(doctype)
        soup_listing.append(doctype)


def _add_element(soup, element, soup_listing):
    tag = Tag(name=element.tag_name, attrs=element.attributes.as_dict())
    soup_listing.append(tag)
    for child in element.children:
        tag.append(_add_node(soup, child, soup_listing))
    tag.offset = element.offset
    return tag


def _add_text(cls):
    def add_text_internal(soup, element, soup_listing):
        text = cls(element.text)
        text.offset = element.offset
        soup_listing.append(text)
        return text
    return add_text_internal


def _add_next_prev_pointers(soup_listing):
    nodes = sorted(soup_listing, key=lambda node: node.offset)
    if nodes:
        nodes[0].previous_element = None
        nodes[-1].next_element = None
        for i, node in enumerate(nodes[1:-1], 1):
            nodes[i-1].next_element = node
            node.previous_element = nodes[i - 1]


_HANDLERS = (
    _add_document,
    _add_element,
    _add_text(NavigableString),
    _add_text(CData),
    _add_text(Comment),
    _add_text(NavigableString),
    _add_element,
    )


def _add_node(soup, node, soup_listing):
    return _HANDLERS[node.type](soup, node, soup_listing)


def get_soup(html):
    doc = _gumbo.parse(html)
    soup = BeautifulSoup(features='html.parser')
    soup_listing = []
    soup.append(_add_node(soup, doc.root, soup_listing))
    soup.offset = -1
    _add_next_prev_pointers(soup_listing)
    return soup
