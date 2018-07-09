"""Test BeatifulSoup4 adapter"""

import gumbo
from .fixtures import HTML


def test_soup():
    soup = gumbo.get_soup(HTML)
    p_tags = soup.find_all('p')
    assert len(p_tags) == 4
