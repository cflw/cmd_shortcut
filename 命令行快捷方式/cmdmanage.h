#pragma once
#ifndef CMDMANAGE_H
#define CMDMANAGE_H
#include <memory>
#include <deque>
#include <QProcess>
class C输出管理;
struct S待执行命令 {
	QString m命令;
	QString m工作目录;
};
class C命令管理 : public QObject {
	Q_OBJECT
public:
	C命令管理(QWidget *父, C输出管理 &);
	void f添加命令(const S待执行命令 &);	//命令添加到命令队列
	void f调度命令();	//从命令队列执行命令
private slots:
	void onReadyReadOutput();
	void onReadyReadError();
	void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
private:
	void f开始命令(const S待执行命令 &命令);	//执行进程直接执行命令
	std::unique_ptr<QProcess> m执行进程;
	std::deque<S待执行命令> m命令队列;
	C输出管理 *m输出 = nullptr;
};
#endif // CMDMANAGE_H