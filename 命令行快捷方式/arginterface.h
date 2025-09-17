#pragma once
#ifndef ARGINTERFACE_H
#define ARGINTERFACE_H
#include <QString>
struct S命令参数;
enum E颜色 {	//用来提示
	e白,	//正常状态
	e红,	//参数错误
	e绿,	//拖曳提示
	e蓝,
};
class I参数控件代理 {
public:
	virtual QString fg值() const = 0;
	virtual void fs值(const QString &) const = 0;
	virtual bool fi值() const = 0;	//是否输入值
	virtual void fs颜色(E颜色) const = 0;	//用来提示
	virtual void f清除输入() const = 0;
};
#endif	//ARGINTERFACE_H