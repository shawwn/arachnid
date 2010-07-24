//========================================================================
//	file:		e_expr_number.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_expr_number.h"
//========================================================================


//===================
// EExprNumber::Compare
//===================
bool				EExprNumber::Compare(const ERef<EExpr>& expr) const
{
	if (expr->GetType() != EXPR_NUMBER)
		return false;

	EExprNumber* numExpr((EExprNumber*)expr.Get());
	if (!FloatEqual(_num, numExpr->_num))
		return false;

	return true;
}

