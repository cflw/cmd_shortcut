#pragma once
#ifndef ARGFILEBOX_H
#define ARGFILEBOX_H
#include <memory>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include "argtextbox.h"
struct S命令参数;
class W参数文件框 : public QWidget {
	Q_OBJECT
public:
	W参数文件框(QWidget *, const S命令参数 &);
	//属性
	QString text() const;
	void setText(const QString &);
	QSize sizeHint() const override;
	void setPlaceholderText(const QString &);
	W参数文本框 &fg文本框() const;
private slots:
	void on_filebutton_clicked();
private:
	std::unique_ptr<W参数文本框> w文本框;
	std::unique_ptr<QPushButton> w文件按钮;
	std::unique_ptr<QHBoxLayout> w布局;
	const S命令参数 *m参数 = nullptr;
};
#endif	//ARGFILEBOX_H