// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "unproject_ray.h"
#include "unproject.h"

template <
  typename Derivedpos,
  typename Derivedmodel,
  typename Derivedproj,
  typename Derivedviewport,
  typename Deriveds,
  typename Deriveddir>

	//这里通过各种视角和鼠标坐标计算出了射线的两个顶点坐标
IGL_INLINE void igl::unproject_ray(
  const Eigen::PlainObjectBase<Derivedpos> & pos,
  const Eigen::PlainObjectBase<Derivedmodel> & model,
  const Eigen::PlainObjectBase<Derivedproj> & proj,
  const Eigen::PlainObjectBase<Derivedviewport> & viewport,
  Eigen::PlainObjectBase<Deriveds> & s,
  Eigen::PlainObjectBase<Deriveddir> & dir)//是方向，不是重点位置
{
  using namespace std;
  using namespace Eigen;
  // Source and direction on screen
  typedef Eigen::Matrix<typename Deriveds::Scalar,3,1> Vec3;
  Vec3 win_s(pos(0),pos(1),0);//最近处
  Vec3 win_d(pos(0),pos(1),1);//最远处

  // Source, destination and direction in world
  Vec3 d;
  igl::unproject(win_s,model,proj,viewport,s);//反推出了射线在世界坐标中的端点位置
  igl::unproject(win_d,model,proj,viewport,d);
  dir = d-s;
}

#ifdef IGL_STATIC_LIBRARY
template void igl::unproject_ray<Eigen::Matrix<float, 2, 1, 0, 2, 1>, Eigen::Matrix<float, 4, 4, 0, 4, 4>, Eigen::Matrix<float, 4, 4, 0, 4, 4>, Eigen::Matrix<float, 4, 1, 0, 4, 1>, Eigen::Matrix<float, 3, 1, 0, 3, 1>, Eigen::Matrix<float, 3, 1, 0, 3, 1> >(Eigen::PlainObjectBase<Eigen::Matrix<float, 2, 1, 0, 2, 1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 4, 4, 0, 4, 4> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 4, 4, 0, 4, 4> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 4, 1, 0, 4, 1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> >&, Eigen::PlainObjectBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> >&);
#endif
