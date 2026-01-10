#pragma once
#include <QString>
#include <QStringList>
#include <QVector>

//输入法核心:输入框内容 → 解析 → 阶段 → 候选项 → 高亮项 → 解释文本
class IMEController
{
public:
    IMEController();

    // 输入框内容变化时调用
    void updateInput(const QString& text);

    // 当前是否应该显示输入法窗口
    bool isActive() const;

    // 当前候选项
    const QStringList& candidates() const;

    // 当前高亮项（用于 Tab 补全）
    int highlightIndex() const;

    // 设置高亮项（鼠标悬停时调用）
    void setHighlightIndex(int index);

    // 当前高亮项的解释文本
    QString description() const;

    // Tab 补全：返回补全后的文本
    QString applyTabCompletion() const;

    // 数字键补全：返回补全后的文本
    QString applyNumberCompletion(int number) const;

private:
    // 内部解析逻辑
    void parseInput();
    void updateCandidates();
private:
    QString m_input;          // 输入框内容
    QStringList m_tokens;     // 分词结果
    int m_stage = -1;         // 当前阶段（0=命令, 1=对象, 2+=参数）
    QStringList m_candidates; // 候选项
    int m_highlight = 0;      // 高亮项
    bool m_hasTrailingSpace = false;//是否以空格结尾
};
