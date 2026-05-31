#pragma once
#ifndef ARGLISTBUILD_H
#define	ARGLISTBUILD_H
#include <memory>
#include <QWidget>
#include <QTableView>
class I参数控件代理;
class C参数列表;
struct S命令参数;
class I参数列表建造 {
public:
	I参数列表建造(QTableView *父, const S命令参数 &参数);	//派生类应创建控件和代理
	virtual QWidget *f创建控件() = 0;	//创建并保存
	virtual std::unique_ptr<I参数控件代理> f创建代理() const = 0;	//创建不保存
	virtual void fs占位文本(QString) const = 0;
	virtual void fs初始值(QString) const = 0;
	virtual void f连接(C参数列表 *) const = 0;
protected:
	QTableView *const m父 = nullptr;
	const S命令参数 *const m参数 = nullptr;
	QWidget *m控件 = nullptr;
};
class C建造文本框 : public I参数列表建造 {
public:
	using I参数列表建造::I参数列表建造;
	QWidget *f创建控件() override;
	std::unique_ptr<I参数控件代理> f创建代理() const override;
	void fs占位文本(QString) const override;
	void fs初始值(QString) const override;
	void f连接(C参数列表 *) const override;
};
class C建造文件框 : public I参数列表建造 {
public:
	using I参数列表建造::I参数列表建造;
	QWidget *f创建控件() override;
	std::unique_ptr<I参数控件代理> f创建代理() const override;
	void fs占位文本(QString) const override;
	void fs初始值(QString) const override;
	void f连接(C参数列表 *) const override;
};
class C建造复选框 : public I参数列表建造 {
public:
	using I参数列表建造::I参数列表建造;
	QWidget *f创建控件() override;
	std::unique_ptr<I参数控件代理> f创建代理() const override;
	void fs占位文本(QString) const override;
	void fs初始值(QString) const override;
	void f连接(C参数列表 *) const override;
};
class C建造组合框 : public I参数列表建造 {
public:
	using I参数列表建造::I参数列表建造;
	QWidget *f创建控件() override;
	std::unique_ptr<I参数控件代理> f创建代理() const override;
	void fs占位文本(QString) const override;
	void fs初始值(QString) const override;
	void f连接(C参数列表 *) const override;
};
#endif // ARGLISTBUILD_H