#pragma once
#include <QString> 
#include <QVector>

struct Parameter {
	QString name;          // 参数名（如 "id", "pos"）
	QString defaultValue;  // 默认值（如 "1", "0 0 0"）
};

struct ObjectType {
	QString name;                 // 作用对象名（如 "entityModel"）
	QString description;          // 作用对象解释
	QVector<Parameter> params;    // 参数列表（固定顺序）
};

struct Command {
	QString name;                 // 命令名（如 "add"）
	QString description;          // 命令解释
	QVector<ObjectType> objects;  // 可作用的对象类型
};