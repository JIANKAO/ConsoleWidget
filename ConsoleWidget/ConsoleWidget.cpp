#include "ConsoleWidget.h"
#include <QVBoxLayout>
#include <QKeyEvent>

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QWidget(parent)
{
    // 整体布局
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    // 创建输入框
    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("输入 / 开始命令...");
    layout->addWidget(m_input);

    // 创建 IME 控制器
    m_imeController = new IMEController();

    // 创建 IME UI
    m_imeWidget = new CommandIMEWidget(nullptr); // 独立窗口
    m_imeWidget->setController(m_imeController);
    m_imeWidget->hide();

    // 输入框事件
    connect(m_input, &QLineEdit::textChanged,this, &ConsoleWidget::onInputChanged);

    connect(m_imeWidget, &CommandIMEWidget::candidateClicked,this, &ConsoleWidget::onCandidateClicked);

    // 捕获按键
    m_input->installEventFilter(this);
}

ConsoleWidget::~ConsoleWidget()
{}


void ConsoleWidget::onInputChanged(const QString& text)
{
    m_imeController->updateInput(text);

    // 如果 IME 被挂起，则不显示输入法
    if (m_imeSuspended)
    {
        // 只有当用户输入空格时才解除挂起
        if (text.endsWith(' '))
        {
            m_imeSuspended = false; // 解除挂起
        }
        else
        {
            m_imeWidget->hide();
            return; // 不显示输入法
        }
    }

    // 输入法未激活 → 隐藏
    if (!m_imeController->isActive()) {
        m_imeWidget->hide();
        return;
    }

    // 正常显示输入法
    QRect rect = m_input->geometry();
    QPoint globalPos = m_input->mapToGlobal(QPoint(0, 0));
    rect.moveTopLeft(globalPos);

    m_imeWidget->setInputGeometry(rect);
    m_imeWidget->refresh();
}

bool ConsoleWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_input && event->type() == QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);

        if (!m_imeController->isActive())
            return QWidget::eventFilter(obj, event);

        // Tab 补全
        if (key->key() == Qt::Key_Tab)
        {
            QString newText = m_imeController->applyTabCompletion();
            m_input->setText(newText);

            // 补全后暂时关闭 IME，等待用户空格再打开
            m_imeSuspended = true;
            m_imeWidget->hide();
            return true;
        }
        // 数字键 1~7 补全
        if (key->key() >= Qt::Key_1 && key->key() <= Qt::Key_7)
        {
            int number = key->key() - Qt::Key_0;
            QString newText = m_imeController->applyNumberCompletion(number);
            m_input->setText(newText);

            m_imeSuspended = true;
            m_imeWidget->hide();
            return true;
        }

        // 空格：进入下一阶段（正常插入空格）
        if (key->key() == Qt::Key_Space)
        {
            return QWidget::eventFilter(obj, event);
        }

        // Esc：关闭输入法
        if (key->key() == Qt::Key_Escape)
        {
            m_imeWidget->hide();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void ConsoleWidget::onCandidateClicked(int index)
{
    QString newText = m_imeController->applyNumberCompletion(index + 1);
    m_input->setText(newText);

    m_imeSuspended = true;
    m_imeWidget->hide();
}
