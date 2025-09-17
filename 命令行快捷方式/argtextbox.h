#pragma once
#ifndef ARGTEXTBOX_H
#define ARGTEXTBOX_H
#include <QLineEdit>
#include "arginterface.h"
//控件
class W参数文本框 : public QLineEdit {
	Q_OBJECT
public:
	W参数文本框(QWidget *);
signals:
	void setMoreText(const QStringList &);	//通过 选择文件/拖曳 输入了多个文本,在参数列表管理类处理多文本的情况
protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dropEvent(QDropEvent *event) override;
private:
	QString m拖曳前样式;
};
//代理
class C参数文本框代理 : public I参数控件代理 {
public:
	C参数文本框代理(QLineEdit &);
	QString fg值() const override;
	void fs值(const QString &) const override;
	bool fi值() const override;
	void fs颜色(E颜色) const override;
	void f清除输入() const override;
	QLineEdit *w文本框 = nullptr;
};
//函数
void f文本框_s颜色(QLineEdit &, E颜色);
#endif	//ARGTEXTBOX_H