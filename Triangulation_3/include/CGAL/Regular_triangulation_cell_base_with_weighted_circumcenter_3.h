// Copyright (c) 1999-2006  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
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
// Author(s)     : Monique Teillaud <Monique.Teillaud@sophia.inria.fr>
//                 Sylvain Pion

// cell of a triangulation of any dimension <=3,
// storing its circumcenter lazily.

#ifndef CGAL_REGULAR_TRIANGULATION_CELL_BASE_WITH_CIRCUMCENTER_3_H
#define CGAL_REGULAR_TRIANGULATION_CELL_BASE_WITH_CIRCUMCENTER_3_H


#include <CGAL/basic.h>
#include <CGAL/triangulation_assertions.h>
#include <CGAL/Triangulation_cell_base_3.h>

namespace CGAL {

template < typename GT, typename Cb = Regular_triangulation_cell_base_3<GT> >
class Regular_triangulation_cell_base_with_weighted_circumcenter_3
  : public Cb
{
  typedef typename GT::Point_3                         Point_3;
  typedef typename GT::Bare_point                      Bare_point;

  mutable Bare_point * weighted_circumcenter_;

public:
  void invalidate_circumcenter()
  {
      if (weighted_circumcenter_) {
          delete weighted_circumcenter_;
          weighted_circumcenter_ = NULL;
      }
  }

public:
  typedef typename Cb::Vertex_handle                   Vertex_handle;
  typedef typename Cb::Cell_handle                     Cell_handle;

  typedef GT                                           Geom_traits;

  template < typename TDS2 >
  struct Rebind_TDS {
    typedef typename Cb::template Rebind_TDS<TDS2>::Other         Cb2;
    typedef 
      Regular_triangulation_cell_base_with_weighted_circumcenter_3<GT, Cb2>
      Other;
  };

  Regular_triangulation_cell_base_with_weighted_circumcenter_3()
    : Cb(), weighted_circumcenter_(NULL) {}

  Regular_triangulation_cell_base_with_weighted_circumcenter_3
        (const Regular_triangulation_cell_base_with_weighted_circumcenter_3 &c)
    : Cb(c), 
      weighted_circumcenter_(c.weighted_circumcenter_ != NULL ? 
                             new Bare_point(*(c.weighted_circumcenter_)) : 
                             NULL)
  {}

  Regular_triangulation_cell_base_with_weighted_circumcenter_3&
  operator=
    (const Regular_triangulation_cell_base_with_weighted_circumcenter_3 &c)
  {
      if (this != &c)
      {
        if (weighted_circumcenter_ != NULL)
        {
            delete weighted_circumcenter_;
            weighted_circumcenter_ = NULL;
        }
        if (c.weighted_circumcenter_ != NULL)
            weighted_circumcenter_ = new Bare_point(*(c.weighted_circumcenter_));
        Cb::operator=(c);
      }
      return *this;
  }

  Regular_triangulation_cell_base_with_weighted_circumcenter_3(
	                    Vertex_handle v0, Vertex_handle v1,
                            Vertex_handle v2, Vertex_handle v3)
    : Cb(v0, v1, v2, v3), weighted_circumcenter_(NULL) {}

  Regular_triangulation_cell_base_with_weighted_circumcenter_3(
	                    Vertex_handle v0, Vertex_handle v1,
                            Vertex_handle v2, Vertex_handle v3,
                            Cell_handle   n0, Cell_handle   n1,
                            Cell_handle   n2, Cell_handle   n3)
    : Cb(v0, v1, v2, v3, n0, n1, n2, n3), weighted_circumcenter_(NULL) {}

  ~Regular_triangulation_cell_base_with_weighted_circumcenter_3()
  {
      delete weighted_circumcenter_;
  }

  // We must override the functions that modify the vertices.
  // And if the point inside a vertex is modified, we fail,
  // but there's not much we can do for this now.
  void set_vertex(int i, Vertex_handle v)
  {
      invalidate_circumcenter();
      Cb::set_vertex(i, v);
  }

  void set_vertices()
  {
      invalidate_circumcenter();
      Cb::set_vertices();
  }

  void set_vertices(Vertex_handle v0, Vertex_handle v1,
                    Vertex_handle v2, Vertex_handle v3)
  {
      invalidate_circumcenter();
      Cb::set_vertices(v0, v1, v2, v3);
  }

  const Bare_point &
  weighted_circumcenter(const Geom_traits& gt = Geom_traits()) const
  {
      if (weighted_circumcenter_ == NULL) {
    	  weighted_circumcenter_ 
            = new Bare_point(this->Cb::weighted_circumcenter(gt));
      } else {
        CGAL_expensive_assertion(
          this->Cb::weighted_circumcenter(gt) == *weighted_circumcenter_);
      }

      return *weighted_circumcenter_;
  }
};

} //namespace CGAL

#endif // CGAL_REGULAR_TRIANGULATION_CELL_BASE_WITH_CIRCUMCENTER_3_H
