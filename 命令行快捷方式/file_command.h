#ifndef FILE_COMMAND_H
#define FILE_COMMAND_H
#include <vector>
#include <map>
#include <generator>
#include <QList>
#include <QDir>
#include "common.h"
#include "cmdarg.h"
class C命令设置;
//==============================================================================
// 命令文件
//==============================================================================
class C命令文件 {
	friend class C命令文件缓存;
public:
	static const QDir c命令目录;
	static std::generator<QString> fe所有文件();	//列出command文件夹所有xml文件
	bool f打开(const QString &路径);	//目前无编辑命令功能,所以没写保存函数
	void fs设置(const C命令设置 &);
	//属性
	const QString &fg文件名() const;
	const QString &fg名称() const;
	E平台 fg平台() const;
	const QString &fg用法() const;
	const QString &fg描述() const;
	const QString &fg分组() const;
	std::vector<S命令参数> &fe命令参数();
	const std::vector<S命令参数> &fe命令参数() const;
	bool fi打开() const;
	QString f生成(const C实参组 &) const;
	QString fg绑定参数值(const QString &名称) const;
	const C命令设置 &fg设置() const;	//关联的设置
private:
	QString m文件名;
	QString m名称;	//命令名称,显示在左侧列表
	C命令模板 m命令模板;
	QString m用法;	//帮助文本:命令格式
	QString m描述;	//帮助文本:命令说明
	QString m分组;	//命令文件指定的默认分组(无则为空)
	std::vector<S命令参数> ma命令参数;
	E平台 m平台;	//命令可以在哪些操作系统使用,可能组合
	bool m打开 = false;
	const C命令设置 *m命令设置 = nullptr;	//关联的命令设置,用来获取绑定参数值
};
class C命令文件缓存 {	//命令文件可能多次打开,所以写一个可以缓存命令文件的类
public:
	C命令文件 *f打开(const QString &文件名);
private:
	std::map<QString, std::unique_ptr<C命令文件>> ma命令文件;
};
extern C命令文件 *g当前打开命令;
#endif // FILE_COMMAND_H
