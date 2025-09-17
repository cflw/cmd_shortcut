#pragma once
#ifndef ARGLIST_H
#define ARGLIST_H
#include <memory>
class MainWindow;
class QStandardItemModel;
struct S参数;
class C参数列表控制 {
public:
	C参数列表控制(MainWindow &);
	void f清空行();
	void f添加行(const S参数 &);
private:
	MainWindow *m主窗口 = nullptr;
	std::unique_ptr<QStandardItemModel> m参数列表模型;
};
#endif	//ARGLIST_H