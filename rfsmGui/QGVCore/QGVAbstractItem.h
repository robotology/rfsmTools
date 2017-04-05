
/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Ali Paikan <ali.paikan@iit.it>, Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

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
