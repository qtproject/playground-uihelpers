%modules = ( # path to module name map
    "UiHelpers" => "$basedir/src",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
%classnames = (
    "uihelpersversion.h" => "UiHelpersVersion",
    "uiundostack.h" => "UiUndoStack",
    "uiundogroup.h" => "UiUndoGroup",
    "uifilesystemmodel.h" => "UiFileSystemModel",
    "uistandarditemmodel.h" => "UiStandardModel",
    "uiaction.h" => "UiAction",
    "uiactiongroup.h" => "UiActionGroup",
);
%mastercontent = (
);
%modulepris = (
    "UiHelpers" => "$basedir/modules/qt_uihelpers.pri",
);

# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
        "qtbase" => "refs/heads/master",
);
