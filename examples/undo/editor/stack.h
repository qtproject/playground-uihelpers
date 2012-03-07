#include <UiHelpers/UiUndoStack>

QT_USE_NAMESPACE_UIHELPERS;

class Stack : public UiUndoStack
{
    Q_OBJECT
public:
    Stack(QObject *parent = 0);
    ~Stack();

    Q_INVOKABLE void pushCommand();
};
