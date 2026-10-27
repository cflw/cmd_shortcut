#include <map>
#include "cmdarg.h"
#include "outmanage.h"
#include "file_command.h"
//==============================================================================
// 参数类型
//==============================================================================
const std::map<E参数类型, QString> ca值映射 = {
	{E参数类型::e任意, "任意"},
	{E参数类型::e整数, "整数"},
	{E参数类型::e路径, "路径"},
	{E参数类型::e开关, "开关"},
	{E参数类型::e选项, "选项"},
};
QString f生成参数类型(E参数类型 a参数类型) {
	return ca值映射.at(a参数类型);
}
E参数类型 f解析参数类型(const QString &a参数类型) {
	for (const auto &[k, v] : ca值映射) {
		if (v == a参数类型) {
			return k;
		}
	}
	return E参数类型::e任意;
}
//==============================================================================
// 参数
//==============================================================================
S组合框选项 f解析组合框选项(const QDomElement &a选项节点) {
	S组合框选项 v选项;
	v选项.m名称 = a选项节点.attribute("名称");
	v选项.m值 = a选项节点.attribute("值");
	return v选项;
}
S命令参数::S命令参数(const QDomElement &a参数节点) :
	m名称(a参数节点.attribute("名称")),
	m描述(a参数节点.firstChildElement("描述").text()),
	m提示(a参数节点.firstChildElement("提示").text()),
	m模板(a参数节点.firstChildElement("模板").text()),
	m默认值(a参数节点.firstChildElement("默认值").text().trimmed()),	//参数默认值
	m过滤器(a参数节点.firstChildElement("过滤器").text()),
	m参数类型(f解析参数类型(a参数节点.attribute("类型"))),
	m必需(a参数节点.firstChildElement("必需").text() == "true"),
	m绑定设置(a参数节点.firstChildElement("绑定设置").text() == "true"),
	m多参数(a参数节点.firstChildElement("多参数").text() == "true"),
	m分组(a参数节点.attribute("分组").trimmed()),
	mf模板(F自动参数模板(a参数节点)) {
	if (const QString v冲突参数s = a参数节点.firstChildElement("冲突参数").text(); !v冲突参数s.isEmpty()) {
		for (const QString &v冲突参数 : v冲突参数s.split(",")) {
			ma冲突参数.push_back(v冲突参数);
		}
	}
	switch (m参数类型) {
	case E参数类型::e选项:
		for (const QDomNode &v选项节点 : a参数节点.elementsByTagName("选项")) {
			const QDomElement v选项元素 = v选项节点.toElement();
			const S组合框选项 &v选项 = ma选项.emplace_back(f解析组合框选项(v选项元素));
			if (v选项.m值.isEmpty()) {
				C输出管理::g这->f输出行(QString("警告: 命令文件\"%1\"的参数\"%2\"的选项\"%3\"的值为空白").arg(g当前打开命令->fg文件名(), m名称, v选项.m名称));
			}
		}
		if (ma选项.empty()) {
			C输出管理::g这->f输出行(QString("警告: 命令文件\"%1\"的参数\"%2\"类型为选项, 但是选项空白").arg(g当前打开命令->fg文件名(), m名称));
		}
		if (m多参数) {
			C输出管理::g这->f输出行(QString("警告: 命令文件\"%1\"的参数\"%2\"类型为选项, 且有多参数, 多参数将被忽略").arg(g当前打开命令->fg文件名(), m名称));
		}
		if (!m默认值.isEmpty() && f选项_找名称索引(m默认值) == -1 && f选项_找索引(m默认值) == -1) {
			C输出管理::g这->f输出行(QString("警告: 命令文件\"%1\"的参数\"%2\"的默认值\"%3\"不是有效的选项名称").arg(g当前打开命令->fg文件名(), m名称, m默认值));
		}
		break;
	}
}
QString S命令参数::f生成(const QString &a值) const {
	return mf模板(a值);
}
QString S命令参数::fg默认值() const {
	switch (m参数类型) {
	case E参数类型::e选项:	//选项的<默认值>填的是选项名称,需转换成选项值
		return f选项_g值(f选项_默认索引());
	case E参数类型::e开关:	//开关只有默认值为true时才勾选
		return (m默认值 == "true") ? "true" : "";
	default:
		return m默认值;
	}
}
QString S命令参数::f选项_g值(int a索引) const {
	return (a索引 != -1) ? ma选项.at(a索引).m值 : "";
}
int S命令参数::f选项_默认索引() const {
	if (m默认值.isEmpty()) {
		return -1;
	}
	const int v名称索引 = f选项_找名称索引(m默认值);
	if (v名称索引 != -1) {
		return v名称索引;
	}
	return f选项_找索引(m默认值);	//名称未匹配时,兼容按选项值填写默认值
}
int S命令参数::f选项_找索引(const QString &a值) const {
	const int c大小 = ma选项.size();
	for (int i = 0; i != c大小; ++i) {
		const S组合框选项 &v选项 = ma选项.at(i);
		if (v选项.m值 == a值) {
			return i;
		}
	}
	return -1;
}
int S命令参数::f选项_找名称索引(const QString &a名称) const {
	const int c大小 = ma选项.size();
	for (int i = 0; i != c大小; ++i) {
		const S组合框选项 &v选项 = ma选项.at(i);
		if (v选项.m名称 == a名称) {
			return i;
		}
	}
	return -1;
}