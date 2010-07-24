//========================================================================
//	file:		e_expr.h
//	author:		Shawn Presser 
//	date:		7/17/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
enum EExprType
{
	EXPR_ADD,
	EXPR_MULT,
	EXPR_NUMBER,
	EXPR_SYMBOL,
	EXPR_SIN,
	EXPR_COS
};
//========================================================================

//========================================================================
// EExpr
//========================================================================
class ENGINE_API EExpr : public ERefCounted
{
public:
	EExpr()					{ }

	static const ERef<EExpr>&		Zero();
	static const ERef<EExpr>&		One();
	static const ERef<EExpr>&		NegOne();

	static ERef<EExpr>		Add(const ERef<EExpr>& A, const ERef<EExpr>& B);
	static ERef<EExpr>		Mult(const ERef<EExpr>& A, const ERef<EExpr>& B);
	static ERef<EExpr>		Number(float num);
	static ERef<EExpr>		Symbol(const wstring& sym);
	static ERef<EExpr>		Negate(const ERef<EExpr>& expr)			{ return Mult(NegOne(), expr); }
	static ERef<EExpr>		Sin(const ERef<EExpr>& expr)			{ return Symbol(_TP("sin(%s)", expr->ToString().c_str())); }
	static ERef<EExpr>		Cos(const ERef<EExpr>& expr)			{ return Symbol(_TP("cos(%s)", expr->ToString().c_str())); }

	virtual EExprType		GetType() const=0;
	virtual ERef<EExpr>		Eval() const=0;
	virtual wstring			ToString() const=0;

	virtual ERef<EExpr>		GetLeft() const			{ return this; }
	virtual ERef<EExpr>		GetRight() const		{ return this; }

	ERef<EExpr>				operator -() const		{ return Negate(this); }

	virtual bool			Compare(const ERef<EExpr>& expr) const=0;
};
//========================================================================
