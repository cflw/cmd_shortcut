#pragma once
#ifndef ARGLISTMANAGE_H
#define ARGLISTMANAGE_H
#include <memory>
#include <QMainWindow>
#include <QStandardItemModel>
#include "arginterface.h"
class QTableView;
class QPushButton;
class MainWindow;
struct S命令参数;
class C命令文件;
class C实参组;
class C参数列表 : public QObject {	//管理参数列表的类
	Q_OBJECT;
public:
	static constexpr int c列数 = 3;	//标签, 输入框, 删除按钮
	static constexpr int c标签列 = 0;
	static constexpr int c输入列 = 1;
	static constexpr int c删除列 = 2;
	struct S附加数据 {
		const S命令参数 *m参数 = nullptr;
		std::unique_ptr<const I参数控件代理> m代理;
	};
	C参数列表(MainWindow &);	//主窗口显示后调用
	void f重置尺寸(int 宽);
	void f刷新列表(const C命令文件 &);
	int fg行数() const;
	void f清空所有行();
	void f添加行(const S命令参数 &);	//在末尾添加并构造
	void f插入行(int 位置, const S命令参数 &);	//插入并构造
	void f构造行(int 位置, const S命令参数 &, S附加数据 &);
	void f删除行(int 行号);	//从0开始
	void f删除行0(int 行号);	//只删除控件
	void f删除行1(int 行号);	//删除指定行(不删除控件)和附加数据,后面控件往前挪
	bool fi可执行() const;	//必需参数是否都已填写
	void f清除输入(int 行号);
	C实参组 f生成实参组();	//创建临时对象
	int fg输入框所在行(QWidget *) const;	//控件不含位置信息,只能遍历查找
	int fg删除按钮所在行(QPushButton *) const;
private slots:
	void on_argedit_textEdited(const QString &);
	void on_argcheck_stateChanged(int);
	void on_argdelete_clicked();
	void on_argedit_returnPress();
	void on_argedit_setMoreText(const QStringList &);
private:
	MainWindow *w主窗口 = nullptr;
	QTableView *w参数列表 = nullptr;
	const C命令文件 **mp当前命令 = nullptr;
	std::unique_ptr<QStandardItemModel> m模型;
	std::vector<S附加数据> ma附加数据;	//控件以外的变量
	std::map<QString, int> ma多参数计数;
};
#endif	//ARGLISTMANAGE_H