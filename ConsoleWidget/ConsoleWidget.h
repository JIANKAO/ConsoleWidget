#pragma once

#include <QtWidgets/QWidget>
#include "IMEController.h"
#include "CommandIMEWidget.h"
#include <QLineEdit>

class ConsoleWidget : public QWidget
{
	Q_OBJECT

public:
	ConsoleWidget(QWidget* parent = nullptr);
	~ConsoleWidget();

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

private:
	QLineEdit* m_input = nullptr;

	IMEController* m_imeController = nullptr;
	CommandIMEWidget* m_imeWidget = nullptr;

	bool m_imeSuspended = false; // 新增：IME 是否因补全而暂时关闭

private slots:
	void onInputChanged(const QString& text);
	void onCandidateClicked(int index);
};

