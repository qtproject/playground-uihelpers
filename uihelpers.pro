TEMPLATE = subdirs
CONFIG += ordered

module_uihelpers_src.subdir = src
module_uihelpers_src.target = module-uihelpers-src

module_uihelpers_examples.subdir = examples
module_uihelpers_examples.target = module-uihelpers-examples

module_uihelpers_tests.subdir = tests
module_uihelpers_tests.target = module-uihelpers-tests
module_uihelpers_tests.depends = module_uihelpers_src
module_uihelpers_tests.CONFIG = no_default_install

SUBDIRS += module_uihelpers_src \
           module_uihelpers_examples \
           module_uihelpers_tests
