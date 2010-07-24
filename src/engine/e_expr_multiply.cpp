//========================================================================
//	file:		e_expr_multiply.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_expr_multiply.h"

// expression headers.
#include "e_expr_number.h"
#include "e_expr_symbol.h"
#include "e_expr_add.h"
//========================================================================


//===================
// EExprMultiply::EExprMultiply
//===================
EExprMultiply::EExprMultiply(const ERef<EExpr>& left, const ERef<EExpr>& right)
: _left(left)
, _right(right)
{
}


//===================
// EExprMultiply::~EExprMultiply
//===================
EExprMultiply::~EExprMultiply()
{
}


//===================
// EExprMultiply::Eval
//===================
ERef<EExpr>			EExprMultiply::Eval() const
{
	ERef<EExpr> left(_left->Eval());
	ERef<EExpr> right(_right->Eval());

	if (left->Compare(EExpr::Zero()) || right->Compare(EExpr::Zero()))
		return EExpr::Zero();

	if (left->Compare(EExpr::One()))
		return right;

	if (right->Compare(EExpr::One()))
		return left;

	if (right->Compare(EExpr::NegOne()))
		Swap(left, right);

	if (left->Compare(EExpr::NegOne()))
	{
		if (right->Compare(EExpr::One()))
			return EExpr::NegOne();

		if (right->Compare(EExpr::NegOne()))
			return EExpr::One();

		if (right->GetType() == EXPR_MULT)
		{
			const EExprMultiply* mult((const EExprMultiply*)right.Get());
			if (mult->GetLeft()->Compare(EExpr::NegOne()))
				return mult->GetRight()->Eval();
			if (mult->GetRight()->Compare(EExpr::NegOne()))
				return mult->GetLeft()->Eval();

			return Mult(
				Mult(EExpr::NegOne(), mult->GetLeft()),
				Mult(EExpr::NegOne(), mult->GetRight()))->Eval();
		}

		if (right->GetType() == EXPR_ADD)
		{
			const EExprAdd* add((const EExprAdd*)right.Get());
			return Add(
				Mult(EExpr::NegOne(), add->GetLeft()),
				Mult(EExpr::NegOne(), add->GetRight()))->Eval();
		}

		if (right->GetType() == EXPR_NUMBER)
			return this;
	}

	if (left->GetType() == EXPR_MULT && right->GetType() == EXPR_MULT)
	{
		const EExprMultiply* leftMult((const EExprMultiply*)left.Get());
		ERef<EExpr> leftMultLeft(leftMult->GetLeft());
		ERef<EExpr> leftMultRight(leftMult->GetRight());
		if (leftMultRight->Compare(EExpr::NegOne()))
			Swap(leftMultLeft, leftMultRight);

		const EExprMultiply* rightMult((const EExprMultiply*)right.Get());
		ERef<EExpr> rightMultLeft(rightMult->GetLeft());
		ERef<EExpr> rightMultRight(rightMult->GetRight());
		if (rightMultRight->Compare(EExpr::NegOne()))
			Swap(rightMultLeft, rightMultRight);

		if (leftMultLeft->Compare(EExpr::NegOne()))
			if (rightMultLeft->Compare(EExpr::NegOne()))
				return Mult(leftMultRight, rightMultRight)->Eval();
	}

	if (right->GetType() == EXPR_MULT)
	{
		const EExprMultiply* rightMult((const EExprMultiply*)right.Get());
		ERef<EExpr> rightMultLeft(rightMult->GetLeft());
		ERef<EExpr> rightMultRight(rightMult->GetRight());
		if (rightMultRight->Compare(EExpr::NegOne()))
			Swap(rightMultLeft, rightMultRight);

		if (rightMultLeft->Compare(EExpr::NegOne()))
		{
			return Mult(
				Mult(EExpr::NegOne(), left),
				rightMultRight)->Eval();
		}
	}

	if (left->GetType() == EXPR_NUMBER)
	{
		if (right->GetType() == EXPR_NUMBER)
		{
			const EExprNumber* leftNum((const EExprNumber*)left.Get());
			const EExprNumber* rightNum((const EExprNumber*)right.Get());
			return E_NEW("expr", EExprNumber)(leftNum->GetNum() * rightNum->GetNum());
		}

		if (right->GetType() == EXPR_ADD)
		{
			return EExpr::Add(
				EExpr::Mult(left, right->GetLeft()),
				EExpr::Mult(left, right->GetRight()))->Eval();
		}
	}

	if (left->GetType() == EXPR_SYMBOL)
	{
		if (right->GetType() == EXPR_ADD)
		{
			return EExpr::Add(
				EExpr::Mult(left, right->GetLeft()),
				EExpr::Mult(left, right->GetRight()))->Eval();
		}
	}

	return EExpr::Mult(left, right);
}


//===================
// EExprMultiply::ToString
//===================
wstring				EExprMultiply::ToString() const
{
	ERef<EExpr> left(_left);
	ERef<EExpr> right(_right);

	if (right->Compare(EExpr::NegOne()) || right->Compare(EExpr::One()))
		Swap(left, right);

	wstring leftFmt;
	if (left->GetType() == EXPR_ADD)
		leftFmt.append(_T("(%s) * "));
	else
		leftFmt.append(_T("%s * "));

	if (left->Compare(EExpr::NegOne()))
		leftFmt = _T("-");

	if (left->Compare(EExpr::One()))
		leftFmt = _T("");


	wstring rightFmt;
	if (right->GetType() == EXPR_ADD)
		rightFmt.append(_T("(%s)"));
	else
		rightFmt.append(_T("%s"));


	wstring str(_TF(leftFmt.c_str(), left->ToString().c_str()));
	str.append(_TF(rightFmt.c_str(), right->ToString().c_str()));
	return str;
}


//===================
// EExprMultiply::Compare
//===================
bool				EExprMultiply::Compare(const ERef<EExpr>& expr) const
{
	if (expr->GetType() != EXPR_MULT)
		return false;

	EExprMultiply* mult((EExprMultiply*)expr.Get());

	if (!_left->Compare(mult->_left))
		return false;

	if (!_right->Compare(mult->_right))
		return false;

	return true;
}

