/*
// Dao Standard Modules
// http://www.daovm.net
//
// Copyright (c) 2011,2012, Limin Fu
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include<stdlib.h>
#include<math.h>
#include"daoValue.h"
#include"daoNumtype.h"

#ifdef _MSC_VER
#define hypot _hypot
#define isnan _isnan
#define isfinite _finite
#define copysign _copysign
#define fmax __max
#define fmin __min
#endif


static void MATH_abs( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, fabs( p[0]->xFloat.value ) );
}
static void MATH_acos( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, acos( p[0]->xFloat.value ) );
}
static void MATH_asin( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, asin( p[0]->xFloat.value ) );
}
static void MATH_atan( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, atan( p[0]->xFloat.value ) );
}
static void MATH_ceil( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, ceil( p[0]->xFloat.value ) );
}
static void MATH_cos( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, cos( p[0]->xFloat.value ) );
}
static void MATH_cosh( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, cosh( p[0]->xFloat.value ) );
}
static void MATH_exp( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, exp( p[0]->xFloat.value ) );
}
static void MATH_floor( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, floor( p[0]->xFloat.value ) );
}
static void MATH_log( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, log( p[0]->xFloat.value ) );
}
static void MATH_sin( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, sin( p[0]->xFloat.value ) );
}
static void MATH_sinh( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, sinh( p[0]->xFloat.value ) );
}
static void MATH_sqrt( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, sqrt( p[0]->xFloat.value ) );
}
static void MATH_tan( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, tan( p[0]->xFloat.value ) );
}
static void MATH_tanh( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, tanh( p[0]->xFloat.value ) );
}
static void MATH_rand( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, p[0]->xFloat.value * DaoProcess_Random( proc ) );
}
static void MATH_srand( DaoProcess *proc, DaoValue *p[], int N )
{
	srand( (unsigned int)p[0]->xInteger.value );
	rand();
}

typedef struct DaoGaussRandCache DaoGaussRandCache;
struct DaoGaussRandCache
{
	int     iset;
	double  gset;
};
static DaoGaussRandCache* DaoGaussRandCache_New()
{
	DaoGaussRandCache *self = (DaoGaussRandCache*) dao_calloc( 1, sizeof(DaoGaussRandCache) );
	return self;
}
static void DaoGaussRandCache_Delete( DaoGaussRandCache *self )
{
	dao_free( self );
}

static void MATH_rand_gaussian( DaoProcess *proc, DaoValue *p[], int N )
{
	double fac, rsq, v1, v2;
	double R = p[0]->xFloat.value;
	DaoGaussRandCache *cache = (DaoGaussRandCache*) DaoProcess_GetAuxData( proc, DaoGaussRandCache_Delete );

	if( cache == NULL ){
		cache = DaoGaussRandCache_New();
		DaoProcess_SetAuxData( proc, DaoGaussRandCache_Delete, cache );
	}

	if( cache->iset ==0 ){
		do{
			v1 = 2.0 * ( DaoProcess_Random( proc ) ) -1.0;
			v2 = 2.0 * ( DaoProcess_Random( proc ) ) -1.0;
			rsq = v1*v1 + v2*v2 ;
		} while( rsq >= 1.0 || rsq == 0.0 );
		fac = sqrt( -2.0 * log( rsq ) / rsq );
		cache->gset = v1*fac;
		cache->iset = 1;
		DaoProcess_PutFloat( proc, R*v2*fac );
	} else {
		cache->iset = 0;
		DaoProcess_PutFloat( proc, R*cache->gset );
	}
}
static void MATH_pow( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, pow( p[0]->xFloat.value, p[1]->xFloat.value ) );
}
static void MATH_atan2( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, atan2( p[0]->xFloat.value, p[1]->xFloat.value ) );
}

/**/
static void MATH_abs_c( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com = p[0]->xComplex.value;
	DaoProcess_PutFloat( proc, sqrt( com.real * com.real + com.imag * com.imag ) );
}
static void MATH_arg_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, arg_c( p[0]->xComplex.value ) );
}
static void MATH_norm_c( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com = p[0]->xComplex.value;
	DaoProcess_PutFloat( proc, com.real * com.real + com.imag * com.imag );
}
static void MATH_imag_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, p[0]->xComplex.value.imag );
}
static void MATH_real_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, p[0]->xComplex.value.real );
}

static void MATH_cos_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutComplex( proc, cos_c( p[0]->xComplex.value ) );
}
static void MATH_cosh_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutComplex( proc, cosh_c( p[0]->xComplex.value ) );
}
static void MATH_exp_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutComplex( proc, exp_c( p[0]->xComplex.value ) );
}
static void MATH_log_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutComplex( proc, log_c( p[0]->xComplex.value ) );
}
static void MATH_sin_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutComplex( proc, sin_c( p[0]->xComplex.value ) );
}
static void MATH_sinh_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutComplex( proc, sinh_c( p[0]->xComplex.value ) );
}
static void MATH_sqrt_c( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutComplex( proc, sqrt_c( p[0]->xComplex.value ) );
}
static void MATH_tan_c( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com = p[0]->xComplex.value;
	dao_complex *res = DaoProcess_PutComplex( proc, com );
	dao_complex R = sin_c( com );
	dao_complex L = cos_c( com );
	res->real = ( L.real*R.real + L.imag*R.imag ) / ( R.real*R.real + R.imag*R.imag );
	res->imag = ( L.imag*R.real - L.real*R.imag ) / ( R.real*R.real + R.imag*R.imag );
}
static void MATH_tanh_c( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com = p[0]->xComplex.value;
	dao_complex *res = DaoProcess_PutComplex( proc, com );
	dao_complex R = sinh_c( com );
	dao_complex L = cosh_c( com );
	res->real = ( L.real*R.real + L.imag*R.imag ) / ( R.real*R.real + R.imag*R.imag );
	res->imag = ( L.imag*R.real - L.real*R.imag ) / ( R.real*R.real + R.imag*R.imag );
}
static void MATH_ceil_c( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com = p[0]->xComplex.value;
	dao_complex *res = DaoProcess_PutComplex( proc, com );
	res->real = ceil( com.real );
	res->imag = ceil( com.imag );
}
static void MATH_floor_c( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com = p[0]->xComplex.value;
	dao_complex *res = DaoProcess_PutComplex( proc, com );
	res->real = floor( com.real );
	res->imag = floor( com.imag );
}

static void MATH_pow_rc( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com = { 0, 0 };
	dao_complex *res = DaoProcess_PutComplex( proc, com );
	double lg = log( p[0]->xFloat.value );
	com.real = lg * p[1]->xComplex.value.real;
	com.imag = lg * p[1]->xComplex.value.imag;
	*res = exp_c( com );
}
static void MATH_pow_cr( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com2 = { 0, 0 };
	dao_complex *res = DaoProcess_PutComplex( proc, com2 );
	dao_complex com = log_c( p[0]->xComplex.value );
	double v = p[1]->xFloat.value;
	com2.real = v * com.real;
	com2.imag = v * com.imag;
	*res = exp_c( com2 );
}
static void MATH_pow_cc( DaoProcess *proc, DaoValue *p[], int N )
{
	dao_complex com2 = {0,0};
	dao_complex *res = DaoProcess_PutComplex( proc, com2 );
	dao_complex com = log_c( p[0]->xComplex.value );
	COM_MUL( com2, com, p[1]->xComplex.value );
	*res = exp_c( com2 );
}
static void MATH_round( DaoProcess *proc, DaoValue *p[], int N )
{
	double val = p[0]->xFloat.value;
	DaoProcess_PutFloat( proc, ( val > 0 )? floor( val + 0.5 ) : ceil( val - 0.5 ) );
}
static void MATH_hypot( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, hypot( p[0]->xFloat.value, p[1]->xFloat.value ) );
}
static void MATH_isnan( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutInteger( proc, isnan( p[0]->xFloat.value ) );
}
static void MATH_isinf( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutInteger( proc, !isfinite( p[0]->xFloat.value ) );
}
static void MATH_copysign( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, copysign( p[0]->xFloat.value, p[1]->xFloat.value ) );
}
static void MATH_max( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, fmax( p[0]->xFloat.value, p[1]->xFloat.value ) );
}
static void MATH_min( DaoProcess *proc, DaoValue *p[], int N )
{
	DaoProcess_PutFloat( proc, fmin( p[0]->xFloat.value, p[1]->xFloat.value ) );
}



#if 0
/* z = x * x + r */
/* binary searching */
void DLong_Sqrt( DLong *z, DLong *x, DLong *r )
{
	int k, b1, b2;
	int i = 0;
	DLong *max = DLong_New();
	DLong *min = DLong_New();
	DLong_Resize( x, z->size + 1 );
	DLong_Resize( r, z->size + 2 );
	DLong_Copy( max, z );
	min->size = 0;
	while(1){
		i ++;
		b1 = DLong_NormCount( min );
		b2 = DLong_NormCount( max );
		/* printf( "%i  %i\n", b1, b2 ); */
		DLong_UAdd( x, min, max );
		if( b2 > b1 + 1 ){
			DLong_ShiftRight( x, (b2-b1)>>1 );
		}else{
			DLong_ShiftRight( x, 1 );
		}
		/* DLong_Print( x, NULL ); */
		DLong_Mul( r, x, x );
		k = DLong_UCompare( r, z );
		if( k ==0 ){
			DLong_Clear(r);
			return;
		}else if( k >0 ){
			DLong_Move( max, x );
		}else{
			if( DLong_UCompare( x, min ) ==0 ) break;
			DLong_Move( min, x );
		}
	}
	/* printf( "iterations: %i\n", i ); */
	DLong_Move( x, min );
	x->sign = 1;
	DLong_UMul( max, x, x );
	DLong_Sub( r, z, max );
	DLong_Delete( min );
	DLong_Delete( max );
}
static void LONG_Sqrt( DaoProcess *proc, DaoValue *p[], int N )
{
	DLong *z = p[0]->xLong.value;
	DaoTuple *tuple = DaoProcess_PutTuple( proc, 0 );
	DaoValue **items = tuple->items;
	if( z->sign <0 ){
		DaoProcess_RaiseError( proc, NULL, "need positive long integer" );
		return;
	}
	DLong_Sqrt( z, items[0]->xLong.value, items[1]->xLong.value );
}
#endif


static DaoFuncItem mathMeths[]=
{
#if 0
	{ MATH_abs,       "abs( p :double )=>double" },
	{ MATH_acos,      "acos( p :double )=>double" },
	{ MATH_asin,      "asin( p :double )=>double" },
	{ MATH_atan,      "atan( p :double )=>double" },
	{ MATH_ceil,      "ceil( p :double )=>double" },
	{ MATH_cos,       "cos( p :double )=>double" },
	{ MATH_cosh,      "cosh( p :double )=>double" },
	{ MATH_exp,       "exp( p :double )=>double" },
	{ MATH_floor,     "floor( p :double )=>double" },
	{ MATH_log,       "log( p :double )=>double" },
	{ MATH_sin,       "sin( p :double )=>double" },
	{ MATH_sinh,      "sinh( p :double )=>double" },
	{ MATH_sqrt,      "sqrt( p :double )=>double" },
	{ MATH_tan,       "tan( p :double )=>double" },
	{ MATH_tanh,      "tanh( p :double )=>double" },
	{ MATH_rand,      "rand( p :double=1.0D )=>double" },
#endif

	{ MATH_srand,     "srand( p :int)=>int" },
	{ MATH_rand_gaussian,  "rand_gaussian( p :double=1.0D )=>double" },
	{ MATH_round,     "round( p :double )=>double" },
	{ MATH_hypot,     "hypot( p1 :double, p2 :double )=>double" },
	{ MATH_isnan,     "isnan( p :double )=>int" },
	{ MATH_isinf,     "isinf( p :double )=>int" },
	{ MATH_copysign,  "copysign( p1 :double, p2 :double )=>double" },
	{ MATH_max,       "max( p1 :double, p2 :double )=>double" },
	{ MATH_min,       "min( p1 :double, p2 :double )=>double" },
	{ MATH_pow,       "pow( p1 :double, p2 :double )=>double" },
	{ MATH_atan2,     "atan2( p1 :double, p2 :double )=>double" },

#if 0
	{ MATH_abs_c,     "abs( p :complex )=>double" },
	{ MATH_arg_c,     "arg( p :complex )=>double" },
	{ MATH_imag_c,    "imag( p :complex )=>double" },
	{ MATH_norm_c,    "norm( p :complex )=>double" },
	{ MATH_real_c,    "real( p :complex )=>double" },

	{ MATH_cos_c,     "cos( p :complex )=>complex" },
	{ MATH_cosh_c,    "cosh( p :complex )=>complex" },
	{ MATH_exp_c,     "exp( p :complex )=>complex" },
	{ MATH_log_c,     "log( p :complex )=>complex" },
	{ MATH_sin_c,     "sin( p :complex )=>complex" },
	{ MATH_sinh_c,    "sinh( p :complex )=>complex" },
	{ MATH_sqrt_c,    "sqrt( p :complex )=>complex" },
	{ MATH_tan_c,     "tan( p :complex )=>complex" },
	{ MATH_tanh_c,    "tanh( p :complex )=>complex" },
	{ MATH_ceil_c,    "ceil( p :complex )=>complex" },
	{ MATH_floor_c,   "floor( p :complex )=>complex" },
#endif

	{ MATH_pow_rc,    "pow( p1 :double, p2 :complex )=>complex" },
	{ MATH_pow_cr,    "pow( p1 :complex, p2 :double )=>complex" },
	{ MATH_pow_cc,    "pow( p1 :complex, p2 :complex )=>complex" },

	//{ LONG_Sqrt, "sqrt( self : long ) => tuple<long,long>" },

	{ NULL, NULL }
};

DaoNumItem mathConsts[] =
{
	{ "MATH_E",        DAO_FLOAT, M_E },
	{ "MATH_LOG2E",    DAO_FLOAT, M_LOG2E },
	{ "MATH_LOG10E",   DAO_FLOAT, M_LOG10E },
	{ "MATH_LN2",      DAO_FLOAT, M_LN2 },
	{ "MATH_LN10",     DAO_FLOAT, M_LN10 },
	{ "MATH_PI",       DAO_FLOAT, M_PI },
	{ "MATH_PI_2",     DAO_FLOAT, M_PI_2 },
	{ "MATH_PI_4",     DAO_FLOAT, M_PI_4 },
	{ "MATH_1_PI",     DAO_FLOAT, M_1_PI },
	{ "MATH_2_PI",     DAO_FLOAT, M_2_PI },
	{ "MATH_2_SQRTPI", DAO_FLOAT, M_2_SQRTPI },
	{ "MATH_SQRT2",    DAO_FLOAT, M_SQRT2 },
	{ "MATH_SQRT1_2",  DAO_FLOAT, M_SQRT1_2 },
	{ NULL, 0.0, 0.0 }
};

DAO_DLL int DaoMath_OnLoad( DaoVmSpace *vmSpace, DaoNamespace *ns )
{
	DaoNamespace_WrapFunctions( ns, mathMeths );
	DaoNamespace_AddConstNumbers( ns, mathConsts );
	return 0;
}
