#pragma once
#ifndef ARGLIST_H
#define ARGLIST_H
#include <memory>
class MainWindow;
class QStandardItemModel;
struct S����;
class C�����б���� {
public:
	C�����б����(MainWindow &);
	void f�����();
	void f�����(const S���� &);
private:
	MainWindow *m������ = nullptr;
	std::unique_ptr<QStandardItemModel> m�����б�ģ��;
};
#endif	//ARGLIST_H