#pragma once
#ifndef ARGCHECKBOX_H
#define ARGCHECKBOX_H
#include <QCheckBox>
#include "arginterface.h"
class C参数复选框代理 : public I参数控件代理 {
public:
	C参数复选框代理(QCheckBox &);
	QString fg值() const override;
	void fs值(const QString &) const override;
	bool fi值() const override;
	void fs颜色(E颜色) const override;
	void f清除输入() const override;
	QCheckBox *w复选框 = nullptr;
};
#endif	//ARGCHECKBOX_H