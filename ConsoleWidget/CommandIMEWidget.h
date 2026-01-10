#pragma once
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVector>
#include <QPushButton>
#include "IMEController.h"

class CommandIMEWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommandIMEWidget(QWidget* parent = nullptr);

    // 绑定 IMEController
    void setController(IMEController* controller);

    // 更新 UI（由外部输入框触发）
    void refresh();

    // 设置输入框位置（用于定位）
    void setInputGeometry(const QRect& rect);

protected:
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event);
    bool eventFilter(QObject* obj, QEvent* event) override;
private:
    void updateExplanation();
    void updateCandidates();
    void rebuildCandidateButtons();
signals:
    void candidateClicked(int index);

private:
    IMEController* m_controller = nullptr;

    QLabel* m_explanationLabel = nullptr;
    QWidget* m_candidateArea = nullptr;
    QHBoxLayout* m_candidateLayout = nullptr;

    QVector<QPushButton*> m_candidateButtons;

    QRect m_inputRect; // 输入框位置
};
