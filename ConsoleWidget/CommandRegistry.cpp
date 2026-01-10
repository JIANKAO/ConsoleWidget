#include "CommandRegistry.h"

static QVector<Command> g_commands =
{
    {
        "add",
        "向场景中添加对象",
        {
            {
                "entityModel",
                "实体模型对象，可实例化到场景中测试测试测试测试测试测试测试",
                {
                    { "id",  "1" },
                    { "pos", "0 0 0" }
                }
            },
            {
                "light",
                "光源对象",
                {
                    { "intensity", "1.0" },
                    { "pos",       "0 10 0" }
                }
            }
        }
    },

    {
        "delete",
        "删除场景中的对象",
        {
            {
                "entityModel",
                "实体模型对象",
                {
                    { "id", "1" }
                }
            }
        }
    },

    {
        "move",
        "移动场景中的对象",
        {
            {
                "entityModel",
                "实体模型对象",
                {
                    { "id",  "1" },
                    { "pos", "0 0 0" }
                }
            }
        }
    }
};

const QVector<Command>& CommandRegistry::commands()
{
    return g_commands;
}

const Command* CommandRegistry::findCommand(const QString& name)
{
    for (const auto& cmd : g_commands)
    {
        if (cmd.name == name)
            return &cmd;
    }
    return nullptr;
}

const ObjectType* CommandRegistry::findObject(const Command& cmd, const QString& name)
{
    for (const auto& obj : cmd.objects)
    {
        if (obj.name == name)
            return &obj;
    }
    return nullptr;
}
