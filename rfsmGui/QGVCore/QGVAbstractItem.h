/***************************************************************

***************************************************************/
#ifndef QGV_ABSTRACT_ITEM_H
#define QGV_ABSTRACT_ITEM_H

#include <QString>

class QGVAbstractItem
{
public:
    QGVAbstractItem() {}
    ~QGVAbstractItem() { }

    virtual void setAttribute(const QString &label, const QString &value) = 0;
    virtual QString getAttribute(const QString &name) const = 0;
};


#endif // QGV_ABSTRACT_ITEM_H
