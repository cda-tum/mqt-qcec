from importlib.metadata import version
import pybtex.plugin
from pybtex.style.formatting.unsrt import Style as UnsrtStyle
from pybtex.style.template import field, href

# -- Project information -----------------------------------------------------
project = 'QCEC'
author = 'Lukas Burgholzer'

release = version('mqt.qcec')
version = '.'.join(release.split('.')[:3])

# -- General configuration ---------------------------------------------------
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autosummary",
    "sphinx.ext.mathjax",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "sphinx.ext.githubpages",
    "sphinxcontrib.bibtex",
    "sphinx_copybutton"
]


class CDAStyle(UnsrtStyle):
    def format_url(self, e):
        url = field('url', raw=True)
        return href()[url, '[PDF]']


pybtex.plugin.register_plugin(
    'pybtex.style.formatting', 'cda_style', CDAStyle)

bibtex_bibfiles = ['refs.bib']
bibtex_default_style = 'cda_style'

copybutton_prompt_text = r"(?:\(venv\) )?\$ "
copybutton_prompt_is_regexp = True
copybutton_line_continuation_character = "\\"

autosummary_generate = True

# -- Options for HTML output -------------------------------------------------
html_theme = 'sphinx_rtd_theme'
