#pragma once
#ifndef CMDMANAGE_H
#define CMDMANAGE_H
#include <memory>
#include <deque>
#include <QProcess>
class C输出管理;
enum E执行条件 {	//命令队列中本条命令的执行条件(由连续执行符号 & && || 决定)
	e无条件,	//& 或第一条命令: 直接执行
	e成功后,	//&& : 上一条命令成功后执行
	e失败后,	//|| : 上一条命令失败后执行
};
struct S待执行命令 {
	QString m命令;
	QString m工作目录;
	E执行条件 m执行条件 = E执行条件::e无条件;
};
class C命令管理 : public QObject {
	Q_OBJECT
public:
	C命令管理(QWidget *父, C输出管理 &);
	void f添加命令(const S待执行命令 &);	//点击执行按钮调用,命令添加到命令队列
	void f调度命令();	//从命令队列执行命令
	void f停止命令();	//点击停止按钮调用,停止当前正在执行的命令,如果命令队列中还有命令,则继续执行下一条命令
	bool fi正在执行() const;	//进程正在运行或队列中仍有待执行命令
private slots:
	void onReadyReadOutput();
	void onReadyReadError();
	void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
private:
	void f开始命令(const S待执行命令 &命令);	//执行进程直接执行命令
	bool f条件满足(E执行条件) const;	//根据上一条命令的执行结果判断本条命令是否执行
	std::unique_ptr<QProcess> m执行进程;
	std::deque<S待执行命令> m命令队列;
	C输出管理 *m输出 = nullptr;
	bool m手动停止 = false;	//标记当前命令是否被手动停止
	bool m上条成功 = true;	//上一条命令是否成功(退出代码0且正常退出)
};
#endif // CMDMANAGE_H