#ifndef FILE_COMMAND_H
#define FILE_COMMAND_H
#include <vector>
#include <map>
#include <generator>
#include <QList>
#include <QDomDocument>
#include <QDir>
#include "common.h"
#include "argtemplate.h"
struct S命令设置;
//==============================================================================
// 参数
//==============================================================================
struct S命令参数 {
public:
	S命令参数() = default;
	S命令参数(const QDomElement &);
	QString f生成(const QString &) const;
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
	QStringList ma冲突参数;
};
//==============================================================================
// 命令文件
//==============================================================================
class C命令文件 {
	friend class C命令文件缓存;
public:
	static const QDir c命令目录;
	static std::generator<QString> fe所有文件();	//列出command文件夹所有xml文件
	bool f打开(const QString &路径);	//目前无编辑命令功能,所以没写保存函数
	void fs设置(const S命令设置 &);
	//属性
	const QString &fg文件名() const;
	const QString &fg名称() const;
	E平台 fg平台() const;
	const QString &fg用法() const;
	const QString &fg描述() const;
	std::vector<S命令参数> &fe命令参数();
	const std::vector<S命令参数> &fe命令参数() const;
	bool fi打开() const;
	QString f生成(const C实参组 &) const;
	QString fg绑定参数值(const QString &名称) const;
	const S命令设置 &fg设置() const;	//关联的设置
private:
	QString m文件名;
	QString m名称;	//命令名称,显示在左侧列表
	C命令模板 m命令模板;
	QString m用法;	//帮助文本:命令格式
	QString m描述;	//帮助文本:命令说明
	std::vector<S命令参数> ma命令参数;
	E平台 m平台;	//命令可以在哪些操作系统使用,可能组合
	bool m打开 = false;
	const S命令设置 *m命令设置 = nullptr;	//关联的命令设置,用来获取绑定参数值
};
class C命令文件缓存 {	//命令文件可能多次打开,所以写一个可以缓存命令文件的类
public:
	C命令文件 *f打开(const QString &文件名);
private:
	std::map<QString, std::unique_ptr<C命令文件>> ma命令文件;
};

#endif // FILE_COMMAND_H
