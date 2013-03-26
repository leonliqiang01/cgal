// Copyright (c) 2008  GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Andreas Fabri <Andreas.Fabri@geometryfactory.com>
//                 Laurent Rineau <Laurent.Rineau@geometryfactory.com>
//                 Nico Kruithof <Nico@nghk.nl>

#ifndef CGAL_QT_PERIODIC_TRIANGULATION_GRAPHICS_ITEM_H
#define CGAL_QT_PERIODIC_TRIANGULATION_GRAPHICS_ITEM_H

#include <CGAL/Bbox_2.h>
#include <CGAL/Qt/PainterOstream.h>
#include <CGAL/Qt/GraphicsItem.h>
#include <CGAL/Qt/Converter.h>

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

namespace CGAL {
  namespace Qt {
    
    template <typename T>
    class PeriodicTriangulationGraphicsItem : public GraphicsItem
    {
      typedef typename T::Geom_traits Geom_traits;
    public:
      PeriodicTriangulationGraphicsItem(T* t_);
      
      void modelChanged();
      
    public:
      
      QRectF boundingRect() const;
      
      void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
      
      virtual void operator()(typename T::Face_handle fh);
      
      const QPen& verticesPen() const
      {
        return vertices_pen;
      }
      
      const QPen& edgesPen() const
      {
        return edges_pen;
      }
      
      const QPen& domainPen() const
      {
        return domain_pen;
      }
      
      void setVerticesPen(const QPen& pen)
      {
        vertices_pen = pen;
      }
      
      void setEdgesPen(const QPen& pen)
      {
        edges_pen = pen;
      }
      
      void setDomainPen(const QPen& pen)
      {
        domain_pen = pen;
      }
      
      bool visibleVertices() const
      {
        return visible_vertices;
      }
      
      void setVisibleVertices(const bool b)
      {
        visible_vertices = b;
        update();
      }
      
      bool visibleEdges() const
      {
        return visible_edges;
      }
      
      void setVisibleEdges(const bool b)
      {
        visible_edges = b;
        update();
      }
      
    protected:
      virtual void drawAll(QPainter *painter);
      void paintVertices(QPainter *painter);
      void paintOneVertex(const typename T::Point& point);
      virtual void paintVertex(typename T::Vertex_handle vh);
      void updateBoundingBox();
      
      T * t;
      QPainter* m_painter;
      PainterOstream<Geom_traits> painterostream;
      
      typename T::Vertex_handle vh;
      typename T::Point p;
      QRectF bounding_rect;
      
      QPen vertices_pen;
      QPen edges_pen;
      QPen domain_pen;
      bool visible_edges;
      bool visible_vertices;
    };
    
    
    template <typename T>
    PeriodicTriangulationGraphicsItem<T>::PeriodicTriangulationGraphicsItem(T * t_)
    :  t(t_), painterostream(0),
    visible_edges(true), visible_vertices(true)
    {
      setVerticesPen(QPen(::Qt::red, 3.));
      setDomainPen(QPen(::Qt::blue, .01));
      if(t->number_of_vertices() == 0){
        this->hide();
      }
      updateBoundingBox();
      setZValue(3);
    }
    
    template <typename T>
    QRectF 
    PeriodicTriangulationGraphicsItem<T>::boundingRect() const
    {
      return bounding_rect;
    }
    
    
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::operator()(typename T::Face_handle fh)
    {
      if(visible_edges) {
        for (int i=0; i<3; i++) {
          if (fh < fh->neighbor(i)){
            m_painter->setPen(this->edgesPen());
            painterostream << t->segment(fh,i);
          }
        }
      }
      if(visible_vertices) {
        for (int i=0; i<3; i++) {
          paintVertex(fh->vertex(i));
        }
      }
    }
    
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::drawAll(QPainter *painter)
    {
      painterostream = PainterOstream<Geom_traits>(painter);
      
      if(visibleEdges()) {
        painter->setPen(this->edgesPen());
        t->draw_triangulation(painterostream);
      }
      
      paintVertices(painter);
    }
    
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::paintVertices(QPainter *painter)
    {
      if(visibleVertices()) {
        Converter<Geom_traits> convert;
        
        QMatrix matrix = painter->matrix();
        painter->resetMatrix();

        QPen pen = verticesPen();
        if (t->number_of_vertices() < 8) {
          int v_index=1;
          for (typename T::Unique_vertex_iterator vit = t->unique_vertices_begin();
               vit != t->unique_vertices_end(); ++vit) {
            pen.setColor(QColor(255*(v_index&1), 255*((v_index>>1)&1), 255*((v_index>>2)&1)));
            painter->setPen(pen);

            painter->drawPoint(matrix.map(convert(t->point(vit))));
            std::vector<typename T::Vertex_handle> copies = t->periodic_copies(vit);
            for (size_t i=0; i<copies.size(); ++i)
              painter->drawPoint(matrix.map(convert(t->point(copies[i]))));

            ++v_index;
          }
          
        } else {
          painter->setPen(verticesPen());
          for (typename T::Periodic_point_iterator ppit = t->periodic_points_begin();
               ppit != t->periodic_points_end(); ++ppit)
            {
              QPointF point = matrix.map(convert(t->point(*ppit)));
              painter->drawPoint(point);
            }
        }

        painter->setMatrix(matrix);
      }
    }
    
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::paintOneVertex(const typename T::Point& point)
    {
      Converter<Geom_traits> convert;
      
      m_painter->setPen(this->verticesPen());
      QMatrix matrix = m_painter->matrix();
      m_painter->resetMatrix();
      m_painter->drawPoint(matrix.map(convert(point)));
      m_painter->setMatrix(matrix);
    }
    
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::paintVertex(typename T::Vertex_handle vh)
    {
      Converter<Geom_traits> convert;
      
      m_painter->setPen(this->verticesPen());
      QMatrix matrix = m_painter->matrix();
      m_painter->resetMatrix();
      m_painter->drawPoint(matrix.map(convert(vh->point())));
      m_painter->setMatrix(matrix);
    }
    
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::paint(QPainter *painter, 
                                                const QStyleOptionGraphicsItem *,
                                                QWidget *)
    {
      painter->setPen(this->domainPen());
      const typename Geom_traits::Iso_rectangle_2 &domain = t->domain();
      if (t->is_1_cover()) {
        painter->drawRect(domain.xmin(), 
                          domain.ymin(), 
                          domain.xmax()-domain.xmin(), 
                          domain.ymax()-domain.ymin());
      } else {
        painter->drawRect(domain.xmax(), 
                          domain.ymax(), 
                          domain.xmax()-domain.xmin(), 
                          domain.ymax()-domain.ymin());
      }
      drawAll(painter);
      m_painter = painter;
    }
    
    // We let the bounding box only grow, so that when vertices get removed
    // the maximal bbox gets refreshed in the GraphicsView
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::updateBoundingBox()
    {
      prepareGeometryChange();
      
      CGAL::Bbox_2 bb = t->domain().bbox();
      double xmin = bb.xmin();
      double ymin = bb.ymin();
      double dx = bb.xmax() - xmin;
      double dy = bb.ymax() - ymin;

      double delta = 0.1;
      bounding_rect = QRectF(xmin - dx*delta,
                             ymin - dy*delta,
                             dx * t->number_of_sheets()[0] + 2*dx*delta,
                             dy * t->number_of_sheets()[0] + 2*dy*delta);
    }
    
    
    template <typename T>
    void 
    PeriodicTriangulationGraphicsItem<T>::modelChanged()
    {
      if((t->number_of_vertices() == 0) ){
        this->hide();
      } else if((t->number_of_vertices() > 0) && (! this->isVisible())){
        this->show();
      }
      updateBoundingBox();
      update();
    }
    
    
  } // namespace Qt
} // namespace CGAL

#endif // CGAL_QT_PERIODIC_TRIANGULATION_GRAPHICS_ITEM_H
