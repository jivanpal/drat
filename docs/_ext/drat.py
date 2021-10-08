from docutils import nodes
from docutils.parsers.rst import Directive
from sphinx.util.docutils import SphinxDirective

def drat_role_app(name, rawtext, text, lineno, inliner, options={}, content=[]):
    # TODO: Maybe make this link to a description of the app?
    node = nodes.strong(None, text)
    return [node], []

def drat_role_argument(name, rawtext, text, lineno, inliner,options={}, content=[]):
    # TODO: Create directive to use at top of each `argument` page,
    # and make this role create a backlink to that reference 
    node = nodes.literal(None, '--' + text)
    return [node], []

def drat_role_datatype(name, rawtext, text, lineno, inliner, options={}, content=[]):
    # TODO: Maybe make this link to a definition of the dataype?
    node = nodes.literal(None, text)
    return [node], []

def drat_role_drat_command(name, rawtext, text, lineno, inliner, options={}, content=[]):
    # TODO: Create directive to use at top of each `drat-command` page,
    # and make this role create a backlink to that reference 
    node = nodes.literal(None, text)
    return [node], []

def setup(app):
    app.add_role('app', drat_role_app)
    app.add_role('argument', drat_role_argument)
    app.add_role('datatype', drat_role_datatype)
    app.add_role('drat-command', drat_role_drat_command)
    return
