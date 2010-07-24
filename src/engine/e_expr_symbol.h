//========================================================================
//	file:		e_expr_symbol.h
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
// EExprSymbol
//========================================================================
class ENGINE_API EExprSymbol : public EExpr
{
private:
	wstring					_symbol;
public:
	EExprSymbol()						{ }
	EExprSymbol(const wstring& symbol)	{ _symbol = symbol; }

	const wstring&			GetSymbol() const		{ return _symbol; }

	virtual EExprType		GetType() const			{ return EXPR_SYMBOL; }
	virtual ERef<EExpr>		Eval() const			{ return ERef<EExpr>(this); }
	virtual wstring			ToString() const		{ return _symbol; }

	virtual bool			Compare(const ERef<EExpr>& expr) const;
};
//========================================================================
