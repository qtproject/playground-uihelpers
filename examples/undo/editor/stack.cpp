#include "stack.h"
#include "commands.h"

Stack::Stack(QObject *parent)
    : QUndoStack(parent)
{
}

Stack::~Stack()
{
}

void Stack::pushCommand()
{
    CreateFileCommand *cmd = new CreateFileCommand;
    push(cmd);
}
