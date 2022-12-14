#include "PointGraphicsItem.h"
#include <QDebug>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
PointGraphicsItem::PointGraphicsItem(QGraphicsItem* parent)
    : QGraphicsItem(parent), m_bResize(false), m_oldRect(0, 0, 100, 100), m_bRotate(false), m_RotateAngle(0), m_stateFlag(DEFAULT_FLAG)
{
    setRectSize(m_oldRect);
    setCursor(Qt::ArrowCursor);
    pPointFofSmallRotateRect = new QPointF[4];
    setRotate(0);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptDrops(true);
}

PointGraphicsItem::~PointGraphicsItem()
{
    delete []pPointFofSmallRotateRect;
    pPointFofSmallRotateRect = nullptr;
}

QRectF PointGraphicsItem::boundingRect() const
{
    QRectF boundingRectF = m_oldRectPolygon.boundingRect();
    return QRectF(boundingRectF.x() - 40, boundingRectF.y() - 40, boundingRectF.width() + 80, boundingRectF.height() + 80);
}

void PointGraphicsItem::setRectSize(QRectF mrect, bool bResetRotateCenter)
{
    m_oldRect = mrect;
    if(bResetRotateCenter)
    {
        m_RotateCenter.setX(m_oldRect.x()+m_oldRect.width()/2);
        m_RotateCenter.setY(m_oldRect.y()+m_oldRect.height()/2);
    }
    m_oldRectPolygon = getRotatePolygonFromRect(m_RotateCenter, m_oldRect, m_RotateAngle);

    m_insicedRectF = QRectF(m_oldRect.x() + 8, m_oldRect.y() + 8, m_oldRect.width() - 16, m_oldRect.height() -16);
    m_insicedPolygon = getRotatePolygonFromRect(m_RotateCenter, m_insicedRectF, m_RotateAngle);

    m_leftRectF = QRectF(m_oldRect.x(), m_oldRect.y(), 8, m_oldRect.height() - 8);
    m_leftPolygon = getRotatePolygonFromRect(m_RotateCenter, m_leftRectF, m_RotateAngle);

    m_topRectF = QRectF(m_oldRect.x() + 8, m_oldRect.y(), m_oldRect.width() - 8, 8);
    m_topPolygon = getRotatePolygonFromRect(m_RotateCenter, m_topRectF, m_RotateAngle);

    m_rightRectF = QRectF(m_oldRect.right() - 8, m_oldRect.y() + 8, 8, m_oldRect.height() - 8);
    m_rightPolygon = getRotatePolygonFromRect(m_RotateCenter,m_rightRectF,m_RotateAngle);

    m_bottomRectF = QRectF(m_oldRect.x(),m_oldRect.bottom()-8,m_oldRect.width()-8,8);
    m_bottomPolygon = getRotatePolygonFromRect(m_RotateCenter,m_bottomRectF,m_RotateAngle);

    m_SmallRotateRect = getSmallRotateRect(mrect.topLeft(), mrect.topRight());
    m_SmallRotatePolygon = getRotatePolygonFromRect(m_RotateCenter,m_SmallRotateRect,m_RotateAngle);



}

void PointGraphicsItem::setRotate(qreal RotateAngle, QPointF ptCenter)
{
    m_bRotate = true;
    if(ptCenter.x()==-999 && ptCenter.y()==-999)
    {
        m_RotateCenter = QPointF(m_oldRect.x()+m_oldRect.width()/2,m_oldRect.y()+m_oldRect.height()/2);
    }
    else
    {
        m_RotateCenter = ptCenter;
    }
    m_RotateAngle = RotateAngle;
    this->update();
}

QPointF PointGraphicsItem::getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle)
{
    double dx =ptCenter.x();
    double dy = ptCenter.y();
    double x = ptIn.x();
    double y = ptIn.y();

    double xx, yy;
    xx = (x - dx) * cos(angle * M_PI / 180) - (y - dy) * sin(angle * M_PI / 180) + dx;
    yy = (x - dx) * sin(angle * M_PI / 180) + (y - dy) * cos(angle * M_PI / 180) + dy;

    return QPointF(xx, yy);

}

QList<QPointF> PointGraphicsItem::getRotatePoints(QPointF ptCenter, QList<QPointF> ptIns, qreal angle)
{
    QList<QPointF> lstPt;
    for (int i = 0; i < ptIns.count(); i++)
    {
        lstPt.append(getRotatePoint(ptCenter, ptIns.at(i), angle));
    }
    return lstPt;
}

QPolygonF PointGraphicsItem::getRotatePolygonFromRect(QPointF ptCenter, QRectF rectIn, qreal angle)
{

    QVector<QPointF> vpt;
    QPointF pf = getRotatePoint(ptCenter, rectIn.topLeft(), angle);

    vpt.append(pf);
    pf = getRotatePoint(ptCenter, rectIn.topRight(), angle);

    vpt.append(pf);
    pf = getRotatePoint(ptCenter, rectIn.bottomRight(), angle);

    vpt.append(pf);
    pf = getRotatePoint(ptCenter, rectIn.bottomLeft(), angle);

    vpt.append(pf);
    pf = getRotatePoint(ptCenter, rectIn.topLeft(), angle);

    vpt.append(pf);

    return QPolygonF(vpt);

}

QRectF PointGraphicsItem::getCrtPosRectToSceen()
{
    QRectF retRect = QRectF(m_oldRect.x()+pos().x(),m_oldRect.y()+pos().y(),m_oldRect.width(),m_oldRect.height());
     return retRect;
}

QPointF PointGraphicsItem::getSmallRotateRectCenter(QPointF ptA, QPointF ptB)
{

    QPointF ptCenter = QPointF((ptA.x()+ptB.x())/2,(ptA.y()+ptB.y())/2);//A,B????????????C
        //????????????????????? y=x*k + b;
        qreal x,y;//???????????????????????????
        if(abs(ptB.y()-ptA.y())<0.1)
        {
            if(ptA.x()<ptB.x())//????????????????????????
            {
                x = ptCenter.x();
                y = ptCenter.y()-20;
            }
            else//????????????????????????
            {
                x = ptCenter.x();
                y = ptCenter.y()+20;
            }
        }
        else if(ptB.y()>ptA.y())//???????????????0-180
        {
            qreal k = (ptA.x()-ptB.x())/(ptB.y()-ptA.y());//???????????????
            qreal b = (ptA.y()+ptB.y())/2-k*(ptA.x()+ptB.x())/2;
            //???AB??????????????????AB??????20???????????????C?????????
            x = 20*cos(atan(k))+ptCenter.x();
            y = k*x+b;
        }
        else if(ptB.y()<ptA.y())//???????????????180-360
        {
            qreal k = (ptA.x()-ptB.x())/(ptB.y()-ptA.y());//???????????????
            qreal b = (ptA.y()+ptB.y())/2-k*(ptA.x()+ptB.x())/2;
            //???AB??????????????????AB??????20???????????????C?????????
            x = -20*cos(atan(k))+ptCenter.x();
            y = k*x+b;
        }
        return QPointF(x,y);
}

QRectF PointGraphicsItem::getSmallRotateRect(QPointF ptA, QPointF ptB)
{
    QPointF pt = getSmallRotateRectCenter(ptA,ptB);
        return QRectF(pt.x()-10,pt.y()-10,20,20);
}

void PointGraphicsItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
//        painter->save();
       QPen mPen = QPen(Qt::red);
       painter->setPen(mPen);
       //????????????????????????
       painter->drawPolygon(m_oldRectPolygon);
//       painter->restore();
       //??????????????????
//       painter->save();
       mPen.setWidth(2);
       mPen.setColor(QColor("#64CCFF00"));
       painter->setPen(mPen);
       QPointF pf = getSmallRotateRectCenter(m_oldRectPolygon[0],m_oldRectPolygon[1]);
       QRectF rect = QRectF(pf.x()-10,pf.y()-10,20,20);
       painter->drawEllipse(rect);//????????????
       painter->drawPoint(pf);//?????????
//       painter->restore();

}

void PointGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_startPos = event->pos(); // ???????????????????????????
        // ????????????
        if (m_SmallRotatePolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::PointingHandCursor);
            m_stateFlag = ROTATE;
        }
        // ???????????????
        else if (m_insicedPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::ClosedHandCursor);
            m_stateFlag = MOV_RECT;
        }
        // ??????
        else if (m_leftPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeHorCursor);
            m_stateFlag = MOV_LEFT_LINE;
        }
        else if (m_rightPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeHorCursor);
            m_stateFlag = MOV_RIGHT_LINE;
        }
        else if (m_topPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeVerCursor);
            m_stateFlag = MOV_TOP_LINE;
        }
        else if (m_bottomPolygon.containsPoint(m_startPos, Qt::WindingFill))
        {
            setCursor(Qt::SizeVerCursor);
            m_stateFlag = MOV_BOTTOM_LINE;
        }
        else
        {
            m_stateFlag = DEFAULT_FLAG;
        }
    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

void PointGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_stateFlag == ROTATE)
    {
        int nRotateAngle = atan2((event->pos().x()-m_RotateCenter.x()),(event->pos().y()-m_RotateCenter.y()))*180/M_PI;
               setRotate(180-nRotateAngle);
               setRectSize(m_oldRect);
    }
    else if (m_stateFlag == MOV_RECT)
    {
        QPointF point = (event->pos() - m_startPos);
        moveBy(point.x(), point.y());
        setRectSize(m_oldRect);
        scene()->update();
    }
    else if (m_stateFlag == MOV_LEFT_LINE)
    {
        QPointF pf = QPointF((m_oldRectPolygon.at(1).x() + m_oldRectPolygon.at(2).x()) /2,
                             (m_oldRectPolygon.at(1).y() + m_oldRectPolygon.at(2).y())/2);

        //?????????????????????????????????
        qreal dis = abs(event->pos().x()-pf.x());
        qreal dis2LT = sqrt((event->pos().x() - m_oldRectPolygon.at(0).x()) * (event->pos().x() - m_oldRectPolygon.at(0).x()) +
                            (event->pos().y() - m_oldRectPolygon.at(0).y()) * (event->pos().y() - m_oldRectPolygon.at(0).y()));
        qreal dis2RT = sqrt((event->pos().x() - m_oldRectPolygon.at(1).x()) * (event->pos().x() - m_oldRectPolygon.at(1).x()) +
                            (event->pos().y() - m_oldRectPolygon.at(1).y()) * (event->pos().y() - m_oldRectPolygon.at(1).y()));
        if(dis<16||dis2LT>dis2RT)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setLeft(m_oldRect.right()-dis);
            newRect.setRight(m_oldRect.right());
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//????????????scene()->update()????????????update();?????????????????????
        }

    }
    else if (m_stateFlag == MOV_TOP_LINE)
    {
        //????????????
        QPointF pf = QPointF((m_oldRectPolygon.at(2).x()+m_oldRectPolygon.at(3).x())/2,((m_oldRectPolygon.at(2).y()+m_oldRectPolygon.at(3).y())/2));
        //??????????????????????????????
        qreal dis = abs(event->pos().y()-pf.y());
        qreal dis2LT = sqrt((event->pos().x()-m_oldRectPolygon.at(0).x())*(event->pos().x()-m_oldRectPolygon.at(0).x()) +(event->pos().y()-m_oldRectPolygon.at(0).y())*(event->pos().y()-m_oldRectPolygon.at(0).y()));
        qreal dis2LB = sqrt((event->pos().x()-m_oldRectPolygon.at(3).x())*(event->pos().x()-m_oldRectPolygon.at(3).x()) +(event->pos().y()-m_oldRectPolygon.at(3).y())*(event->pos().y()-m_oldRectPolygon.at(3).y()));
        if(dis<16||dis2LT>dis2LB)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setTop(m_oldRect.bottom()-dis);
            newRect.setBottom(m_oldRect.bottom());
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//????????????scene()->update()????????????update();?????????????????????
        }
    }
    else if (m_stateFlag == MOV_RIGHT_LINE)
    {
        QPointF pf = QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(3).x())/2,((m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(3).y())/2));
        //?????????????????????????????????
        qreal dis = abs((event->pos().x()-pf.x()));
        qreal dis2LT = sqrt((event->pos().x()-m_oldRectPolygon.at(0).x())*(event->pos().x()-m_oldRectPolygon.at(0).x()) +(event->pos().y()-m_oldRectPolygon.at(0).y())*(event->pos().y()-m_oldRectPolygon.at(0).y()));
        qreal dis2RT = sqrt((event->pos().x()-m_oldRectPolygon.at(1).x())*(event->pos().x()-m_oldRectPolygon.at(1).x()) +(event->pos().y()-m_oldRectPolygon.at(1).y())*(event->pos().y()-m_oldRectPolygon.at(1).y()));
        if(dis<16||dis2LT<dis2RT)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setLeft(m_oldRect.left());
            newRect.setRight(m_oldRect.left()+dis);
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//????????????scene()->update()????????????update();?????????????????????
        }
    }
    else if (m_stateFlag == MOV_BOTTOM_LINE)
    {
        //????????????
        QPointF pf = QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(1).x())/2,((m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(1).y())/2));
        //??????????????????????????????
        qreal dis = abs(event->pos().y()-pf.y());
        qreal dis2LT = sqrt((event->pos().x()-m_oldRectPolygon.at(0).x())*(event->pos().x()-m_oldRectPolygon.at(0).x()) +(event->pos().y()-m_oldRectPolygon.at(0).y())*(event->pos().y()-m_oldRectPolygon.at(0).y()));
        qreal dis2LB = sqrt((event->pos().x()-m_oldRectPolygon.at(3).x())*(event->pos().x()-m_oldRectPolygon.at(3).x()) +(event->pos().y()-m_oldRectPolygon.at(3).y())*(event->pos().y()-m_oldRectPolygon.at(3).y()));
        if(dis<16||dis2LT<dis2LB)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setTop(m_oldRect.top());
            newRect.setBottom(m_oldRect.top()+dis);
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//????????????scene()->update()????????????update();?????????????????????
        }
    }


}

void PointGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    if(m_stateFlag == MOV_RECT)
    {
        m_stateFlag = DEFAULT_FLAG;
    }
    else {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

