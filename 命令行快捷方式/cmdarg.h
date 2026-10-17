#pragma once
#ifndef CMDARG_H
#define CMDARG_H
#include <QString>
#include <QDomDocument>
#include "argtemplate.h"
#include "argcombobox.h"
//==============================================================================
// 类型
//==============================================================================
enum E参数类型 {	//禁止组合
	e任意,
	e整数,
	e路径,	//控件:文本框+打开文件按钮
	e开关,	//控件:复选框
	e选项,	//控件:组合框
};
QString f生成参数类型(E参数类型);
E参数类型 f解析参数类型(const QString &);
//==============================================================================
// 参数
//==============================================================================
struct S命令参数 {
public:
	S命令参数() = default;
	S命令参数(const QDomElement &);
	QString f生成(const QString &) const;
	QString fg默认值() const;
	QString f选项_g值(int) const;
	int f选项_默认索引() const;	//找默认选项的索引,没有则返回-1
	int f选项_找索引(const QString &) const;	//找值所在的索引,没有则返回-1
public:
	QString m名称;
	QString m描述;	//帮助文本:参数说明
	QString m提示;
	QString m模板;	//模板字符串
	QString m过滤器;	//参数类型=路径 用到
	tf参数模板 mf模板;	//模板
	E参数类型 m参数类型 = E参数类型::e任意;
	bool m必需 = false;	//参数不可缺少,否则命令无效
	bool m绑定设置 = false;	//参数值保存到设置文件中
	bool m多参数 = false;	//是否可以指定多个同名参数
	QString m分组;	//参数分组(来自命令文件<参数>的分组属性, 空表示未分组)
	QStringList ma冲突参数;
	std::vector<S组合框选项> ma选项;	//参数类型=选项 用到
};
#endif // CMDARG_H