#pragma once
#include <QObject>
#include "TotalHead.h"

//命令结构系统
class CommandRegistry : public QObject
{
public:
	CommandRegistry(QObject* parent = nullptr) : QObject(parent) {}

	static const QVector<Command>& commands();
	static const Command* findCommand(const QString& name);
	static const ObjectType* findObject(const Command& cmd, const QString& name);
};

