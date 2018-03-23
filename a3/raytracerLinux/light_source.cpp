/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"
#include <typeinfo>


void PointLight::shade( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.
	
	//Initializing all our variables
	Colour Ka;
	Colour Kd;
	Colour Ks;
	Colour Ia;
	Colour Id;
	Colour Is;
	double alpha;

	//Retrieving the variables
	Ka = (*ray.intersection.mat).ambient;
	Kd = (*ray.intersection.mat).diffuse;
	Ks = (*ray.intersection.mat).specular;
	alpha = (*ray.intersection.mat).specular_exp;
	
	Ia = _col_ambient;
	Id = _col_diffuse;
	Is = _col_specular;
	Vector3D N = ray.intersection.normal;
	Vector3D L = _pos-ray.intersection.point;
	Vector3D V = -ray.dir;
	
	//Normalizing the variables
	L[0] = L[0]/ sqrt(pow(L[0],2) + pow(L[1],2) + pow(L[2],2));
	L[1] = L[1]/ sqrt(pow(L[0],2) + pow(L[1],2) + pow(L[2],2));
	L[2] = L[2]/ sqrt(pow(L[0],2) + pow(L[1],2) + pow(L[2],2));
	
	V[0] = V[0]/ sqrt(pow(V[0],2) + pow(V[1],2) + pow(V[2],2));
	V[1] = V[1]/ sqrt(pow(V[0],2) + pow(V[1],2) + pow(V[2],2));
	V[2] = V[2]/ sqrt(pow(V[0],2) + pow(V[1],2) + pow(V[2],2));
	
	
	double LN = L[0]*N[0] + L[1]*N[1] + L[2]*N[2]; //Dot product
	Vector3D R = 2*(LN)*N - L;
	
	//Normalization
	R[0] = R[0]/ sqrt(pow(R[0],2) + pow(R[1],2) + pow(R[2],2));
	R[1] = R[1]/ sqrt(pow(R[0],2) + pow(R[1],2) + pow(R[2],2));
	R[2] = R[2]/ sqrt(pow(R[0],2) + pow(R[1],2) + pow(R[2],2));
	
	double VR = V[0]*R[0] + V[1]*R[1] + V[2]*R[2]; //Dot product
	Colour KaIa;
	Colour KdId;
	Colour KsIs;


	KsIs[0] = Ks[0]*Is[0];
	KsIs[1] = Ks[1]*Is[1];
	KsIs[2] = Ks[2]*Is[2];
	KdId[0] = Kd[0]*Id[0]; 
	KdId[1] = Kd[1]*Id[1];
	KdId[2] = Kd[2]*Id[2];
	KaIa[0] = Ka[0]*Ia[0];
	KaIa[1] = Ka[1]*Ia[1];
	KaIa[2] = Ka[2]*Ia[2];
	ray.col = KaIa + std::max(0.0,LN)*KdId + std::max(0.0,pow(VR,alpha))*KsIs; //Computing the Phong reflection model
	ray.col[0] = std::min(1.0, ray.col[0]);
	ray.col[1] = std::min(1.0, ray.col[1]);
	ray.col[2] = std::min(1.0, ray.col[2]);
	

}

