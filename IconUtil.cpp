#include "IconUtil.h"

QMap<QString, QIcon> IconUtil::cache;

const QIcon &IconUtil::load(const QString &path) {
    const auto it{cache.find(path)};
    if (it != cache.end()) {
        return it.value();
    }
    return cache.insert(path, QIcon{path}).value();
}
