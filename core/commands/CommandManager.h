#pragma once

#include <QObject>
#include <QStringList>

class CommandManager : public QObject
{
    Q_OBJECT
public:
    explicit CommandManager(QObject* parent = nullptr);

    void log(const QString& text);
    QStringList history() const;

signals:
    void historyChanged();

private:
    QStringList m_history;
};
