//========================================================================
//	file:		e_expr_add.h
//	author:		Shawn Presser 
//	date:		7/17/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "e_expr.h"
//========================================================================

//========================================================================
// EExprAdd
//========================================================================
class EExprAdd : public EExpr
{
private:
	ERef<EExpr>				_left;
	ERef<EExpr>				_right;
public:
	EExprAdd(const ERef<EExpr>& left, const ERef<EExpr>& right);
	~EExprAdd();

	virtual EExprType		GetType() const			{ return EXPR_ADD; }
	virtual ERef<EExpr>		Eval() const;
	virtual wstring			ToString() const;

	virtual ERef<EExpr>		GetLeft() const			{ return _left; }
	virtual ERef<EExpr>		GetRight() const		{ return _right; }

	virtual bool			Compare(const ERef<EExpr>& expr) const;
};
//========================================================================
