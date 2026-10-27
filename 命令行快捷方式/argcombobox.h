#pragma once
#ifndef ARGCOMBOBOX_H
#define ARGCOMBOBOX_H
#include <QComboBox>
#include "arginterface.h"
//参数
struct S组合框选项 {
	QString m名称;	//显示名称,没有则显示值; 也用于<参数>的<默认值>指定默认选项
	QString m值;	//参数值,必需
	QString fg显示文本() const;	//格式: 名称(参数)
};
//代理
class C参数组合框代理 : public I参数控件代理 {
public:
	C参数组合框代理(QComboBox&);
	QString fg值() const override;
	void fs值(const QString&) const override;
	bool fi值() const override;	//是否输入值
	void fs颜色(E颜色) const override;	//用来提示
	void f清除输入() const override;
	void fs索引(int) const;
	int fg索引() const;
	QComboBox *w组合框 = nullptr;
};
#endif	//ARGCOMBOBOX_H