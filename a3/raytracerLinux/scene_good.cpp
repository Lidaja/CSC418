/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	Ray3D r_h;
	r_h.origin = worldToModel*ray.origin;
	r_h.dir = worldToModel*ray.dir;
	double t = -(r_h.origin[2])/(r_h.dir[2]);
	if (t <= 0)
	{
	  return false;
	}
	double x_check = r_h.origin[0] + t*r_h.dir[0];
	double y_check = r_h.origin[1] + t*r_h.dir[1];
	//std::cout<<"x_check=" << x_check<< "\n";
	//std::cout<<"y_check=" << y_check << "\n";
	//std::cout<<"Hi";
	if (x_check >= -0.5 && x_check <= 0.5 && y_check >= -0.5 && y_check <= 0.5)
	{
	  //std::cout<<"x_check=" << x_check<< "\n";
	  //std::cout<<"y_check=" << y_check << "\n";
	  if (ray.intersection.none || t < ray.intersection.t_value)
	  {
	      ray.intersection.t_value = t;
	      Point3D inter;
	      Vector3D normal;
	      inter[0] = x_check;
	      inter[1] = y_check;
	      inter[2] = 0;
	      normal[0] = 0;
	      normal[1] = 0;
	      normal[2] = 1;
	      
	      ray.intersection.point = modelToWorld * inter;
	      ray.intersection.normal = worldToModel.transpose()*normal;
	      ray.intersection.none = false;
	      return true;
	  }
	}
	return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	
	Ray3D r_h;
	r_h.origin = worldToModel*ray.origin;
	r_h.dir = worldToModel*ray.dir;
	double t = -(r_h.origin[2])/(r_h.dir[2]);
	if (t <= 0)
	{
	  return false;
	}
	double x_check = r_h.origin[0] + t*r_h.dir[0];
	double y_check = r_h.origin[1] + t*r_h.dir[1];
	double z_check = r_h.origin[2] + t*r_h.dir[2];
	if (pow(x_check,2) + pow(y_check,2)  + pow(z_check,2) <= 1.0)
	{
	  if (ray.intersection.none || t < ray.intersection.t_value)
	  {
	      ray.intersection.t_value = t;
	      Point3D inter;
	      Vector3D normal;
	      inter[0] = x_check;
	      inter[1] = y_check;
	      inter[2] = z_check;
	      normal[0] = x_check/sqrt((pow(x_check,2) + pow(y_check,2)  + pow(z_check,2)));
	      normal[1] = y_check/sqrt((pow(x_check,2) + pow(y_check,2)  + pow(z_check,2)));
	      normal[2] = z_check/sqrt((pow(x_check,2) + pow(y_check,2)  + pow(z_check,2)));
	      
	      ray.intersection.point = modelToWorld * inter;
	      ray.intersection.normal = worldToModel.transpose()*normal;
	      ray.intersection.none = false;
	      return true;
	  }
	}
	return false;
}

