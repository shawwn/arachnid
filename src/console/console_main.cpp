//========================================================================
//	file:		console_main.cpp
//	author:		Shawn Presser 
//	date:		7/17/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "../engine/e_common.h"

// engine headers.
#include "../engine/e_system.h"
#include "../engine/e_filemanager.h"
#include "../engine/e_engine.h"

// graphics headers.
#include "../engine/gr_scene.h"
#include "../engine/gr_model.h"
#include "../engine/gr_model_node.h"
#include "../engine/gr_anim_mixer.h"
#include "../engine/gr_mesh.h"
#include "../engine/gr_material.h"
#include "../engine/gr_texture.h"
#include "../engine/gr_camera.h"
#include "../engine/gr_anim.h"

// math headers.
#include "../engine/m_vec3.h"
#include "../engine/m_quat.h"

// import headers.
#include "../engine/import_k2_model.h"
#include "../engine/import_k2_anim.h"
#include "../engine/test_anim.h"

// expression headers.
#include "../engine/e_expr.h"
#include "../engine/e_expr_number.h"
#include "../engine/e_expr_add.h"
#include "../engine/e_expr_multiply.h"

// std headers.
#include <iostream>
//========================================================================

//========================================================================
// Constants
//========================================================================
//========================================================================

//========================================================================
// Globals
//========================================================================
//========================================================================

//===================
// StartupRenderer
//===================
bool			StartupRenderer()
{
	E_VERIFY(gEngine != NULL, return false);
	GrDriver& renderer(gEngine->GetRenderer());
	GrModel& sceneModel(gEngine->GetScene().GetModel());

	return true;
}


//===================
// ShutdownRenderer
//===================
void			ShutdownRenderer()
{
}


//===================
// RunEngine
//===================
bool				RunEngine()
{
	// startup system
	E_NEW("main", ESystem);

	// startup subsystems
	E_NEW("main", EFileManager);

	// startup the engine
	SEngineConfig config;
	config.renderer = _T("none");
	EEngine::Create(_T("main"), config);

	// startup the renderer.
	if (StartupRenderer())
	{
		// main engine loop
		uint dt = 1000 / 20;

		while (gEngine->PerFrame(dt))
		{
			GrModel& sceneModel(gEngine->GetRenderer().GetScene().GetModel());
			sceneModel.Update(dt);
		}
	}

	// shutdown the renderer.
	ShutdownRenderer();

	// shutdown the engine
	E_DELETE("main", gEngine);

	// shutdown subsystems
	E_DELETE("main", gFileManager);

	// shutdown system
	E_DELETE("main", gSystem);

	return 0;
}

class ExprMat44 : public ERefCounted
{
private:
	ERef<EExpr>		_vals[4 * 4];

public:
	ExprMat44()
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				Get(i, j) = EExpr::Zero();

		Get(0, 0) = EExpr::One();
		Get(1, 1) = EExpr::One();
		Get(2, 2) = EExpr::One();
		Get(3, 3) = EExpr::One();
	}

	ExprMat44(
		ERef<EExpr> Xx, ERef<EExpr> Yx, ERef<EExpr> Zx, ERef<EExpr> Wx,
		ERef<EExpr> Xy, ERef<EExpr> Yy, ERef<EExpr> Zy, ERef<EExpr> Wy,
		ERef<EExpr> Xz, ERef<EExpr> Yz, ERef<EExpr> Zz, ERef<EExpr> Wz,
		ERef<EExpr> Xt, ERef<EExpr> Yt, ERef<EExpr> Zt, ERef<EExpr> Wt)
	{
		Get(0, 0) = Xx;
		Get(0, 1) = Xy;
		Get(0, 2) = Xz;
		Get(0, 3) = Xt;

		Get(1, 0) = Yx;
		Get(1, 1) = Yy;
		Get(1, 2) = Yz;
		Get(1, 3) = Yt;

		Get(2, 0) = Zx;
		Get(2, 1) = Zy;
		Get(2, 2) = Zz;
		Get(2, 3) = Zt;

		Get(3, 0) = Wx;
		Get(3, 1) = Wy;
		Get(3, 2) = Wz;
		Get(3, 3) = Wt;
	}

	ExprMat44(
		ERef<EExpr> Xx, ERef<EExpr> Yx, ERef<EExpr> Zx,
		ERef<EExpr> Xy, ERef<EExpr> Yy, ERef<EExpr> Zy,
		ERef<EExpr> Xz, ERef<EExpr> Yz, ERef<EExpr> Zz)
	{
		Get(0, 0) = Xx;
		Get(0, 1) = Xy;
		Get(0, 2) = Xz;
		Get(0, 3) = EExpr::Zero();

		Get(1, 0) = Yx;
		Get(1, 1) = Yy;
		Get(1, 2) = Yz;
		Get(1, 3) = EExpr::Zero();

		Get(2, 0) = Zx;
		Get(2, 1) = Zy;
		Get(2, 2) = Zz;
		Get(2, 3) = EExpr::Zero();

		Get(3, 0) = EExpr::Zero();
		Get(3, 1) = EExpr::Zero();
		Get(3, 2) = EExpr::Zero();
		Get(3, 3) = EExpr::One();
	}

	static ERef<ExprMat44>			Create(
						ERef<EExpr> Xx, ERef<EExpr> Yx, ERef<EExpr> Zx, ERef<EExpr> Wx,
						ERef<EExpr> Xy, ERef<EExpr> Yy, ERef<EExpr> Zy, ERef<EExpr> Wy,
						ERef<EExpr> Xz, ERef<EExpr> Yz, ERef<EExpr> Zz, ERef<EExpr> Wz,
						ERef<EExpr> Xt, ERef<EExpr> Yt, ERef<EExpr> Zt, ERef<EExpr> Wt)
	{
		return E_NEW("expr", ExprMat44)(
			Xx, Yx, Zx, Wx,
			Xy, Yy, Zy, Wy,
			Xz, Yz, Zz, Wz,
			Xt, Yt, Zt, Wt);
	}

	static ERef<ExprMat44>			Create(
						ERef<EExpr> Xx, ERef<EExpr> Yx, ERef<EExpr> Zx,
						ERef<EExpr> Xy, ERef<EExpr> Yy, ERef<EExpr> Zy,
						ERef<EExpr> Xz, ERef<EExpr> Yz, ERef<EExpr> Zz)
	{
		return E_NEW("expr", ExprMat44)(
			Xx, Yx, Zx,
			Xy, Yy, Zy,
			Xz, Yz, Zz);
	}

	static ERef<ExprMat44>			FromXRot(const ERef<EExpr>& theta)
	{
		// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
		ERef<EExpr> s(EExpr::Sin(theta));
		ERef<EExpr> c(EExpr::Cos(theta));
		const ERef<EExpr>& one(EExpr::One());
		const ERef<EExpr>& zero(EExpr::Zero());
		return Create(	one,	zero,	zero,
						zero,	c,		s,
						zero,	-*s,	c);
	}


	static ERef<ExprMat44>			FromYRot(const ERef<EExpr>& theta)
	{
		// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
		ERef<EExpr> s(EExpr::Sin(theta));
		ERef<EExpr> c(EExpr::Cos(theta));
		const ERef<EExpr>& one(EExpr::One());
		const ERef<EExpr>& zero(EExpr::Zero());
		return Create(	c,		zero,	-*s,
						zero,	one,	zero,
						s,		zero,	c);
	}


	static ERef<ExprMat44>			FromZRot(const ERef<EExpr>& theta)
	{
		// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

		ERef<EExpr> s(EExpr::Sin(theta));
		ERef<EExpr> c(EExpr::Cos(theta));
		const ERef<EExpr>& one(EExpr::One());
		const ERef<EExpr>& zero(EExpr::Zero());
		return Create(	c,		s,		zero,
						-*s,	c,		zero,
						zero,	zero,	one);
	}

	const ERef<EExpr>&		Get(int col, int row) const		{ return _vals[4*col + row]; }
	ERef<EExpr>&			Get(int col, int row)			{ return _vals[4*col + row]; }

	const ERef<EExpr>&		operator()(int col, int row) const		{ return _vals[4*col + row]; }
	ERef<EExpr>&			operator()(int col, int row)			{ return _vals[4*col + row]; }

	ERef<ExprMat44>			Mult(const ERef<ExprMat44>& m) const
	{
		const ERef<EExpr>& zero(EExpr::Zero());
		ERef<ExprMat44> r(Create(
			zero, zero, zero, zero,
			zero, zero, zero, zero,
			zero, zero, zero, zero,
			zero, zero, zero, zero));

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				for (int k = 0; k < 4; ++k)
				{
					r->Get(i, j) = 
						EExpr::Add(
							r->Get(i, j),
							EExpr::Mult(
								m->Get(k, j),
								Get(i, k)))->Eval();
				}
			}
		}

		return r;
	}

	void					Print()
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				std::cout << PRINT_STR("M(%d,%d) = ", i, j) << WStringToString(Get(i, j)->ToString()) << std::endl;

			std::cout << std::endl;
		}
	}
};


//===================
// main
//===================
int		main()
{
#if 0
	ERef<EExpr>		root(
		EExpr::Mult(
			EExpr::Symbol(_T("x")),
			EExpr::Number(2.0f))
		);
#endif

	/*
	ERef<EExpr>		root(
		EExpr::Mult(
		EExpr::Symbol(_T("y")),
		EExpr::Add(
			EExpr::Symbol(_T("x")),
			EExpr::Number(2.0f))
			));

	wstring resultStr(root->ToString());

	//ERef<EExpr> result(root->Eval());

	std::cout << WStringToString(root->ToString()) << std::endl;
	std::cout << WStringToString(root->Eval()->ToString()) << std::endl;
			*/

	/*
	ERef<EExpr>		root(
		EExpr::Mult(
			EExpr::NegOne(),
			EExpr::Symbol(_T("x"))));

	wstring resultStr(root->ToString());

	std::cout << WStringToString(resultStr) << std::endl;
	*/

	//ERef<ExprMat44> yRot(ExprMat44::FromYRot(EExpr::Symbol(_T("pitch"))));
	//ERef<ExprMat44> xRot(ExprMat44::FromXRot(EExpr::Symbol(_T("roll"))));
	//ERef<ExprMat44> zRot(ExprMat44::FromZRot(EExpr::Symbol(_T("yaw"))));

	// from the website..
	{
		ERef<ExprMat44> yRot(ExprMat44::FromYRot(EExpr::Symbol(_T(" h "))));
		ERef<ExprMat44> zRot(ExprMat44::FromZRot(EExpr::Symbol(_T(" a "))));
		ERef<ExprMat44> xRot(ExprMat44::FromXRot(EExpr::Symbol(_T(" b "))));

		ERef<ExprMat44> rot(zRot->Mult(yRot));
		rot = xRot->Mult(rot);

		//rot->Print();
	}

	// from the engine...
	{
		ERef<ExprMat44> roll(ExprMat44::FromYRot(EExpr::Symbol(_T(" r "))));
		ERef<ExprMat44> pitch(ExprMat44::FromXRot(EExpr::Symbol(_T(" p "))));
		ERef<ExprMat44> yaw(ExprMat44::FromZRot(EExpr::Symbol(_T(" y "))));

		ERef<ExprMat44> rot(pitch->Mult(roll));
		rot = yaw->Mult(rot);

		rot->Print();
	}

	// matrix multiplication.
	{
#define S(a, i, j)		EExpr::Symbol(_TP("%s(%d,%d)", #a, ##i, ##j))

		ERef<ExprMat44> A(ExprMat44::Create(
			S(A, 0, 0), S(A, 0, 1), S(A, 0, 2), S(A, 0, 3),
			S(A, 1, 0), S(A, 1, 1), S(A, 1, 2), S(A, 1, 3),
			S(A, 2, 0), S(A, 2, 1), S(A, 2, 2), S(A, 2, 3),
			S(A, 3, 0), S(A, 3, 1), S(A, 3, 2), S(A, 3, 3)));

		ERef<ExprMat44> B(ExprMat44::Create(
			S(B, 0, 0), S(B, 0, 1), S(B, 0, 2), S(B, 0, 3),
			S(B, 1, 0), S(B, 1, 1), S(B, 1, 2), S(B, 1, 3),
			S(B, 2, 0), S(B, 2, 1), S(B, 2, 2), S(B, 2, 3),
			S(B, 3, 0), S(B, 3, 1), S(B, 3, 2), S(B, 3, 3)));

		ERef<ExprMat44> result(A->Mult(B));
		//result->Print();
	}


	return 0;
}
