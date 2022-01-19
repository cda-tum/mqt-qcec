# Configuration file for the Sphinx documentation builder.

# -- Path setup --------------------------------------------------------------
import os
import sys

sys.path.insert(0, os.path.abspath('../../jkq/qcec'))
sys.path.insert(0, os.path.abspath('../jkq/qcec'))

# -- Project information -----------------------------------------------------
project = 'QCEC'
copyright = '2022, Lukas Burgholzer'
author = 'Lukas Burgholzer'

# The full version, including alpha/beta/rc tags
release = '2.0.0'

# -- General configuration ---------------------------------------------------
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autosummary",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "sphinx.ext.githubpages",
    "sphinxcontrib.bibtex",
]

bibtex_bibfiles = ['refs.bib']
bibtex_reference_style = 'author_year'

autosummary_generate = True

# -- Options for HTML output -------------------------------------------------
html_theme = 'sphinx_rtd_theme'
