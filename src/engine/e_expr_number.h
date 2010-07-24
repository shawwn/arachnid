//========================================================================
//	file:		e_expr_number.h
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
// EExprNumber
//========================================================================
class ENGINE_API EExprNumber : public EExpr
{
private:
	float					_num;
public:
	EExprNumber()			{ _num = 0.0f; }
	EExprNumber(float num)	{ _num = num; }

	float					GetNum() const			{ return _num; }

	virtual EExprType		GetType() const			{ return EXPR_NUMBER; }
	virtual ERef<EExpr>		Eval() const			{ return ERef<EExpr>(this); }
	virtual wstring			ToString() const		{ return TO_STR(_num); }

	virtual bool			Compare(const ERef<EExpr>& expr) const;
};
//========================================================================
