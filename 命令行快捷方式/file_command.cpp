#include "file_command.h"
#include <QList>
#include "xml_utility.h"
#include "file_setting.h"
C命令文件 *g当前打开命令 = nullptr;
//==============================================================================
// 命令文件
//==============================================================================
const QDir C命令文件::c命令目录("./command");
std::generator<QString> C命令文件::fe所有文件() {
	const QStringList va文件 = c命令目录.entryList(QDir::Files);
	for (const QString &v文件名 : va文件) {
		if (v文件名.right(4) == ".xml") {
			co_yield v文件名;
		}
	}
}
bool C命令文件::f打开(const QString &a路径) {
	g当前打开命令 = this;
	m文件名 = QFileInfo(a路径).fileName();
	ma命令参数.clear();	//清空旧的形参,避免重新打开时重复累积
	//打开文档
	QDomDocument v文档;
	if (!openXmlWithDom(a路径, v文档)) {
		m打开 = false;
		return false;
	}
	const QDomElement v根节点 = v文档.documentElement();
	//命令
	const QDomElement v命令节点 = v根节点.firstChildElement("命令");
	m名称 = v命令节点.firstChildElement("名称").text().trimmed();
	m平台 = f解析平台(v命令节点.firstChildElement("平台").text());
	m用法 = v命令节点.firstChildElement("用法").text().trimmed();
	m分组 = v命令节点.firstChildElement("分组").text().trimmed();
	//形参组
	const QDomElement v参数组节点 = v根节点.firstChildElement("参数组");
	for (const QDomNode &v参数节点 : v参数组节点.elementsByTagName("参数")) {
		ma命令参数.emplace_back(v参数节点.toElement());
	}
	m命令模板 = C命令模板(v命令节点.firstChildElement("模板").text(), ma命令参数);
	//检查文件
	//结束
	m打开 = true;
	return true;
}
void C命令文件::fs设置(const C命令设置 &a设置) {
	m命令设置 = &a设置;
}
const QString &C命令文件::fg文件名() const {
	return m文件名;
}
const QString &C命令文件::fg名称() const {
	return m名称;
}
E平台 C命令文件::fg平台() const {
	return m平台;
}
const QString &C命令文件::fg用法() const {
	return m用法;
}
const QString &C命令文件::fg描述() const {
	return m描述;
}
const QString &C命令文件::fg分组() const {
	return m分组;
}
std::vector<S命令参数> &C命令文件::fe命令参数() {
	return ma命令参数;
}
const std::vector<S命令参数> &C命令文件::fe命令参数() const {
	return ma命令参数;
}
bool C命令文件::fi打开() const {
	return m打开;
}
QString C命令文件::f生成(const C实参组 &aa参数) const {
	return m命令模板.f生成(aa参数);
}
QString C命令文件::fg绑定参数值(const QString &a绑定参数名) const {
	return m命令设置->fg绑定参数值(a绑定参数名);
}
const C命令设置 &C命令文件::fg设置() const {
	return *m命令设置;
}
//==============================================================================
// 命令文件缓存
//==============================================================================
C命令文件 *C命令文件缓存::f打开(const QString &a文件名, bool a重新打开) {
	if (auto iter = ma命令文件.find(a文件名); iter != ma命令文件.end()) {
		const std::unique_ptr<C命令文件> &vp命令文件 = iter->second;
		g当前打开命令 = vp命令文件.get();
		if (a重新打开) {
			vp命令文件->f打开(C命令文件::c命令目录.filePath(a文件名));
		}
		return vp命令文件->fi打开() ? vp命令文件.get() : nullptr;
	}
	C命令文件 *v新文件 = new C命令文件();
	v新文件->f打开(C命令文件::c命令目录.filePath(a文件名));
	ma命令文件.emplace(a文件名, v新文件);
	return v新文件->fi打开() ? v新文件 : nullptr;
}
