//========================================================================
//	file:		e_expr_symbol.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_expr_symbol.h"
//========================================================================


//===================
// EExprSymbol::Compare
//===================
bool				EExprSymbol::Compare(const ERef<EExpr>& expr) const
{
	if (expr->GetType() != EXPR_SYMBOL)
		return false;

	EExprSymbol* symExpr((EExprSymbol*)expr.Get());
	if (_symbol != symExpr->_symbol)
		return false;

	return true;
}

