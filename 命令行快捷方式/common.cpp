#include "common.h"
#include <map>
#include <QList>
const std::map<E参数类型, QString> ca值映射 = {
	{E参数类型::e任意, "任意"},
	{E参数类型::e整数, "整数"},
	{E参数类型::e路径, "路径"},
	{E参数类型::e开关, "开关"},
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
const std::map<E平台, QString> ca平台映射 = {
	{E平台::windows, "windows"},
	{E平台::linux, "linux"},
	{E平台::macos, "macos"},
};
QString f生成平台(E平台 a平台) {
	QString v返回;
	for (const auto &[k, v] : ca平台映射) {
		if (a平台 & k) {
			v返回 += v + ',';
		}
	}
	return v返回.left(v返回.size() - 1);
}
E平台 f解析平台(const QString &aa平台) {
	if (aa平台.isEmpty()) {
		return E平台::e全部;
	}
	E平台 v返回 = E平台::e空;
	QList<QString> va拆分 = aa平台.split(',');
	for (const auto &[k, v] : ca平台映射) {
		for (const QString &v平台 : va拆分) {
			if (v == v平台) {
				v返回 = static_cast<E平台>(v返回 | k);
			}
		}
	}
	return v返回;
}
