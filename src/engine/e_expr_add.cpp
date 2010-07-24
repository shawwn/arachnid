//========================================================================
//	file:		e_expr_add.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_expr_add.h"

// expression headers.
#include "e_expr_number.h"
//========================================================================

//===================
// EExprAdd::EExprAdd
//===================
EExprAdd::EExprAdd(const ERef<EExpr>& left, const ERef<EExpr>& right)
: _left(left)
, _right(right)
{
}


//===================
// EExprAdd::~EExprAdd
//===================
EExprAdd::~EExprAdd()
{
}


//===================
// EExprAdd::Eval
//===================
ERef<EExpr>			EExprAdd::Eval() const
{
	const EExpr* left(_left.Get());
	const EExpr* right(_right.Get());

	if (left->Compare(EExpr::Zero()))
		return _right;

	if (right->Compare(EExpr::Zero()))
		return _left;

	if (left->GetType() == EXPR_NUMBER && right->GetType() == EXPR_NUMBER)
	{
		const EExprNumber* leftNum((const EExprNumber*)left);
		const EExprNumber* rightNum((const EExprNumber*)right);
		return E_NEW("expr", EExprNumber)(leftNum->GetNum() + rightNum->GetNum());
	}

	return E_NEW("expr", EExprAdd)(_left, _right);
}


//===================
// EExprAdd::ToString
//===================
wstring				EExprAdd::ToString() const
{
	return _TP("%s + %s", _left->ToString().c_str(), _right->ToString().c_str());
}


//===================
// EExprAdd::Compare
//===================
bool				EExprAdd::Compare(const ERef<EExpr>& expr) const
{
	if (expr->GetType() != EXPR_ADD)
		return false;

	EExprAdd* add((EExprAdd*)expr.Get());

	if (!_left->Compare(add->_left))
		return false;

	if (!_right->Compare(add->_right))
		return false;

	return true;
}

