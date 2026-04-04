#include "CommandManager.h"

CommandManager::CommandManager(QObject* parent)
    : QObject(parent)
{
}

void CommandManager::log(const QString& text)
{
    m_history.push_back(text);
    emit historyChanged();
}

QStringList CommandManager::history() const
{
    return m_history;
}
