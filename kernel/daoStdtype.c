/*=========================================================================================
  This file is a part of a virtual machine for the Dao programming language.
  Copyright (C) 2006-2011, Fu Limin. Email: fu@daovm.net, limin.fu@yahoo.com

  This software is free software; you can redistribute it and/or modify it under the terms
  of the GNU Lesser General Public License as published by the Free Software Foundation;
  either version 2.1 of the License, or (at your option) any later version.

  This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  =========================================================================================*/

#include"stdlib.h"
#include"stdio.h"
#include"string.h"
#include"ctype.h"
#include"math.h"

#include"daoType.h"
#include"daoVmspace.h"
#include"daoNamespace.h"
#include"daoNumtype.h"
#include"daoStream.h"
#include"daoRoutine.h"
#include"daoObject.h"
#include"daoContext.h"
#include"daoProcess.h"
#include"daoGC.h"
#include"daoStdlib.h"
#include"daoClass.h"
#include"daoParser.h"
#include"daoMacro.h"
#include"daoRegex.h"
#include"daoSched.h"
#include"daoValue.h"

int ObjectProfile[100];

void DaoValue_Init( void *value, char type )
{
	DaoNull *self = (DaoNull*) value;
	self->type = type;
	self->subtype = self->trait = self->marks = 0;
	self->refCount = 0;
	if( type >= DAO_ENUM ) ((DaoValue*)self)->xGC.cycRefCount = 0;
#ifdef DAO_GC_PROF
	if( type < 100 )  ObjectProfile[(int)type] ++;
#endif
}

DaoNull* DaoNull_New()
{
	DaoNull *self = (DaoNull*) dao_malloc( sizeof(DaoNull) );
	DaoValue_Init( self, DAO_NULL );
	return self;
}
DaoInteger* DaoInteger_New( dint value )
{
	DaoInteger *self = (DaoInteger*) dao_malloc( sizeof(DaoInteger) );
	DaoValue_Init( self, DAO_INTEGER );
	self->value = value;
	return self;
}
dint DaoInteger_Get( DaoInteger *self )
{
	return self->value;
}
void DaoInteger_Set( DaoInteger *self, dint value )
{
	self->value = value;
}

DaoFloat* DaoFloat_New( float value )
{
	DaoFloat *self = (DaoFloat*) dao_malloc( sizeof(DaoFloat) );
	DaoValue_Init( self, DAO_FLOAT );
	self->value = value;
	return self;
}
float DaoFloat_Get( DaoFloat *self )
{
	return self->value;
}
void DaoFloat_Set( DaoFloat *self, float value )
{
	self->value = value;
}

DaoDouble* DaoDouble_New( double value )
{
	DaoDouble *self = (DaoDouble*) dao_malloc( sizeof(DaoDouble) );
	DaoValue_Init( self, DAO_DOUBLE );
	self->value = value;
	return self;
}
double DaoDouble_Get( DaoDouble *self )
{
	return self->value;
}
void DaoDouble_Set( DaoDouble *self, double value )
{
	self->value = value;
}

DaoComplex* DaoComplex_New( complex16 value )
{
	DaoComplex *self = (DaoComplex*) dao_malloc( sizeof(DaoComplex) );
	DaoValue_Init( self, DAO_COMPLEX );
	self->value = value;
	return self;
}
complex16  DaoComplex_Get( DaoComplex *self )
{
	return self->value;
}
void DaoComplex_Set( DaoComplex *self, complex16 value )
{
	self->value = value;
}

DaoLong* DaoLong_New()
{
	DaoLong *self = (DaoLong*) dao_malloc( sizeof(DaoLong) );
	DaoValue_Init( self, DAO_LONG );
	self->value = DLong_New();
	return self;
}
DLong* DaoLong_Get( DaoLong *self )
{
	return self->value;
}
DaoLong* DaoLong_Copy( DaoLong *self )
{
	DaoLong *copy = (DaoLong*) dao_malloc( sizeof(DaoLong) );
	DaoValue_Init( copy, DAO_LONG );
	copy->value = DLong_New();
	DLong_Move( copy->value, self->value );
	return copy;
}
void DaoLong_Delete( DaoLong *self )
{
	DLong_Delete( self->value );
	dao_free( self );
}
void DaoLong_Set( DaoLong *self, DLong *value )
{
	DLong_Move( self->value, value );
}

DaoString* DaoString_New( int mbs )
{
	DaoString *self = (DaoString*) dao_malloc( sizeof(DaoString) );
	DaoValue_Init( self, DAO_STRING );
	self->data = DString_New( mbs );
	return self;
}
DaoString* DaoString_NewMBS( const char *mbs )
{
	DaoString *self = DaoString_New(1);
	DString_SetMBS( self->data, mbs );
	return self;
}
DaoString* DaoString_NewWCS( const wchar_t *wcs )
{
	DaoString *self = DaoString_New(0);
	DString_SetWCS( self->data, wcs );
	return self;
}
DaoString* DaoString_NewBytes( const char *bytes, int n )
{
	DaoString *self = DaoString_New(1);
	DString_SetDataMBS( self->data, bytes, n );
	return self;
}
DaoString* DaoString_Copy( DaoString *self )
{
	DaoString *copy = (DaoString*) dao_malloc( sizeof(DaoString) );
	DaoValue_Init( copy, DAO_STRING );
	copy->data = DString_Copy( self->data );
	return copy;
}
void DaoString_Delete( DaoString *self )
{
	DString_Delete( self->data );
	dao_free( self );
}
size_t  DaoString_Size( DaoString *self )
{
	return self->data->size;
}
DString* DaoString_Get( DaoString *self )
{
	return self->data;
}
const char* DaoString_GetMBS( DaoString *self )
{
	return DString_GetMBS( self->data );
}
const wchar_t* DaoString_GetWCS( DaoString *self )
{
	return DString_GetWCS( self->data );
}

void DaoString_Set( DaoString *self, DString *str )
{
	DString_Assign( self->data, str );
}
void DaoString_SetMBS( DaoString *self, const char *mbs )
{
	DString_SetMBS( self->data, mbs );
}
void DaoString_SetWCS( DaoString *self, const wchar_t *wcs )
{
	DString_SetWCS( self->data, wcs );
}
void DaoString_SetBytes( DaoString *self, const char *bytes, int n )
{
	DString_SetDataMBS( self->data, bytes, n );
}

enum{ IDX_NULL, IDX_SINGLE, IDX_FROM, IDX_TO, IDX_PAIR, IDX_ALL, IDX_MULTIPLE };

static DaoValue* DaoValue_MakeCopy( DaoValue *self, DaoContext *ctx, DMap *cycData )
{
	DaoTypeBase *typer;
	if( self->type <= DAO_COMPLEX ) return self;
	typer = DaoValue_GetTyper( self );
	return typer->priv->Copy( self, ctx, cycData );
}

static DArray* MakeIndex( DaoContext *ctx, DaoValue *index, size_t N, size_t *start, size_t *end, int *idtype )
{
	size_t i;
	llong_t n1, n2;
	DaoValue **items;
	DaoValue *first, *second;
	DArray *array;

	*idtype = IDX_NULL;
	*start = 0;
	*end = N - 1;
	if( index == NULL ) return NULL;

	switch( index->type ){
	case DAO_INTEGER :
		*idtype = IDX_SINGLE;
		n1 = index->xInteger.value;
		if( n1 <0 ) n1 += N;
		*start = n1;
		*end = n1;
		break;
	case DAO_FLOAT :
		*idtype = IDX_SINGLE;
		n1 = (llong_t)(index->xFloat.value);
		if( n1 <0 ) n1 += N;
		*start = n1;
		*end = n1;
		break;
	case DAO_DOUBLE :
		*idtype = IDX_SINGLE;
		n1 = (llong_t)(index->xDouble.value);
		if( n1 <0 ) n1 += N;
		*start = n1;
		*end = n1;
		break;
	case DAO_TUPLE:
		*idtype = IDX_PAIR;
		if( index->xTuple.unitype == dao_type_for_iterator ){
			DaoValue **data = index->xTuple.items;
			if( data[0]->type == data[1]->type && data[0]->type == DAO_INTEGER ){
				*idtype = IDX_SINGLE;
				*start = data[1]->xInteger.value;
				data[1]->xInteger.value += 1;
				data[0]->xInteger.value = data[1]->xInteger.value < N;
				break;
			}
		}
		first = index->xTuple.items[0];
		second = index->xTuple.items[1];
		/* a[ : 1 ] ==> pair(nil,int) */
		if( first->type > DAO_DOUBLE || second->type > DAO_DOUBLE )
			DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "need number" );
		n1 = DaoValue_GetInteger( first );
		n2 = DaoValue_GetInteger( second );
		if( n1 <0 ) n1 += N;
		if( n2 <0 ) n2 += N;
		*start = n1;
		*end = n2;
		if( first->type ==DAO_NULL && second->type ==DAO_NULL ){
			*idtype = IDX_ALL;
		}else if( first->type ==DAO_NULL ){
			*idtype = IDX_TO;
		}else if( second->type ==DAO_NULL ){
			*idtype = IDX_FROM;
		}
		/* when specify an index range, allow out of range: (eg, str[:5]=='abcde') */
		if( n1 >= N ) n1 = N-1;
		if( n2 >= N ) n2 = N-1;
		break;
	case DAO_LIST:
		*idtype = IDX_MULTIPLE;
		items = index->xList.items->items.pValue;
		array = DArray_New(0);
		DArray_Resize( array, index->xList.items->size, 0 );
		for( i=0; i<array->size; i++ ){
			if( ! DaoValue_IsNumber( items[i] ) )
				DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "need number" );
			array->items.pInt[i] = (size_t) DaoValue_GetDouble( items[i] );
			if( array->items.pInt[i] >= N ){
				DaoContext_RaiseException( ctx, DAO_ERROR_INDEX_OUTOFRANGE, "" );
				array->items.pInt[i] = N - 1;
			}
		}
		return array;
	default : break;
	}
	return NULL;
}

void DaoValue_Delete( void *self ){ dao_free( self ); }

DaoValue* DaoValue_NoCopy( DaoValue *self, DaoContext *ctx, DMap *cycData )
{
	return self;
}

DaoTypeCore baseCore =
{
	0, NULL, NULL, NULL, NULL,
	DaoValue_GetField,
	DaoValue_SetField,
	DaoValue_GetItem,
	DaoValue_SetItem,
	DaoValue_Print,
	DaoValue_NoCopy,
};
DaoTypeBase baseTyper =
{
	"null", & baseCore, NULL, NULL, {0}, {0}, DaoValue_Delete, NULL
};
DaoNull null0 = {0,0,DAO_DATA_CONST,0,1};
DaoValue *null = (DaoValue*) & null0;


extern DaoTypeBase numberTyper;
extern DaoTypeBase comTyper;
extern DaoTypeBase longTyper;
extern DaoTypeBase stringTyper;

DaoEnum* DaoEnum_New( DaoType *type, dint value )
{
	DaoEnum *self = (DaoEnum*) dao_malloc( sizeof(DaoEnum) );
	DaoValue_Init( self, DAO_ENUM );
	self->value = value;
	self->etype = type;
	if( type ) GC_IncRC( type );
	return self;
}
DaoEnum* DaoEnum_Copy( DaoEnum *self )
{
	return DaoEnum_New( self->etype, self->value );
}
void DaoEnum_Delete( DaoEnum *self )
{
	if( self->etype ) GC_DecRC( self->etype );
	dao_free( self );
}
void DaoEnum_MakeName( DaoEnum *self, DString *name )
{
	DMap *mapNames;
	DNode *node;
	DString_Clear( name );
	mapNames = self->etype->mapNames;
	for(node=DMap_First(mapNames);node;node=DMap_Next(mapNames,node)){
		if( self->etype->flagtype ){
			if( !(node->value.pInt & self->value) ) continue;
		}else if( node->value.pInt != self->value ){
			continue;
		}
		DString_AppendChar( name, '$' );
		DString_Append( name, node->key.pString );
	}
}
void DaoEnum_SetType( DaoEnum *self, DaoType *type )
{
	if( self->etype == type ) return;
	GC_ShiftRC( type, self->etype );
	self->etype = type;
	self->value = type->mapNames->root->value.pInt;
}
int DaoEnum_SetSymbols( DaoEnum *self, const char *symbols )
{
	DString *names;
	dint first = 0;
	dint value = 0;
	int notfound = 0;
	int i, k = 0;
	if( self->etype->name->mbs[0] == '$' ) return 0;
	names = DString_New(1);
	DString_SetMBS( names, symbols );
	for(i=0; i<names->size; i++) if( names->mbs[i] == '$' ) names->mbs[i] = 0;
	i = 0;
	if( names->mbs[0] == '\0' ) i += 1;
	do{ /* for multiple symbols */
		DString name = DString_WrapMBS( names->mbs + i );
		DNode *node = DMap_Find( self->etype->mapNames, &name );
		if( node ){
			if( ! k ) first = node->value.pInt;
			value |= node->value.pInt;
			k += 1;
		}else{
			notfound = 1;
		}
		i += name.size + 1;
	}while( i < names->size );
	DString_Delete( names );
	if( k == 0 ) return 0;
	if( self->etype->flagtype ==0 && k > 1 ){
		self->value = first;
		return 0;
	}
	self->value = value;
	return notfound == 0;
}
int DaoEnum_SetValue( DaoEnum *self, DaoEnum *other, DString *enames )
{
	DMap *selfNames = self->etype->mapNames;
	DMap *otherNames = other->etype->mapNames;
	DNode *node, *search;

	if( self->etype == other->etype ){
		self->value = other->value;
		return 1;
	}
	if( self->etype->name->mbs[0] == '$' ) return 0;

	self->value = 0;
	if( self->etype->flagtype && other->etype->flagtype ){
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( !(node->value.pInt & other->value) ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value |= search->value.pInt;
		}
	}else if( self->etype->flagtype ){
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( node->value.pInt != other->value ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value |= search->value.pInt;
		}
	}else if( other->etype->flagtype ){
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( !(node->value.pInt & other->value) ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value = search->value.pInt;
			break;
		}
		return node && (node->value.pInt == other->value);
	}else{
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( node->value.pInt != other->value ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value = search->value.pInt;
			break;
		}
	}
	return other->etype->name->mbs[0] == '$';
}
int DaoEnum_AddValue( DaoEnum *self, DaoEnum *other, DString *enames )
{
	DMap *selfNames = self->etype->mapNames;
	DMap *otherNames = other->etype->mapNames;
	DNode *node, *search;

	if( self->etype->flagtype ==0 || self->etype->name->mbs[0] == '$' ) return 0;

	if( self->etype == other->etype ){
		self->value |= other->value;
		return 1;
	}else if( other->etype->flagtype ){
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( !(node->value.pInt & other->value) ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value |= search->value.pInt;
		}
	}else{
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( node->value.pInt != other->value ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value |= search->value.pInt;
		}
	}
	return other->etype->name->mbs[0] == '$';
}
int DaoEnum_RemoveValue( DaoEnum *self, DaoEnum *other, DString *enames )
{
	DMap *selfNames = self->etype->mapNames;
	DMap *otherNames = other->etype->mapNames;
	DNode *node, *search;

	if( self->etype->flagtype ==0 || self->etype->name->mbs[0] == '$' ) return 0;

	if( self->etype == other->etype ){
		self->value &= ~ other->value;
		return 1;
	}else if( other->etype->flagtype ){
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( !(node->value.pInt & other->value) ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value &= ~search->value.pInt;
		}
	}else{
		for(node=DMap_First(otherNames);node;node=DMap_Next(otherNames,node)){
			if( node->value.pInt != other->value ) continue;
			search = DMap_Find( selfNames, node->key.pVoid );
			if( search == NULL ) return 0;
			self->value &= ~search->value.pInt;
		}
	}
	return other->etype->name->mbs[0] == '$';
}
int DaoEnum_AddSymbol( DaoEnum *self, DaoEnum *s1, DaoEnum *s2, DaoNamespace *ns )
{
	DaoType *type;
	DMap *names1 = s1->etype->mapNames;
	DMap *names2 = s2->etype->mapNames;
	DMap *mapNames;
	DNode *node;
	DString *name;
	dint value = 0;
	if( s1->etype->name->mbs[0] != '$' && s2->etype->name->mbs[0] != '$' ) return 0;
	name = DString_New(1);
	for(node=DMap_First(names1);node;node=DMap_Next(names1,node)){
		DString_AppendChar( name, '$' );
		DString_Append( name, node->key.pString );
	}
	for(node=DMap_First(names2);node;node=DMap_Next(names2,node)){
		if( DMap_Find( names1, node->key.pVoid ) ) continue;
		DString_AppendChar( name, '$' );
		DString_Append( name, node->key.pString );
	}
	type = DaoNamespace_FindType( ns, name );
	if( type == NULL ){
		type = DaoType_New( name->mbs, DAO_ENUM, NULL, NULL );
		type->flagtype = 1;
		type->mapNames = mapNames = DMap_Copy( names1 );
		value = s1->value;
		if( mapNames->size == 1 ){
			mapNames->root->value.pInt = 1;
			value = 1;
		}
		for(node=DMap_First(names2);node;node=DMap_Next(names2,node)){
			if( DMap_Find( names1, node->key.pVoid ) ) continue;
			value |= (1<<mapNames->size);
			MAP_Insert( mapNames, node->key.pVoid, 1<<mapNames->size );
		}
		DaoNamespace_AddType( ns, name, type );
	}
	DaoEnum_SetType( self, type );
	DString_Delete( name );
	self->value = value;
	return 1;
}
int DaoEnum_SubSymbol( DaoEnum *self, DaoEnum *s1, DaoEnum *s2, DaoNamespace *ns )
{
	DaoType *type;
	DMap *names1 = s1->etype->mapNames;
	DMap *names2 = s2->etype->mapNames;
	DMap *mapNames;
	DNode *node;
	DString *name;
	dint value = 0;
	int count = 0;
	if( s1->etype->name->mbs[0] != '$' && s2->etype->name->mbs[0] != '$' ) return 0;
	name = DString_New(1);
	for(node=DMap_First(names1);node;node=DMap_Next(names1,node)){
		if( DMap_Find( names2, node->key.pVoid ) ) continue;
		DString_AppendChar( name, '$' );
		DString_Append( name, node->key.pString );
		count += 1;
	}
	if( count ==0 ){
		DString_Delete( name );
		return 0;
	}
	type = DaoNamespace_FindType( ns, name );
	if( type == NULL ){
		type = DaoType_New( name->mbs, DAO_ENUM, NULL, NULL );
		type->flagtype = count > 1 ? 1 : 0;
		type->mapNames = mapNames = DMap_New(D_STRING,0);
		value = type->flagtype;
		for(node=DMap_First(names1);node;node=DMap_Next(names1,node)){
			if( DMap_Find( names2, node->key.pVoid ) ) continue;
			value |= (1<<mapNames->size);
			MAP_Insert( mapNames, node->key.pVoid, 1<<mapNames->size );
		}
		DaoNamespace_AddType( ns, name, type );
	}
	DaoEnum_SetType( self, type );
	DString_Delete( name );
	self->value = value;
	return 1;
}

DaoTypeBase enumTyper=
{
	"enum", & baseCore, NULL, NULL, {0}, {0},
	(FuncPtrDel) DaoEnum_Delete, NULL
};

extern DaoTypeBase funcTyper;
DaoTypeBase* DaoValue_GetTyper( DaoValue *self )
{
	if( self == NULL ) return & baseTyper;
	switch( self->type ){
	case DAO_NULL : return & baseTyper;
	case DAO_INTEGER :
	case DAO_FLOAT   :
	case DAO_DOUBLE  : return & numberTyper;
	case DAO_COMPLEX : return & comTyper;
	case DAO_LONG  : return & longTyper;
	case DAO_ENUM  : return & enumTyper;
	case DAO_STRING  : return & stringTyper;
	case DAO_CTYPE   :
	case DAO_CDATA   : return self->xCdata.typer;
	case DAO_FUNCTION : return & funcTyper;
	default : break;
	}
	return DaoVmSpace_GetTyper( self->type );
}

void DaoValue_GetField( DaoValue *self, DaoContext *ctx, DString *name )
{
	DaoTypeBase *typer = DaoValue_GetTyper( self );
	DaoValue *p = DaoFindValue( typer, name );
	if( p == NULL ){
		DString *mbs = DString_New(1);
		DString_Append( mbs, name );
		DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, DString_GetMBS( mbs ) );
		DString_Delete( mbs );
		return;
	}
	DaoContext_PutValue( ctx, p );
}
void DaoValue_SetField( DaoValue *self, DaoContext *ctx, DString *name, DaoValue *value )
{
}
void DaoValue_SafeGetField( DaoValue *self, DaoContext *ctx, DString *name )
{
	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	DaoValue_GetField( self, ctx, name );
}
void DaoValue_SafeSetField( DaoValue *self, DaoContext *ctx, DString *name, DaoValue *value )
{
	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	DaoValue_SetField( self, ctx, name, value );
}
void DaoValue_GetItem( DaoValue *self, DaoContext *ctx, DaoValue *pid[], int N )
{
	DaoValue *func = DaoFindFunction2( DaoValue_GetTyper( self ), "[]" );
	if( func ) func = (DaoValue*) DRoutine_Resolve( func, self, pid, N, DVM_CALL );
	if( func == NULL ){
		DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, "" );
		return;
	}
	DaoFunction_Call( (DaoFunction*) func, ctx, self, pid, N );
}
void DaoValue_SetItem( DaoValue *self, DaoContext *ctx, DaoValue *pid[], int N, DaoValue *value )
{
	DaoValue *func = DaoFindFunction2( DaoValue_GetTyper( self ), "[]=" );
	DaoValue *p[ DAO_MAX_PARAM ];
	memcpy( p, pid, N*sizeof(DaoValue*) );
	p[N+1] = value;
	if( func ) func = (DaoValue*) DRoutine_Resolve( func, self, p, N+1, DVM_CALL );
	if( func == NULL ){
		DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, "" );
		return;
	}
	DaoFunction_Call( (DaoFunction*) func, ctx, self, p, N+1 );
}

/**/
static void DaoNumber_Print( DaoValue *self, DaoContext *ctx, DaoStream *stream, DMap *cycData )
{
	switch( self->type ){
	case DAO_INTEGER : DaoStream_WriteInt( stream, self->xInteger.value ); break;
	case DAO_FLOAT   : DaoStream_WriteFloat( stream, self->xFloat.value ); break;
	case DAO_DOUBLE  : DaoStream_WriteFloat( stream, self->xDouble.value ); break;
	}
}
static void DaoNumber_GetItem1( DaoValue *self, DaoContext *ctx, DaoValue *pid )
{
	uint_t bits = (uint_t) DaoValue_GetDouble( self );
	size_t size = 8*sizeof(uint_t);
	size_t start, end;
	int idtype;
	DArray *ids = MakeIndex( ctx, pid, size, & start, & end, & idtype );
	dint *res = DaoContext_PutInteger( ctx, 0 );
	switch( idtype ){
	case IDX_NULL : *res = bits; break;
	case IDX_SINGLE : *res = ( bits >> start ) & 0x1; break;
	case IDX_MULTIPLE : DArray_Delete( ids ); /* fall through */
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static void DaoNumber_SetItem1( DaoValue *self, DaoContext *ctx, DaoValue *pid, DaoValue *value )
{
	uint_t bits = (uint_t) DaoValue_GetDouble( self );
	uint_t val = (uint_t) DaoValue_GetDouble( value );
	size_t size = 8*sizeof(uint_t);
	size_t start, end;
	int idtype;
	DArray *ids = MakeIndex( ctx, pid, size, & start, & end, & idtype );
	switch( idtype ){
	case IDX_NULL : bits = val; break;
	case IDX_SINGLE : bits |= ( ( val != 0 ) & 0x1 ) << start; break;
	case IDX_MULTIPLE : DArray_Delete( ids ); /* fall through */
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
	self->xInteger.value = bits;
}
static void DaoNumber_GetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N )
{
	switch( N ){
	case 0 : DaoNumber_GetItem1( self, ctx, NULL ); break;
	case 1 : DaoNumber_GetItem1( self, ctx, ids[0] ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static void DaoNumber_SetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N, DaoValue *value )
{
	switch( N ){
	case 0 : DaoNumber_SetItem1( self, ctx, NULL, value ); break;
	case 1 : DaoNumber_SetItem1( self, ctx, ids[0], value ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}

static DaoTypeCore numberCore=
{
	0, NULL, NULL, NULL, NULL,
	DaoValue_GetField,
	DaoValue_SetField,
	DaoNumber_GetItem,
	DaoNumber_SetItem,
	DaoNumber_Print,
	DaoValue_NoCopy,
};

DaoTypeBase numberTyper=
{
	"double", & numberCore, NULL, NULL, {0}, {0}, DaoValue_Delete, NULL
};

/**/
static void DaoString_Print( DaoValue *self, DaoContext *ctx, DaoStream *stream, DMap *cycData )
{
	if( stream->useQuote ) DaoStream_WriteChar( stream, '\"' );
	DaoStream_WriteString( stream, self->xString.data );
	if( stream->useQuote ) DaoStream_WriteChar( stream, '\"' );
}
static void DaoString_GetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid )
{
	DString *self = self0->xString.data;
	size_t size = DString_Size( self );
	size_t i, start, end;
	int idtype;
	DArray *ids = MakeIndex( ctx, pid, size, & start, & end, & idtype );
	DString *res = NULL;
	if( idtype != IDX_SINGLE ) res = DaoContext_PutMBString( ctx, "" );
	switch( idtype ){
	case IDX_NULL :
		DString_Assign( res, self );
		break;
	case IDX_SINGLE :
		{
			dint *num = DaoContext_PutInteger( ctx, 0 );
			*num = self->mbs ? self->mbs[start] : self->wcs[start];
			break;
		}
	case IDX_FROM :
		DString_SubString( self, res, start, -1 );
		break;
	case IDX_TO :
		DString_SubString( self, res, 0, end+1 );
		break;
	case IDX_PAIR :
		DString_SubString( self, res, start, end-start+1 );
		break;
	case IDX_ALL :
		DString_SubString( self, res, 0, -1 );
		break;
	case IDX_MULTIPLE :
		{
			dint *ip = ids->items.pInt;
			res = DaoContext_PutMBString( ctx, "" );
			DString_Clear( res );
			if( self->mbs ){
				char *data = self->mbs;
				for( i=0; i<ids->size; i++ ) DString_AppendChar( res, data[ ip[i] ] );
			}else{
				wchar_t *data = self->wcs;
				for( i=0; i<ids->size; i++ ) DString_AppendWChar( res, data[ ip[i] ] );
			}
			DArray_Delete( ids );
		}
		break;
	default : break;
	}
}
static void DaoString_SetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid, DaoValue *value )
{
	DString *self = self0->xString.data;
	size_t size = DString_Size( self );
	size_t start, end;
	int idtype;
	DArray *ids = MakeIndex( ctx, pid, size, & start, & end, & idtype );
	if( value->type >= DAO_INTEGER && value->type <= DAO_DOUBLE ){
		int i, id = value->xInteger.value;
		if( idtype == IDX_MULTIPLE ){
			dint *ip = ids->items.pInt;
			if( self->mbs ){
				for(i=0; i<ids->size; i++) self->mbs[ ip[i] ] = id;
			}else{
				for(i=0; i<ids->size; i++) self->wcs[ ip[i] ] = id;
			}
			DArray_Delete( ids );
			return;
		}
		if( self->mbs ){
			for(i=start; i<=end; i++) self->mbs[i] = id;
		}else{
			for(i=start; i<=end; i++) self->wcs[i] = id;
		}
	}else if( value->type == DAO_STRING ){
		DString *str = value->xString.data;
		switch( idtype ){
		case IDX_NULL :
			DString_Assign( self, str );
			break;
		case IDX_SINGLE :
			{
				int ch = str->mbs ? str->mbs[0] : str->wcs[0];
				if( self->mbs )
					self->mbs[start] = ch;
				else
					self->wcs[start] = ch;
				break;
			}
		case IDX_FROM :
			DString_Replace( self, str, start, -1 );
			break;
		case IDX_TO :
			DString_Replace( self, str, 0, end+1 );
			break;
		case IDX_PAIR :
			DString_Replace( self, str, start, end-start+1 );
			break;
		case IDX_ALL :
			DString_Assign( self, str );
			break;
		case IDX_MULTIPLE :
			DArray_Delete( ids );
			DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
		default : break;
		}
	}
}
static void DaoString_GetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N )
{
	switch( N ){
	case 0 : DaoString_GetItem1( self, ctx, NULL ); break;
	case 1 : DaoString_GetItem1( self, ctx, ids[0] ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static void DaoString_SetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N, DaoValue *value )
{
	switch( N ){
	case 0 : DaoString_SetItem1( self, ctx, NULL, value ); break;
	case 1 : DaoString_SetItem1( self, ctx, ids[0], value ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static DaoTypeCore stringCore=
{
	0, NULL, NULL, NULL, NULL,
	DaoValue_GetField,
	DaoValue_SetField,
	DaoString_GetItem,
	DaoString_SetItem,
	DaoString_Print,
	DaoValue_NoCopy,
};

static void DaoSTR_Size( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoContext_PutInteger( ctx, p[0]->xString.data->size );
}
static void DaoSTR_Resize( DaoContext *ctx, DaoValue *p[], int N )
{
	if( ( ctx->vmSpace->options & DAO_EXEC_SAFE ) && p[1]->xInteger.value > 1E5 ){
		DaoContext_RaiseException( ctx, DAO_ERROR,
				"not permitted to create long string in safe running mode" );
		return;
	}
	DString_Resize( p[0]->xString.data, p[1]->xInteger.value );
}

static void DaoSTR_Insert( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *str = p[1]->xString.data;
	size_t at = (size_t)p[2]->xInteger.value;
	size_t rm = (size_t)p[3]->xInteger.value;
	size_t cp = (size_t)p[4]->xInteger.value;
	DString_Insert( self, str, at, rm, cp );
}
static void DaoSTR_Clear( DaoContext *ctx, DaoValue *p[], int N )
{
	DString_Clear( p[0]->xString.data );
}
static void DaoSTR_Erase( DaoContext *ctx, DaoValue *p[], int N )
{
	DString_Erase( p[0]->xString.data, p[1]->xInteger.value, p[2]->xInteger.value );
}
static void DaoSTR_Chop( DaoContext *ctx, DaoValue *p[], int N )
{
	int i, k;
	unsigned char *chs;
	DString *self = p[0]->xString.data;
	DString_Detach( self );
	DString_Chop( self );

	if( DString_CheckUTF8( self ) && self->mbs && self->size ){
		chs = (unsigned char*) self->mbs;
		i = self->size - 1;
		k = utf8_markers[ chs[i] ];
		if( k ==1 ){
			while( utf8_markers[ chs[i] ] ==1 ) i --;
			k = utf8_markers[ chs[i] ];
			if( k == 0 ){
				chs[k+1] = 0;
				self->size = k+1;
			}else if( (self->size - i) != k ){
				if( (self->size - i) < k ){
					chs[i] = 0;
					self->size = i;
				}else{
					chs[i+k] = 0;
					self->size = i + k;
				}
			}
		}else if( k !=0 ){
			chs[i] = 0;
			self->size --;
		}
	}
	DaoContext_PutReference( ctx, p[0] );
}
static void DaoSTR_Trim( DaoContext *ctx, DaoValue *p[], int N )
{
	DString_Trim( p[0]->xString.data );
	DaoContext_PutReference( ctx, p[0] );
}
static void DaoSTR_Find( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *str = p[1]->xString.data;
	size_t from = (size_t)p[2]->xInteger.value;
	dint pos = -1000;
	if( p[3]->xInteger.value ){
		pos = DString_RFind( self, str, from );
		if( pos == MAXSIZE ) pos = -1000;
	}else{
		pos = DString_Find( self, str, from );
		if( pos == MAXSIZE ) pos = -1000;
	}
	DaoContext_PutInteger( ctx, pos );
}
static void DaoSTR_Replace( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *str1 = p[1]->xString.data;
	DString *str2 = p[2]->xString.data;
	size_t index = (size_t)p[3]->xInteger.value;
	size_t pos, from = 0, count = 0;
	if( self->mbs ){
		DString_ToMBS( str1 );
		DString_ToMBS( str2 );
	}else{
		DString_ToWCS( str1 );
		DString_ToWCS( str2 );
	}
	if( index == 0 ){
		pos = DString_Find( self, str1, from );
		while( pos != MAXSIZE ){
			count ++;
			DString_Insert( self, str2, pos, DString_Size( str1 ), 0 );
			from = pos + DString_Size( str2 );
			pos = DString_Find( self, str1, from );
		}
	}else if( index > 0){
		pos = DString_Find( self, str1, from );
		while( pos != MAXSIZE ){
			count ++;
			if( count == index ){
				DString_Insert( self, str2, pos, DString_Size( str1 ), 0 );
				break;
			}
			from = pos + DString_Size( str1 );
			pos = DString_Find( self, str1, from );
		}
		count = 1;
	}else{
		from = MAXSIZE;
		pos = DString_RFind( self, str1, from );
		while( pos != MAXSIZE ){
			count --;
			if( count == index ){
				DString_Insert( self, str2, pos-DString_Size( str1 )+1, DString_Size( str1 ), 0 );
				break;
			}
			from = pos - DString_Size( str1 );
			pos = DString_RFind( self, str1, from );
		}
		count = 1;
	}
	DaoContext_PutInteger( ctx, count );
}
static void DaoSTR_Replace2( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *res, *key;
	DString *self = p[0]->xString.data;
	DMap *par = p[1]->xMap.items;
	DMap *words = DMap_New(D_STRING,D_STRING);
	DMap *sizemap = DMap_New(0,0);
	DNode *node = DMap_First( par );
	DArray *sizes = DArray_New(0);
	int max = p[2]->xInteger.value;
	int i, j, k, n;
	for( ; node != NULL; node = DMap_Next(par, node) )
		DMap_Insert( words, node->key.pValue->xString.data, node->value.pValue->xString.data );
	if( self->mbs ){
		key = DString_New(1);
		res = DString_New(1);
		for(node=DMap_First(words); node !=NULL; node = DMap_Next(words, node) ){
			DString_ToMBS( node->key.pString );
			DString_ToMBS( node->value.pString );
			MAP_Insert( sizemap, node->key.pString->size, 0 );
		}
		for(node=DMap_First(sizemap); node !=NULL; node = DMap_Next(sizemap, node) )
			DArray_Append( sizes, node->key.pInt );
		i = 0;
		n = self->size;
		while( i < n ){
			DString *val = NULL;
			for(j=0; j<sizes->size; j++){
				k = sizes->items.pInt[j];
				if( i+k > n ) break;
				DString_SubString( self, key, i, k );
				node = DMap_FindGE( words, key );
				if( node == NULL ) break;
				if( DString_EQ( node->key.pString, key ) ){
					val = node->value.pString;
					if( max ==0 ) break;
				}
			}
			if( val ){
				DString_Append( res, val );
				i += key->size;
			}else{
				DString_AppendChar( res, self->mbs[i] );
				i ++;
			}
		}
	}else{
		key = DString_New(0);
		res = DString_New(0);
		for(node=DMap_First(words); node !=NULL; node = DMap_Next(words, node) ){
			DString_ToWCS( node->key.pString );
			DString_ToWCS( node->value.pString );
			MAP_Insert( sizemap, node->key.pString->size, 0 );
		}
		for(node=DMap_First(sizemap); node !=NULL; node = DMap_Next(sizemap, node) )
			DArray_Append( sizes, node->key.pInt );
		i = 0;
		n = self->size;
		while( i < n ){
			DString *val = NULL;
			for(j=0; j<sizes->size; j++){
				k = sizes->items.pInt[j];
				if( i+k > n ) break;
				DString_SubString( self, key, i, k );
				node = DMap_FindGE( words, key );
				if( node == NULL ) break;
				if( DString_EQ( node->key.pString, key ) ){
					val = node->value.pString;
					if( max ==0 ) break;
				}
			}
			if( val ){
				DString_Append( res, val );
				i += key->size;
			}else{
				DString_AppendWChar( res, self->wcs[i] );
				i ++;
			}
		}
	}
	DString_Assign( self, res );
	DString_Delete( key );
	DString_Delete( res );
	DArray_Delete( sizes );
	DMap_Delete( words );
	DMap_Delete( sizemap );
}
static void DaoSTR_Expand( DaoContext *ctx, DaoValue *p[], int N )
{
	DMap    *keys = NULL;
	DaoString *key = NULL;
	DaoTuple *tup = & p[1]->xTuple;
	DString *self = p[0]->xString.data;
	DString *spec = p[2]->xString.data;
	DString *res = NULL, *val = NULL, *sub = NULL;
	DNode *node = NULL;
	int keep = p[3]->xInteger.value;
	size_t i, pos1, pos2, prev = 0;
	wchar_t spec1;
	char spec2;
	int replace;
	int ch;
	if( DString_Size( spec ) ==0 ){
		DaoContext_PutString( ctx, self );
		return;
	}
	if(  p[1]->type == DAO_TUPLE ){
		if( tup->unitype ){
			keys = tup->unitype->mapNames;
		}else{
			DaoContext_RaiseException( ctx, DAO_ERROR_PARAM, "invalid tuple" );
			return;
		}
	}else{
		tup = NULL;
		keys = p[1]->xMap.items;
	}
	if( self->mbs && spec->wcs ) DString_ToMBS( spec );
	if( self->wcs && spec->mbs ) DString_ToWCS( spec );
	if( self->mbs ){
		res = DaoContext_PutMBString( ctx, "" );
		key = DaoString_New(1);
		sub = DString_New(1);
		spec2 = spec->mbs[0];
		pos1 = DString_FindChar( self, spec2, prev );
		while( pos1 != MAXSIZE ){
			pos2 = DString_FindChar( self, ')', pos1 );
			replace = 0;
			if( pos2 != MAXSIZE && self->mbs[pos1+1] == '(' ){
				replace = 1;
				for(i=pos1+2; i<pos2; i++){
					ch = self->mbs[i];
					if( ch != '_' && ! isalnum( ch ) ){
						replace = 0;
						break;
					}
				}
				if( replace ){
					DString_SubString( self, key->data, pos1+2, pos2-pos1-2 );
					if( tup ){
						node = DMap_Find( keys, key->data );
					}else{
						node = DMap_Find( keys, key );
					}
					if( node ){
						if( tup ){
							i = node->value.pInt;
							DaoValue_GetString( tup->items[i], key->data );
							val = key->data;
						}else{
							val = node->value.pValue->xString.data;
						}
					}else if( keep ){
						replace = 0;
					}else{
						DString_Clear( key->data );
						val = key->data;
					}
				}
			}
			DString_SubString( self, sub, prev, pos1 - prev );
			DString_Append( res, sub );
			prev = pos1 + 1;
			if( replace ){
				DString_Append( res, val );
				prev = pos2 + 1;
			}else{
				DString_AppendChar( res, spec2 );
			}
			pos1 = DString_FindChar( self, spec2, prev );
		}
	}else{
		res = DaoContext_PutWCString( ctx, L"" );
		key = DaoString_New(0);
		sub = DString_New(0);
		spec1 = spec->wcs[0];
		pos1 = DString_FindWChar( self, spec1, prev );
		while( pos1 != MAXSIZE ){
			pos2 = DString_FindWChar( self, L')', pos1 );
			replace = 0;
			if( pos2 != MAXSIZE && self->wcs[pos1+1] == L'(' ){
				replace = 1;
				for(i=pos1+2; i<pos2; i++){
					ch = self->wcs[i];
					if( ch != L'_' && ! isalnum( ch ) ){
						replace = 0;
						break;
					}
				}
				if( replace ){
					DString_SubString( self, key->data, pos1+2, pos2-pos1-2 );
					if( tup ){
						node = DMap_Find( keys, key->data );
					}else{
						node = DMap_Find( keys, key );
					}
					if( node ){
						if( tup ){
							i = node->value.pInt;
							DaoValue_GetString( tup->items[i], key->data );
							val = key->data;
						}else{
							val = node->value.pValue->xString.data;
						}
					}else if( keep ){
						replace = 0;
					}else{
						DString_Clear( key->data );
						val = key->data;
					}
				}
			}
			DString_SubString( self, sub, prev, pos1 - prev );
			DString_Append( res, sub );
			prev = pos1 + 1;
			if( replace ){
				DString_Append( res, val );
				prev = pos2 + 1;
			}else{
				DString_AppendWChar( res, spec1 );
			}
			pos1 = DString_FindWChar( self, spec1, prev );
		}
	}
	DString_SubString( self, sub, prev, DString_Size( self ) - prev );
	DString_Append( res, sub );
	DString_Delete( sub );
	DaoString_Delete( key );
}
static void DaoSTR_Split( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *delm = p[1]->xString.data;
	DString *quote = p[2]->xString.data;
	int rm = (int)p[3]->xInteger.value;
	DaoList *list = DaoContext_PutList( ctx );
	DaoValue *value = (DaoValue*) DaoString_New(1);
	DString *str = value->xString.data;
	size_t dlen = DString_Size( delm );
	size_t qlen = DString_Size( quote );
	size_t size = DString_Size( self );
	size_t last = 0;
	size_t posDelm = DString_Find( self, delm, last );
	size_t posQuote = DString_Find( self, quote, last );
	size_t posQuote2 = -1;
	if( N ==1 || DString_Size( delm ) ==0 ){
		size_t i = 0;
		if( self->mbs ){
			unsigned char *mbs = (unsigned char*) self->mbs;
			size_t j, k;
			while( i < size ){
				k = utf8_markers[ mbs[i] ];
				if( k ==0 || k ==7 ){
					DString_SetDataMBS( str, (char*)mbs + i, 1 );
					DArray_Append( list->items, value );
					i ++;
				}else if( k ==1 ){
					k = i;
					while( i < size && utf8_markers[ mbs[i] ] ==1 ) i ++;
					DString_SetDataMBS( str, (char*)mbs + k, i-k );
					DArray_Append( list->items, value );
				}else{
					for( j=1; j<k; j++ ){
						if( i + j >= size ) break;
						if( utf8_markers[ mbs[i+j] ] != 1 ) break;
					}
					DString_SetDataMBS( str, (char*)mbs + i, j );
					DArray_Append( list->items, value );
					i += j;
				}
			}
		}else{
			wchar_t *wcs = self->wcs;
			DString_ToWCS( str );
			DString_Resize( str, 1 );
			for(i=0; i<size; i++){
				DString_Detach( str );
				str->wcs[0] = wcs[i];
				DArray_Append( list->items, value );
			}
		}
		DaoString_Delete( (DaoString*) value );
		return;
	}
	if( posDelm != MAXSIZE && posQuote != MAXSIZE && posQuote < posDelm ){
		posQuote2 = DString_Find( self, quote, posQuote+qlen );
		if( posQuote2 != MAXSIZE && posQuote2 > posDelm )
			posDelm = DString_Find( self, delm, posQuote2 );
	}
	while( posDelm != MAXSIZE ){
		if( rm && posQuote == last && posQuote2 == posDelm-qlen )
			DString_SubString( self, str, last+qlen, posDelm-last-2*qlen );
		else
			DString_SubString( self, str, last, posDelm-last );
		/* if( last !=0 || posDelm !=0 ) */
		DArray_Append( list->items, value );

		last = posDelm + dlen;
		posDelm = DString_Find( self, delm, last );
		posQuote = DString_Find( self, quote, last );
		posQuote2 = -1;
		if( posDelm != MAXSIZE && posQuote != MAXSIZE && posQuote < posDelm ){
			posQuote2 = DString_Find( self, quote, posQuote+qlen );
			if( posQuote2 != MAXSIZE && posQuote2 > posDelm )
				posDelm = DString_Find( self, delm, posQuote2 );
		}
	}
	if( posQuote != MAXSIZE && posQuote < size )
		posQuote2 = DString_Find( self, quote, posQuote+qlen );
	if( rm && posQuote == last && posQuote2 == size-qlen )
		DString_SubString( self, str, last+qlen, size-last-2*qlen );
	else
		DString_SubString( self, str, last, size-last );
	DArray_Append( list->items, value );
	DaoString_Delete( (DaoString*) value );
}
static void DaoSTR_Tokenize( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *delms = p[1]->xString.data;
	DString *quotes = p[2]->xString.data;
	int bkslash = (int)p[3]->xInteger.value;
	int simplify = (int)p[4]->xInteger.value;
	DaoList *list = DaoContext_PutList( ctx );
	DaoValue *value = (DaoValue*) DaoString_New(1);
	DString *str = value->xString.data;
	if( self->mbs ){
		char *s = self->mbs;
		DString_ToMBS( str );
		DString_ToMBS( delms );
		DString_ToMBS( quotes );
		while( *s ){
			if( bkslash && *s == '\\' ){
				DString_AppendChar( str, *s );
				DString_AppendChar( str, *(s+1) );
				s += 2;
				continue;
			}
			if( ( bkslash == 0 || s == self->mbs || *(s-1) !='\\' )
					&& DString_FindChar( quotes, *s, 0 ) != MAXSIZE ){
				DString_AppendChar( str, *s );
				s ++;
				while( *s ){
					if( bkslash && *s == '\\' ){
						DString_AppendChar( str, *s );
						DString_AppendChar( str, *(s+1) );
						s += 2;
					}
					if( ( bkslash == 0 || *(s-1) !='\\' )
							&& DString_FindChar( quotes, *s, 0 ) != MAXSIZE )
						break;
					DString_AppendChar( str, *s );
					s ++;
				}
				DString_AppendChar( str, *s );
				s ++;
				continue;
			}
			if( DString_FindChar( delms, *s, 0 ) != MAXSIZE ){
				if( s != self->mbs && *(s-1)=='\\' ){
					DString_AppendChar( str, *s );
					s ++;
					continue;
				}else{
					if( simplify ) DString_Trim( str );
					if( str->size > 0 ){
						DArray_Append( list->items, value );
						DString_Clear( str );
					}
					DString_AppendChar( str, *s );
					s ++;
					if( simplify ) DString_Trim( str );
					if( str->size > 0 ) DArray_Append( list->items, value );
					DString_Clear( str );
					continue;
				}
			}
			DString_AppendChar( str, *s );
			s ++;
		}
		if( simplify ) DString_Trim( str );
		if( str->size > 0 ) DArray_Append( list->items, value );
	}else{
		wchar_t *s = self->wcs;
		DString_ToWCS( str );
		DString_ToWCS( delms );
		DString_ToWCS( quotes );
		while( *s ){
			if( ( s == self->wcs || bkslash ==0 || *(s-1)!=L'\\' )
					&& DString_FindWChar( quotes, *s, 0 ) != MAXSIZE ){
				DString_AppendChar( str, *s );
				s ++;
				while( *s ){
					if( ( bkslash ==0 || *(s-1)!=L'\\' )
							&& DString_FindWChar( quotes, *s, 0 ) != MAXSIZE ) break;
					DString_AppendChar( str, *s );
					s ++;
				}
				DString_AppendChar( str, *s );
				s ++;
				continue;
			}
			if( DString_FindWChar( delms, *s, 0 ) != MAXSIZE ){
				if( s != self->wcs && ( bkslash && *(s-1)==L'\\' ) ){
					DString_AppendChar( str, *s );
					s ++;
					continue;
				}else{
					if( simplify ) DString_Trim( str );
					if( str->size > 0 ){
						DArray_Append( list->items, value );
						DString_Clear( str );
					}
					DString_AppendChar( str, *s );
					s ++;
					if( simplify ) DString_Trim( str );
					if( str->size > 0 ) DArray_Append( list->items, value );
					DString_Clear( str );
					continue;
				}
			}
			DString_AppendChar( str, *s );
			s ++;
		}
		if( simplify ) DString_Trim( str );
		if( str->size > 0 ) DArray_Append( list->items, value );
	}
	DaoString_Delete( (DaoString*) value );
}
extern int ConvertStringToNumber( DaoContext *ctx, DaoValue *dA, DaoValue **dC );
static void DaoSTR_Tonumber( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoValue *value = NULL;
	double *num = DaoContext_PutDouble( ctx, 0.0 );
	ConvertStringToNumber( ctx, p[0], & value );
	if( value ) *num = DaoValue_GetDouble( value );
}
static void DaoSTR_Tolower( DaoContext *ctx, DaoValue *p[], int N )
{
	DString_ToLower( p[0]->xString.data );
	DaoContext_PutReference( ctx, p[0] );
}
static void DaoSTR_Toupper( DaoContext *ctx, DaoValue *p[], int N )
{
	DString_ToUpper( p[0]->xString.data );
	DaoContext_PutReference( ctx, p[0] );
}
static void DaoSTR_PFind( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *pt = p[1]->xString.data;
	size_t index = p[2]->xInteger.value;
	size_t start = (size_t)p[3]->xInteger.value;
	size_t end = (size_t)p[4]->xInteger.value;
	size_t i, p1=start, p2=end;
	DaoTuple *tuple = NULL;
	DaoList *list = DaoContext_PutList( ctx );
	DaoType *itp = list->unitype->nested->items.pType[0];
	DaoRegex *patt = DaoProcess_MakeRegex( ctx->process, pt, self->wcs ==NULL );
	if( patt ==NULL ) return;
	if( end == 0 ) p2 = end = DString_Size( self );
	i = 0;
	while( DaoRegex_Match( patt, self, & p1, & p2 ) ){
		if( index ==0 || (++i) == index ){
			tuple = DaoTuple_New( 2 );
			GC_IncRC( itp );
			tuple->unitype = itp;
			tuple->items[0] = DaoValue_NewInteger( p1 );
			tuple->items[1] = DaoValue_NewInteger( p2 );
			DArray_Append( list->items, tuple );
			if( index ) break;
		}
		p1 = p2 + 1;
		p2 = end;
	}
}
static void DaoSTR_Match0( DaoContext *ctx, DaoValue *p[], int N, int subm )
{
	DString *self = p[0]->xString.data;
	DString *pt = p[1]->xString.data;
	size_t start = (size_t)p[2]->xInteger.value;
	size_t end = (size_t)p[3]->xInteger.value;
	size_t p1=start, p2=end;
	int capt = p[4]->xInteger.value;
	int gid = p[2]->xInteger.value;
	DaoTuple *tuple = DaoContext_PutTuple( ctx );
	DaoRegex *patt = DaoProcess_MakeRegex( ctx->process, pt, self->wcs ==NULL );
	if( patt ==NULL ) return;
	if( subm ){
		end = capt;
		p1 = start = end;
	}
	if( end == 0 ) p2 = end = DString_Size( self );
	pt = DString_Copy( pt );
	DString_Clear( pt );
	if( DaoRegex_Match( patt, self, & p1, & p2 ) ){
		if( subm && DaoRegex_SubMatch( patt, gid, & p1, & p2 ) ==0 ) p1 = -1;
	}else{
		p1 = -1;
	}
	tuple->items[0]->xInteger.value = p1;
	tuple->items[1]->xInteger.value = p2;
	if( p1 >=0 && ( subm || capt ) ) DString_SubString( self, pt, p1, p2-p1+1 );
	DString_Assign( tuple->items[2]->xString.data, pt );
	DString_Delete( pt );
}
static void DaoSTR_Match( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoSTR_Match0( ctx, p, N, 0 );
}
static void DaoSTR_SubMatch( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoSTR_Match0( ctx, p, N, 1 );
}
static void DaoSTR_Extract( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *pt = p[1]->xString.data;
	DString *mask = p[3]->xString.data;
	int i, from, to, step, type = p[2]->xInteger.value;
	int rev = p[4]->xInteger.value;
	size_t size = DString_Size( self );
	size_t end=size, p1=0, p2=size;
	DaoString *subs = DaoString_New(1);
	DArray *masks = DArray_New(0);
	DArray *matchs = DArray_New(0);
	DaoList *list = DaoContext_PutList( ctx );
	DaoRegex *patt = DaoProcess_MakeRegex( ctx->process, pt, self->wcs ==NULL );
	DaoRegex *ptmask = NULL;
	DString_Delete( subs->data );
	pt = subs->data = DString_Copy( pt );
	if( size == 0 ) goto DoNothing;
	if( DString_Size( mask ) ==0 ) mask = NULL;
	if( mask ){
		ptmask = DaoProcess_MakeRegex( ctx->process, mask, self->wcs ==NULL );
		if( ptmask ==NULL ) goto DoNothing;
	}
	if( patt ==NULL ) goto DoNothing;
	if( mask == NULL || rev ) DArray_Append( masks, 0 );
	if( mask ){
		while( DaoRegex_Match( ptmask, self, & p1, & p2 ) ){
			DArray_Append( masks, p1 );
			DArray_Append( masks, p2 + 1 );
			p1 = p2 + 1;  p2 = size;
		}
	}
	if( mask == NULL || rev ) DArray_Append( masks, size );
	DArray_Append( matchs, 0 );
	for(i=0; i<masks->size; i+=2){
		p1 = masks->items.pInt[i];
		p2 = end = masks->items.pInt[i+1] - 1;
		while( DaoRegex_Match( patt, self, & p1, & p2 ) ){
			DArray_Append( matchs, p1 );
			DArray_Append( matchs, p2 + 1 );
			p1 = p2 + 1;  p2 = end;
		}
	}
	DArray_Append( matchs, size );
	step = 2;
	from = 0;
	to = matchs->size -1;
	if( type > 0 ){
		from = 1;
	}else if( type < 0 ){
		to = matchs->size;
	}else{
		step = 1;
	}
	for(i=from; i<to; i+=step){
		p1 = matchs->items.pInt[i];
		p2 = matchs->items.pInt[i+1];
		/*
		   printf( "p1 = %i, p2 = %i\n", p1, p2 );
		 */
		if( (p1 >0 && p1 <size) || p2 > p1 ){
			DString_SubString( self, pt, p1, p2-p1 );
			DArray_Append( list->items, (DaoValue*) subs );
		}
	}
DoNothing:
	DaoString_Delete( subs );
	DArray_Delete( masks );
	DArray_Delete( matchs );
}
static void DaoSTR_Capture( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *pt = p[1]->xString.data;
	size_t start = (size_t)p[2]->xInteger.value;
	size_t end = (size_t)p[3]->xInteger.value;
	size_t p1=start, p2=end;
	int gid;
	DaoString *subs = DaoString_New(1);
	DaoList *list = DaoContext_PutList( ctx );
	DaoRegex *patt = DaoProcess_MakeRegex( ctx->process, pt, self->wcs ==NULL );
	if( patt ==NULL ) return;
	if( end == 0 ) p2 = end = DString_Size( self );
	if( DaoRegex_Match( patt, self, & p1, & p2 ) ==0 ) return;
	DString_Delete( subs->data );
	pt = subs->data = DString_Copy( pt );
	for( gid=0; gid<=patt->group; gid++ ){
		DString_Clear( pt );
		if( DaoRegex_SubMatch( patt, gid, & p1, & p2 ) ){
			DString_SubString( self, pt, p1, p2-p1+1 );
		}
		DArray_Append( list->items, (DaoValue*) subs );
	}
	DString_Delete( pt );
}
static void DaoSTR_Change( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *pt = p[1]->xString.data;
	DString *str = p[2]->xString.data;
	size_t start = (size_t)p[4]->xInteger.value;
	size_t end = (size_t)p[5]->xInteger.value;
	dint n, index = p[3]->xInteger.value;
	DaoRegex *patt = DaoProcess_MakeRegex( ctx->process, pt, self->wcs ==NULL );
	size_t size = self->size;
	n = DaoRegex_ChangeExt( patt, self, str, index, & start, & end );
	DaoContext_PutInteger( ctx, n );
}
static void DaoSTR_Mpack( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString *pt = p[1]->xString.data;
	DString *str = p[2]->xString.data;
	dint index = p[3]->xInteger.value;
	DaoRegex *patt = DaoProcess_MakeRegex( ctx->process, pt, self->wcs ==NULL );
	if( N == 5 ){
		DaoList *res = DaoContext_PutList( ctx );
		dint count = p[4]->xInteger.value;
		DaoRegex_MatchAndPack( patt, self, str, index, count, res->items );
	}else{
		DArray *packs = DArray_New(D_VALUE);
		DaoRegex_MatchAndPack( patt, self, str, index, 1, packs );
		if( packs->size ){
			DaoContext_PutValue( ctx, packs->items.pValue[0] );
		}else{
			DaoContext_PutMBString( ctx, "" );
		}
		DArray_Delete( packs );
	}
}
static const char *errmsg[2] =
{
	"invalid key",
	"invalid source"
};
static void DaoSTR_Encrypt( DaoContext *ctx, DaoValue *p[], int N )
{
	int rc = DString_Encrypt( p[0]->xString.data, p[1]->xString.data, p[2]->xEnum.value );
	if( rc ) DaoContext_RaiseException( ctx, DAO_ERROR, errmsg[rc-1] );
	DaoContext_PutReference( ctx, p[0] );
}
static void DaoSTR_Decrypt( DaoContext *ctx, DaoValue *p[], int N )
{
	int rc = DString_Decrypt( p[0]->xString.data, p[1]->xString.data, p[2]->xEnum.value );
	if( rc ) DaoContext_RaiseException( ctx, DAO_ERROR, errmsg[rc-1] );
	DaoContext_PutReference( ctx, p[0] );
}
static void DaoSTR_Iter( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DaoTuple *tuple = & p[1]->xTuple;
	DaoValue **data = tuple->items;
	DaoValue *iter = DaoValue_NewInteger(0);
	data[0]->xInteger.value = self->size >0;
	DaoValue_Copy( iter, & data[1] );
	GC_DecRC( iter );
}

static void DaoSTR_Type( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoContext_PutEnum( ctx, ( p[0]->xString.data->mbs != NULL )? "mbs" : "wcs" );
}

static void DaoSTR_Convert( DaoContext *ctx, DaoValue *p[], int N )
{
	if( p[1]->xEnum.value == 0 )
		DString_ToMBS( p[0]->xString.data );
	else
		DString_ToWCS( p[0]->xString.data );
	DaoContext_PutReference( ctx, p[0] );
}
static void DaoSTR_Reverse( DaoContext *ctx, DaoValue *p[], int N )
{
	DString *self = p[0]->xString.data;
	DString_Reverse( self );
	DaoContext_PutReference( ctx, p[0] );
}

static DaoFuncItem stringMeths[] =
{
	{ DaoSTR_Size,    "size( self :string )=>int" },
	{ DaoSTR_Resize,  "resize( self :string, size :int )" },
	{ DaoSTR_Type,    "type( self :string )=>enum<mbs, wcs>" },
	{ DaoSTR_Convert, "convert( self :string, to :enum<mbs, wcs> ) =>string" },
	{ DaoSTR_Insert,  "insert( self :string, str :string, index=0, remove=0, copy=0 )" },
	{ DaoSTR_Clear,   "clear( self :string )" },
	{ DaoSTR_Erase,   "erase( self :string, start=0, n=-1 )" },
	{ DaoSTR_Chop,    "chop( self :string ) =>string" },
	{ DaoSTR_Trim,    "trim( self :string ) =>string" },
	/* return -1, if not found. */
	{ DaoSTR_Find,    "find( self :string, str :string, from=0, reverse=0 )=>int" },
	/* replace index-th occurrence: =0: replace all; >0: from begin; <0: from end. */
	/* return int of occurrence replaced. */
	{ DaoSTR_Replace, "replace( self :string, str1 :string, str2 :string, index=0 )=>int" },
	{ DaoSTR_Replace2, "replace( self :string, table : map<string,string>, max=0 )" },
	{ DaoSTR_Expand,  "expand( self :string, keys :map<string,string>, spec='$', keep=1 )=>string" },
	{ DaoSTR_Expand,  "expand( self :string, keys : tuple, spec='$', keep=1 )=>string" },
	{ DaoSTR_Split, "split( self :string, sep='', quote='', rm=1 )=>list<string>" },
	{ DaoSTR_Tokenize, "tokenize( self :string, seps :string, quotes='', backslash=0, simplify=0 )=>list<string>" },
	{ DaoSTR_PFind, "pfind( self :string, pt :string, index=0, start=0, end=0 )=>list<tuple<start:int,end:int>>" },
	{ DaoSTR_Match, "match( self :string, pt :string, start=0, end=0, substring=1 )=>tuple<start:int,end:int,substring:string>" },
	{ DaoSTR_SubMatch, "submatch( self :string, pt :string, group:int, start=0, end=0 )=>tuple<start:int,end:int,substring:string>" },
	{ DaoSTR_Extract, "extract( self :string, pt :string, matched=1, mask='', rev=0 )=>list<string>" },
	{ DaoSTR_Capture, "capture( self :string, pt :string, start=0, end=0 )=>list<string>" },
	{ DaoSTR_Change,  "change( self :string, pt :string, s:string, index=0, start=0, end=0 )=>int" },
	{ DaoSTR_Mpack,  "mpack( self :string, pt :string, s:string, index=0 )=>string" },
	{ DaoSTR_Mpack,  "mpack( self :string, pt :string, s:string, index : int, count : int )=>list<string>" },
	{ DaoSTR_Tonumber, "tonumber( self :string, base=10 )=>double" },
	{ DaoSTR_Tolower, "tolower( self :string ) =>string" },
	{ DaoSTR_Toupper, "toupper( self :string ) =>string" },
	{ DaoSTR_Reverse, "reverse( self :string ) =>string" },
	{ DaoSTR_Encrypt, "encrypt( self :string, key :string, format :enum<regular, hex> = $regular )=>string" },
	{ DaoSTR_Decrypt, "decrypt( self :string, key :string, format :enum<regular, hex> = $regular )=>string" },
	{ DaoSTR_Iter, "__for_iterator__( self :string, iter : for_iterator )" },
	{ NULL, NULL }
};

DaoTypeBase stringTyper=
{
	"string", & stringCore, NULL, (DaoFuncItem*) stringMeths, {0}, {0}, 
	(FuncPtrDel) DaoString_Delete, NULL
};

/* also used for printing tuples */
static void DaoListCore_Print( DaoValue *self0, DaoContext *ctx, DaoStream *stream, DMap *cycData )
{
	DaoList *self = & self0->xList;
	DaoValue **data = NULL;
	DNode *node = NULL;
	size_t i, size = 0;
	char *lb = "{ ";
	char *rb = " }";
	if( self->type == DAO_TUPLE ){
		data = self0->xTuple.items;
		size = self0->xTuple.size;
		lb = "( ";
		rb = " )";
	}else{
		data = self->items->items.pValue;
		size = self->items->size;
	}

	if( cycData ) node = MAP_Find( cycData, self );
	if( node ){
		DaoStream_WriteMBS( stream, lb );
		DaoStream_WriteMBS( stream, "..." );
		DaoStream_WriteMBS( stream, rb );
		return;
	}
	if( cycData ) MAP_Insert( cycData, self, self );
	DaoStream_WriteMBS( stream, lb );

	for( i=0; i<size; i++ ){
		stream->useQuote = 1;
		DaoValue_Print( data[i], ctx, stream, cycData );
		stream->useQuote = 0;
		if( i != size-1 ) DaoStream_WriteMBS( stream, ", " );
	}
	DaoStream_WriteMBS( stream, rb );
	if( cycData ) MAP_Erase( cycData, self );
}
static void DaoListCore_GetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid )
{
	DaoList *res, *self = & self0->xList;
	const size_t size = self->items->size;
	size_t i, start, end;
	int idtype, e = ctx->process->exceptions->size;
	DArray *ids = MakeIndex( ctx, pid, size, & start, & end, & idtype );
	if( ctx->process->exceptions->size > e ) return;

	switch( idtype ){
	case IDX_NULL :
		res = DaoList_Copy( self, NULL );
		DaoContext_PutValue( ctx, (DaoValue*) res );
		break;
	case IDX_SINGLE :
		/* scalar duplicated in DaoMoveAC() */
		if( start >=0 && start <size )
			DaoContext_PutReference( ctx, self->items->items.pValue[start] );
		else DaoContext_RaiseException( ctx, DAO_ERROR_INDEX_OUTOFRANGE, "" );
		break;
	case IDX_FROM :
		res = DaoContext_PutList( ctx );
		if( start >= self->items->size ) break;
		DArray_Resize( res->items, self->items->size - start, NULL );
		for(i=start; i<self->items->size; i++)
			DaoList_SetItem( res, self->items->items.pValue[i], i-start );
		break;
	case IDX_TO :
		res = DaoContext_PutList( ctx );
		if( end + 1 <0 ) break;
		if( end + 1 >= self->items->size ) end = self->items->size-1;
		DArray_Resize( res->items, end +1, NULL );
		for(i=0; i<=end; i++) DaoList_SetItem( res, self->items->items.pValue[i], i );
		break;
	case IDX_PAIR :
		res = DaoContext_PutList( ctx );
		if( end < start ) break;
		if( end + 1 >= self->items->size ) end = self->items->size-1;
		DArray_Resize( res->items, end - start + 1, NULL );
		for(i=start; i<=end; i++) DaoList_SetItem( res, self->items->items.pValue[i], i-start );
		break;
	case IDX_ALL :
		res = DaoList_Copy( self, NULL );
		DaoContext_PutValue( ctx, (DaoValue*) res );
		break;
	case IDX_MULTIPLE :
		res = DaoContext_PutList( ctx );
		DArray_Resize( res->items, ids->size, NULL );
		for(i=0; i<ids->size; i++ )
			DaoList_SetItem( res, self->items->items.pValue[ ids->items.pInt[i] ], i );
		DArray_Delete( ids );
		break;
	default : break;
	}
}
static void DaoListCore_SetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid, DaoValue *value )
{
	DaoList *self = & self0->xList;
	const size_t size = self->items->size;
	size_t i, start, end;
	int idtype, rc = 0;
	DArray *ids = MakeIndex( ctx, pid, size, & start, & end, & idtype );
	if( self->unitype == NULL ){
		/* a : tuple<string,list<int>> = ('',{});
		   duplicating the constant to assign to a may not set the unitype properly */
		self->unitype = ctx->regTypes[ ctx->vmc->c ];
		GC_IncRC( self->unitype );
	}
	switch( idtype ){
	case IDX_NULL :
		for( i=0; i<size; i++ ) rc |= DaoList_SetItem( self, value, i );
		break;
	case IDX_SINGLE :
		if( start >=0 && start <size ) rc = DaoList_SetItem( self, value, start );
		else DaoContext_RaiseException( ctx, DAO_ERROR_INDEX_OUTOFRANGE, "" );
		break;
	case IDX_FROM :
		for( i=start; i<self->items->size; i++ ) rc |= DaoList_SetItem( self, value, i );
		break;
	case IDX_TO :
		for( i=0; i<=end; i++ ) rc |= DaoList_SetItem( self, value, i );
		break;
	case IDX_PAIR :
		for( i=start; i<=end; i++ ) rc |= DaoList_SetItem( self, value, i );
		break;
	case IDX_ALL :
		for( i=0; i<self->items->size; i++ ) rc |= DaoList_SetItem( self, value, i );
		break;
	case IDX_MULTIPLE :
		DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
		DArray_Delete( ids );
		break;
	default : break;
	}
	if( rc ) DaoContext_RaiseException( ctx, DAO_ERROR_VALUE, "value type" );
}
static void DaoListCore_GetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N )
{
	switch( N ){
	case 0 : DaoListCore_GetItem1( self, ctx, NULL ); break;
	case 1 : DaoListCore_GetItem1( self, ctx, ids[0] ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static void DaoListCore_SetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N, DaoValue *value )
{
	switch( N ){
	case 0 : DaoListCore_SetItem1( self, ctx, NULL, value ); break;
	case 1 : DaoListCore_SetItem1( self, ctx, ids[0], value ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
void DaoCopyValues( DaoValue **copy, DaoValue **data, int N, DaoContext *ctx, DMap *cycData )
{
	int i;
	for(i=0; i<N; i++){
		if( data[i]->type <= DAO_ENUM ){
			DaoValue_Move( data[i], copy+i, NULL );
		}else{
			DaoValue *src = data[i];
			if( cycData ){
				/* deep copy */
				DaoTypeBase *typer = DaoValue_GetTyper( data[i] );
				src = typer->priv->Copy( data[i], ctx, cycData );
			}
			GC_ShiftRC( src, copy[i] );
			copy[i] = src;
		}
	}
}
static DaoValue* DaoListCore_Copy( DaoValue *self0, DaoContext *ctx, DMap *cycData )
{
	DaoList *copy, *self = & self0->xList;
	DaoValue **data = self->items->items.pValue;

	if( cycData ){
		DNode *node = MAP_Find( cycData, self );
		if( node ) return node->value.pValue;
	}

	copy = DaoList_New();
	copy->unitype = self->unitype;
	GC_IncRC( copy->unitype );
	if( cycData ) MAP_Insert( cycData, self, copy );

	DArray_Resize( copy->items, self->items->size, NULL );
	DaoCopyValues( copy->items->items.pValue, data, self->items->size, ctx, cycData );
	return (DaoValue*) copy;
}
DaoList* DaoList_Copy( DaoList *self, DMap *cycData )
{
	return (DaoList*) DaoListCore_Copy( (DaoValue*)self, NULL, cycData );
}
static DaoTypeCore listCore=
{
	0, NULL, NULL, NULL, NULL,
	DaoValue_GetField,
	DaoValue_SetField,
	DaoListCore_GetItem,
	DaoListCore_SetItem,
	DaoListCore_Print,
	DaoListCore_Copy,
};

static void DaoLIST_Insert( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	int size = self->items->size;
	DaoList_Insert( self, p[1], p[2]->xInteger.value );
	if( size == self->items->size )
		DaoContext_RaiseException( ctx, DAO_ERROR_VALUE, "value type" );
}
static void DaoLIST_Erase( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	size_t start = (size_t)p[1]->xInteger.value;
	size_t n = (size_t)p[2]->xInteger.value;
	DArray_Erase( self->items, start, n );
}
static void DaoLIST_Erase2( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	DaoList *ids = & p[1]->xList;
	size_t i, k = self->items->size;
	for(i=0; i<ids->items->size; i++){
		dint id = ids->items->items.pValue[i]->xInteger.value;
		if( id < 0 ) id += self->items->size;
		if( id < 0 || id >= self->items->size ) continue;
		if( id < k ) k = id;
		GC_DecRC( self->items->items.pValue[id] );
		self->items->items.pValue[id] = NULL; /* mark as deleted */
	}
	for(i=k; i<self->items->size; i++){
		DaoValue *val = self->items->items.pValue[i];
		if( val == NULL ) continue;
		self->items->items.pValue[k] = val;
		k += 1;
	}
	self->items->size = k;
	/* to possibly reduce buffer size: */
	DArray_Resize( self->items, k, NULL );
}
static void DaoLIST_Clear( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	DaoList_Clear( self );
}
static void DaoLIST_Size( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	DaoContext_PutInteger( ctx, self->items->size );
}
static void DaoLIST_Resize( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	DaoValue *fill = null;
	size_t size = (size_t)p[1]->xInteger.value;
	if( ( ctx->vmSpace->options & DAO_EXEC_SAFE ) && size > 1000 ){
		DaoContext_RaiseException( ctx, DAO_ERROR,
				"not permitted to create large list in safe running mode" );
		return;
	}
	if( self->unitype && self->unitype->value ) fill = self->unitype->value;
	DArray_Resize( self->items, size, fill );
}
static int DaoList_CheckType( DaoList *self, DaoContext *ctx )
{
	size_t i, type;
	DaoValue **data = self->items->items.pValue;
	if( self->items->size == 0 ) return 0;
	type = data[0]->type;
	for(i=1; i<self->items->size; i++){
		if( type != data[i]->type ){
			DaoContext_RaiseException( ctx, DAO_WARNING, "need list of same type of elements" );
			return 0;
		}
	}
	if( type < DAO_INTEGER || type >= DAO_ARRAY ){
		DaoContext_RaiseException( ctx, DAO_WARNING, "need list of primitive data" );
		return 0;
	}
	return type;
}
#if 0
static void DaoList_PutDefault( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	complex16 com = { 0.0, 0.0 };
	if( self->unitype && self->unitype->nested->size ){
		switch( self->unitype->nested->items.pType[0]->tid ){
		case DAO_INTEGER : DaoContext_PutInteger( ctx, 0 ); break;
		case DAO_FLOAT   : DaoContext_PutFloat( ctx, 0.0 ); break;
		case DAO_DOUBLE  : DaoContext_PutDouble( ctx, 0.0 ); break;
		case DAO_COMPLEX : DaoContext_PutComplex( ctx, com ); break;
		case DAO_STRING  : DaoContext_PutMBString( ctx, "" ); break;
		default : DaoContext_PutValue( ctx, NULL ); break;
		}
	}else{
		DaoContext_PutValue( ctx, NULL );
	}
}
#endif
static void DaoLIST_Max( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoTuple *tuple = DaoContext_PutTuple( ctx );
	DaoList *self = & p[0]->xList;
	DaoValue *res, **data = self->items->items.pValue;
	size_t i, imax, type, size = self->items->size;

	tuple->items[1]->xInteger.value = -1;
	type = DaoList_CheckType( self, ctx );
	if( type == 0 ){
		/* DaoList_PutDefault( ctx, p, N ); */
		return;
	}
	imax = 0;
	res = data[0];
	for(i=1; i<size; i++){
		if( DaoValue_Compare( res, data[i] ) <0 ){
			imax = i;
			res = data[i];
		}
	}
	tuple->items[1]->xInteger.value = imax;
	DaoTuple_SetItem( tuple, res, 0 );
}
static void DaoLIST_Min( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoTuple *tuple = DaoContext_PutTuple( ctx );
	DaoList *self = & p[0]->xList;
	DaoValue *res, **data = self->items->items.pValue;
	size_t i, imin, type, size = self->items->size;

	tuple->items[1]->xInteger.value = -1;
	type = DaoList_CheckType( self, ctx );
	if( type == 0 ){
		/* DaoList_PutDefault( ctx, p, N ); */
		return;
	}
	imin = 0;
	res = data[0];
	for(i=1; i<size; i++){
		if( DaoValue_Compare( res, data[i] ) >0 ){
			imin = i;
			res = data[i];
		}
	}
	tuple->items[1]->xInteger.value = imin;
	DaoTuple_SetItem( tuple, res, 0 );
}
extern DLong* DaoContext_GetLong( DaoContext *self, DaoVmCode *vmc );
extern DaoEnum* DaoContext_GetEnum( DaoContext *self, DaoVmCode *vmc );
static void DaoLIST_Sum( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	size_t i, type, size = self->items->size;
	DaoValue **data = self->items->items.pValue;
	type = DaoList_CheckType( self, ctx );
	if( type == 0 ){
		/* DaoList_PutDefault( ctx, p, N ); */
		return;
	}
	switch( type ){
	case DAO_INTEGER :
		{
			dint res = 0;
			for(i=0; i<size; i++) res += data[i]->xInteger.value;
			DaoContext_PutInteger( ctx, res );
			break;
		}
	case DAO_FLOAT :
		{
			float res = 0.0;
			for(i=0; i<size; i++) res += data[i]->xFloat.value;
			DaoContext_PutFloat( ctx, res );
			break;
		}
	case DAO_DOUBLE :
		{
			double res = 0.0;
			for(i=0; i<size; i++) res += data[i]->xDouble.value;
			DaoContext_PutDouble( ctx, res );
			break;
		}
	case DAO_COMPLEX :
		{
			complex16 res = { 0.0, 0.0 };
			for(i=0; i<self->items->size; i++) COM_IP_ADD( res, data[i]->xComplex.value );
			DaoContext_PutComplex( ctx, res );
			break;
		}
	case DAO_LONG :
		{
			DLong *dlong = DaoContext_GetLong( ctx, ctx->vmc );
			for(i=0; i<self->items->size; i++) DLong_Add( dlong, dlong, data[i]->xLong.value );
			break;
		}
	case DAO_ENUM :
		{
			/* XXX */
			DaoEnum *denum = DaoContext_GetEnum( ctx, ctx->vmc );
			for(i=0; i<self->items->size; i++) denum->value += data[i]->xEnum.value;
			break;
		}
	case DAO_STRING :
		{
			DString *m = DaoContext_PutString( ctx, data[0]->xString.data );
			for(i=1; i<size; i++) DString_Append( m, data[i]->xString.data );
			break;
		}
	default : break;
	}
}
static void DaoLIST_Push( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	int size = self->items->size;
	if ( p[2]->xEnum.value == 0 )
		DaoList_PushFront( self, p[1] );
	else
		DaoList_Append( self, p[1] );
	if( size == self->items->size )
		DaoContext_RaiseException( ctx, DAO_ERROR_VALUE, "value type" );
}
static void DaoLIST_Pop( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	if( self->items->size == 0 ){
		DaoContext_RaiseException( ctx, DAO_ERROR_VALUE, "list is empty" );
		return;
	}
	if ( p[1]->xEnum.value == 0 )
		DaoList_Erase( self, 0 );
	else
		DaoList_Erase( self, self->items->size -1 );
}
static void DaoLIST_PushBack( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	int size = self->items->size;
	DaoList_Append( self, p[1] );
	if( size == self->items->size )
		DaoContext_RaiseException( ctx, DAO_ERROR_VALUE, "value type" );
}
static void DaoLIST_Front( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	if( self->items->size == 0 ){
		DaoContext_PutValue( ctx, null );
		DaoContext_RaiseException( ctx, DAO_ERROR_VALUE, "list is empty" );
		return;
	}
	DaoContext_PutReference( ctx, self->items->items.pValue[0] );
}
static void DaoLIST_Top( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	if( self->items->size == 0 ){
		DaoContext_PutValue( ctx, null );
		DaoContext_RaiseException( ctx, DAO_ERROR_VALUE, "list is empty" );
		return;
	}
	DaoContext_PutReference( ctx, self->items->items.pValue[ self->items->size -1 ] );
}
/* Quick Sort.
 * Adam Drozdek: Data Structures and Algorithms in C++, 2nd Edition.
 */
static int Compare( DaoContext *ctx, int entry, int reg0, int reg1, DaoValue *v0, DaoValue *v1 )
{
	DaoValue **locs = ctx->regValues;
	DaoValue_Copy( v0, locs + reg0 );
	DaoValue_Copy( v1, locs + reg1 );
	DaoProcess_ExecuteSection( ctx->process, entry );
	return DaoValue_GetInteger( ctx->process->returned );
}
static void PartialQuickSort( DaoContext *ctx, int entry, int r0, int r1,
		DaoValue **data, int first, int last, int part )
{
	int lower=first+1, upper=last;
	DaoValue *val;
	DaoValue *pivot;
	if( first >= last ) return;
	val = data[first];
	data[first] = data[ (first+last)/2 ];
	data[ (first+last)/2 ] = val;
	pivot = data[ first ];

	while( lower <= upper ){
		while( lower < last && Compare( ctx, entry, r0, r1, data[lower], pivot ) ) lower ++;
		while( upper > first && Compare( ctx, entry, r0, r1, pivot, data[upper] ) ) upper --;
		if( lower < upper ){
			val = data[lower];
			data[lower] = data[upper];
			data[upper] = val;
			upper --;
		}
		lower ++;
	}
	val = data[first];
	data[first] = data[upper];
	data[upper] = val;
	if( first < upper-1 ) PartialQuickSort( ctx, entry, r0, r1, data, first, upper-1, part );
	if( upper >= part ) return;
	if( upper+1 < last ) PartialQuickSort( ctx, entry, r0, r1, data, upper+1, last, part );
}
void QuickSort( IndexValue *data, int first, int last, int part, int asc )
{
	int lower=first+1, upper=last;
	IndexValue val;
	DaoValue *pivot;
	if( first >= last ) return;
	val = data[first];
	data[first] = data[ (first+last)/2 ];
	data[ (first+last)/2 ] = val;
	pivot = data[ first ].value;

	while( lower <= upper ){
		if( asc ){
			while( lower < last && DaoValue_Compare( data[lower].value, pivot ) <0 ) lower ++;
			while( upper > first && DaoValue_Compare( pivot, data[upper].value ) <0 ) upper --;
		}else{
			while( lower < last && DaoValue_Compare( data[lower].value, pivot ) >0 ) lower ++;
			while( upper > first && DaoValue_Compare( pivot, data[upper].value ) >0 ) upper --;
		}
		if( lower < upper ){
			val = data[lower];
			data[lower] = data[upper];
			data[upper] = val;
			upper --;
		}
		lower ++;
	}
	val = data[first];
	data[first] = data[upper];
	data[upper] = val;
	if( first < upper-1 ) QuickSort( data, first, upper-1, part, asc );
	if( upper >= part ) return;
	if( upper+1 < last ) QuickSort( data, upper+1, last, part, asc );
}
static void DaoLIST_Rank( DaoContext *ctx, DaoValue *p[], int npar)
{
	DaoList *list = & p[0]->xList;
	DaoList *res = DaoContext_PutList( ctx );
	DaoValue **items = list->items->items.pValue;
	DaoValue **ids;
	IndexValue *data;
	dint part = p[2]->xInteger.value;
	size_t i, N;

	N = list->items->size;
	DArray_Resize( res->items, N, p[2] ); /* init to be integers */
	ids = res->items->items.pValue;
	for(i=0; i<N; i++) ids[i]->xInteger.value = i;
	if( N < 2 ) return;
	if( part ==0 ) part = N;
	data = dao_malloc( N * sizeof( IndexValue ) );
	for(i=0; i<N; i++){
		data[i].index = i;
		data[i].value = items[i];
	}
	QuickSort( data, 0, N-1, part, ( p[1]->xEnum.value == 0 ) ? 1 : 0 );
	for(i=0; i<N; i++) ids[i]->xInteger.value = data[i].index;
	dao_free( data );
}
static void DaoLIST_Sort( DaoContext *ctx, DaoValue *p[], int npar )
{
	DaoList *list = & p[0]->xList;
	DaoValue **items = list->items->items.pValue;
	IndexValue *data;
	dint part = p[2]->xInteger.value;
	size_t i, N;

	DaoContext_PutReference( ctx, p[0] );
	N = list->items->size;
	if( N < 2 ) return;
	if( part ==0 ) part = N;
	data = dao_malloc( N * sizeof( IndexValue ) );
	for(i=0; i<N; i++){
		data[i].index = i;
		data[i].value = items[i];
	}
	QuickSort( data, 0, N-1, part, ( p[1]->xEnum.value == 0 ) ? 1 : 0 );
	for(i=0; i<N; i++) items[i] = data[i].value;
	dao_free( data );
}
void DaoContext_Sort( DaoContext *ctx, DaoVmCode *vmc, int index, int entry )
{
	DaoList *list = NULL;
	DaoValue **items = NULL;
	DaoValue *param = ctx->regValues[ vmc->b ];
	int reg0 = index + 1;
	int reg1 = index + 2;
	int nsort = 0;
	size_t N = 0;
	if( param->type == DAO_LIST ){
		list = & param->xList;
		items = list->items->items.pValue;
		nsort = N = list->items->size;
	}else if( param->type == DAO_TUPLE ){
		if( param->xTuple.size != 2 ) goto ErrorParam;
		if( param->xTuple.items[0]->type != DAO_LIST ) goto ErrorParam;
		if( param->xTuple.items[1]->type != DAO_INTEGER ) goto ErrorParam;
		list = & param->xTuple.items[0]->xList;
		nsort = param->xTuple.items[1]->xInteger.value;
		items = list->items->items.pValue;
		N = list->items->size;
	}else{
		goto ErrorParam;
	}
	DaoContext_PutValue( ctx, (DaoValue*) list );
	PartialQuickSort( ctx, entry, reg0, reg1, items, 0, N-1, nsort );
	return;
ErrorParam :
	DaoContext_RaiseException( ctx, DAO_ERROR_PARAM, "invalid parameter for sort()" );
}
static void DaoLIST_Iter( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	DaoTuple *tuple = & p[1]->xTuple;
	DaoValue **data = tuple->items;
	DaoValue *iter = DaoValue_NewInteger(0);
	data[0]->xInteger.value = self->items->size >0;
	DaoValue_Copy( iter, & data[1] );
	GC_DecRC( iter );
}
static void DaoLIST_Join( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoList *self = & p[0]->xList;
	DaoValue **data = self->items->items.pValue;
	DString *sep = p[1]->xString.data;
	DString *buf = DString_New( 1 );
	DString *res;
	size_t size = 0, i;
	int digits, mbs = 1;
	wchar_t ewcs[] = {0};
	for( i = 0; i < self->items->size; i++ ){
		switch( data[i]->type ){
		case DAO_STRING:
			if( data[i]->xString.data->mbs == NULL ) mbs = 0;
			size += data[i]->xString.data->size;
			break;
		case DAO_INTEGER:
			size += ( data[i]->xInteger.value < 0 ) ? 2 : 1;
			break;
		case DAO_FLOAT:
			size += ( data[i]->xFloat.value < 0 ) ? 2 : 1;
			break;
		case DAO_DOUBLE:
			size += ( data[i]->xDouble.value < 0 ) ? 2 : 1;
			break;
		case DAO_COMPLEX:
			size += ( data[i]->xComplex.value.real < 0 ) ? 5 : 4;
			break;
		case DAO_LONG:
			digits = self->items->items.pValue[i]->xLong.value->size;
			digits = digits > 1 ? (LONG_BITS * (digits - 1) + 1) : 1; /* bits */
			digits /= (int)(log( self->items->items.pValue[i]->xLong.value->base ) / log(2)); /* digits */
			size += digits + (data[i]->xLong.value->sign < 0) ? 3 : 2; /* sign + suffix */
			break;
		case DAO_ENUM :
			size += 1;
			break;
		default:
			DaoContext_RaiseException( ctx, DAO_ERROR, "Incompatible list type (expected numeric or string)" );
			return;
		}
	}
	if( !mbs || ( sep->size != 0 && sep->mbs == NULL ) )
		res = DaoContext_PutWCString( ctx, ewcs );
	else
		res = DaoContext_PutMBString( ctx, "" );
	if( self->items->size != 0 ){
		DString_Reserve( res, size + ( self->items->size - 1 ) * sep->size );
		for( i = 0; i < self->items->size - 1; i++ ){
			DString_Append( res, DaoValue_GetString( self->items->items.pValue[i], buf ) );
			if( sep->size != 0 ) DString_Append( res, sep );
		}
		DString_Append( res, DaoValue_GetString( self->items->items.pValue[i], buf ) );
	}
	DString_Delete( buf );
}
static void DaoLIST_Reverse( DaoContext *ctx, DaoValue *p[], int npar )
{
	DaoList *list = & p[0]->xList;
	DaoValue **items = list->items->items.pValue;
	size_t i = 0, N = list->items->size;

	DaoContext_PutReference( ctx, p[0] );
	for(i=0; i<N/2; i++){
		DaoValue *tmp = items[N-i-1];
		items[N-i-1] = items[i];
		items[i] = tmp;
	}
}
static DaoFuncItem listMeths[] =
{
	{ DaoLIST_Insert,   "insert( self :list<@T>, item : @T, pos=0 )" },
	{ DaoLIST_Erase,    "erase( self :list<any>, start=0, n=1 )" },
	{ DaoLIST_Erase2,   "erase( self :list<any>, indices : list<int> )" },
	{ DaoLIST_Clear,    "clear( self :list<any> )" },
	{ DaoLIST_Size,     "size( self :list<any> )=>int" },
	{ DaoLIST_Resize,   "resize( self :list<any>, size :int )" },
	{ DaoLIST_Max,      "max( self :list<@T<int|long|float|double|complex|string|enum>> )=>tuple<@T,int>" },
	{ DaoLIST_Min,      "min( self :list<@T<int|long|float|double|complex|string|enum>> )=>tuple<@T,int>" },
	{ DaoLIST_Sum,      "sum( self :list<@T<int|long|float|double|complex|string|enum>> )=>@T" },
	{ DaoLIST_Join,     "join( self :list<int|float|double|long|complex|string|enum>, separator='' )=>string" },
	{ DaoLIST_PushBack, "append( self :list<@T>, item :@T )" },
	{ DaoLIST_Push,     "push( self :list<@T>, item :@T, to :enum<front, back> = $back )" },
	{ DaoLIST_Pop,      "pop( self :list<any>, from :enum<front, back> = $back )" },
	{ DaoLIST_Front,    "front( self :list<@T> )=>@T" },
	{ DaoLIST_Top,      "back( self :list<@T> )=>@T" },
	{ DaoLIST_Rank,     "rank( self :list<any>, order :enum<ascend, descend>=$ascend, k=0 )=>list<int>" },
	{ DaoLIST_Sort,     "sort( self :list<@T>, order :enum<ascend, descend>=$ascend, k=0 )=>list<@T>" },
	{ DaoLIST_Reverse,  "reverse( self :list<@T> )=>list<@T>" },
	{ DaoLIST_Iter,     "__for_iterator__( self :list<any>, iter : for_iterator )" },
	{ NULL, NULL }
};

int DaoList_Size( DaoList *self )
{
	return self->items->size;
}
DaoValue* DaoList_Front( DaoList *self )
{
	if( self->items->size == 0 ) return NULL;
	return self->items->items.pValue[0];
}
DaoValue* DaoList_Back( DaoList *self )
{
	if( self->items->size == 0 ) return NULL;
	return self->items->items.pValue[ self->items->size-1 ];
}
DaoValue* DaoList_GetItem( DaoList *self, int pos )
{
	if( pos <0 || pos >= self->items->size ) return NULL;
	return self->items->items.pValue[pos];
}
DaoTuple* DaoList_ToTuple( DaoList *self, DaoTuple *proto )
{
	/* XXX */
	return NULL;
}
int DaoList_SetItem( DaoList *self, DaoValue *it, int pos )
{
	DaoValue **val;
	if( pos <0 || pos >= self->items->size ) return 1;
	val = self->items->items.pValue + pos;
	if( self->unitype && self->unitype->nested->size ){
		return DaoValue_Move( it, val, self->unitype->nested->items.pType[0] ) == 0;
	}else{
		DaoValue_Copy( it, val );
	}
	return 0;
}

int DaoList_Insert( DaoList *self, DaoValue *item, int pos )
{
	DaoType *tp = self->unitype ? self->unitype->nested->items.pType[0] : NULL;
	DaoValue *temp = NULL;
	if( DaoValue_Move( item, & temp, tp ) ==0 ){
		GC_DecRC( temp );
		return 1;
	}
	DArray_Insert( self->items, NULL, pos );
	self->items->items.pValue[ pos ] = temp;
	return 0;
}
int DaoList_PushFront( DaoList *self, DaoValue *item )
{
	DaoType *tp = self->unitype ? self->unitype->nested->items.pType[0] : NULL;
	DaoValue *temp = NULL;
	if( DaoValue_Move( item, & temp, tp ) ==0 ){
		GC_DecRC( temp );
		return 1;
	}
	DArray_PushFront( self->items, NULL );
	self->items->items.pValue[ 0 ] = temp;
	return 0;
}
int DaoList_PushBack( DaoList *self, DaoValue *item )
{
	DaoType *tp = self->unitype ? self->unitype->nested->items.pType[0] : NULL;
	DaoValue *temp = NULL;
	if( DaoValue_Move( item, & temp, tp ) ==0 ){
		GC_DecRC( temp );
		return 1;
	}
	DArray_PushBack( self->items, NULL );
	self->items->items.pValue[ self->items->size - 1 ] = temp;
	return 0;
}
void DaoList_PopFront( DaoList *self )
{
	if( self->items->size ==0 ) return;
	DArray_PopFront( self->items );
}
void DaoList_PopBack( DaoList *self )
{
	if( self->items->size ==0 ) return;
	DArray_PopBack( self->items );
}

DaoTypeBase listTyper=
{
	"list", & listCore, NULL, (DaoFuncItem*)listMeths, {0}, {0},
	(FuncPtrDel) DaoList_Delete, NULL
};

DaoList* DaoList_New()
{
	DaoList *self = (DaoList*) dao_malloc( sizeof(DaoList) );
	DaoValue_Init( self, DAO_LIST );
	self->items = DArray_New(D_VALUE);
	self->meta = NULL;
	self->unitype = NULL;
	return self;
}
void DaoList_Delete( DaoList *self )
{
	if( self->meta ) GC_DecRC( self->meta );
	GC_DecRC( self->unitype );
	DaoList_Clear( self );
	DArray_Delete( self->items );
	dao_free( self );
}
void DaoList_Clear( DaoList *self )
{
	DArray_Clear( self->items );
}
int DaoList_Append( DaoList *self, DaoValue *value )
{
	return DaoList_PushBack( self, value );
}
void DaoList_Erase( DaoList *self, int pos )
{
	if( pos < 0 || pos >= self->items->size ) return;
	DArray_Erase( self->items, pos, 1 );
}

/**/
static void DaoMap_Print( DaoValue *self0, DaoContext *ctx, DaoStream *stream, DMap *cycData )
{
	DaoMap *self = & self0->xMap;
	char *kvsym = self->items->hashing ? " : " : " => ";
	const int size = self->items->size;
	int i = 0;

	DNode *node = NULL;
	if( cycData ) node = MAP_Find( cycData, self );
	if( node ){
		DaoStream_WriteMBS( stream, "{ ... }" );
		return;
	}
	if( cycData ) MAP_Insert( cycData, self, self );
	DaoStream_WriteMBS( stream, "{ " );

	node = DMap_First( self->items );
	for( ; node!=NULL; node=DMap_Next(self->items,node) ){
		stream->useQuote = 1;
		DaoValue_Print( node->key.pValue, ctx, stream, cycData );
		DaoStream_WriteMBS( stream, kvsym );
		DaoValue_Print( node->value.pValue, ctx, stream, cycData );
		stream->useQuote = 0;
		if( i+1<size ) DaoStream_WriteMBS( stream, ", " );
		i++;
	}
	if( size==0 ) DaoStream_WriteMBS( stream, kvsym );
	DaoStream_WriteMBS( stream, " }" );
	if( cycData ) MAP_Erase( cycData, self );
}
static void DaoMap_GetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid )
{
	DaoMap *self = & self0->xMap;
	if( pid->type == DAO_TUPLE && pid->xTuple.unitype == dao_type_for_iterator ){
		DaoTuple *iter = & pid->xTuple;
		DaoTuple *tuple = DaoContext_PutTuple( ctx );
		DNode *node = (DNode*) iter->items[1]->xCdata.data;
		if( node == NULL || tuple->size != 2 ) return;
		DaoValue_Copy( node->key.pValue, tuple->items );
		DaoValue_Copy( node->value.pValue, tuple->items + 1 );
		node = DMap_Next( self->items, node );
		iter->items[0]->xInteger.value = node != NULL;
		iter->items[1]->xCdata.data = node;
	}else{
		DNode *node = MAP_Find( self->items, pid );
		if( node ==NULL ){
			DaoContext_RaiseException( ctx, DAO_ERROR_KEY, NULL );
			return;
		}
		DaoContext_PutReference( ctx, node->value.pValue );
	}
}
extern DaoType *dao_map_any;
static void DaoMap_SetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid, DaoValue *value )
{
	DaoMap *self = & self0->xMap;
	int c = DaoMap_Insert( self, pid, value );
	if( c ==1 ){
		DaoContext_RaiseException( ctx, DAO_ERROR_TYPE, "key not matching" );
	}else if( c ==2 ){
		DaoContext_RaiseException( ctx, DAO_ERROR_TYPE, "value not matching" );
	}
}
static void DaoMap_GetItem2( DaoValue *self0, DaoContext *ctx, DaoValue *ids[], int N )
{
	DaoMap *self = & self0->xMap;
	DaoMap *map = DaoContext_PutMap( ctx );
	DNode *node1 = DMap_First( self->items );
	DNode *node2 = NULL;
	if( ids[0]->type ) node1 = MAP_FindGE( self->items, ids[0] );
	if( ids[1]->type ) node2 = MAP_FindLE( self->items, ids[1] );
	if( node2 ) node2 = DMap_Next(self->items, node2 );
	for(; node1 != node2; node1 = DMap_Next(self->items, node1 ) )
		DaoMap_Insert( map, node1->key.pValue, node1->value.pValue );
}
static void DaoMap_GetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N )
{
	switch( N ){
	case 0 : DaoMap_GetItem1( self, ctx, null ); break;
	case 1 : DaoMap_GetItem1( self, ctx, ids[0] ); break;
	case 2 : DaoMap_GetItem2( self, ctx, ids, N ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static void DaoMap_SetItem2( DaoValue *self0, DaoContext *ctx, DaoValue *ids[], int N, DaoValue *value )
{
	DaoMap *self = & self0->xMap;
	DaoType *tp = self->unitype;
	DaoType *tp2=NULL;
	DNode *node1 = DMap_First( self->items );
	DNode *node2 = NULL;
	if( tp == NULL ){
		/* a : tuple<string,map<string,int>> = ('',{=>});
		   duplicating the constant to assign to "a" may not set the unitype properly */
		tp = ctx->regTypes[ ctx->vmc->c ];
		if( tp == NULL || tp->tid == 0 ) tp = dao_map_any;
		self->unitype = tp;
		GC_IncRC( tp );
	}
	if( tp ){
		if( tp->nested->size != 2 || tp->nested->items.pType[1] == NULL ){
			DaoContext_RaiseException( ctx, DAO_ERROR_TYPE, "invalid map" );
			return;
		}
		tp2 = tp->nested->items.pType[1];
		if( DaoType_MatchValue( tp2, value, NULL ) ==0 )
			DaoContext_RaiseException( ctx, DAO_ERROR_TYPE, "value not matching" );
	}
	if( ids[0]->type ) node1 = MAP_FindGE( self->items, ids[0] );
	if( ids[1]->type ) node2 = MAP_FindLE( self->items, ids[1] );
	if( node2 ) node2 = DMap_Next(self->items, node2 );
	for(; node1 != node2; node1 = DMap_Next(self->items, node1 ) )
		DaoValue_Move( value, & node1->value.pValue, tp2 );
}
static void DaoMap_SetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N, DaoValue *value )
{
	switch( N ){
	case 0 : DaoMap_SetItem1( self, ctx, null, value ); break;
	case 1 : DaoMap_SetItem1( self, ctx, ids[0], value ); break;
	case 2 : DaoMap_SetItem2( self, ctx, ids, N, value ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static DaoValue* DaoMap_Copy( DaoValue *self0, DaoContext *ctx, DMap *cycData )
{
	DaoMap *copy, *self = & self0->xMap;
	DNode *node;

	if( cycData ){
		DNode *node = MAP_Find( cycData, self );
		if( node ) return node->value.pValue;
	}

	copy = DaoMap_New( self->items->hashing );
	copy->unitype = self->unitype;
	GC_IncRC( copy->unitype );

	node = DMap_First( self->items );
	if( cycData ){
		MAP_Insert( cycData, self, copy );
		for( ; node!=NULL; node = DMap_Next(self->items, node) ){
			DaoValue *key = DaoValue_MakeCopy( node->key.pValue, ctx, cycData );
			DaoValue *value = DaoValue_MakeCopy( node->value.pValue, ctx, cycData );
			MAP_Insert( copy->items, & key, & value );
			GC_IncRC( key ); GC_IncRC( value );
			GC_DecRC( key ); GC_DecRC( value );
		}
	}else{
		for( ; node!=NULL; node = DMap_Next(self->items, node) ){
			MAP_Insert( copy->items, node->key.pValue, node->value.pValue );
		}
	}
	return (DaoValue*) copy;
}
static DaoTypeCore mapCore =
{
	0, NULL, NULL, NULL, NULL,
	DaoValue_GetField,
	DaoValue_SetField,
	DaoMap_GetItem,
	DaoMap_SetItem,
	DaoMap_Print,
	DaoMap_Copy,
};

static void DaoMAP_Clear( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap_Clear( & p[0]->xMap );
}
static void DaoMAP_Reset( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap_Reset( & p[0]->xMap );
}
static void DaoMAP_Erase( DaoContext *ctx, DaoValue *p[], int N )
{
	DMap *self = p[0]->xMap.items;
	DNode *ml, *mg;
	DArray *keys;
	N --;
	switch( N ){
	case 0 :
		DMap_Clear( self ); break;
	case 1 :
		MAP_Erase( self, p[1] );
		break;
	case 2 :
		mg = MAP_FindGE( self, p[1] );
		ml = MAP_FindLE( self, p[2] );
		if( mg ==NULL || ml ==NULL ) return;
		ml = DMap_Next( self, ml );
		keys = DArray_New(0);
		for(; mg != ml; mg=DMap_Next(self, mg)) DArray_Append( keys, mg->key.pVoid );
		while( keys->size ){
			MAP_Erase( self, keys->items.pVoid[0] );
			DArray_PopFront( keys );
		}
		DArray_Delete( keys );
		break;
	default : break;
	}
}
static void DaoMAP_Insert( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap *self = & p[0]->xMap;
	int c = DaoMap_Insert( self, p[1], p[2] );
	if( c ==1 ){
		DaoContext_RaiseException( ctx, DAO_ERROR_TYPE, "key not matching" );
	}else if( c ==2 ){
		DaoContext_RaiseException( ctx, DAO_ERROR_TYPE, "value not matching" );
	}
}
static void DaoMAP_Find( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap *self = & p[0]->xMap;
	DaoTuple *res = DaoContext_PutTuple( ctx );
	DNode *node;
	switch( (int)p[2]->xEnum.value ){
	case 0 :
		node = MAP_FindLE( self->items, p[1] );
		if( node == NULL ) return;
		res->items[0]->xInteger.value = 1;
		DaoValue_Copy( node->key.pValue, res->items + 1 );
		DaoValue_Copy( node->value.pValue, res->items + 2 );
		break;
	case 1  :
		node = MAP_Find( self->items, p[1] );
		if( node == NULL ) return;
		res->items[0]->xInteger.value = 1;
		DaoValue_Copy( node->key.pValue, res->items + 1 );
		DaoValue_Copy( node->value.pValue, res->items + 2 );
		break;
	case 2  :
		node = MAP_FindGE( self->items, p[1] );
		if( node == NULL ) return;
		res->items[0]->xInteger.value = 1;
		DaoValue_Copy( node->key.pValue, res->items + 1 );
		DaoValue_Copy( node->value.pValue, res->items + 2 );
		break;
	default : break;
	}
}
static void DaoMAP_Key( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap *self = & p[0]->xMap;
	DaoList *list = DaoContext_PutList( ctx );
	DNode *node, *ml=NULL, *mg=NULL;
	N --;
	switch( N ){
	case 0 :
		mg = DMap_First( self->items );
		break;
	case 1 :
		mg = MAP_FindGE( self->items, p[1] );
		break;
	case 2 :
		mg = MAP_FindGE( self->items, p[1] );
		ml = MAP_FindLE( self->items, p[2] );
		if( ml == NULL ) return;
		ml = DMap_Next( self->items, ml );
		break;
	default: break;
	}
	if( mg == NULL ) return;
	for( node=mg; node != ml; node = DMap_Next( self->items, node ) )
		DaoList_Append( list, node->key.pValue );
}
static void DaoMAP_Value( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap *self = & p[0]->xMap;
	DaoList *list = DaoContext_PutList( ctx );
	DNode *node, *ml=NULL, *mg=NULL;
	N --;
	switch( N ){
	case 0 :
		mg = DMap_First( self->items );
		break;
	case 1 :
		mg = MAP_FindGE( self->items, p[1] );
		break;
	case 2 :
		mg = MAP_FindGE( self->items, p[1] );
		ml = MAP_FindLE( self->items, p[2] );
		if( ml ==NULL ) return;
		ml = DMap_Next( self->items, ml );
		break;
	default: break;
	}
	if( mg == NULL ) return;
	for( node=mg; node != ml; node = DMap_Next( self->items, node ) ){
		DaoList_Append( list, node->value.pValue );
	}
}
static void DaoMAP_Has( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap *self = & p[0]->xMap;
	DaoContext_PutInteger( ctx, DMap_Find( self->items, p[1] ) != NULL );
}
static void DaoMAP_Size( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap *self = & p[0]->xMap;
	DaoContext_PutInteger( ctx, self->items->size );
}
static void DaoMAP_Iter( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoMap *self = & p[0]->xMap;
	DaoTuple *tuple = & p[1]->xTuple;
	DaoValue **data = tuple->items;
	DNode *node = DMap_First( self->items );
	data[0]->xInteger.value = self->items->size >0;
	if( data[1]->type != DAO_CDATA || data[1]->xCdata.typer != & cdataTyper ){
		DaoCdata *it = DaoCdata_New( & cdataTyper, node );
		GC_ShiftRC( it, data[1] );
		data[1] = (DaoValue*) it;
	}else{
		data[1]->xCdata.data = node;
	}
}
static DaoFuncItem mapMeths[] =
{
	{ DaoMAP_Clear,  "clear( self :map<any,any> )" },
	{ DaoMAP_Reset,  "reset( self :map<any,any> )" },
	{ DaoMAP_Erase,  "erase( self :map<any,any> )" },
	{ DaoMAP_Erase,  "erase( self :map<@K,@V>, from :@K )" },
	{ DaoMAP_Erase,  "erase( self :map<@K,@V>, from :@K, to :@K )" },
	{ DaoMAP_Insert, "insert( self :map<@K,@V>, key :@K, value :@V )" },
	/* 0:EQ; -1:MaxLess; 1:MinGreat */
	{ DaoMAP_Find,   "find( self :map<@K,@V>, key :@K, type : enum<le,eq,ge>=$eq )=>tuple<int,@K,@V>" },
	{ DaoMAP_Key,    "keys( self :map<@K,any> )=>list<@K>" },
	{ DaoMAP_Key,    "keys( self :map<@K,any>, from :@K )=>list<@K>" },
	{ DaoMAP_Key,    "keys( self :map<@K,any>, from :@K, to :@K )=>list<@K>" },
	{ DaoMAP_Value,  "values( self :map<any,@V> )=>list<@V>" },
	{ DaoMAP_Value,  "values( self :map<@K,@V>, from :@K )=>list<@V>" },
	{ DaoMAP_Value,  "values( self :map<@K,@V>, from :@K, to :@K )=>list<@V>" },
	{ DaoMAP_Has,    "has( self :map<@K,any>, key :@K )=>int" },
	{ DaoMAP_Size,   "size( self :map<any,any> )=>int" },
	{ DaoMAP_Iter,   "__for_iterator__( self :map<any,any>, iter : for_iterator )" },
	{ NULL, NULL }
};

int DaoMap_Size( DaoMap *self )
{
	return self->items->size;
}
DaoValue* DaoMap_GetValue( DaoMap *self, DaoValue *key  )
{
	DNode *node = MAP_Find( self->items, & key );
	if( node ) return node->value.pValue;
	return NULL;
}
int DaoMap_InsertMBS( DaoMap *self, const char *key, DaoValue *value )
{
	DaoString vkey = { DAO_STRING,0,0,0,1,NULL};
	DString str = DString_WrapMBS( key );
	vkey.data = & str;
	return DaoMap_Insert( self, (DaoValue*) & vkey, value );
}
int DaoMap_InsertWCS( DaoMap *self, const wchar_t *key, DaoValue *value )
{
	DaoString vkey = { DAO_STRING,0,0,0,1,NULL};
	DString str = DString_WrapWCS( key );
	vkey.data = & str;
	return DaoMap_Insert( self, (DaoValue*) & vkey, value );
}
void DaoMap_EraseMBS ( DaoMap *self, const char *key )
{
	DaoString vkey = { DAO_STRING,0,0,0,1,NULL};
	DString str = DString_WrapMBS( key );
	vkey.data = & str;
	DaoMap_Erase( self, (DaoValue*) & vkey );
}
void DaoMap_EraseWCS ( DaoMap *self, const wchar_t *key )
{
	DaoString vkey = { DAO_STRING,0,0,0,1,NULL};
	DString str = DString_WrapWCS( key );
	vkey.data = & str;
	DaoMap_Erase( self, (DaoValue*) & vkey );
}
DaoValue* DaoMap_GetValueMBS( DaoMap *self, const char *key  )
{
	DaoString vkey = { DAO_STRING,0,0,0,1,NULL};
	DString str = DString_WrapMBS( key );
	DNode *node;
	vkey.data = & str;
	node = MAP_Find( self->items, (DaoValue*) &  vkey );
	if( node ) return node->value.pValue;
	return NULL;
}
DaoValue* DaoMap_GetValueWCS( DaoMap *self, const wchar_t *key  )
{
	DaoString vkey = { DAO_STRING,0,0,0,1,NULL};
	DString str = DString_WrapWCS( key );
	DNode *node;
	vkey.data = & str;
	node = MAP_Find( self->items, (DaoValue*) &  vkey );
	if( node ) return node->value.pValue;
	return NULL;
}

DaoTypeBase mapTyper=
{
	"map", & mapCore, NULL, (DaoFuncItem*) mapMeths, {0}, {0},
	(FuncPtrDel)DaoMap_Delete, NULL
};

DaoMap* DaoMap_New( int hashing )
{
	DaoMap *self = (DaoMap*) dao_malloc( sizeof( DaoMap ) );
	DaoValue_Init( self, DAO_MAP );
	self->items = hashing ? DHash_New( D_VALUE, D_VALUE ) : DMap_New( D_VALUE, D_VALUE );
	self->meta = NULL;
	self->unitype = NULL;
	return self;
}
void DaoMap_Delete( DaoMap *self )
{
	if( self->meta ) GC_DecRC( self->meta );
	GC_DecRC( self->unitype );
	DaoMap_Clear( self );
	DMap_Delete( self->items );
	dao_free( self );
}
void DaoMap_Clear( DaoMap *self )
{
	DMap_Clear( self->items );
}
void DaoMap_Reset( DaoMap *self )
{
	DMap_Reset( self->items );
}
void DaoValue_SetType( DaoValue *to, DaoType *tp );
int DaoMap_Insert( DaoMap *self, DaoValue *key, DaoValue *value )
{
	DaoType *tp = self->unitype;
	DaoType *tp1=NULL, *tp2=NULL;
	DaoValue *key2 = NULL;
	DaoValue *value2 = NULL;
	DaoEnum ekey = {DAO_ENUM,0,0,0,1,0,NULL,0};
	DaoEnum evalue = {DAO_ENUM,0,0,0,1,0,NULL,0};
	DNode *node;
	if( tp ){
		if( tp->nested->size >=2 ){
			tp1 = tp->nested->items.pType[0];
			tp2 = tp->nested->items.pType[1];
		}else if( tp->nested->size >=1 ){
			tp1 = tp->nested->items.pType[0];
		}
	}
	/* type checking and setting */
	if( tp1 ){
		if( DaoType_MatchValue( tp1, key, NULL ) ==0 ) return 1;
		if( (key->type == DAO_ENUM && tp1->tid == DAO_ENUM) || key->type == DAO_TUPLE ){
			if( DaoValue_Move( key, & key2, tp1 ) == 0 ){
				GC_DecRC( key2 );
				return 1;
			}
			key = key2;
		}
	}
	if( tp2 ){
		if( DaoType_MatchValue( tp2, value, NULL ) ==0 ) return 2;
		if( (value->type == DAO_ENUM && tp2->tid == DAO_ENUM) || value->type == DAO_TUPLE ){
			if( DaoValue_Move( value, & value2, tp2 ) == 0 ){
				GC_DecRC( value2 );
				return 1;
			}
			value = value2;
		}
	}
	node = DMap_Insert( self->items, key, value );
	if( tp1 && key->type >= DAO_ENUM ) DaoValue_SetType( node->key.pValue, tp1 );
	if( tp2 && value->type >= DAO_ENUM ) DaoValue_SetType( node->value.pValue, tp2 );
	GC_DecRC( key2 );
	GC_DecRC( value2 );
	return 0;
}
void DaoMap_Erase( DaoMap *self, DaoValue *key )
{
	MAP_Erase( self->items, key );
}
DNode* DaoMap_First( DaoMap *self )
{
	return DMap_First(self->items);
}
DNode* DaoMap_Next( DaoMap *self, DNode *iter )
{
	return DMap_Next(self->items,iter);
}

DMap *dao_cdata_bindings = NULL;
static DaoCdata* DaoCdataBindings_Find( void *data )
{
	DNode *node = DMap_Find( dao_cdata_bindings, data );
	if( node ) return (DaoCdata*) node->value.pVoid;
	return NULL;
}
#ifdef DAO_WITH_THREAD
DMutex dao_cdata_mutex;
static void DaoCdataBindings_Insert( void *data, DaoCdata *wrap )
{
	if( data == NULL ) return;
	DMutex_Lock( & dao_cdata_mutex );
	DMap_Insert( dao_cdata_bindings, data, wrap );
	DMutex_Unlock( & dao_cdata_mutex );
}
static void DaoCdataBindings_Erase( void *data )
{
	if( data == NULL ) return;
	DMutex_Lock( & dao_cdata_mutex );
	DMap_Erase( dao_cdata_bindings, data );
	DMutex_Unlock( & dao_cdata_mutex );
}
#else
static void DaoCdataBindings_Insert( void *data, DaoCdata *wrap )
{
	if( data == NULL ) return;
	DMap_Insert( dao_cdata_bindings, data, wrap );
}
static void DaoCdataBindings_Erase( void *data )
{
	if( data == NULL ) return;
	DMap_Erase( dao_cdata_bindings, data );
}
#endif

/**/
DaoCdata* DaoCdata_New( DaoTypeBase *typer, void *data )
{
	DaoCdata *self = DaoCdataBindings_Find( data );
	if( self && self->typer == typer && self->data == data ) return self;
	self = (DaoCdata*)dao_calloc( 1, sizeof(DaoCdata) );
	DaoValue_Init( self, DAO_CDATA );
	self->attribs = DAO_CDATA_FREE;
	self->extref = 0;
	self->data = data;
	self->typer = typer;
	if( typer && typer->priv ) self->ctype = typer->priv->abtype;
	if( typer == NULL ){
		self->ctype = cdataTyper.priv->abtype;
		self->typer = & cdataTyper;
		self->buffer = data;
	}else if( data ){
		DaoCdataBindings_Insert( data, self );
	}
	if( typer && typer->priv ){
		self->cmodule = typer->priv->nspace->cmodule;
		GC_IncRC( self->cmodule );
	}
	if( self->ctype ) GC_IncRC( self->ctype );
	return self;
}
DaoCdata* DaoCdata_Wrap( DaoTypeBase *typer, void *data )
{
	DaoCdata *self = DaoCdataBindings_Find( data );
	if( self && self->typer == typer && self->data == data ) return self;
	self = DaoCdata_New( typer, data );
	self->attribs = 0;
	return self;
}
static void DaoCdata_Delete( DaoCdata *self )
{
	DaoCdata_DeleteData( self );
	dao_free( self );
}
void DaoCdata_DeleteData( DaoCdata *self )
{
	DaoCdataCore *c = (DaoCdataCore*)self->typer->priv;
	void (*fdel)(void*) = (void (*)(void *))DaoCdata_Delete;
	if( self->buffer == NULL ) DaoCdataBindings_Erase( self->data );
	if( self->ctype ) GC_DecRC( self->ctype );
	if( self->meta ) GC_DecRC( self->meta );
	if( self->daoObject ) GC_DecRC( self->daoObject );
	if( self->cmodule ) GC_DecRC( self->cmodule );
	self->meta = NULL;
	self->daoObject = NULL;
	self->cmodule = NULL;
	self->ctype = NULL;
	if( !(self->attribs & DAO_CDATA_FREE) ) return;
	if( self->buffer ){
		dao_free( self->buffer );
	}else if( self->data ){
		if( c && c->DelData && c->DelData != fdel ){
			c->DelData( self->data );
		}else if( c ==0 && self->typer->Delete && self->typer->Delete != fdel ){
			/* if the methods of typer has not been setup, typer->priv would be NULL */
			self->typer->Delete( self->data );
		}else if( self->bufsize > 0 ){
			dao_free( self->data );
		}
	}
	self->buffer = NULL;
	self->data = NULL;
}
int DaoCdata_IsType( DaoCdata *self, DaoTypeBase *typer )
{
	return DaoCdata_ChildOf( self->typer, typer );
}
int DaoCdata_OwnData( DaoCdata *self )
{
	return (self->attribs & DAO_CDATA_FREE) != 0;
}
void DaoCdata_SetExtReference( DaoCdata *self, int bl )
{
	if( (bl && self->extref) || (bl==0 && self->extref ==0) ) return;
	if( bl ){
		GC_IncRC( self );
		self->cycRefCount += 1;
	}else{
		GC_DecRC( self );
	}
	self->extref = bl != 0;
}
void DaoCdata_SetData( DaoCdata *self, void *data )
{
	if( self->buffer == NULL && self->data ) DaoCdataBindings_Erase( self->data );
	self->data = data;
	if( self->buffer == NULL && data ) DaoCdataBindings_Insert( data, self );
}
void DaoCdata_SetBuffer( DaoCdata *self, void *data, size_t size )
{
	self->data = data;
	self->buffer = data;
	self->size = size;
	self->bufsize = size;
}
void DaoCdata_SetArray( DaoCdata *self, void *data, size_t size, int memsize )
{
	self->data = data;
	self->buffer = data;
	self->memsize = memsize;
	self->size = size;
	self->bufsize = size;
	if( memsize ==0 ){
		self->data = ((void**)data)[0];
	}else{
		self->bufsize *= memsize;
	}
}
void* DaoCdata_GetData( DaoCdata *self )
{
	return self->data;
}
void* DaoCdata_GetBuffer( DaoCdata *self )
{
	return self->buffer;
}
void** DaoCdata_GetData2( DaoCdata *self )
{
	return & self->data;
}
DaoObject* DaoCdata_GetObject( DaoCdata *self )
{
	return (DaoObject*)self->daoObject;
}
DaoTypeBase* DaoCdata_GetTyper(DaoCdata *self )
{
	return self->typer;
}
void* DaoCdata_CastData( DaoCdata *self, DaoTypeBase *totyper )
{
	DaoTypeBase *typer = self->typer;
	DaoCdata tmp = *self;
	void *data;
	int i;
	if( typer == totyper || totyper == NULL || self->data == NULL ) return self->data;
	for(i=0; i<DAO_MAX_CDATA_SUPER; i++){
		if( typer->supers[i] == NULL ) break;
		if( typer->supers[i] == totyper ){
			if( typer->casts[i] ) return (*typer->casts[i])( self->data );
			return self->data;
		}
		if( typer->casts[i] ) tmp.data = (*typer->casts[i])( self->data );
		tmp.ctype = typer->supers[i]->priv->abtype;
		tmp.typer = typer->supers[i];
		data = DaoCdata_CastData( & tmp, totyper );
		if( data ) return data;
	}
	return NULL;
}
static void DaoCdata_GetField( DaoValue *self, DaoContext *ctx, DString *name )
{
	DaoTypeBase *typer = self->xCdata.typer;
	DaoValue *p = DaoFindValue( typer, name );
	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	if( p == NULL ){
		DaoValue *func = NULL;
		DString_SetMBS( ctx->process->mbstring, "." );
		DString_Append( ctx->process->mbstring, name );
		func = DaoFindFunction( typer, ctx->process->mbstring );
		func = (DaoValue*) DRoutine_Resolve( func, self, NULL, 0, DVM_CALL );
		if( func == NULL ){
			DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, "not exist" );
			return;
		}
		func->xFunction.pFunc( ctx, & self, 1 );
	}else{
		DaoContext_PutValue( ctx, p );
	}
}
static void DaoCdata_SetField( DaoValue *self, DaoContext *ctx, DString *name, DaoValue *value )
{
	DaoTypeBase *typer = self->xCdata.typer;
	DaoValue *func = NULL;
	DString_SetMBS( ctx->process->mbstring, "." );
	DString_Append( ctx->process->mbstring, name );
	DString_AppendMBS( ctx->process->mbstring, "=" );
	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	func = DaoFindFunction( typer, ctx->process->mbstring );
	if( func ) func = (DaoValue*) DRoutine_Resolve( func, self, & value, 1, DVM_CALL );
	if( func == NULL ){
		DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, name->mbs );
		return;
	}
	DaoFunction_Call( (DaoFunction*) func, ctx, self, & value, 1 );
}
static void DaoCdata_GetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid )
{
	DaoTypeBase *typer = self0->xCdata.typer;
	DaoCdata *self = & self0->xCdata;

	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	if( self->buffer && pid->type >=DAO_INTEGER && pid->type <=DAO_DOUBLE){
		int id = DaoValue_GetInteger( pid );
		self->data = self->buffer;
		if( self->size && ( id <0 || id > self->size ) ){
			DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "index out of range" );
			return;
		}
		if( self->memsize ){
			self->data = (void*)( (char*)self->buffer + id * self->memsize );
		}else{
			self->data = ((void**)self->buffer)[id];
		}
		DaoContext_PutValue( ctx, self0 );
	}else{
		DaoValue *func = NULL;
		func = DaoFindFunction2( typer, "[]" );
		if( func ) func = (DaoValue*) DRoutine_Resolve( func, self0, & pid, 1, DVM_CALL );
		if( func == NULL ){
			DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, "" );
			return;
		}
		DaoFunction_Call( (DaoFunction*) func, ctx, self0, & pid, 1 );
	}
}
static void DaoCdata_SetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid, DaoValue *value )
{
	DaoTypeBase *typer = self0->xCdata.typer;
	DaoValue *func = NULL;
	DaoValue *p[2];

	DString_SetMBS( ctx->process->mbstring, "[]=" );
	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	func = DaoFindFunction( typer, ctx->process->mbstring );
	if( func ){
		p[0] = pid;
		p[1] = value;
		func = (DaoValue*) DRoutine_Resolve( func, self0, p, 2, DVM_CALL );
	}
	if( func == NULL ){
		DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, "" );
		return;
	}
	DaoFunction_Call( (DaoFunction*) func, ctx, self0, p, 2 );
}
static void DaoCdata_GetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N )
{
	DaoTypeBase *typer = self->xCdata.typer;
	DaoValue *func = NULL;
	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	if( N == 1 ){
		DaoCdata_GetItem1( self, ctx, ids[0] );
		return;
	}
	func = DaoFindFunction2( typer, "[]" );
	if( func ) func = (DaoValue*) DRoutine_Resolve( func, self, ids, N, DVM_CALL );
	if( func == NULL ){
		DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, "" );
		return;
	}
	DaoFunction_Call( (DaoFunction*) func, ctx, self, ids, N );
}
static void DaoCdata_SetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N, DaoValue *value )
{
	DaoTypeBase *typer = self->xCdata.typer;
	DaoValue *func = NULL;
	DaoValue *p[ DAO_MAX_PARAM ];
	if( ctx->vmSpace->options & DAO_EXEC_SAFE ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "not permitted" );
		return;
	}
	if( N == 1 ){
		DaoCdata_SetItem1( self, ctx, ids[0], value );
		return;
	}
	func = DaoFindFunction2( typer, "[]=" );
	if( func ){
		memcpy( p, ids, N*sizeof(DaoValue*) );
		p[N] = value;
		func = (DaoValue*) DRoutine_Resolve( func, self, p, N+1, DVM_CALL );
	}
	if( func == NULL ){
		DaoContext_RaiseException( ctx, DAO_ERROR_FIELD_NOTEXIST, "" );
		return;
	}
	DaoFunction_Call( (DaoFunction*) func, ctx, self, p, N+1 );
}

DaoCdataCore* DaoCdataCore_New()
{
	DaoCdataCore *self = (DaoCdataCore*) dao_calloc( 1, sizeof(DaoCdataCore) );
	self->GetField = DaoCdata_GetField;
	self->SetField = DaoCdata_SetField;
	self->GetItem = DaoCdata_GetItem;
	self->SetItem = DaoCdata_SetItem;
	self->Print = DaoValue_Print;
	self->Copy = DaoValue_NoCopy;
	return self;
}
void DaoTypeCdata_SetMethods( DaoTypeBase *self )
{
	self->Delete = (FuncPtrDel)DaoCdata_Delete;
}

void DaoBuffer_Resize( DaoCdata *self, int size )
{
	self->size = size;
	if( self->size + 1 >= self->bufsize ){
		self->bufsize = self->size + self->bufsize * 0.1 + 1;
		self->buffer = dao_realloc( self->buffer, self->bufsize );
	}else if( self->size < self->bufsize * 0.75 ){
		self->bufsize = self->bufsize * 0.8 + 1;
		self->buffer = dao_realloc( self->buffer, self->bufsize );
	}
	self->data = self->buffer;
}
static void DaoBuf_New( DaoContext *ctx, DaoValue *p[], int N )
{
	int size = p[0]->xInteger.value;
	DaoCdata *self = DaoCdata_New( NULL, NULL );
	self->attribs |= DAO_CDATA_FREE;
	DaoContext_PutValue( ctx, (DaoValue*) self );
	if( ( ctx->vmSpace->options & DAO_EXEC_SAFE ) && size > 1000 ){
		DaoContext_RaiseException( ctx, DAO_ERROR,
				"not permitted to create large buffer object in safe running mode" );
		return;
	}
	DaoBuffer_Resize( self, size );
}
static void DaoBuf_Size( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	DaoContext_PutInteger( ctx, self->size );
}
static void DaoBuf_Resize( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( ( ctx->vmSpace->options & DAO_EXEC_SAFE ) && p[1]->xInteger.value > 1000 ){
		DaoContext_RaiseException( ctx, DAO_ERROR,
				"not permitted to create large buffer object in safe running mode" );
		return;
	}
	DaoBuffer_Resize( self, p[1]->xInteger.value );
}
static void DaoBuf_CopyData( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	DaoCdata *cdat = & p[1]->xCdata;
	if( cdat->bufsize == 0 ){
		DaoContext_RaiseException( ctx, DAO_ERROR_PARAM, "invalid value" );
		return;
	}
	if( self->bufsize < cdat->size ) DaoBuffer_Resize( self, cdat->size );
	memcpy( self->buffer, cdat->buffer, cdat->size );
	self->size = cdat->size;
}
static void DaoBuf_GetString( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	DString *str = DaoContext_PutMBString( ctx, "" );
	if( p[1]->xEnum.value == 0 ){
		DString_Resize( str, self->size );
		memcpy( str->mbs, self->buffer, self->size );
	}else{
		DString_ToWCS( str );
		DString_Resize( str, self->size / sizeof( wchar_t ) );
		memcpy( str->wcs, self->buffer, str->size * sizeof( wchar_t ) );
	}
}
static void DaoBuf_SetString( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	DString *str = p[1]->xString.data;
	if( str->mbs ){
		DaoBuffer_Resize( self, str->size );
		memcpy( self->buffer, str->mbs, str->size );
	}else{
		DaoBuffer_Resize( self, str->size * sizeof(wchar_t) );
		memcpy( self->buffer, str->wcs, str->size * sizeof(wchar_t) );
	}
}
static int DaoBuf_CheckRange( DaoCdata *self, int i, int m, DaoContext *ctx )
{
	if( i*m >=0 && i*m < self->size ) return 0;
	DaoContext_RaiseException( ctx, DAO_ERROR_INDEX_OUTOFRANGE, "" );
	return 1;
}
static void DaoBuf_GetByte( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	int i = p[1]->xInteger.value;
	int it = ( p[2]->xEnum.value == 0 )? ((signed char*)self->buffer)[i] : ((unsigned char*)self->buffer)[i];
	if( DaoBuf_CheckRange( self, i, sizeof(char), ctx ) ) return;
	DaoContext_PutInteger( ctx, it );
}
static void DaoBuf_GetShort( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	int i = p[1]->xInteger.value;
	int it = ( p[2]->xEnum.value == 0 )? ((signed short*)self->buffer)[i] : ((unsigned short*)self->buffer)[i];
	if( DaoBuf_CheckRange( self, i, sizeof(short), ctx ) ) return;
	DaoContext_PutInteger( ctx, it );
}
static void DaoBuf_GetInt( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	int i = p[1]->xInteger.value;
	int it = ( p[2]->xEnum.value == 0 )? ((signed int*)self->buffer)[i] : ((unsigned int*)self->buffer)[i];
	if( DaoBuf_CheckRange( self, i, sizeof(int), ctx ) ) return;
	DaoContext_PutInteger( ctx, it );
}
static void DaoBuf_GetFloat( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( DaoBuf_CheckRange( self, p[1]->xInteger.value, sizeof(float), ctx ) ) return;
	DaoContext_PutFloat( ctx, ((float*)self->buffer)[ p[1]->xInteger.value ] );
}
static void DaoBuf_GetDouble( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( DaoBuf_CheckRange( self, p[1]->xInteger.value, sizeof(double), ctx ) ) return;
	DaoContext_PutDouble( ctx, ((double*)self->buffer)[ p[1]->xInteger.value ] );
}
static void DaoBuf_SetByte( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( DaoBuf_CheckRange( self, p[1]->xInteger.value, sizeof(char), ctx ) ) return;
	if( p[3]->xEnum.value == 0 )
		((signed char*)self->buffer)[ p[1]->xInteger.value ] = (signed char)p[2]->xInteger.value;
	else
		((unsigned char*)self->buffer)[ p[1]->xInteger.value ] = (unsigned char)p[2]->xInteger.value;
}
static void DaoBuf_SetShort( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( DaoBuf_CheckRange( self, p[1]->xInteger.value, sizeof(short), ctx ) ) return;
	if( p[3]->xEnum.value == 0 )
		((signed short*)self->buffer)[ p[1]->xInteger.value ] = (signed short)p[2]->xInteger.value;
	else
		((unsigned short*)self->buffer)[ p[1]->xInteger.value ] = (unsigned short)p[2]->xInteger.value;
}
static void DaoBuf_SetInt( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( DaoBuf_CheckRange( self, p[1]->xInteger.value, sizeof(int), ctx ) ) return;
	if( p[3]->xEnum.value == 0 )
		((signed int*)self->buffer)[ p[1]->xInteger.value ] = (signed int)p[2]->xInteger.value;
	else
		((unsigned int*)self->buffer)[ p[1]->xInteger.value ] = (unsigned int)p[2]->xInteger.value;
}
static void DaoBuf_SetFloat( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( DaoBuf_CheckRange( self, p[1]->xInteger.value, sizeof(float), ctx ) ) return;
	((float*)self->buffer)[ p[1]->xInteger.value ] = p[2]->xFloat.value;
}
static void DaoBuf_SetDouble( DaoContext *ctx, DaoValue *p[], int N )
{
	DaoCdata *self = & p[0]->xCdata;
	if( DaoBuf_CheckRange( self, p[1]->xInteger.value, sizeof(double), ctx ) ) return;
	((double*)self->buffer)[ p[1]->xInteger.value ] = p[2]->xDouble.value;
}
static DaoFuncItem cptrMeths[]=
{
	{  DaoBuf_New,       "cdata( size=0 )=>cdata" },
	{  DaoBuf_Size,      "size( self : cdata )=>int" },
	{  DaoBuf_Resize,    "resize( self : cdata, size :int )" },
	{  DaoBuf_CopyData,  "copydata( self : cdata, buf : cdata )" },
	{  DaoBuf_GetString, "getstring( self : cdata, type :enum<mbs, wcs> = $mbs )=>string" },
	{  DaoBuf_SetString, "setstring( self : cdata, str : string )" },
	{  DaoBuf_GetByte,   "getbyte( self : cdata, index : int, type :enum<signed, unsigned> = $signed )=>int" },
	{  DaoBuf_GetShort,  "getshort( self : cdata, index : int, type :enum<signed, unsigned> = $signed )=>int" },
	{  DaoBuf_GetInt,    "getint( self : cdata, index : int, type :enum<signed, unsigned> = $signed )=>int" },
	{  DaoBuf_GetFloat,  "getfloat( self : cdata, index : int )=>float" },
	{  DaoBuf_GetDouble, "getdouble( self : cdata, index : int )=>double" },
	{  DaoBuf_SetByte,   "setbyte( self : cdata, index : int, value: int, type :enum<signed, unsigned> = $signed)" },
	{  DaoBuf_SetShort,  "setshort( self : cdata, index : int, value: int, type :enum<signed, unsigned> = $signed)"},
	{  DaoBuf_SetInt,    "setint( self : cdata, index : int, value: int, type :enum<signed, unsigned> = $signed)" },
	{  DaoBuf_SetFloat,  "setfloat( self : cdata, index : int, value : float )" },
	{  DaoBuf_SetDouble, "setdouble( self : cdata, index : int, value : double )" },
	{ NULL, NULL },
};

DaoTypeBase cdataTyper =
{
	"cdata", NULL, NULL, (DaoFuncItem*) cptrMeths, {0}, {0},
	(FuncPtrDel)DaoCdata_Delete, NULL
};
DaoCdata cptrCdata = { DAO_CDATA, 0, DAO_DATA_CONST, 0, 1, 0, 
	NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0,0 };

void DaoNameValue_Delete( DaoNameValue *self )
{
	DString_Delete( self->name );
	DaoValue_Clear( & self->value );
	GC_DecRC( self->unitype );
	dao_free( self );
}
static DaoValue* DaoNameValue_Copy( DaoValue *self0, DaoContext *ctx, DMap *cycData )
{
	DaoNameValue *self = & self0->xNameValue;
	DaoNameValue *copy = DaoNameValue_New( self->name, self->value );
	copy->unitype = self->unitype;
	GC_IncRC( self->unitype );
	return (DaoValue*) copy;
}
static void DaoNameValue_Print( DaoValue *self0, DaoContext *ctx, DaoStream *stream, DMap *cycData )
{
	DaoNameValue *self = & self0->xNameValue;
	DaoStream_WriteString( stream, self->name );
	DaoStream_WriteMBS( stream, "=>" );
	DaoValue_Print( self->value, ctx, stream, cycData );
}
static DaoTypeCore namevaCore=
{
	0, NULL, NULL, NULL, NULL,
	DaoValue_GetField,
	DaoValue_SetField,
	DaoValue_GetItem,
	DaoValue_SetItem,
	DaoNameValue_Print,
	DaoNameValue_Copy
};
DaoTypeBase namevaTyper =
{
	"NameValue", & namevaCore, NULL, NULL, {0}, {0}, (FuncPtrDel) DaoNameValue_Delete, NULL
};
DaoNameValue* DaoNameValue_New( DString *name, DaoValue *value )
{
	DaoNameValue *self = (DaoNameValue*)dao_malloc( sizeof(DaoNameValue) );
	DaoValue_Init( self, DAO_PAR_NAMED );
	self->name = DString_Copy( name );
	self->unitype = NULL;
	self->value = NULL;
	DaoValue_Copy( value, & self->value );
	return self;
}

/* ---------------------
 * Dao Tuple
 * ---------------------*/
static int DaoTuple_GetIndex( DaoTuple *self, DaoContext *ctx, DString *name )
{
	DaoType *abtp = self->unitype;
	DNode *node = NULL;
	int id;
	if( abtp && abtp->mapNames ) node = MAP_Find( abtp->mapNames, name );
	if( node == NULL ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "invalid field" );
		return -1;
	}
	id = node->value.pInt;
	if( id <0 || id >= self->size ){
		DaoContext_RaiseException( ctx, DAO_ERROR, "invalid tuple" );
		return -1;
	}
	return id;
}
static void DaoTupleCore_GetField( DaoValue *self0, DaoContext *ctx, DString *name )
{
	DaoTuple *self = & self0->xTuple;
	int id = DaoTuple_GetIndex( self, ctx, name );
	if( id <0 ) return;
	DaoContext_PutReference( ctx, self->items[id] );
}
static void DaoTupleCore_SetField( DaoValue *self0, DaoContext *ctx, DString *name, DaoValue *value )
{
	DaoTuple *self = & self0->xTuple;
	DaoType *t, **type = self->unitype->nested->items.pType;
	int id = DaoTuple_GetIndex( self, ctx, name );
	if( id <0 ) return;
	t = type[id];
	if( t->tid == DAO_PAR_NAMED ) t = & t->aux->xType;
	if( DaoValue_Move( value, self->items + id, t ) ==0)
		DaoContext_RaiseException( ctx, DAO_ERROR, "type not matching" );
}
static void DaoTupleCore_GetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid )
{
	DaoTuple *self = & self0->xTuple;
	int ec = DAO_ERROR_INDEX;
	if( pid->type == DAO_NULL ){
		ec = 0;
		/* return a copy. TODO */
	}else if( pid->type >= DAO_INTEGER && pid->type <= DAO_DOUBLE ){
		int id = DaoValue_GetInteger( pid );
		if( id >=0 && id < self->size ){
			DaoContext_PutReference( ctx, self->items[id] );
			ec = 0;
		}else{
			ec = DAO_ERROR_INDEX_OUTOFRANGE;
		}
	}
	if( ec ) DaoContext_RaiseException( ctx, ec, "" );
}
static void DaoTupleCore_SetItem1( DaoValue *self0, DaoContext *ctx, DaoValue *pid, DaoValue *value )
{
	DaoTuple *self = & self0->xTuple;
	DaoType *t, **type = self->unitype->nested->items.pType;
	int ec = 0;
	if( pid->type >= DAO_INTEGER && pid->type <= DAO_DOUBLE ){
		int id = DaoValue_GetInteger( pid );
		if( id >=0 && id < self->size ){
			t = type[id];
			if( t->tid == DAO_PAR_NAMED ) t = & t->aux->xType;
			if( DaoValue_Move( value, self->items + id, t ) ==0 ) ec = DAO_ERROR_TYPE;
		}else{
			ec = DAO_ERROR_INDEX_OUTOFRANGE;
		}
	}else{
		ec = DAO_ERROR_INDEX;
	}
	if( ec ) DaoContext_RaiseException( ctx, ec, "" );
}
static void DaoTupleCore_GetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N )
{
	switch( N ){
	case 0 : DaoTupleCore_GetItem1( self, ctx, null ); break;
	case 1 : DaoTupleCore_GetItem1( self, ctx, ids[0] ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static void DaoTupleCore_SetItem( DaoValue *self, DaoContext *ctx, DaoValue *ids[], int N, DaoValue *value )
{
	switch( N ){
	case 0 : DaoTupleCore_SetItem1( self, ctx, null, value ); break;
	case 1 : DaoTupleCore_SetItem1( self, ctx, ids[0], value ); break;
	default : DaoContext_RaiseException( ctx, DAO_ERROR_INDEX, "not supported" );
	}
}
static DaoValue* DaoTupleCore_Copy( DaoValue *self0, DaoContext *ctx, DMap *cycData )
{
	DaoTuple *copy, *self = & self0->xTuple;
	DaoValue **data = self->items;

	if( cycData ){
		DNode *node = MAP_Find( cycData, self );
		if( node ) return node->value.pValue;
	}

	copy = DaoTuple_New( self->size );
	copy->unitype = self->unitype;
	GC_IncRC( copy->unitype );
	if( cycData ) MAP_Insert( cycData, self, copy );

	DaoCopyValues( copy->items, data, self->size, ctx, cycData );
	return (DaoValue*) copy;
}
static DaoTypeCore tupleCore=
{
	0, NULL, NULL, NULL, NULL,
	DaoTupleCore_GetField,
	DaoTupleCore_SetField,
	DaoTupleCore_GetItem,
	DaoTupleCore_SetItem,
	DaoListCore_Print,
	DaoTupleCore_Copy,
};
DaoTypeBase tupleTyper=
{
	"tuple", & tupleCore, NULL, NULL, {0}, {0}, (FuncPtrDel) DaoTuple_Delete, NULL
};
DaoTuple* DaoTuple_New( int size )
{
	DaoTuple *self = (DaoTuple*) dao_calloc( 1, sizeof(DaoTuple) + (size-1)*sizeof(DaoValue*) );
	DaoValue_Init( self, DAO_TUPLE );
	self->size = size;
	//self->meta = NULL;
	self->unitype = NULL;
	return self;
}
void DaoTuple_Delete( DaoTuple *self )
{
	int i;
	//if( self->meta ) GC_DecRC( self->meta );
	for(i=0; i<self->size; i++) GC_DecRC( self->items[i] );
	GC_DecRC( self->unitype );
	dao_free( self );
}

int  DaoTuple_Size( DaoTuple *self )
{
	return self->size;
}
void DaoTuple_SetItem( DaoTuple *self, DaoValue *it, int pos )
{
	DaoValue **val;
	if( pos <0 || pos >= self->size ) return;
	val = self->items + pos;
	if( self->unitype && self->unitype->nested->size ){
		DaoType *t = self->unitype->nested->items.pType[pos];
		if( t->tid == DAO_PAR_NAMED ) t = & t->aux->xType;
		DaoValue_Move( it, val, t );
	}else{
		DaoValue_Copy( it, val );
	}
}
DaoValue* DaoTuple_GetItem( DaoTuple *self, int pos )
{
	if( pos <0 || pos >= self->size ) return NULL;
	return self->items[pos];
}

static void DaoException_Init( DaoException *self, DaoTypeBase *typer );

DaoException* DaoException_New( DaoTypeBase *typer )
{
	DaoException *self = dao_malloc( sizeof(DaoException) );
	self->fromLine = 0;
	self->toLine = 0;
	self->routine = NULL;
	self->callers = DArray_New(0);
	self->lines = DArray_New(0);

	self->name = DString_New(1);
	self->info = DString_New(1);
	self->data = NULL;

	DaoException_Init( self, typer );
	return self;
}
DaoException* DaoException_New2( DaoTypeBase *typer, DaoValue *v )
{
	DaoException *self = DaoException_New( typer );
	DaoValue_Move( v, & self->data, NULL );
	return self;
}
void DaoException_Delete( DaoException *self )
{
	GC_DecRC( self->data );
	DString_Delete( self->name );
	DString_Delete( self->info );
	DArray_Delete( self->callers );
	DArray_Delete( self->lines );
	dao_free( self );
}

static void Dao_Exception_Get_name( DaoContext *ctx, DaoValue *p[], int n );
static void Dao_Exception_Set_name( DaoContext *ctx, DaoValue *p[], int n );
static void Dao_Exception_Get_info( DaoContext *ctx, DaoValue *p[], int n );
static void Dao_Exception_Set_info( DaoContext *ctx, DaoValue *p[], int n );
static void Dao_Exception_Get_data( DaoContext *ctx, DaoValue *p[], int n );
static void Dao_Exception_Set_data( DaoContext *ctx, DaoValue *p[], int n );
static void Dao_Exception_New( DaoContext *ctx, DaoValue *p[], int n );
static void Dao_Exception_New22( DaoContext *ctx, DaoValue *p[], int n );

static DaoFuncItem dao_Exception_Meths[] =
{
	{ Dao_Exception_Get_name, ".name( self : Exception )=>string" },
	{ Dao_Exception_Set_name, ".name=( self : Exception, name : string)" },
	{ Dao_Exception_Get_info, ".info( self : Exception )=>string" },
	{ Dao_Exception_Set_info, ".info=( self : Exception, info : string)" },
	{ Dao_Exception_Get_data, ".data( self : Exception )=>any" },
	{ Dao_Exception_Set_data, ".data=( self : Exception, data : any)" },
	{ Dao_Exception_New, "Exception( info = '' )=>Exception" },
	{ Dao_Exception_New22, "Exception( data : any )=>Exception" },
	/* for testing or demonstration */
	{ Dao_Exception_Get_info, "serialize( self : Exception )=>string" },
	//XXX { Dao_Exception_Get_info, "cast( self : Exception, @T<string> )=>@T" },
	{ NULL, NULL }
};

DaoTypeBase dao_Exception_Typer =
{ "Exception", NULL, NULL, dao_Exception_Meths,
	{ 0 }, { 0 }, (FuncPtrDel) DaoException_Delete, NULL };

static void Dao_Exception_Get_name( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoException* self = (DaoException*) p[0]->xCdata.data;
	DaoContext_PutString( ctx, self->name );
}
static void Dao_Exception_Set_name( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoException* self = (DaoException*) p[0]->xCdata.data;
	DString *name = p[1]->xString.data;
	DString_Assign( self->name, name );
}
static void Dao_Exception_Get_info( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoException* self = (DaoException*) p[0]->xCdata.data;
	DaoContext_PutString( ctx, self->info );
}
static void Dao_Exception_Set_info( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoException* self = (DaoException*) p[0]->xCdata.data;
	DString_Assign( self->info, p[1]->xString.data );
}
static void Dao_Exception_Get_data( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoException* self = (DaoException*) p[0]->xCdata.data;
	DaoContext_PutValue( ctx, self->data );
}
static void Dao_Exception_Set_data( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoException* self = (DaoException*) p[0]->xCdata.data;
	DaoValue_Move( p[1], & self->data, NULL );
}
static void Dao_Exception_New( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoTypeBase *typer = ctx->regTypes[ ctx->vmc->c ]->typer;
	DaoException *self = (DaoException*)DaoException_New( typer );
	if( n ) DString_Assign( self->info, p[0]->xString.data );
	DaoContext_PutCdata( ctx, self, typer );
}
static void Dao_Exception_New22( DaoContext *ctx, DaoValue *p[], int n )
{
	DaoTypeBase *typer = ctx->regTypes[ ctx->vmc->c ]->typer;
	DaoException *self = (DaoException*)DaoException_New2( typer, p[0] );
	DaoContext_PutCdata( ctx, self, typer );
}

static DaoFuncItem dao_ExceptionNone_Meths[] =
{
	{ Dao_Exception_New, "None( info = '' )=>None" },
	{ Dao_Exception_New22, "None( data : any )=>None" },
	{ NULL, NULL }
};
DaoTypeBase dao_ExceptionNone_Typer =
{
	"None", NULL, NULL, dao_ExceptionNone_Meths,
	{ & dao_Exception_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ExceptionAny_Meths[] =
{
	{ Dao_Exception_New, "Any( info = '' )=>Any" },
	{ Dao_Exception_New22, "Any( data : any )=>Any" },
	{ NULL, NULL }
};
DaoTypeBase dao_ExceptionAny_Typer =
{
	"Any", NULL, NULL, dao_ExceptionAny_Meths,
	{ & dao_Exception_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ExceptionWarning_Meths[] =
{
	{ Dao_Exception_New, "Warning( info = '' )=>Warning" },
	{ Dao_Exception_New22, "Warning( data : any )=>Warning" },
	{ NULL, NULL }
};
DaoTypeBase dao_ExceptionWarning_Typer =
{
	"Warning", NULL, NULL, dao_ExceptionWarning_Meths,
	{ & dao_Exception_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ExceptionError_Meths[] =
{
	{ Dao_Exception_New, "Error( info = '' )=>Error" },
	{ Dao_Exception_New22, "Error( data : any )=>Error" },
	{ NULL, NULL }
};
DaoTypeBase dao_ExceptionError_Typer =
{
	"Error", NULL, NULL, dao_ExceptionError_Meths,
	{ & dao_Exception_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ErrorField_Meths[] =
{
	{ Dao_Exception_New, "Field( info = '' )=>Field" },
	{ Dao_Exception_New22, "Field( data : any )=>Field" },
	{ NULL, NULL }
};
DaoTypeBase dao_ErrorField_Typer =
{
	"Field", NULL, NULL, dao_ErrorField_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_NotExist_Meths[] =
{
	{ Dao_Exception_New, "NotExist( info = '' )=>NotExist" },
	{ Dao_Exception_New22, "NotExist( data : any )=>NotExist" },
	{ NULL, NULL }
};
DaoTypeBase dao_FieldNotExist_Typer =
{
	"NotExist", NULL, NULL, dao_NotExist_Meths,
	{ & dao_ErrorField_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_FieldNotPermit_Meths[] =
{
	{ Dao_Exception_New, "NotPermit( info = '' )=>NotPermit" },
	{ Dao_Exception_New22, "NotPermit( data : any )=>NotPermit" },
	{ NULL, NULL }
};
DaoTypeBase dao_FieldNotPermit_Typer =
{
	"NotPermit", NULL, NULL, dao_FieldNotPermit_Meths,
	{ & dao_ErrorField_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ErrorFloat_Meths[] =
{
	{ Dao_Exception_New, "Float( info = '' )=>Float" },
	{ Dao_Exception_New22, "Float( data : any )=>Float" },
	{ NULL, NULL }
};
DaoTypeBase dao_ErrorFloat_Typer =
{
	"Float", NULL, NULL, dao_ErrorFloat_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_FloatDivByZero_Meths[] =
{
	{ Dao_Exception_New, "DivByZero( info = '' )=>DivByZero" },
	{ Dao_Exception_New22, "DivByZero( data : any )=>DivByZero" },
	{ NULL, NULL }
};
DaoTypeBase dao_FloatDivByZero_Typer =
{
	"DivByZero", NULL, NULL, dao_FloatDivByZero_Meths,
	{ & dao_ErrorFloat_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_FloatOverFlow_Meths[] =
{
	{ Dao_Exception_New, "OverFlow( info = '' )=>OverFlow" },
	{ Dao_Exception_New22, "OverFlow( data : any )=>OverFlow" },
	{ NULL, NULL }
};
DaoTypeBase dao_FloatOverFlow_Typer =
{
	"OverFlow", NULL, NULL, dao_FloatOverFlow_Meths,
	{ & dao_ErrorFloat_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_FloatUnderFlow_Meths[] =
{
	{ Dao_Exception_New, "UnderFlow( info = '' )=>UnderFlow" },
	{ Dao_Exception_New22, "UnderFlow( data : any )=>UnderFlow" },
	{ NULL, NULL }
};
DaoTypeBase dao_FloatUnderFlow_Typer =
{
	"UnderFlow", NULL, NULL, dao_FloatUnderFlow_Meths,
	{ & dao_ErrorFloat_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ErrorIndex_Meths[] =
{
	{ Dao_Exception_New, "Index( info = '' )=>Index" },
	{ Dao_Exception_New22, "Index( data : any )=>Index" },
	{ NULL, NULL }
};
DaoTypeBase dao_ErrorIndex_Typer =
{
	"Index", NULL, NULL, dao_ErrorIndex_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_IndexOutOfRange_Meths[] =
{
	{ Dao_Exception_New, "OutOfRange( info = '' )=>OutOfRange" },
	{ Dao_Exception_New22, "OutOfRange( data : any )=>OutOfRange" },
	{ NULL, NULL }
};
DaoTypeBase dao_IndexOutOfRange_Typer =
{
	"OutOfRange", NULL, NULL, dao_IndexOutOfRange_Meths,
	{ & dao_ErrorIndex_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ErrorKey_Meths[] =
{
	{ Dao_Exception_New, "Key( info = '' )=>Key" },
	{ Dao_Exception_New22, "Key( data : any )=>Key" },
	{ NULL, NULL }
};
DaoTypeBase dao_ErrorKey_Typer =
{
	"Key", NULL, NULL, dao_ErrorKey_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

DaoTypeBase dao_KeyNotExist_Typer =
{
	"NotExist", NULL, NULL, dao_NotExist_Meths,
	{ & dao_ErrorKey_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ErrorParam_Meths[] =
{
	{ Dao_Exception_New, "Param( info = '' )=>Param" },
	{ Dao_Exception_New22, "Param( data : any )=>Param" },
	{ NULL, NULL }
};
DaoTypeBase dao_ErrorParam_Typer =
{
	"Param", NULL, NULL, dao_ErrorParam_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_Syntax_Meths[] =
{
	{ Dao_Exception_New, "Syntax( info = '' )=>Syntax" },
	{ Dao_Exception_New22, "Syntax( data : any )=>Syntax" },
	{ NULL, NULL }
};
DaoTypeBase dao_WarningSyntax_Typer =
{
	"Syntax", NULL, NULL, dao_Syntax_Meths,
	{ & dao_ExceptionWarning_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};
DaoTypeBase dao_ErrorSyntax_Typer =
{
	"Syntax", NULL, NULL, dao_Syntax_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_ErrorType_Meths[] =
{
	{ Dao_Exception_New, "Type( info = '' )=>Type" },
	{ Dao_Exception_New22, "Type( data : any )=>Type" },
	{ NULL, NULL }
};
DaoTypeBase dao_ErrorType_Typer =
{
	"Type", NULL, NULL, dao_ErrorType_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static DaoFuncItem dao_Value_Meths[] =
{
	{ Dao_Exception_New, "Value( info = '' )=>Value" },
	{ Dao_Exception_New22, "Value( data : any )=>Value" },
	{ NULL, NULL }
};
DaoTypeBase dao_WarningValue_Typer =
{
	"Value", NULL, NULL, dao_Value_Meths,
	{ & dao_ExceptionWarning_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};
DaoTypeBase dao_ErrorValue_Typer =
{
	"Value", NULL, NULL, dao_Value_Meths,
	{ & dao_ExceptionError_Typer, NULL }, {0},
	(FuncPtrDel) DaoException_Delete, NULL
};

static FuncPtrTest DaoTypeBase_GetDeleteTest( DaoTypeBase *typer )
{
	FuncPtrTest fptr;
	int i;
	if( typer->DelTest ) return typer->DelTest;
	for(i=0; i<DAO_MAX_CDATA_SUPER; i++){
		if( typer->supers[i] == NULL ) break;
		fptr = DaoTypeBase_GetDeleteTest( typer->supers[i] );
		if( fptr ) return fptr;
	}
	return NULL;
}
DaoType* DaoCdata_WrapType( DaoNamespace *ns, DaoTypeBase *typer )
{
	DaoCdataCore *plgCore;
	DaoCdata *cdata;
	DaoType *ctype_type;
	DaoType *cdata_type;

	cdata = DaoCdata_New( typer, NULL );
	cdata->type = DAO_CTYPE;
	cdata->trait |= DAO_DATA_NOCOPY;
	ctype_type = DaoType_New( typer->name, DAO_CTYPE, (DaoValue*)cdata, NULL );
	GC_ShiftRC( ctype_type, cdata->ctype );
	cdata->ctype = ctype_type;
	ctype_type->typer = typer;

	cdata_type = DaoType_New( typer->name, DAO_CDATA, (DaoValue*)cdata, NULL );
	cdata_type->typer = typer;
	cdata = DaoCdata_New( typer, NULL );
	GC_ShiftRC( cdata_type, cdata->ctype );
	cdata->ctype = cdata_type;
	cdata_type->value = (DaoValue*) cdata;
	cdata->trait |= DAO_DATA_CONST|DAO_DATA_NOCOPY;
	GC_IncRC( cdata );

	plgCore = DaoCdataCore_New();
	plgCore->abtype = cdata_type;
	plgCore->nspace = ns;
	plgCore->DelData = typer->Delete;
	plgCore->DelTest = DaoTypeBase_GetDeleteTest( typer );
	typer->priv = (DaoTypeCore*)plgCore;
	DaoTypeCdata_SetMethods( typer );
	return ctype_type;
}
void DaoException_Setup( DaoNamespace *ns )
{
	DaoType *exception = DaoCdata_WrapType( ns, & dao_Exception_Typer );
	DaoType *none = DaoCdata_WrapType( ns, & dao_ExceptionNone_Typer );
	DaoType *any = DaoCdata_WrapType( ns, & dao_ExceptionAny_Typer );
	DaoType *warning = DaoCdata_WrapType( ns, & dao_ExceptionWarning_Typer );
	DaoType *error = DaoCdata_WrapType( ns, & dao_ExceptionError_Typer );
	DaoType *field = DaoCdata_WrapType( ns, & dao_ErrorField_Typer );
	DaoType *fdnotexist = DaoCdata_WrapType( ns, & dao_FieldNotExist_Typer );
	DaoType *fdnotperm = DaoCdata_WrapType( ns, & dao_FieldNotPermit_Typer );
	DaoType *tfloat = DaoCdata_WrapType( ns, & dao_ErrorFloat_Typer );
	DaoType *fltzero = DaoCdata_WrapType( ns, & dao_FloatDivByZero_Typer );
	DaoType *fltoflow = DaoCdata_WrapType( ns, & dao_FloatOverFlow_Typer );
	DaoType *fltuflow = DaoCdata_WrapType( ns, & dao_FloatUnderFlow_Typer );
	DaoType *index = DaoCdata_WrapType( ns, & dao_ErrorIndex_Typer );
	DaoType *idorange = DaoCdata_WrapType( ns, & dao_IndexOutOfRange_Typer );
	DaoType *key = DaoCdata_WrapType( ns, & dao_ErrorKey_Typer );
	DaoType *keynotexist = DaoCdata_WrapType( ns, & dao_KeyNotExist_Typer );
	DaoType *param = DaoCdata_WrapType( ns, & dao_ErrorParam_Typer );
	DaoType *wsyntax = DaoCdata_WrapType( ns, & dao_WarningSyntax_Typer );
	DaoType *esyntax = DaoCdata_WrapType( ns, & dao_ErrorSyntax_Typer );
	DaoType *wvalue = DaoCdata_WrapType( ns, & dao_WarningValue_Typer );
	DaoType *evalue = DaoCdata_WrapType( ns, & dao_ErrorValue_Typer );
	DaoType *type = DaoCdata_WrapType( ns, & dao_ErrorType_Typer );

	DaoNamespace_AddConst( ns, exception->name, exception->aux, DAO_DATA_PUBLIC );
	DaoNamespace_AddType( ns, exception->name, exception );

	DaoNamespace_SetupValues( ns, & dao_Exception_Typer );
	DaoNamespace_SetupValues( ns, & dao_ExceptionNone_Typer );
	DaoNamespace_SetupValues( ns, & dao_ExceptionAny_Typer );
	DaoNamespace_SetupValues( ns, & dao_ExceptionWarning_Typer );
	DaoNamespace_SetupValues( ns, & dao_ExceptionError_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorField_Typer );
	DaoNamespace_SetupValues( ns, & dao_FieldNotExist_Typer );
	DaoNamespace_SetupValues( ns, & dao_FieldNotPermit_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorFloat_Typer );
	DaoNamespace_SetupValues( ns, & dao_FloatDivByZero_Typer );
	DaoNamespace_SetupValues( ns, & dao_FloatOverFlow_Typer );
	DaoNamespace_SetupValues( ns, & dao_FloatUnderFlow_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorIndex_Typer );
	DaoNamespace_SetupValues( ns, & dao_IndexOutOfRange_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorKey_Typer );
	DaoNamespace_SetupValues( ns, & dao_KeyNotExist_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorParam_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorSyntax_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorValue_Typer );
	DaoNamespace_SetupValues( ns, & dao_ErrorType_Typer );
	DaoNamespace_SetupValues( ns, & dao_WarningSyntax_Typer );
	DaoNamespace_SetupValues( ns, & dao_WarningValue_Typer );

	/* setup hierarchicy of exception types: */
	DMap_Insert( exception->typer->priv->values, none->name, none->aux );
	DMap_Insert( exception->typer->priv->values, any->name, any->aux );
	DMap_Insert( exception->typer->priv->values, warning->name, warning->aux );
	DMap_Insert( exception->typer->priv->values, error->name, error->aux );
	DMap_Insert( error->typer->priv->values, field->name, field->aux );
	DMap_Insert( field->typer->priv->values, fdnotexist->name, fdnotexist->aux );
	DMap_Insert( field->typer->priv->values, fdnotperm->name, fdnotperm->aux );
	DMap_Insert( error->typer->priv->values, tfloat->name, tfloat->aux );
	DMap_Insert( tfloat->typer->priv->values, fltzero->name, fltzero->aux );
	DMap_Insert( tfloat->typer->priv->values, fltoflow->name, fltoflow->aux );
	DMap_Insert( tfloat->typer->priv->values, fltuflow->name, fltuflow->aux );
	DMap_Insert( error->typer->priv->values, index->name, index->aux );
	DMap_Insert( index->typer->priv->values, idorange->name, idorange->aux );
	DMap_Insert( error->typer->priv->values, key->name, key->aux );
	DMap_Insert( key->typer->priv->values, keynotexist->name, keynotexist->aux );
	DMap_Insert( error->typer->priv->values, param->name, param->aux );
	DMap_Insert( error->typer->priv->values, esyntax->name, esyntax->aux );
	DMap_Insert( error->typer->priv->values, evalue->name, evalue->aux );
	DMap_Insert( error->typer->priv->values, type->name, type->aux );
	DMap_Insert( warning->typer->priv->values, wsyntax->name, wsyntax->aux );
	DMap_Insert( warning->typer->priv->values, wvalue->name, wvalue->aux );

	DaoNamespace_SetupMethods( ns, & dao_Exception_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ExceptionNone_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ExceptionAny_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ExceptionWarning_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ExceptionError_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorField_Typer );
	DaoNamespace_SetupMethods( ns, & dao_FieldNotExist_Typer );
	DaoNamespace_SetupMethods( ns, & dao_FieldNotPermit_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorFloat_Typer );
	DaoNamespace_SetupMethods( ns, & dao_FloatDivByZero_Typer );
	DaoNamespace_SetupMethods( ns, & dao_FloatOverFlow_Typer );
	DaoNamespace_SetupMethods( ns, & dao_FloatUnderFlow_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorIndex_Typer );
	DaoNamespace_SetupMethods( ns, & dao_IndexOutOfRange_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorKey_Typer );
	DaoNamespace_SetupMethods( ns, & dao_KeyNotExist_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorParam_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorSyntax_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorValue_Typer );
	DaoNamespace_SetupMethods( ns, & dao_ErrorType_Typer );
	DaoNamespace_SetupMethods( ns, & dao_WarningSyntax_Typer );
	DaoNamespace_SetupMethods( ns, & dao_WarningValue_Typer );
}
extern void DaoTypeBase_Free( DaoTypeBase *typer );
void DaoException_CleanUp()
{
	DaoTypeBase_Free( & dao_Exception_Typer );
	DaoTypeBase_Free( & dao_ExceptionNone_Typer );
	DaoTypeBase_Free( & dao_ExceptionAny_Typer );
	DaoTypeBase_Free( & dao_ExceptionWarning_Typer );
	DaoTypeBase_Free( & dao_ExceptionError_Typer );
	DaoTypeBase_Free( & dao_ErrorField_Typer );
	DaoTypeBase_Free( & dao_FieldNotExist_Typer );
	DaoTypeBase_Free( & dao_FieldNotPermit_Typer );
	DaoTypeBase_Free( & dao_ErrorFloat_Typer );
	DaoTypeBase_Free( & dao_FloatDivByZero_Typer );
	DaoTypeBase_Free( & dao_FloatOverFlow_Typer );
	DaoTypeBase_Free( & dao_FloatUnderFlow_Typer );
	DaoTypeBase_Free( & dao_ErrorIndex_Typer );
	DaoTypeBase_Free( & dao_IndexOutOfRange_Typer );
	DaoTypeBase_Free( & dao_ErrorKey_Typer );
	DaoTypeBase_Free( & dao_KeyNotExist_Typer );
	DaoTypeBase_Free( & dao_ErrorParam_Typer );
	DaoTypeBase_Free( & dao_WarningSyntax_Typer );
	DaoTypeBase_Free( & dao_ErrorSyntax_Typer );
	DaoTypeBase_Free( & dao_WarningValue_Typer );
	DaoTypeBase_Free( & dao_ErrorValue_Typer );
	DaoTypeBase_Free( & dao_ErrorType_Typer );
}
DaoTypeBase* DaoException_GetType( int type )
{
	switch( type ){
	case DAO_EXCEPTION : return & dao_Exception_Typer;
	case DAO_EXCEPTION_NONE : return & dao_ExceptionNone_Typer;
	case DAO_EXCEPTION_ANY : return & dao_ExceptionAny_Typer;
	case DAO_WARNING : return & dao_ExceptionWarning_Typer;
	case DAO_ERROR : return & dao_ExceptionError_Typer;
	case DAO_ERROR_FIELD : return & dao_ErrorField_Typer;
	case DAO_ERROR_FIELD_NOTEXIST : return & dao_FieldNotExist_Typer;
	case DAO_ERROR_FIELD_NOTPERMIT : return & dao_FieldNotPermit_Typer;
	case DAO_ERROR_FLOAT : return & dao_ErrorFloat_Typer;
	case DAO_ERROR_FLOAT_DIVBYZERO : return & dao_FloatDivByZero_Typer;
	case DAO_ERROR_FLOAT_OVERFLOW : return & dao_FloatOverFlow_Typer;
	case DAO_ERROR_FLOAT_UNDERFLOW : return & dao_FloatUnderFlow_Typer;
	case DAO_ERROR_INDEX : return & dao_ErrorIndex_Typer;
	case DAO_ERROR_INDEX_OUTOFRANGE : return & dao_IndexOutOfRange_Typer;
	case DAO_ERROR_KEY : return & dao_ErrorKey_Typer;
	case DAO_ERROR_KEY_NOTEXIST : return & dao_KeyNotExist_Typer;
	case DAO_ERROR_PARAM : return & dao_ErrorParam_Typer;
	case DAO_ERROR_SYNTAX : return & dao_ErrorSyntax_Typer;
	case DAO_ERROR_TYPE : return & dao_ErrorType_Typer;
	case DAO_ERROR_VALUE : return & dao_ErrorValue_Typer;
	case DAO_WARNING_SYNTAX : return & dao_WarningSyntax_Typer;
	case DAO_WARNING_VALUE : return & dao_WarningValue_Typer;
	default : break;
	}
	return & dao_Exception_Typer;
}
void DaoException_Init( DaoException *self, DaoTypeBase *typer )
{
	int i;
	for(i=DAO_EXCEPTION; i<ENDOF_BASIC_EXCEPT; i++){
		if( typer == DaoException_GetType( i ) ){
			DString_SetMBS( self->name, daoExceptionName[i] );
			DString_SetMBS( self->info, daoExceptionInfo[i] );
			return;
		}
	}
}

static void DaoFuture_Lib_Value( DaoContext *ctx, DaoValue *par[], int N )
{
	DaoProcess *proc = ctx->process;
	DaoFuture *self = (DaoFuture*) par[0];
	if( self->state == DAO_CALL_FINISHED ){
		DaoContext_PutValue( ctx, self->value );
		return;
	}
#if( defined DAO_WITH_THREAD && defined DAO_WITH_SYNCLASS )
	proc->status = DAO_VMPROC_SUSPENDED;
	proc->pauseType = DAO_VMP_ASYNC;
	proc->topFrame->entry = (short)(ctx->vmc - ctx->codes);
	DaoCallServer_Add( NULL, proc, self );
#endif
}
static DaoFuncItem futureMeths[] =
{
	{ DaoFuture_Lib_Value,      "value( self : future<@V> )=>@V" },
	{ NULL, NULL }
};
static void DaoFuture_Delete( DaoFuture *self )
{
	GC_DecRC( self->value );
	GC_DecRC( self->unitype );
	GC_DecRC( self->context );
	GC_DecRC( self->process );
	GC_DecRC( self->precondition );
}

static DaoTypeCore futureCore =
{
	0, NULL, NULL, NULL, NULL,
	DaoValue_SafeGetField,
	DaoValue_SafeSetField,
	DaoValue_GetItem,
	DaoValue_SetItem,
	DaoValue_Print,
	DaoValue_NoCopy,
};
DaoTypeBase futureTyper =
{
	"future", & futureCore, NULL, (DaoFuncItem*) futureMeths, {0}, {0},
	(FuncPtrDel) DaoFuture_Delete, NULL
};

DaoFuture* DaoFuture_New()
{
	DaoFuture *self = (DaoFuture*)dao_calloc(1,sizeof(DaoFuture));
	DaoValue_Init( self, DAO_FUTURE );
	self->state = DAO_CALL_QUEUED;
	return self;
}
