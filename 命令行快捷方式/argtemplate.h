#pragma once
#ifndef ARGTEMPLATE_H
#define ARGTEMPLATE_H
#include <vector>
#include <functional>
#include <generator>
#include <QString>
struct S命令参数;
class QDomElement;
//命令参数
class C实参组 {	//保存模板参数值的容器,键值对,允许多个同名键
public:
	void f清空();
	bool fi空() const;
	void f添加(const QString &键, const QString &值);
	QString f查找(const QString &键) const;
	std::generator<QString> fe查找(const QString &键) const;
	bool fi存在(const QString &键) const;
	std::vector<std::pair<QString, QString>> ma参数值;
};
//参数模板
using tf参数模板 = std::function<QString(const QString &)>;
tf参数模板 F自动参数模板(const QDomElement &);
//生成命令
struct S生成 {
	const S命令参数 *m参数 = nullptr;
	tf参数模板 mf生成;	//来自S参数,单独拿出来是为了确保没参数时也可以调用
};
std::vector<S生成> f构造生成序列(const QString &, const std::vector<S命令参数> &);	//先解析<参数组>再解析<模板>
//命令模板
class C命令模板 {	//命令模板,生成命令
public:
	C命令模板() = default;
	C命令模板(const QString &, const std::vector<S命令参数> &);
	operator const QString &() const;
	QString f生成(const C实参组 &) const;
private:
	std::vector<S生成> ma生成;
	QString m模板;	//模板字符串
};
#endif // ARGTEMPLATE_H