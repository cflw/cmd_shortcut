#ifndef FILE_SETTING_H
#define FILE_SETTING_H
#include <vector>
#include <QString>
#include <QDomDocument>
#include <QDir>
#include <generator>
#include "common.h"
class C实参组;
class C命令文件;
//==============================================================================
// 参数
//==============================================================================
using t绑定参数 = std::pair<const QString, QString>;
t绑定参数 f绑定参数_构造(const QDomElement &);
QDomElement f绑定参数_转元素(QDomDocument &, const t绑定参数 &);
struct S命令设置 {
	friend class C设置文件;
public:
	S命令设置() = default;
	S命令设置(const QDomElement &);
	QDomElement ft元素(QDomDocument &) const;
	QString fg文件名() const;
	bool fs绑定参数值(const QString &, const QString &);
	QString fg绑定参数值(const QString &) const;
public:
	QString m文件名;
	QString m工作目录;
	std::map<QString, QString> ma绑定参数;
	bool m启用 = false;	//是否在命令列表中显示
};
//==============================================================================
// 设置文件
//==============================================================================
class C设置文件 {
public:
	static const QString c设置目录名;
	static const QDir c设置目录;	//设置文件的默认目录
	static const QString c主机名;
	C设置文件();
	static QString fg保存文件名();	//每一台电脑的设置文件名是不同的
	static QString fg保存路径();
	bool f打开(const QString &路径);
	bool f保存(const QString &路径);
	QDomDocument ft文档() const;
	std::generator<S命令设置 &> fe命令设置();
	std::generator<S命令设置 &> fe启用命令();
	std::generator<QString> fe所有文件() const;
	std::generator<QString> fe启用文件() const;
	bool fi空() const;
	S命令设置 &f构造命令();	//在启动的对比文件阶段调用
	S命令设置 *fg命令设置(const QString &命令文件名);
	//属性
	const QString &fg主机名() const;
	E平台 fg平台() const;
	void f更新绑定参数(const C命令文件 &a命令文件, const C实参组 &参数组);
	void f更新工作目录(const C命令文件 &a命令文件, const QString &工作目录);
private:
	QString m主机名;
	std::vector<std::unique_ptr<S命令设置>> ma命令设置;
	E平台 m平台 = c当前平台;	//当前平台,没有组合
	bool m编辑 = false;	//编辑标志,为true说明编辑过需要保存
};

#endif // FILE_SETTING_H
