"""Test BeatifulSoup4 adapter"""

from .fixtures import *


def test_soup():
    assert gumbo.get_soup(HTML)
