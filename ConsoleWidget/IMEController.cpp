#include "IMEController.h"
#include "CommandRegistry.h"

IMEController::IMEController()
{
}

void IMEController::updateInput(const QString& text)
{
    m_input = text;

    if (!m_input.startsWith("/")) {
        m_stage = -1;
        m_candidates.clear();
        return;
    }

    m_hasTrailingSpace = m_input.endsWith(' ');

    parseInput();
    updateCandidates();
}


bool IMEController::isActive() const
{
    // stage < 0 → 不激活
    if (m_stage < 0)
        return false;

    // 没有候选项 → 不激活
    if (m_candidates.isEmpty())
        return false;

    return true;
}

const QStringList& IMEController::candidates() const
{
    return m_candidates;
}

int IMEController::highlightIndex() const
{
    return m_highlight;
}

void IMEController::setHighlightIndex(int index)
{
    if (index >= 0 && index < m_candidates.size())
        m_highlight = index;
}

QString IMEController::description() const
{
    if (m_candidates.isEmpty() || m_highlight >= m_candidates.size())
        return "";

    const QString& word = m_candidates[m_highlight];

    // 阶段 0：命令解释
    if (m_stage == 0) {
        if (auto* cmd = CommandRegistry::findCommand(word))
            return cmd->description;
    }

    // 阶段 1：对象解释
    if (m_stage == 1) {
        if (m_tokens.isEmpty())
            return "";

        auto* cmd = CommandRegistry::findCommand(m_tokens[0]); // 不要 mid(1)
        if (!cmd) return "";
        if (auto* obj = CommandRegistry::findObject(*cmd, word))
            return obj->description;
    }

    // 阶段 2+：参数解释
    if (m_stage >= 2) {
        if (m_tokens.size() < 2)
            return "";

        auto* cmd = CommandRegistry::findCommand(m_tokens[0]); // 不要 mid(1)
        if (!cmd) return "";

        auto* obj = CommandRegistry::findObject(*cmd, m_tokens[1]);
        if (!obj) return "";

        int paramIndex = m_stage - 2;
        if (paramIndex >= 0 && paramIndex < obj->params.size())
            return obj->params[paramIndex].name;
    }

    return "";
}

QString IMEController::applyTabCompletion() const
{
    if (m_candidates.isEmpty())
        return m_input;

    QString selected = m_candidates[m_highlight];

    QStringList newTokens = m_tokens;

    if (m_stage < newTokens.size()) {
        // 当前阶段有 token，替换它
        newTokens[m_stage] = selected;
    }
    else {
        // 当前阶段还没有 token，追加一个
        newTokens << selected;
    }

    return "/" + newTokens.join(" ");
}

QString IMEController::applyNumberCompletion(int number) const
{
    int index = number - 1;
    if (index < 0 || index >= m_candidates.size())
        return m_input;

    QString selected = m_candidates[index];

    QStringList newTokens = m_tokens;

    if (m_stage < newTokens.size()) {
        newTokens[m_stage] = selected;
    }
    else {
        newTokens << selected;
    }

    return "/" + newTokens.join(" ");
}

void IMEController::parseInput()
{
    m_tokens = m_input.split(" ", Qt::SkipEmptyParts);

    if (!m_tokens.isEmpty())
        m_tokens[0] = m_tokens[0].mid(1); // "/add" → "add"

    if (m_tokens.isEmpty()) {
        m_stage = 0; // 只有 "/" 或 "/ " 时，认为在命令阶段
        return;
    }

    if (m_hasTrailingSpace) {
        // 用户刚刚输入了空格：进入“下一个 token 阶段”
        m_stage = m_tokens.size();   // 注意：可能大于 m_tokens 的索引
    }
    else {
        // 还在编辑当前 token
        m_stage = m_tokens.size() - 1;
    }
}

void IMEController::updateCandidates()
{
    m_candidates.clear();
    m_highlight = 0;

    // 阶段 0：命令名
    if (m_stage == 0) {
        QString prefix;
        if (!m_tokens.isEmpty())
            prefix = m_tokens[0];

        for (const auto& cmd : CommandRegistry::commands()) {
            if (cmd.name.startsWith(prefix))
                m_candidates << cmd.name;
        }
        return;
    }

    // 阶段 1：对象名
    if (m_stage == 1) {
        if (m_tokens.isEmpty())
            return;

        auto* cmd = CommandRegistry::findCommand(m_tokens[0]);
        if (!cmd) return;

        QString prefix;
        if (m_stage < m_tokens.size())
            prefix = m_tokens[1];      // 正在编辑对象名
        else
            prefix = "";               // 刚空格，还没输入对象名，列出全部

        for (const auto& obj : cmd->objects) {
            if (obj.name.startsWith(prefix))
                m_candidates << obj.name;
        }
        return;
    }

    // 阶段 2+：参数
    if (m_stage >= 2) {
        if (m_tokens.size() < 2)
            return;

        auto* cmd = CommandRegistry::findCommand(m_tokens[0]);
        if (!cmd) return;

        auto* obj = CommandRegistry::findObject(*cmd, m_tokens[1]);
        if (!obj) return;

        int paramIndex = m_stage - 2;
        if (paramIndex < 0 || paramIndex >= obj->params.size())
            return;

        QString prefix;
        if (m_stage < m_tokens.size())
            prefix = m_tokens[m_stage];   // 正在编辑参数
        else
            prefix = "";                  // 刚空格，还没输入参数，直接给默认值

        const QString& def = obj->params[paramIndex].defaultValue;

        if (prefix.isEmpty() || def.startsWith(prefix))
            m_candidates << def;

        return;
    }
}
