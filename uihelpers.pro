TEMPLATE = subdirs

module_uihelpers_src.subdir = src
module_uihelpers_src.target = module-uihelpers-src

module_uihelpers_tests.subdir = tests
module_uihelpers_tests.target = module-uihelpers-tests
module_uihelpers_tests.depends = module_uihelpers_src
module_uihelpers_tests.CONFIG = no_default_install

win32 {
    message("uihelpers is not currently supported on Windows - will not be built")
} else {
    SUBDIRS += module_uihelpers_src \
               module_uihelpers_tests \
}
