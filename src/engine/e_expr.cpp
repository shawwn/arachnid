//========================================================================
//	file:		e_expr.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_expr.h"

// expression headers.
#include "e_expr_add.h"
#include "e_expr_multiply.h"
#include "e_expr_number.h"
#include "e_expr_symbol.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
static ERef<EExpr>		ActualNumber(float num);
//========================================================================

//========================================================================
// Definitions
//========================================================================
static ERef<EExpr>		gZero(ActualNumber(0.0f));
static ERef<EExpr>		gOne(ActualNumber(1.0f));
static ERef<EExpr>		gNegOne(ActualNumber(-1.0f));
//========================================================================


//===================
// ActualNumber
//===================
ERef<EExpr>				ActualNumber(float num)
{
	return E_NEW("expr", EExprNumber)(num);
}


//===================
// EExpr::Zero
//===================
const ERef<EExpr>&		EExpr::Zero()
{
	return gZero;
}


//===================
// EExpr::One
//===================
const ERef<EExpr>&		EExpr::One()
{
	return gOne;
}


//===================
// EExpr::NegOne
//===================
const ERef<EExpr>&		EExpr::NegOne()
{
	return gNegOne;
}


//===================
// EExpr::Add
//===================
ERef<EExpr>				EExpr::Add(const ERef<EExpr>& A, const ERef<EExpr>& B)
{
	if (A->Compare(gZero))
		return B;

	if (B->Compare(gZero))
		return A;

	return E_NEW("expr", EExprAdd)(A, B);
}


//===================
// EExpr::Mult
//===================
ERef<EExpr>				EExpr::Mult(const ERef<EExpr>& Aptr, const ERef<EExpr>& Bptr)
{
	ERef<EExpr> A(Aptr);
	ERef<EExpr> B(Bptr);

	if (A->Compare(gZero) || B->Compare(gZero))
		return gZero;

	if (A->Compare(gOne))
		return B;

	if (B->Compare(gOne))
		return A;

	if (B->Compare(gNegOne))
		Swap(A, B);

	if (A->Compare(gNegOne))
	{
		if (B->Compare(gNegOne))
			return gOne;

		if (B->GetType() == EXPR_MULT)
		{
			const EExprMultiply* mult((const EExprMultiply*)B.Get());
			if (mult->GetLeft()->Compare(gNegOne))
				return mult->GetRight();

			if (mult->GetRight()->Compare(gNegOne))
				return mult->GetLeft();

			return Mult(
				Mult(gNegOne, mult->GetLeft()),
				Mult(gNegOne, mult->GetRight()));
		}

		if (B->GetType() == EXPR_ADD)
		{
			const EExprAdd* add((const EExprAdd*)B.Get());

			return Add(
				Mult(gNegOne, add->GetLeft()),
				Mult(gNegOne, add->GetRight()));
		}
	}

	return E_NEW("expr", EExprMultiply)(A, B);
}


//===================
// EExpr::Number
//===================
ERef<EExpr>				EExpr::Number(float num)
{
	if (num < 0.0f)
		return Mult(gNegOne, Number(ABS(num)));

	return E_NEW("expr", EExprNumber)(num);
}


//===================
// EExpr::Symbol
//===================
ERef<EExpr>				EExpr::Symbol(const wstring& sym)
{
	return E_NEW("expr", EExprSymbol)(sym);
}

