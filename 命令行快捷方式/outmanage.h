#pragma once
#ifndef OUTPUTMANAGE_H
#define OUTPUTMANAGE_H
#include <qplaintextedit.h>
class C输出管理 {
public:
	C输出管理(QPlainTextEdit &);
	void f打印程序信息();
	void f输出(const QString &文本);	//输出不换行
	void f输出行(const QString &文本);	//输出并换行
	void f换行();
private:
	QPlainTextEdit *w输出框 = nullptr;
};
#endif // OUTPUTMANAGE_H