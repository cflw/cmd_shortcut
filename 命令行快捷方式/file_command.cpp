#include "file_command.h"
#include <QList>
#include "xml_utility.h"
#include "file_setting.h"
//==============================================================================
// 参数
//==============================================================================
S命令参数::S命令参数(const QDomElement &a参数节点):
	m名称(a参数节点.attribute("名称")),
	m描述(a参数节点.firstChildElement("描述").text()),
	m提示(a参数节点.firstChildElement("提示").text()),
	m模板(a参数节点.firstChildElement("模板").text()),
	m过滤器(a参数节点.firstChildElement("过滤器").text()),
	m参数类型(f解析参数类型(a参数节点.attribute("类型"))),
	m必需(a参数节点.firstChildElement("必需").text() == "true"),
	m绑定设置(a参数节点.firstChildElement("绑定设置").text() == "true"),
	m多参数(a参数节点.firstChildElement("多参数").text() == "true"),
	mf模板(F自动参数模板(a参数节点)) {
	if (const QString v冲突参数s = a参数节点.firstChildElement("冲突参数").text(); !v冲突参数s.isEmpty()) {
		for (const QString &v冲突参数 : v冲突参数s.split(",")) {
			ma冲突参数.push_back(v冲突参数);
		}
	}
}
QString S命令参数::f生成(const QString &a值) const {
	return mf模板(a值);
}
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
	m文件名 = QFileInfo(a路径).fileName();
	//打开文档
	QDomDocument v文档;
	if (!openXmlWithDom(a路径, v文档)) {
		m打开 = false;
		return false;
	}
	const QDomElement v根节点 = v文档.documentElement();
	//命令
	const QDomElement v命令节点 = v根节点.firstChildElement("命令");
	m名称 = v命令节点.firstChildElement("名称").text();
	m平台 = f解析平台(v命令节点.firstChildElement("平台").text());
	m用法 = v命令节点.firstChildElement("用法").text();
	//形参组
	const QDomElement v参数组节点 = v根节点.firstChildElement("参数组");
	for (const QDomNode &v参数节点 : v参数组节点.elementsByTagName("参数")) {
		ma命令参数.emplace_back(v参数节点.toElement());
	}
	m命令模板 = C命令模板(v命令节点.firstChildElement("模板").text(), ma命令参数);
	//结束
	m打开 = true;
	return true;
}
void C命令文件::fs设置(const S命令设置 &a设置) {
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
const S命令设置 &C命令文件::fg设置() const {
	return *m命令设置;
}
//==============================================================================
// 命令文件缓存
//==============================================================================
C命令文件 *C命令文件缓存::f打开(const QString &a文件名) {
	if (auto iter = ma命令文件.find(a文件名); iter != ma命令文件.end()) {
		const std::unique_ptr<C命令文件> &vp命令文件 = iter->second;
		return vp命令文件->fi打开() ? vp命令文件.get() : nullptr;
	}
	C命令文件 *v新文件 = new C命令文件();
	v新文件->f打开(C命令文件::c命令目录.filePath(a文件名));
	ma命令文件.emplace(a文件名, v新文件);
	return v新文件->fi打开() ? v新文件 : nullptr;
}
