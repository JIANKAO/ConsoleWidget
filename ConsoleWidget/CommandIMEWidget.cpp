#include "CommandIMEWidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QEvent>

CommandIMEWidget::CommandIMEWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::NoFocus); // 关键：窗口不接受焦点
    setMouseTracking(true); // 支持 hover

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    // 上行：解释文本
    m_explanationLabel = new QLabel(this);
    m_explanationLabel->setStyleSheet("color: rgba(255,255,255,220); font-size: 13px;");
    layout->addWidget(m_explanationLabel);

    // 下行：候选项区域
    m_candidateArea = new QWidget(this);
    m_candidateLayout = new QHBoxLayout(m_candidateArea);
    m_candidateLayout->setContentsMargins(0, 0, 0, 0);
    m_candidateLayout->setSpacing(6);
    layout->addWidget(m_candidateArea);

    setFixedHeight(70); // 初步固定高度，后续可调整
}

void CommandIMEWidget::setController(IMEController* controller)
{
    m_controller = controller;
}

void CommandIMEWidget::setInputGeometry(const QRect& rect)
{
    m_inputRect = rect;
}

void CommandIMEWidget::refresh()
{
    if (!m_controller || !m_controller->isActive()) {
        hide();
        return;
    }

    updateExplanation();
    updateCandidates();

    // 计算宽度
    int width = 200;
    for (auto* btn : m_candidateButtons)
        width += btn->sizeHint().width();

    width = qMax(width, 200);
    width = qMin(width, 600);

    // 显示在输入框上方
    int x = m_inputRect.left();
    int y = m_inputRect.top() - height() - 4;

    move(x, y);
    resize(width, height());
    show();
}

void CommandIMEWidget::updateExplanation()
{
    QString text = m_controller->description();
    m_explanationLabel->setText(text);
}

void CommandIMEWidget::updateCandidates()
{
    rebuildCandidateButtons();
}

void CommandIMEWidget::rebuildCandidateButtons()
{
    // 清空旧按钮
    for (auto* btn : m_candidateButtons) {
        m_candidateLayout->removeWidget(btn);
        btn->deleteLater();
    }
    m_candidateButtons.clear();

    const QStringList& list = m_controller->candidates();

    for (int i = 0; i < list.size(); ++i) {
        QPushButton* btn = new QPushButton(list[i], this);
        btn->setStyleSheet(
            "QPushButton { "
            "  background-color: rgba(0,0,0,25); "
            "  color: rgba(255,255,255,220); "
            "  padding: 4px 10px; "
            "  border-radius: 6px; "
            "  border: 1px solid rgba(255,255,255,40); "
            "} "
            "QPushButton:hover { "
            "  background-color: rgba(0,0,0,60); "
            "}"
        );
        btn->setFocusPolicy(Qt::NoFocus); // 不抢焦点
        btn->setMouseTracking(true);
        btn->installEventFilter(this); // 监听 hover

        int index = i;
        connect(btn, &QPushButton::clicked, this, [this, index]() {
            emit candidateClicked(index);
            });

        m_candidateLayout->addWidget(btn);
        m_candidateButtons.push_back(btn);
    }
}

void CommandIMEWidget::leaveEvent(QEvent* event)
{
    // 鼠标离开不关闭窗口
    QWidget::leaveEvent(event);
}

void CommandIMEWidget::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor bg(0, 0, 0, 25); // 黑色 10% 不透明度
    p.setBrush(bg);
    p.setPen(Qt::NoPen);

    p.drawRoundedRect(rect(), 6, 6); // 圆角 6px

    QWidget::paintEvent(event);
}

bool CommandIMEWidget::eventFilter(QObject* obj, QEvent* event)
{
    // 仅处理候选按钮
    auto* btn = qobject_cast<QPushButton*>(obj);
    if (!btn)
        return QWidget::eventFilter(obj, event);

    if (event->type() == QEvent::Enter || event->type() == QEvent::HoverEnter || event->type() == QEvent::MouseMove)
    {
        int index = m_candidateButtons.indexOf(btn);
        if (index >= 0) {
            m_controller->setHighlightIndex(index);
            updateExplanation();
        }
    }

    return QWidget::eventFilter(obj, event);
}
