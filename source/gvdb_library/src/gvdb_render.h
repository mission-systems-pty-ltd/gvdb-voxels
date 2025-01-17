//-----------------------------------------------------------------------------
// NVIDIA(R) GVDB VOXELS
// Copyright 2016 NVIDIA Corporation
// SPDX-License-Identifier: Apache-2.0
// 
// Version 1.0: Rama Hoetzlein, 5/1/2017
// Version 1.1: Rama Hoetzlein, 3/25/2018
//-----------------------------------------------------------------------------
#pragma once 
#include "gvdb_vec.h"
#include "gvdb_scene.h"
using namespace nvdb;

#define UVIEW		0
#define UPROJ		1
#define UMODEL		2
#define UINVVIEW	3
#define ULIGHTPOS	4
#define UCAMPOS		5
#define UCAMDIMS	6
#define UCLRAMB		7
#define UCLRDIFF	8
#define UCLRSPEC	9
#define UTEX		10
#define UTEXRES		11
#define UW			12
#define USHADOWMASK 13
#define USHADOWSIZE	14
#define UEYELIGHT	15
#define UOVERTEX	16
#define UOVERSIZE	17
#define UVOLMIN		18
#define UVOLMAX		19
#define USAMPLES	20
#define UTEXMIN		21
#define UTEXMAX		22

// OpenGL Render
#ifdef BUILD_OPENGL

	#include <GL/glew.h>			// OpenGL extensions
	#include <cudaGL.h>				// Cuda-GL interop
	#include <cuda_gl_interop.h>
	
	GVDB_API void gchkGL( const char* msg );

	GVDB_API void renderCamSetupGL ( Scene* scene, int prog, Matrix4F* model );
	GVDB_API void renderLightSetupGL ( Scene* scene, int prog );
	GVDB_API void renderSceneGL ( Scene* scene, int prog );
	GVDB_API void renderSceneGL ( Scene* scene, int prog, bool bMat );
	GVDB_API void renderSetTex3D ( Scene* scene, int prog, int tex, Vector3DF res );
	GVDB_API void renderSetTex2D ( Scene* scene, int prog, int tex );
	GVDB_API void renderSetMaterialGL (Scene* scene, int prog, Vector4DF amb, Vector4DF diff, Vector4DF spec);
	GVDB_API void renderSetUW ( Scene* scene, int prog, Matrix4F* model, Vector3DF res );
	GVDB_API void renderScreenspaceGL ( Scene* scene, int prog );

	GVDB_API void makeSimpleShaderGL ( Scene* scene, const char* vertfile, const char* fragfile);
	GVDB_API void makeSliceShader ( Scene* scene, const char* vertname, const char* fragname );
	GVDB_API void makeOutlineShader ( Scene* scene, const char* vertname, const char* fragname );
	GVDB_API void makeVoxelizeShader ( Scene* scene, const char* vertname, const char* fragname, const char* geomname );
	GVDB_API void makeRaycastShader ( Scene* scene, const char* vertname, const char* fragname );
	GVDB_API void makeInstanceShader ( Scene* scene, const char* vertname, const char* fragname );
	GVDB_API void makeScreenShader ( Scene* scene, const char* vertname, const char* fragname );

#endif
