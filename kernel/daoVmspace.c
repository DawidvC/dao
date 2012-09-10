/*
// Dao Virtual Machine
// http://www.daovm.net
//
// Copyright (c) 2006-2012, Limin Fu
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

#include"string.h"
#include"ctype.h"
#include"locale.h"

#ifdef _MSC_VER
#include "direct.h"
#define getcwd _getcwd
#else
#include"unistd.h"
#endif

#include"daoNamespace.h"
#include"daoVmspace.h"
#include"daoParser.h"
#include"daoBytecode.h"
#include"daoStream.h"
#include"daoRoutine.h"
#include"daoNumtype.h"
#include"daoClass.h"
#include"daoObject.h"
#include"daoRoutine.h"
#include"daoRegex.h"
#include"daoStdlib.h"
#include"daoProcess.h"
#include"daoGC.h"
#include"daoSched.h"
#include"daoValue.h"

#ifdef DAO_WITH_THREAD
#include"daoThread.h"
#endif

extern int ObjectProfile[100];

DaoConfig daoConfig =
{
	1, /*cpu*/
	0, /*jit*/
	0, /*safe*/
	1, /*typedcode*/
	1, /*optimize*/
	0, /*incompile*/
	0, /*iscgi*/
	8, /*tabspace*/
	0, /*chindent*/
	0, /*mbs*/
	0  /*wcs*/
};

DaoVmSpace *mainVmSpace = NULL;
DaoProcess *mainProcess = NULL;

extern size_t FileChangedTime( const char *file );

static int TestFile( DaoVmSpace *vms, DString *fname )
{
	if( MAP_Find( vms->vfiles, fname ) ) return 1;
	if( MAP_Find( vms->vmodules, fname ) ) return 1;
	return Dao_IsFile( fname->mbs );
}
static int TestPath( DaoVmSpace *vms, DString *fname, int type )
{
	if( type == DAO_FILE_PATH ) return TestFile( vms, fname );
	return Dao_IsDir( fname->mbs );
}

static const char* const daoDllPrefix[] =
{
	"", "", "", "",
	"dao_", "libdao_", "lib"
};
static const char* const daoFileSuffix[] =
{
	".dao.o", ".dao.s", ".dao", DAO_DLL_SUFFIX,
	DAO_DLL_SUFFIX, DAO_DLL_SUFFIX, DAO_DLL_SUFFIX
	/* duplicated for automatically adding "dao/libdao_/lib" prefix; */
};
enum{
	DAO_MODULE_NONE,
	DAO_MODULE_DAO_O,
	DAO_MODULE_DAO_S,
	DAO_MODULE_DAO,
	DAO_MODULE_DLL,
	DAO_MODULE_ANY
};

#ifndef CHANGESET_ID
#define CHANGESET_ID "Undefined"
#endif

const char *const dao_copy_notice =
"  Dao Virtual Machine " DAO_VERSION "\n"
"  Built date: " __DATE__ "\n"
"  Changeset ID: " CHANGESET_ID "\n\n"
"  Copyright(C) 2006-2012, Fu Limin\n"
"  Dao is released under the terms of the Simplified BSD License\n"
"  Dao Language website: http://www.daovm.net\n"
;

static const char *const cmd_help =
"\n Usage: dao [options] script_file\n"
" Options:\n"
"   -h, --help:           print this help information;\n"
"   -v, --version:        print version information;\n"
"   -e, --eval:           evaluate command line codes;\n"
"   -d, --debug:          run in debug mode;\n"
"   -i, --interactive:    run in interactive mode;\n"
"   -c, --compile:        compile to bytecodes;\n"
"   -l, --list-code:      print compiled bytecodes;\n"
"   -j, --jit:            enable just-in-time compiling;\n"
"   -Ox:                  optimization level (x=0 or 1);\n"
;


extern DaoTypeBase  baseTyper;
extern DaoTypeBase  numberTyper;
extern DaoTypeBase  stringTyper;
extern DaoTypeBase  longTyper;
extern DaoTypeBase  enumTyper;
extern DaoTypeBase  listTyper;
extern DaoTypeBase  mapTyper;
extern DaoTypeBase  streamTyper;
extern DaoTypeBase  routTyper;
extern DaoTypeBase  funcTyper;
extern DaoTypeBase  interTyper;
extern DaoTypeBase  classTyper;
extern DaoTypeBase  objTyper;
extern DaoTypeBase  nsTyper;
extern DaoTypeBase  tupleTyper;
extern DaoTypeBase  namevaTyper;

extern DaoTypeBase  numarTyper;
extern DaoTypeBase  comTyper;
extern DaoTypeBase  abstypeTyper;
extern DaoTypeBase  rgxMatchTyper;
extern DaoTypeBase  futureTyper;

extern DaoTypeBase mutexTyper;
extern DaoTypeBase condvTyper;
extern DaoTypeBase semaTyper;

extern DaoTypeBase macroTyper;
extern DaoTypeBase regexTyper;
extern DaoTypeBase vmpTyper;
extern DaoTypeBase typeKernelTyper;

static DaoTypeBase vmsTyper=
{
	"vmspace", NULL, NULL, NULL, {0}, {0},
	(FuncPtrDel) DaoVmSpace_Delete, NULL
};

DaoTypeBase* DaoVmSpace_GetTyper( short type )
{
	switch( type ){
	case DAO_INTEGER  :  return & numberTyper;
	case DAO_FLOAT    :  return & numberTyper;
	case DAO_DOUBLE   :  return & numberTyper;
	case DAO_COMPLEX  :  return & comTyper;
	case DAO_LONG     :  return & longTyper;
	case DAO_ENUM     :  return & enumTyper;
	case DAO_STRING   :  return & stringTyper;
	case DAO_LIST     :  return & listTyper;
	case DAO_MAP      :  return & mapTyper;
	case DAO_PAR_NAMED : return & namevaTyper;
	case DAO_TUPLE     : return & tupleTyper;
#ifdef DAO_WITH_NUMARRAY
	case DAO_ARRAY  :  return & numarTyper;
#else
	case DAO_ARRAY  :  return & baseTyper;
#endif
	case DAO_CTYPE   :
	case DAO_CDATA   :  return & defaultCdataTyper;
	case DAO_ROUTINE   :  return & routTyper;
	case DAO_INTERFACE :  return & interTyper;
	case DAO_CLASS     :  return & classTyper;
	case DAO_OBJECT    :  return & objTyper;
	case DAO_NAMESPACE :  return & nsTyper;
	case DAO_PROCESS   :  return & vmpTyper;
	case DAO_VMSPACE   :  return & vmsTyper;
	case DAO_TYPE      :  return & abstypeTyper;
	case DAO_TYPEKERNEL : return & typeKernelTyper;
#ifdef DAO_WITH_MACRO
	case DAO_MACRO     :  return & macroTyper;
#endif
#ifdef DAO_WITH_CONCURRENT
	case DAO_FUTURE    :  return & futureTyper;
#endif
	default : break;
	}
	return & baseTyper;
}

void DaoVmSpace_SetOptions( DaoVmSpace *self, int options )
{
	self->options = options;
}
int DaoVmSpace_GetOptions( DaoVmSpace *self )
{
	return self->options;
}
DaoNamespace* DaoVmSpace_FindNamespace( DaoVmSpace *self, DString *name )
{
	daoint i;
	DNode *node;
	DaoNamespace *ns = NULL;
	DaoVmSpace_Lock( self );
	node = DMap_Find( self->nsModules, name );
	if( node ){
		ns = (DaoNamespace*) node->value.pValue;
		DArray_PushFront( self->loadedModules, ns );
	}
	DaoVmSpace_Unlock( self );
	return ns;
}
DaoNamespace* DaoVmSpace_GetNamespace( DaoVmSpace *self, const char *name )
{
	DString str = DString_WrapMBS( name );
	DaoNamespace *ns = DaoVmSpace_FindNamespace( self, & str );
	if( ns ) return ns;
	ns = DaoNamespace_New( self, name );
	DaoVmSpace_Lock( self );
	DMap_Insert( self->nsModules, & str, ns );
	DArray_PushFront( self->loadedModules, ns );
	DaoVmSpace_Unlock( self );
	return ns;
}
DaoNamespace* DaoVmSpace_MainNamespace( DaoVmSpace *self )
{
	return self->mainNamespace;
}
DaoProcess* DaoVmSpace_MainProcess( DaoVmSpace *self )
{
	return self->mainProcess;
}
DaoProcess* DaoVmSpace_AcquireProcess( DaoVmSpace *self )
{
	DaoProcess *proc = NULL;
#ifdef DAO_WITH_THREAD
	DMutex_Lock( & self->mutexProc );
#endif
	if( self->processes->size ){
		proc = (DaoProcess*) DArray_Back( self->processes );
		DArray_PopBack( self->processes );
	}else{
		proc = DaoProcess_New( self );
		DMap_Insert( self->allProcesses, proc, 0 );
	}
#ifdef DAO_WITH_THREAD
	DMutex_Unlock( & self->mutexProc );
#endif
	return proc;
}
void DaoVmSpace_ReleaseProcess( DaoVmSpace *self, DaoProcess *proc )
{
#ifdef DAO_WITH_THREAD
	DMutex_Lock( & self->mutexProc );
#endif
	if( DMap_Find( self->allProcesses, proc ) ){
		GC_DecRC( proc->future );
		proc->future = NULL;
#ifdef DAO_WITH_THREAD
		proc->condv = NULL;
		proc->mutex = NULL;
#endif
		if( proc->factory ) DArray_Clear( proc->factory );
		DaoProcess_PopFrames( proc, proc->firstFrame );
		DArray_PushBack( self->processes, proc );
	}
#ifdef DAO_WITH_THREAD
	DMutex_Unlock( & self->mutexProc );
#endif
}
DaoUserStream* DaoVmSpace_SetUserStdio( DaoVmSpace *self, DaoUserStream *stream )
{
	return DaoStream_SetUserStream( self->stdioStream, stream );
}
DaoUserStream* DaoVmSpace_SetUserStdError( DaoVmSpace *self, DaoUserStream *stream )
{
	if( self->errorStream == self->stdioStream ){
		self->errorStream = DaoStream_New();
		GC_ShiftRC( self->errorStream, self->stdioStream );
	}
	return DaoStream_SetUserStream( self->errorStream, stream );
}
DaoUserHandler* DaoVmSpace_SetUserHandler( DaoVmSpace *self, DaoUserHandler *handler )
{
	DaoUserHandler *hd = self->userHandler;
	self->userHandler = handler;
	return hd;
}
void DaoVmSpace_ReadLine( DaoVmSpace *self, ReadLine fptr )
{
	self->ReadLine = fptr;
}
void DaoVmSpace_AddHistory( DaoVmSpace *self, AddHistory fptr )
{
	self->AddHistory = fptr;
}
void DaoVmSpace_Stop( DaoVmSpace *self, int bl )
{
	self->stopit = bl;
}

static void DaoVmSpace_InitPath( DaoVmSpace *self )
{
	char *homedir = getenv( "HOME" );
	char *daodir = getenv( "DAO_DIR" );
	char pwd[512];
	if( self->hasAuxlibPath && self->hasSyslibPath ) return;
	getcwd( pwd, 511 );
	DaoVmSpace_SetPath( self, pwd );
	DaoVmSpace_AddPath( self, pwd );
	if( self->hasAuxlibPath && self->hasSyslibPath ) return;
	DaoVmSpace_AddPath( self, DAO_DIR );
	if( self->hasAuxlibPath && self->hasSyslibPath ) return;
	if( homedir ){
		DString *home = DString_NewMBS( homedir );
		if( homedir[home->size-1] != '/' ) DString_AppendChar( home, '/' );
		DString_AppendMBS( home, "dao" );
		DaoVmSpace_AddPath( self, home->mbs );
		DString_Delete( home );
	}
	if( self->hasAuxlibPath && self->hasSyslibPath ) return;
	if( daodir ) DaoVmSpace_AddPath( self, daodir );
}



DaoVmSpace* DaoVmSpace_New()
{
	DaoVmSpace *self = (DaoVmSpace*) dao_malloc( sizeof(DaoVmSpace) );
	DaoValue_Init( self, DAO_VMSPACE );
	self->stdioStream = DaoStream_New();
	self->options = 0;
	self->stopit = 0;
	self->safeTag = 1;
	self->evalCmdline = 0;
	self->hasAuxlibPath = 0;
	self->hasSyslibPath = 0;
	self->userHandler = NULL;
	self->mainSource = DString_New(1);
	self->vfiles = DHash_New(D_STRING,D_STRING);
	self->vmodules = DHash_New(D_STRING,0);
	self->nsModules = DHash_New(D_STRING,0);
	self->allTokens = DHash_New(D_STRING,0);
	self->pathWorking = DString_New(1);
	self->nameLoading = DArray_New(D_STRING);
	self->pathLoading = DArray_New(D_STRING);
	self->pathSearching = DArray_New(D_STRING);
	self->processes = DArray_New(0);
	self->allProcesses = DMap_New(D_VALUE,0);
	self->loadedModules = DArray_New(D_VALUE);
	self->preloadModules = NULL;

	if( daoConfig.safe ) self->options |= DAO_EXEC_SAFE;

#ifdef DAO_WITH_THREAD
	DMutex_Init( & self->mutexLoad );
	DMutex_Init( & self->mutexProc );
#endif

	self->nsInternal = NULL; /* need to be set for DaoNamespace_New() */
	self->nsInternal = DaoNamespace_New( self, "dao" );
	self->nsInternal->vmSpace = self;
	self->nsInternal->refCount += 1;
	DMap_Insert( self->nsModules, self->nsInternal->name, self->nsInternal );
	DArray_PushFront( self->loadedModules, self->nsInternal );

	self->mainNamespace = DaoNamespace_New( self, "MainNamespace" );
	self->mainNamespace->vmSpace = self;
	self->mainNamespace->refCount ++;
	self->stdioStream->refCount += 2;
	self->errorStream = self->stdioStream;

	self->ReadLine = NULL;
	self->AddHistory = NULL;

	self->mainProcess = DaoProcess_New( self );
	GC_IncRC( self->mainProcess );

	if( mainVmSpace ) DaoNamespace_AddParent( self->nsInternal, mainVmSpace->nsInternal );

	return self;
}
void DaoVmSpace_DeleteData( DaoVmSpace *self )
{
	GC_DecRC( self->nsInternal );
	GC_DecRC( self->mainNamespace );
	GC_DecRC( self->stdioStream );
	GC_DecRC( self->errorStream );
	DString_Delete( self->mainSource );
	DString_Delete( self->pathWorking );
	DArray_Delete( self->nameLoading );
	DArray_Delete( self->pathLoading );
	DArray_Delete( self->pathSearching );
	DArray_Delete( self->processes );
	DArray_Delete( self->loadedModules );
	DMap_Delete( self->vfiles );
	DMap_Delete( self->vmodules );
	DMap_Delete( self->allTokens );
	DMap_Delete( self->allProcesses );
	GC_DecRC( self->mainProcess );
	self->stdioStream = NULL;
	if( self->preloadModules ) DArray_Delete( self->preloadModules );
}
void DaoVmSpace_Delete( DaoVmSpace *self )
{
	if( self->stdioStream ) DaoVmSpace_DeleteData( self );
	DMap_Delete( self->nsModules );
#ifdef DAO_WITH_THREAD
	DMutex_Destroy( & self->mutexLoad );
	DMutex_Destroy( & self->mutexProc );
#endif
	dao_free( self );
}
void DaoVmSpace_Lock( DaoVmSpace *self )
{
#ifdef DAO_WITH_THREAD
	DMutex_Lock( & self->mutexLoad );
#endif
}
void DaoVmSpace_Unlock( DaoVmSpace *self )
{
#ifdef DAO_WITH_THREAD
	DMutex_Unlock( & self->mutexLoad );
#endif
}
static int DaoVmSpace_ReadSource( DaoVmSpace *self, DString *fname, DString *source )
{
	DNode *node = MAP_Find( self->vfiles, fname );
	/* printf( "reading %s\n", fname->mbs ); */
	if( node ){
		DString_Assign( source, node->value.pString );
		return 1;
	}
	if( DaoFile_ReadAll( fopen( fname->mbs, "r" ), source, 1 ) ) return 1;
	DaoStream_WriteMBS( self->errorStream, "ERROR: can not open file \"" );
	DaoStream_WriteMBS( self->errorStream, fname->mbs );
	DaoStream_WriteMBS( self->errorStream, "\".\n" );
	return 0;
}
void SplitByWhiteSpaces( const char *chs, DArray *tokens )
{
	DString temp = DString_WrapMBS( chs );
	DString *tok = DString_New(1);
	DString *str = & temp;
	daoint i, j, k=0, size = str->size;
	DArray_Clear( tokens );
	if( (j=DString_FindChar( str, '\1', k )) != MAXSIZE ){
		while( (j=DString_FindChar( str, '\1', k )) != MAXSIZE ){
			if( j > k ){
				DString_SubString( str, tok, k, j-k );
				DArray_Append( tokens, tok );
			}
			k = j + 1;
		}
		if( k < str->size ){
			DString_SubString( str, tok, k, str->size-k );
			DArray_Append( tokens, tok );
		}
		DString_Delete( tok );
		return;
	}
	for( i=0; i<size; i++){
		if( chs[i] == '\\' && i+1 < size ){
			DString_AppendChar( tok, chs[i+1] );
			++i;
			continue;
		}else if( isspace( chs[i] ) ){
			if( tok->size > 0 ){
				DArray_Append( tokens, tok );
				DString_Clear( tok );
			}
			continue;
		}
		DString_AppendChar( tok, chs[i] );
	}
	if( tok->size > 0 ) DArray_Append( tokens, tok );
	DString_Delete( tok );
}

int DaoVmSpace_TryInitJIT( DaoVmSpace *self, const char *module );
int DaoVmSpace_ParseOptions( DaoVmSpace *self, const char *options )
{
	DString *str = DString_New(1);
	DArray *array = DArray_New(D_STRING);
	DaoNamespace *ns;
	daoint i, j;

	SplitByWhiteSpaces( options, array );
	for( i=0; i<array->size; i++ ){
		DString *token = array->items.pString[i];
		if( self->evalCmdline ){
			DString_Append( self->mainSource, token );
			DString_AppendChar( self->mainSource, ' ' );
			continue;
		}
		if( token->mbs[0] =='-' && token->size >1 && token->mbs[1] =='-' ){
			if( strcmp( token->mbs, "--help" ) ==0 ){
				self->options |= DAO_EXEC_HELP;
			}else if( strcmp( token->mbs, "--version" ) ==0 ){
				self->options |= DAO_EXEC_VINFO;
			}else if( strcmp( token->mbs, "--eval" ) ==0 ){
				self->evalCmdline = 1;
				DString_Clear( self->mainSource );
			}else if( strcmp( token->mbs, "--debug" ) ==0 ){
				self->options |= DAO_EXEC_DEBUG;
			}else if( strcmp( token->mbs, "--interactive" ) ==0 ){
				self->options |= DAO_EXEC_INTERUN;
			}else if( strcmp( token->mbs, "--list-code" ) ==0 ){
				self->options |= DAO_EXEC_LIST_BC;
			}else if( strcmp( token->mbs, "--compile" ) ==0 ){
				self->options |= DAO_EXEC_COMP_BC;
			}else if( strcmp( token->mbs, "--jit" ) ==0 ){
				self->options |= DAO_EXEC_JIT;
				daoConfig.jit = 1;
			}else if( strstr( token->mbs, "--module=" ) == token->mbs ){
				if( self->preloadModules == NULL ) self->preloadModules = DArray_New(D_VALUE);
				if( (ns = DaoVmSpace_Load( self, token->mbs + 9 )) ){
					DArray_Append( self->preloadModules, ns );
					DArray_Append( self->mainNamespace->namespaces, ns );
					DaoNamespace_UpdateLookupTable( self->mainNamespace );
				}else{
					DaoStream_WriteMBS( self->errorStream, "Preloading failed for module: " );
					DaoStream_WriteMBS( self->errorStream, token->mbs );
					DaoStream_WriteMBS( self->errorStream, ";\n" );
				}
			}else if( token->size ){
				DaoStream_WriteMBS( self->errorStream, "Unknown option: " );
				DaoStream_WriteMBS( self->errorStream, token->mbs );
				DaoStream_WriteMBS( self->errorStream, ";\n" );
			}
		}else if( DString_MatchMBS( token, " ^ [%C_]+=.* ", NULL, NULL ) ){
			token = DString_DeepCopy( token );
			putenv( token->mbs );
		}else if( strcmp( token->mbs, "-O0" ) ==0 ){
			daoConfig.optimize = 0;
		}else if( strcmp( token->mbs, "-O1" ) ==0 ){
			daoConfig.optimize = 1;
		}else{
			daoint len = token->size;
			DString_Clear( str );
			for( j=0; j<len; j++ ){
				switch( token->mbs[j] ){
				case 'h' : self->options |= DAO_EXEC_HELP;      break;
				case 'v' : self->options |= DAO_EXEC_VINFO;     break;
				case 'd' : self->options |= DAO_EXEC_DEBUG;     break;
				case 'i' : self->options |= DAO_EXEC_INTERUN;   break;
				case 'l' : self->options |= DAO_EXEC_LIST_BC;   break;
				case 'c' : self->options |= DAO_EXEC_COMP_BC;   break;
				case 's' : self->options |= DAO_EXEC_SAFE;
						   daoConfig.safe = 1;
						   break;
				case 'n' : self->options |= DAO_EXEC_INCR_COMP;
						   daoConfig.incompile = 0;
						   break;
				case 'j' : self->options |= DAO_EXEC_JIT;
						   daoConfig.jit = 1;
						   break;
				case 'T' : self->options |= DAO_EXEC_NO_TC;
						   daoConfig.typedcode = 0;
						   break;
				case 'e' : self->evalCmdline = 1;
						   DString_Clear( self->mainSource );
						   break;
				case '-' : break;
				default :
						   if( token->mbs[j] ){
							   DString_AppendChar( str, token->mbs[j] );
							   DString_AppendChar( str, ' ' );
						   }
						   break;
				}
			}
			if( str->size > 0 ){
				DaoStream_WriteMBS( self->errorStream, "Unknown option: " );
				DaoStream_WriteMBS( self->errorStream, str->mbs );
				DaoStream_WriteMBS( self->errorStream, ";\n" );
			}
		}
	}
	DString_Delete( str );
	DArray_Delete( array );
	if( daoConfig.jit && dao_jit.Compile == NULL && DaoVmSpace_TryInitJIT( self, NULL ) == 0 ){
		DaoStream_WriteMBS( self->errorStream, "Failed to enable Just-In-Time compiling!\n" );
	}
	return 1;
}

static DaoValue* DaoParseNumber( const char *s, DaoValue *value )
{
	if( strchr( s, 'e' ) != NULL ){
		value->type = DAO_FLOAT;
		value->xFloat.value = strtod( s, 0 );
	}else if( strchr( s, 'E' ) != NULL ){
		value->type = DAO_DOUBLE;
		value->xDouble.value = strtod( s, 0 );
	}else if( strchr( s, '.' ) != NULL ){
		int len = strlen( s );
		if( strstr( s, "00" ) == s + (len-2) ){
			value->type = DAO_DOUBLE;
			value->xDouble.value = strtod( s, 0 );
		}else{
			value->type = DAO_FLOAT;
			value->xFloat.value = strtod( s, 0 );
		}
	}else{
		value->type = DAO_INTEGER;
		value->xInteger.value = strtod( s, 0 );
	}
	return value;
}

static void DaoVmSpace_MakePath( DaoVmSpace *self, DString *path );
static int DaoVmSpace_CompleteModuleName( DaoVmSpace *self, DString *fname );
static DaoNamespace* DaoVmSpace_LoadDaoByteCode( DaoVmSpace *self, DString *path, int run );
static DaoNamespace* DaoVmSpace_LoadDaoAssembly( DaoVmSpace *self, DString *path, int run );
static DaoNamespace* DaoVmSpace_LoadDaoModuleExt( DaoVmSpace *self, DString *p, DArray *a, int run );
static DaoNamespace* DaoVmSpace_LoadDllModule( DaoVmSpace *self, DString *libpath );

static void DaoVmSpace_ParseArguments( DaoVmSpace *self, DaoNamespace *ns,
		const char *file, DArray *args, DArray *argNames, DArray *argValues )
{
	DaoList *argv;
	DaoMap *cmdarg;
	DaoType *nested[2];
	DaoValue ival = {DAO_INTEGER};
	DaoValue sval1 = {DAO_STRING};
	DaoValue sval2 = {DAO_STRING};
	DaoValue *nkey = (DaoValue*) & ival;
	DaoValue *skey = (DaoValue*) & sval1;
	DaoValue *sval = (DaoValue*) & sval2;
	DString *str = DString_New(1);
	DString *key = DString_New(1);
	DString *val = DString_New(1);
	DArray *array = args;
	daoint i, tk, offset = 0;
	int eq = 0;

	skey->xString.data = key;
	sval->xString.data = val;

	argv = DaoList_New();
	cmdarg = DaoMap_New(0);
	nested[0] = dao_type_any;
	nested[1] = DaoNamespace_MakeType( ns, "string",DAO_STRING, NULL,NULL,0 );
	cmdarg->unitype = DaoNamespace_MakeType( ns, "map",DAO_MAP,NULL,nested,2);
	argv->unitype = DaoNamespace_MakeType( ns, "list",DAO_LIST,NULL,nested+1,1);
	GC_IncRC( cmdarg->unitype );
	GC_IncRC( argv->unitype );
	DString_SetMBS( str, "ARGV" );
	if( DaoNamespace_AddConst( ns, str, (DaoValue*) argv, DAO_DATA_PUBLIC ) < 0 ){
		DaoList_Delete( argv );
		i = DaoNamespace_FindConst( ns, str );
		argv = (DaoList*) DaoNamespace_GetConst( ns, i );
		if( argv == NULL || argv->type != DAO_LIST ) return;
		DaoList_Clear( argv );
	}
	if( ns == self->mainNamespace ){
		DaoVmSpace_Lock( self );
		DaoNamespace_AddConst( self->nsInternal, str, (DaoValue*) argv, DAO_DATA_PUBLIC );
		DaoVmSpace_Unlock( self );
	}
	DString_SetMBS( str, "CMDARG" );
	if( DaoNamespace_AddConst( ns, str, (DaoValue*) cmdarg, DAO_DATA_PUBLIC ) < 0 ){
		DaoMap_Delete( cmdarg );
		i = DaoNamespace_FindConst( ns, str );
		cmdarg = (DaoMap*) DaoNamespace_GetConst( ns, i );
		if( cmdarg == NULL || cmdarg->type != DAO_MAP ) return;
		DaoMap_Clear( cmdarg );
	}
	if( ns == self->mainNamespace ){
		DaoVmSpace_Lock( self );
		DaoNamespace_AddConst( self->nsInternal, str, (DaoValue*) cmdarg, DAO_DATA_PUBLIC );
		DaoVmSpace_Unlock( self );
	}

	if( array == NULL && file ){
		array = DArray_New(D_STRING);
		SplitByWhiteSpaces( file, array );
		DString_Assign( ns->name, array->items.pString[0] );
	}
	DString_Assign( val, array->items.pString[0] );
	DaoMap_Insert( cmdarg, nkey, sval );
	DaoVmSpace_MakePath( self, ns->name );
	DaoNamespace_SetName( ns, ns->name->mbs ); /* to update ns->path and ns->file; */
	for(i=0; i<array->size; i++){
		DString_Assign( val, array->items.pString[i] );
		DaoList_Append( argv, sval );
	}
	i = 1;
	while( i < array->size ){
		DString *s = array->items.pString[i];
		i ++;
		nkey->xInteger.value ++;
		offset = 0;
		if( s->mbs[0] == '-' ){
			offset += 1;
			if( s->mbs[1] == '-' ) offset += 1;
		}
		tk = DaoToken_Check( s->mbs+offset, s->size-offset, & eq );
		if( tk == DTOK_IDENTIFIER && s->mbs[eq+offset] == '=' ){
			DString_SubString( s, key, offset, eq );
			DString_SubString( s, val, eq+offset+1, s->size-offset-eq );
			DArray_Append( argNames, key );
			DArray_Append( argValues, val );

			DaoMap_Insert( cmdarg, skey, sval );
			DString_SubString( s, key, 0, eq );
			DaoMap_Insert( cmdarg, skey, sval );
			DaoMap_Insert( cmdarg, nkey, sval );
		}else if( tk == DTOK_IDENTIFIER && offset && i < array->size ){
			DString_SubString( s, key, offset, s->size-offset );
			DString_Assign( val, array->items.pString[i] );
			DArray_Append( argNames, key );
			DArray_Append( argValues, val );

			DaoMap_Insert( cmdarg, skey, sval );
			DString_Assign( key, s );
			DaoMap_Insert( cmdarg, skey, sval );
			DaoMap_Insert( cmdarg, nkey, sval );
			i += 1;
		}else{
			DString_Clear( key );
			DString_Assign( val, s );
			DArray_Append( argNames, key );
			DArray_Append( argValues, s );
			DaoMap_Insert( cmdarg, nkey, sval );
		}
	}
	if( args == NULL ) DArray_Delete( array );
	DString_Delete( key );
	DString_Delete( val );
	DString_Delete( str );
}
static void DaoVmSpace_ConvertArguments( DaoNamespace *ns, DArray *argNames, DArray *argValues )
{
	DaoValue ival = {DAO_INTEGER};
	DaoValue sval1 = {DAO_STRING};
	DaoValue sval2 = {DAO_STRING};
	DaoValue *nkey = (DaoValue*) & ival;
	DaoValue *skey = (DaoValue*) & sval1;
	DaoValue *sval = (DaoValue*) & sval2;
	DaoRoutine *rout = ns->mainRoutine;
	DaoType *abtp = rout->routType;
	DString *str, *key, *val;
	int i;
	if( argValues->size == 0 ) return;
	key = DString_New(1);
	val = DString_New(1);
	skey->xString.data = key;
	sval->xString.data = val;
	DaoList_Clear( ns->argParams );
	DString_SetMBS( key, "main" );
	i = ns ? DaoNamespace_FindConst( ns, key ) : -1;
	if( i >=0 ){
		nkey = DaoNamespace_GetConst( ns, i );
		/* It may has not been compiled if it is not called explicitly. */
		if( nkey->type == DAO_ROUTINE ){ // TODO: better handling
			DaoRoutine_Compile( & nkey->xRoutine );
			rout = & nkey->xRoutine;
			abtp = rout->routType;
		}
	}
	if( rout == NULL ){
		DString_Delete( key );
		DString_Delete( val );
		return;
	}
	for( i=0; i<argNames->size; i++ ){
		char *chars = argValues->items.pString[i]->mbs;
		if( chars[0] == '+' || chars[0] == '-' ) chars ++;
		nkey = sval;
		DString_Assign( val, argValues->items.pString[i] );
		if( DaoToken_IsNumber( chars, 0 ) ){
			DaoValue temp = {0};
			nkey = DaoParseNumber( chars, (DaoValue*) & temp );
		}
		if( argNames->items.pString[i]->size ){
			DString *S = argNames->items.pString[i];
			DaoNameValue *nameva = DaoNameValue_New( argNames->items.pString[i], nkey );
			DaoValue *tp = (DaoValue*) DaoNamespace_GetType( ns, nkey );
			DaoType *type = DaoNamespace_MakeType( ns, S->mbs, DAO_PAR_NAMED, tp, NULL, 0 );
			nameva->unitype = type;
			GC_IncRC( nameva->unitype );
			DaoList_Append( ns->argParams, (DaoValue*) nameva );
			nameva->trait |= DAO_VALUE_CONST;
		}else{
			DaoList_Append( ns->argParams, nkey );
		}
	}
	DString_Delete( key );
	DString_Delete( val );
}

DaoNamespace* DaoVmSpace_LoadEx( DaoVmSpace *self, const char *file, int run )
{
	DArray *args = DArray_New(D_STRING);
	DString *path = DString_New(1);
	DaoNamespace *ns = NULL;

	SplitByWhiteSpaces( file, args );
	DString_Assign( path, args->items.pString[0] );
	switch( DaoVmSpace_CompleteModuleName( self, path ) ){
	case DAO_MODULE_DAO_O : ns = DaoVmSpace_LoadDaoByteCode( self, path, 0 ); break;
	case DAO_MODULE_DAO_S : ns = DaoVmSpace_LoadDaoAssembly( self, path, 0 ); break;
	case DAO_MODULE_DAO : ns = DaoVmSpace_LoadDaoModuleExt( self, path, args, run ); break;
	case DAO_MODULE_DLL : ns = DaoVmSpace_LoadDllModule( self, path ); break;
	case DAO_MODULE_ANY : ns = DaoVmSpace_LoadDaoModuleExt( self, path, args, run ); break; /* any suffix */
	default : break;
	}
	DArray_Delete( args );
	DString_Delete( path );
	if( ns == NULL ) return 0;
	return ns;
}
DaoNamespace* DaoVmSpace_Load( DaoVmSpace *self, const char *file )
{
	return DaoVmSpace_LoadEx( self, file, 0 );
}
/* Link "ns" to the module/namespace corresponding to "mod". */
/* If the module "mod" is not loaded yet, it will be loaded first. */
/* Return the namespace corresponding to "mod". */
DaoNamespace* DaoVmSpace_LinkModule( DaoVmSpace *self, DaoNamespace *ns, const char *mod )
{
	DaoNamespace *modns = DaoVmSpace_Load( self, mod );
	if( modns == NULL ) return NULL;
	DaoNamespace_AddParent( ns, modns );
	return modns;
}

static int CheckCodeCompletion( DString *source, DArray *tokens )
{
	int i, bcount, cbcount, sbcount, tki = 0, completed = 1;
	DaoToken_Tokenize( tokens, source->mbs, 0, 1, 1 );
	if( tokens->size ) tki = tokens->items.pToken[tokens->size-1]->type;
	switch( tki ){
	case DTOK_LB :
	case DTOK_LCB :
	case DTOK_LSB :
	case DTOK_VBT_OPEN :
	case DTOK_CMT_OPEN :
	case DTOK_MBS_OPEN :
	case DTOK_WCS_OPEN :
		completed = 0;
		break;
	}
	if( tokens->size && completed ){
		bcount = sbcount = cbcount = 0;
		for(i=0; i<tokens->size; i++){
			DaoToken *tk = tokens->items.pToken[i];
			switch( tk->type ){
			case DTOK_LB : bcount --; break;
			case DTOK_RB : bcount ++; break;
			case DTOK_LCB : cbcount --; break;
			case DTOK_RCB : cbcount ++; break;
			case DTOK_LSB : sbcount --; break;
			case DTOK_RSB : sbcount ++; break;
			default : break;
			}
		}
		if( bcount <0 || sbcount <0 || cbcount <0 ) completed = 0;
	}
	return completed;
}
static void DaoVmSpace_Interun( DaoVmSpace *self, CallbackOnString callback )
{
	DaoValue *value;
	DaoNamespace *ns;
	DArray *tokens = DArray_New( D_TOKEN );
	DString *input = DString_New(1);
	const char *varRegex = "^ %s* = %s* %S+";
	const char *srcRegex = "^ %s* %w+ %. dao .* $";
	const char *sysRegex = "^ %\\ %s* %w+ %s* .* $";
	char *chs;
	int ch, newline = 0;
	DString_SetMBS( self->mainNamespace->name, "interactive codes" );
	self->mainNamespace->options |= DAO_NS_AUTO_GLOBAL;
	ns = DaoVmSpace_LinkModule( self, self->mainNamespace, "help" );
	value = ns ? DaoNamespace_FindData( ns, "help_message" ) : NULL;
	if( value && value->type == DAO_STRING ) printf( "%s\n", DaoValue_TryGetMBString( value ) );
	while(1){
		DString_Clear( input );
		DaoValue_Clear( self->mainProcess->stackValues );
		if( self->ReadLine ){
			chs = self->ReadLine( "(dao) " );
			if( chs == NULL ){
				printf( "\n" );
				break;
			}
			while( chs ){
				DString_AppendMBS( input, chs );
				DString_AppendChar( input, '\n' );
				dao_free( chs );
				if( CheckCodeCompletion( input, tokens ) ){
					DString_Trim( input );
					if( input->size && self->AddHistory ) self->AddHistory( input->mbs );
					break;
				}
				chs = self->ReadLine( "..... " );
			}
		}else{
			printf( "(dao) " );
			fflush( stdout );
			ch = getchar();
			if( ch == EOF ) break;
			while( ch != EOF ){
				if( ch == '\n' ){
					if( CheckCodeCompletion( input, tokens ) ) break;
					printf("..... ");
					fflush( stdout );
				}
				DString_AppendChar( input, (char)ch );
				ch = getchar();
			}
			if( ch == EOF ) clearerr( stdin );
			DString_Trim( input );
		}
		if( input->size == 0 ) continue;
		self->stopit = 0;
		if( strcmp( input->mbs, "q" ) == 0 ){
			break;
		}else if( DString_MatchMBS( input, sysRegex, NULL, NULL ) ){
			if( system( input->mbs+1 ) ==-1) printf( "shell command failed\n" );
		}else if( DString_MatchMBS( input, srcRegex, NULL, NULL ) ){
			DString_InsertMBS( input, "std.load(", 0, 0, 0 );
			DString_AppendMBS( input, ", 0, 1 )" );
			if( callback ){
				(*callback)( input->mbs );
				continue;
			}
			DaoProcess_Eval( self->mainProcess, self->mainNamespace, input->mbs, 1 );
		}else if( DString_MatchMBS( input, varRegex, NULL, NULL ) ){
			DString_ChangeMBS( input, "^ %s* = %s*", "", 0 );
			DString_InsertMBS( input, "return ", 0, 0, 0 );
			if( callback ){
				(*callback)( input->mbs );
				continue;
			}
			DaoProcess_Eval( self->mainProcess, self->mainNamespace, input->mbs, 1 );
		}else{
			if( callback ){
				(*callback)( input->mbs );
				continue;
			}
			DaoProcess_Eval( self->mainProcess, self->mainNamespace, input->mbs, 1 );
		}
#ifdef DAO_WITH_CONCURRENT
		DaoCallServer_Join();
#endif
		/*
		   printf( "%s\n", input->mbs );
		 */
	}
	self->mainNamespace->options &= ~DAO_NS_AUTO_GLOBAL;
	DString_Delete( input );
	DArray_Delete( tokens );
}

static void DaoVmSpace_ExeCmdArgs( DaoVmSpace *self )
{
	DaoNamespace *ns = self->mainNamespace;
	daoint i;
	if( self->options & DAO_EXEC_VINFO ){
		DaoStream_WriteNewLine( self->stdioStream );
		DaoStream_WriteMBS( self->stdioStream, dao_copy_notice );
		DaoStream_WriteNewLine( self->stdioStream );
	}
	if( self->options & DAO_EXEC_HELP )  DaoStream_WriteMBS( self->stdioStream, cmd_help );
	DaoStream_Flush( self->stdioStream );

	if( self->options & DAO_EXEC_LIST_BC ){
		for(i=ns->cstUser; i<ns->constants->size; i++){
			DaoValue *p = ns->constants->items.pConst[i]->value;
			if( p->type == DAO_ROUTINE && & p->xRoutine != ns->mainRoutine ){
				DaoRoutine_Compile( & p->xRoutine );
				DaoRoutine_PrintCode( & p->xRoutine, self->stdioStream );
			}else if( p->type == DAO_CLASS ){
				DaoClass_PrintCode( & p->xClass, self->stdioStream );
			}
		}
		DaoStream_Flush( self->stdioStream );
		if( ns->mainRoutine ) DaoRoutine_PrintCode( ns->mainRoutine, self->stdioStream );
		if( ( self->options & DAO_EXEC_INTERUN ) && self->userHandler == NULL )
			DaoVmSpace_Interun( self, NULL );
	}
}
int DaoVmSpace_RunMain( DaoVmSpace *self, const char *file )
{
	DString *sfile = DString_NewMBS( file ? file : "" );
	DaoNamespace *ns = self->mainNamespace;
	DaoProcess *vmp = self->mainProcess;
	DaoStream *io = self->errorStream;
	DaoRoutine *mainRoutine;
	DaoRoutine *rout = NULL;
	DaoValue **ps;
	DString *name;
	DArray *argNames;
	DArray *argValues;
	size_t tm = 0;
	daoint N;
	int i, j, res;

	if( file == NULL || file[0] ==0 || self->evalCmdline ){
		DArray_PushFront( self->nameLoading, self->pathWorking );
		DArray_PushFront( self->pathLoading, self->pathWorking );
		if( self->evalCmdline ){
			DaoRoutine *rout;
			DString_SetMBS( self->mainNamespace->name, "command line codes" );
			if( DaoProcess_Compile( vmp, ns, self->mainSource->mbs, 1 ) ==0 ) return 0;
			DaoVmSpace_ExeCmdArgs( self );
			rout = ns->mainRoutines->items.pRoutine[ ns->mainRoutines->size-1 ];
			if( DaoProcess_Call( vmp, rout, NULL, NULL, 0 ) ) return 0;
		}else{
			DaoVmSpace_ExeCmdArgs( self );
		}
		if( (self->options & DAO_EXEC_INTERUN) && self->userHandler == NULL )
			DaoVmSpace_Interun( self, NULL );
		return 1;
	}
	argNames = DArray_New(D_STRING);
	argValues = DArray_New(D_STRING);
	DaoVmSpace_ParseArguments( self, ns, file, NULL, argNames, argValues );
	DaoVmSpace_AddPath( self, ns->path->mbs );
	DArray_PushFront( self->nameLoading, ns->name );
	DArray_PushFront( self->pathLoading, ns->path );
	DArray_PushFront( self->loadedModules, ns );
	if( DMap_Find( self->nsModules, ns->name ) == NULL ){
		MAP_Insert( self->nsModules, ns->name, ns );
	}

	tm = FileChangedTime( ns->name->mbs );
	ns->time = tm;

	/* self->fileName may has been changed */
	res = DaoVmSpace_ReadSource( self, ns->name, self->mainSource );
	res = res && DaoProcess_Compile( vmp, ns, self->mainSource->mbs, 1 );
	if( res ) DaoVmSpace_ConvertArguments( ns, argNames, argValues );
	DArray_Delete( argNames );
	DArray_Delete( argValues );

	if( res == 0 ) return 0;

	if( self->options & DAO_EXEC_COMP_BC ){
		FILE *fout;
		DString *bytecodes = DString_New(1);
		DaoByteEncoder *encoder = DaoByteEncoder_New();

		DString_Append( bytecodes, ns->name );
		DString_AppendMBS( bytecodes, ".o" );
		fout = fopen( bytecodes->mbs, "w+" );
		bytecodes->size = 0;
		DaoByteEncoder_Encode( encoder, ns, bytecodes );
		DaoFile_WriteString( fout, bytecodes );
		DaoByteEncoder_Delete( encoder );
		DString_Delete( bytecodes );
		fclose( fout );
		return 1;
	}

	name = DString_New(1);
	mainRoutine = ns->mainRoutine;
	DString_SetMBS( name, "main" );
	i = DaoNamespace_FindConst( ns, name );
	DString_Delete( name );

	ps = ns->argParams->items.items.pValue;
	N = ns->argParams->items.size;
	if( i >=0 ){
		DaoValue *value = DaoNamespace_GetConst( ns, i );
		if( value->type == DAO_ROUTINE ){
			rout = DaoRoutine_ResolveX( (DaoRoutine*) value, NULL, ps, N, DVM_CALL );
		}
		if( rout == NULL && value->type == DAO_ROUTINE ){
			DaoRoutineBody *body = value->xRoutine.body;
			DString *routHelp = body ? body->routHelp : NULL;
			DaoStream_WriteMBS( io, "ERROR: invalid command line arguments.\n" );
			if( routHelp ) DaoStream_WriteString( io, routHelp );
			return 0;
		}
	}
	DaoVmSpace_ExeCmdArgs( self );
	/* always execute default ::main() routine first for initialization: */
	if( mainRoutine ){
		DaoProcess_PushRoutine( vmp, mainRoutine, NULL );
		DaoProcess_Execute( vmp );
	}
	/* check and execute explicitly defined main() routine  */
	if( rout != NULL ){
		if( DaoProcess_Call( vmp, rout, NULL, ps, N ) ){
			DaoStream_WriteMBS( io, "ERROR: invalid command line arguments.\n" );
			if( rout->body->routHelp ) DaoStream_WriteString( io, rout->body->routHelp );
			return 0;
		}
		DaoProcess_Execute( vmp );
	}
	if( (self->options & DAO_EXEC_INTERUN) && self->userHandler == NULL )
		DaoVmSpace_Interun( self, NULL );

	return 1;
}
static int DaoVmSpace_CompleteModuleName( DaoVmSpace *self, DString *fname )
{
	int i, modtype = DAO_MODULE_NONE;
	daoint slen = strlen( DAO_DLL_SUFFIX );
	daoint size;
	DString_ToMBS( fname );
	size = fname->size;
	if( size >6 && DString_FindMBS( fname, ".dao.o", 0 ) == size-6 ){
		DaoVmSpace_SearchPath( self, fname, DAO_FILE_PATH, 1 );
		if( TestFile( self, fname ) ) modtype = DAO_MODULE_DAO_O;
	}else if( size >6 && DString_FindMBS( fname, ".dao.s", 0 ) == size-6 ){
		DaoVmSpace_SearchPath( self, fname, DAO_FILE_PATH, 1 );
		if( TestFile( self, fname ) ) modtype = DAO_MODULE_DAO_S;
	}else if( size >4 && ( DString_FindMBS( fname, ".dao", 0 ) == size-4
				|| DString_FindMBS( fname, ".cgi", 0 ) == size-4 ) ){
		DaoVmSpace_SearchPath( self, fname, DAO_FILE_PATH, 1 );
		if( TestFile( self, fname ) ) modtype = DAO_MODULE_DAO;
	}else if( size > slen && DString_FindMBS( fname, DAO_DLL_SUFFIX, 0 ) == size - slen ){
		modtype = DAO_MODULE_DLL;
#ifdef UNIX
		if( DString_FindMBS( fname, ".dll", 0 ) == size-4 )
			DString_Erase( fname, size-4, 4 );
#ifdef MAC_OSX
		if( DString_FindMBS( fname, ".dylib", 0 ) != size-6 )
			DString_AppendMBS( fname, ".dylib" );
#else
		if( DString_FindMBS( fname, ".so", 0 ) != size-3 )
			DString_AppendMBS( fname, ".so" );
#endif
#elif WIN32
		if( DString_FindMBS( fname, ".so", 0 ) == size-3 )
			DString_Erase( fname, size-3, 3 );
		if( DString_FindMBS( fname, ".dll", 0 ) != size-4 )
			DString_AppendMBS( fname, ".dll" );
#endif
		DaoVmSpace_SearchPath( self, fname, DAO_FILE_PATH, 1 );
		if( TestFile( self, fname ) ) modtype = DAO_MODULE_DLL;
	}else{
		DString *fn = DString_New(1);
		DString *path = DString_New(1);
		DString *file = DString_New(1);
		daoint pos = fname->size;
		while( pos && (fname->mbs[pos-1] == '_' || isalnum( fname->mbs[pos-1] )) ) pos -= 1;
		if( pos && (fname->mbs[pos-1] == '/' || fname->mbs[pos-1] == '\\') ){
			DString_SubString( fname, path, 0, pos );
			DString_SubString( fname, file, pos, fname->size - pos );
		}else{
			DString_Assign( file, fname );
		}
		for(i=0; i<7; i++){
			if( i < DAO_MODULE_DLL ){
				DString_Assign( fn, fname );
			}else{
				if( strncmp( fname->mbs, "lib", 3 ) == 0 ) break;
				DString_Assign( fn, path );
				DString_AppendMBS( fn, daoDllPrefix[i] );
				DString_Append( fn, file );
			}
			DString_AppendMBS( fn, daoFileSuffix[i] );
			DaoVmSpace_SearchPath( self, fn, DAO_FILE_PATH, 1 );
#if 0
			printf( "%s %s\n", fn->mbs, self->nameLoading->items.pString[0]->mbs );
#endif
			/* skip the current file: reason, example, in gsl_vector.dao:
			   load gsl_vector require gsl_complex, gsl_block;
			   which will allow searching for gsl_vector.so, gsl_vector.dylib or gsl_vector.dll. */
			if( self->nameLoading->size && DString_EQ( fn, self->nameLoading->items.pString[0] ) ) continue;
			if( TestFile( self, fn ) ){
				modtype = i+1;
				if( modtype > DAO_MODULE_DLL ) modtype = DAO_MODULE_DLL;
				DString_Assign( fname, fn );
				break;
			}
		}
		if( modtype == DAO_MODULE_NONE ){
			DaoVmSpace_SearchPath( self, fname, DAO_FILE_PATH, 1 );
			if( TestFile( self, fname ) ) modtype = DAO_MODULE_ANY;
		}
		DString_Delete( fn );
		DString_Delete( path );
		DString_Delete( file );
	}
	return modtype;
}
static DaoNamespace* DaoVmSpace_LoadDaoByteCode( DaoVmSpace *self, DString *fname, int run )
{
	DaoStream_WriteMBS( self->errorStream, "ERROR: bytecode loader is not implemented.\n" );
	return NULL;
}
static DaoNamespace* DaoVmSpace_LoadDaoAssembly( DaoVmSpace *self, DString *fname, int run )
{
	DaoStream_WriteMBS( self->errorStream, "ERROR: assembly loader is not implemented.\n" );
	return NULL;
}
static void DaoVmSpace_PopLoadingNamePath( DaoVmSpace *self, int path )
{
	DaoVmSpace_Lock( self );
	if( path ) DArray_PopFront( self->pathLoading );
	DArray_PopFront( self->nameLoading );
	DaoVmSpace_Unlock( self );
}
/* Loading module in Dao source file.
 * The first time the module is loaded:
 * (1) its implicit main (codes outside of any class and function) is executed;
 * (2) then, its explicit main that matches with "args" will be executed.
 * The next time the module is loaded:
 * (1) its implicit main is executed, IF run != 0; (mainly for IDE)
 * (2) its explicit main that matches with "args" will be executed. */
DaoNamespace* DaoVmSpace_LoadDaoModuleExt( DaoVmSpace *self, DString *libpath, DArray *args, int run )
{
	DString *source = NULL;
	DArray *argNames = NULL, *argValues = NULL;
	DaoNamespace *ns = NULL, *ns2 = NULL;
	DaoParser *parser = NULL;
	DaoProcess *process;
	DString name;
	DNode *node;
	size_t tm = 0;
	daoint i = DString_FindMBS( libpath, "/addpath.dao", 0 );
	daoint j = DString_FindMBS( libpath, "/delpath.dao", 0 );
	daoint nsCount = self->loadedModules->size;
	int bl, m, poppath = 0;
	int cfgpath = i != MAXSIZE && i == libpath->size - 12;
	cfgpath = cfgpath || (j != MAXSIZE && j == libpath->size - 12);
	/*  XXX if cfgpath == true, only parsing? */

	if( args ){
		argNames = DArray_New(D_STRING);
		argValues = DArray_New(D_STRING);
	}

	ns = ns2 = DaoVmSpace_FindNamespace( self, libpath );

	tm = FileChangedTime( libpath->mbs );
	/* printf( "time = %lli,  %s  %p\n", tm, libpath->mbs, node ); */
	if( ns && ns->time >= tm ){
		if( args ) DaoVmSpace_ParseArguments( self, ns, NULL, args, argNames, argValues );
		if( run ) goto ExecuteImplicitMain;
		goto ExecuteExplicitMain;
	}

	source = DString_New(1);
	if( ! DaoVmSpace_ReadSource( self, libpath, source ) ) goto LaodingFailed;

	/*
	   printf("%p : loading %s\n", self, libpath->mbs );
	 */
	parser = DaoParser_New();
	DString_Assign( parser->fileName, libpath );
	parser->vmSpace = self;
	if( ! DaoParser_LexCode( parser, DString_GetMBS( source ), 1 ) ) goto LaodingFailed;

	ns = DaoNamespace_New( self, libpath->mbs );
	ns->time = tm;
	if( args ) DaoVmSpace_ParseArguments( self, ns, NULL, args, argNames, argValues );

	DaoVmSpace_Lock( self );
	DArray_PushFront( self->loadedModules, ns );
	MAP_Insert( self->nsModules, libpath, ns );
	DaoVmSpace_Unlock( self );

#if 0
	tok = parser->tokStr->items.pString;
	for( i=0; i<parser->tokStr->size; i++){
		node = MAP_Find( self->allTokens, tok[i] );
		if( node ){
			DArray_Append( ns->tokStr, node->key.pString );
		}else{
			MAP_Insert( self->allTokens, tok[i], 1 );
			DArray_Append( ns->tokStr, tok[i] );
		}
	}
#endif

	/*
	   printf("%p : parsing %s\n", self, libpath->mbs );
	 */
	DaoVmSpace_Lock( self );
	DArray_PushFront( self->nameLoading, ns->name );
	if( ns->path->size ) DArray_PushFront( self->pathLoading, ns->path );
	DaoVmSpace_Unlock( self );

	parser->nameSpace = ns;
	bl = DaoParser_ParseScript( parser );

	poppath = ns->path->size;

	if( ! bl ) goto LaodingFailed;
	if( ns->mainRoutine == NULL ) goto LaodingFailed;
	DString_SetMBS( ns->mainRoutine->routName, "::main" );
	if( args ){
		DaoVmSpace_ConvertArguments( ns, argNames, argValues );
		DArray_Delete( argNames );
		DArray_Delete( argValues );
		argNames = argValues = NULL;
	}

	DaoParser_Delete( parser );

ExecuteImplicitMain :
	if( ns->mainRoutine->body->vmCodes->size > 1 ){
		process = DaoVmSpace_AcquireProcess( self );
		DaoVmSpace_Lock( self );
		DArray_PushFront( self->nameLoading, ns->path );
		DArray_PushFront( self->pathLoading, ns->path );
		DaoVmSpace_Unlock( self );
		DaoProcess_PushRoutine( process, ns->mainRoutine, NULL );
		if( ! DaoProcess_Execute( process ) ){
			DaoVmSpace_ReleaseProcess( self, process );
			DaoVmSpace_Lock( self );
			DArray_PopFront( self->nameLoading );
			DArray_PopFront( self->pathLoading );
			DaoVmSpace_Unlock( self );
			goto LaodingFailed;
		}
		DaoVmSpace_ReleaseProcess( self, process );
		DaoVmSpace_Lock( self );
		DArray_PopFront( self->nameLoading );
		DArray_PopFront( self->pathLoading );
		DaoVmSpace_Unlock( self );
	}

ExecuteExplicitMain :
	name = DString_WrapMBS( "main" );
	m = DaoNamespace_FindConst( ns, & name );
	if( m >=0 ){
		DaoValue *value = DaoNamespace_GetConst( ns, m );
		if( argNames && argValues ){
			DaoVmSpace_ConvertArguments( ns, argNames, argValues );
			DArray_Delete( argNames );
			DArray_Delete( argValues );
			argNames = argValues = NULL;
		}
		if( value && value->type == DAO_ROUTINE ){
			int ret, N = ns->argParams->items.size;
			DaoValue **ps = ns->argParams->items.items.pValue;
			DaoRoutine *rout = & value->xRoutine;
			DaoStream *io = self->errorStream;
			process = DaoVmSpace_AcquireProcess( self );
			ret = DaoProcess_Call( process, rout, NULL, ps, N );
			if( ret == DAO_ERROR_PARAM ){
				DaoStream_WriteMBS( io, "ERROR: invalid command line arguments.\n" );
				if( rout->body->routHelp ) DaoStream_WriteString( io, rout->body->routHelp );
			}
			DaoVmSpace_ReleaseProcess( self, process );
			if( ret ) goto LaodingFailed;
		}
	}
	DaoVmSpace_PopLoadingNamePath( self, poppath );
	if( self->loadedModules->size > (nsCount+1) ){
		DaoVmSpace_Lock( self );
		DArray_Erase( self->loadedModules, 0, self->loadedModules->size - (nsCount+1) );
		DaoVmSpace_Unlock( self );
	}
	if( source ) DString_Delete( source );
	if( argNames ) DArray_Delete( argNames );
	if( argValues ) DArray_Delete( argValues );
	return ns;
LaodingFailed :
	DaoVmSpace_PopLoadingNamePath( self, poppath );
	if( self->loadedModules->size > nsCount ){
		DaoVmSpace_Lock( self );
		DArray_Erase( self->loadedModules, 0, self->loadedModules->size - nsCount );
		DaoVmSpace_Unlock( self );
	}
	if( source ) DString_Delete( source );
	if( argNames ) DArray_Delete( argNames );
	if( argValues ) DArray_Delete( argValues );
	if( parser ) DaoParser_Delete( parser );
	return 0;
}
DaoNamespace* DaoVmSpace_LoadDaoModule( DaoVmSpace *self, DString *libpath )
{
	return DaoVmSpace_LoadDaoModuleExt( self, libpath, NULL, 0 );
}

static void* DaoOpenDLL( const char *name );
static void* DaoGetSymbolAddress( void *handle, const char *name );

static DaoNamespace* DaoVmSpace_LoadDllModule( DaoVmSpace *self, DString *libpath )
{
	DNode *node;
	DaoModuleOnLoad funpter = NULL;
	DaoNamespace *ns = NULL;
	void *handle = NULL;
	daoint nsCount = self->loadedModules->size;
	daoint i, retc;

	if( self->options & DAO_EXEC_SAFE ){
		DaoStream_WriteMBS( self->errorStream,
				"ERROR: not permitted to open shared library in safe running mode.\n" );
		return NULL;
	}
	ns = DaoVmSpace_FindNamespace( self, libpath );
	if( ns ) return ns;

	if( (node = MAP_Find( self->vmodules, libpath ) ) ){
		funpter = (DaoModuleOnLoad) node->value.pVoid;
		ns = DaoNamespace_New( self, libpath->mbs );
	}else{
		handle = DaoOpenDLL( libpath->mbs );
		if( ! handle ){
			DaoStream_WriteMBS( self->errorStream, "ERROR: unable to open the library file \"" );
			DaoStream_WriteMBS( self->errorStream, libpath->mbs );
			DaoStream_WriteMBS( self->errorStream, "\".\n");
			return 0;
		}
		ns = DaoNamespace_New( self, libpath->mbs );
		ns->libHandle = handle;
		funpter = (DaoModuleOnLoad) DaoGetSymbolAddress( handle, "DaoOnLoad" );
		if( funpter == NULL ){
			DString *name = DString_Copy( ns->file );
			const char *prefixes[4] = { "dao_", "dao", "libdao_", "libdao" };
			int size = strlen( DAO_DLL_SUFFIX );
			DString_Erase( name, name->size - size, size );
			for(i=0; i<4; i++){
				if( DString_FindMBS( name, prefixes[i], 0 ) != 0 ) continue;
				DString_Erase( name, 0, strlen( prefixes[i] ) );
			}
			DString_InsertMBS( name, "Dao", 0, 0, 3 );
			DString_AppendMBS( name, "_OnLoad" );
			funpter = (DaoModuleOnLoad) DaoGetSymbolAddress( handle, name->mbs );
			if( funpter == NULL ){
				for(i=3; i<name->size-7; i++) name->mbs[i] = tolower( name->mbs[i] );
				funpter = (DaoModuleOnLoad) DaoGetSymbolAddress( handle, name->mbs );
			}
			if( funpter == NULL ){
				name->mbs[3] = toupper( name->mbs[3] );
				funpter = (DaoModuleOnLoad) DaoGetSymbolAddress( handle, name->mbs );
			}
			if( funpter == NULL ){
				for(i=3; i<name->size-7; i++) name->mbs[i] = toupper( name->mbs[i] );
				funpter = (DaoModuleOnLoad) DaoGetSymbolAddress( handle, name->mbs );
			}
			DString_Delete( name );
		}
	}

	DaoVmSpace_Lock( self );
	DArray_PushFront( self->loadedModules, ns );
	MAP_Insert( self->nsModules, libpath, ns );
	DaoVmSpace_Unlock( self );

	i = DString_RFindChar( libpath, '/', -1 );
	if( i != MAXSIZE ) DString_Erase( libpath, 0, i+1 );
	i = DString_RFindChar( libpath, '\\', -1 );
	if( i != MAXSIZE ) DString_Erase( libpath, 0, i+1 );
	i = DString_FindChar( libpath, '.', 0 );
	if( i != MAXSIZE ) DString_Erase( libpath, i, -1 );
	/* printf( "%s\n", libpath->mbs ); */

	/* no warning or error for loading a C/C++ dynamic linking library
	   for solving symbols in Dao modules. */
	if( funpter == NULL ) return ns;

	DaoVmSpace_Lock( self );
	DArray_PushFront( self->nameLoading, ns->name );
	if( ns->path->size ) DArray_PushFront( self->pathLoading, ns->path );
	DaoVmSpace_Unlock( self );

	retc = (*funpter)( self, ns );

	DaoVmSpace_Lock( self );
	if( ns->path->size ) DArray_PopFront( self->pathLoading );
	DArray_PopFront( self->nameLoading );
	DaoVmSpace_Unlock( self );
	if( retc ){
		DaoVmSpace_Lock( self );
		MAP_Erase( self->nsModules, ns->name );
		DArray_Erase( self->loadedModules, 0, self->loadedModules->size - nsCount );
		DaoVmSpace_Unlock( self );
		return NULL;
	}
	DaoNamespace_UpdateLookupTable( ns );
	if( self->loadedModules->size > (nsCount+1) ){
		DaoVmSpace_Lock( self );
		DArray_Erase( self->loadedModules, 0, self->loadedModules->size - (nsCount+1) );
		DaoVmSpace_Unlock( self );
	}
	return ns;
}
void DaoVmSpace_AddVirtualFile( DaoVmSpace *self, const char *file, const char *data )
{
	DNode *node;
	DString *fname = DString_New(1);
	DString *source = DString_New(1);
	daoint pos;

	DString_SetMBS( fname, "/@/" );
	DString_AppendMBS( fname, file );
	node = DMap_Find( self->vfiles, fname );
	if( node ){
		DString_AppendMBS( node->value.pString, data );
	}else{
		DString_ToMBS( source );
		DString_SetMBS( source, data );
		MAP_Insert( self->vfiles, fname, source );
	}
	pos = DString_RFindChar( fname, '/', -1 );
	DString_Erase( fname, pos, -1 );
	DArray_PushFront( self->pathSearching, fname );
	DString_Delete( fname );
	DString_Delete( source );
}
void DaoVmSpace_AddVirtualModule( DaoVmSpace *self, const char *file, DaoModuleOnLoad onload )
{
	DString *fname = DString_New(1);
	daoint pos;
	DString_SetMBS( fname, "/@/" );
	DString_AppendMBS( fname, file );
	MAP_Insert( self->vmodules, fname, onload );
	pos = DString_RFindChar( fname, '/', -1 );
	DString_Erase( fname, pos, -1 );
	DArray_PushFront( self->pathSearching, fname );
	DString_Delete( fname );
}

/* base is assumed to be absolute, and path is assumed to be relative: */
void Dao_MakePath( DString *base, DString *path )
{
	base = DString_Copy( base );
#ifdef WIN32
	DString_ChangeMBS( base, "\\", "/", 0 );
	DString_ChangeMBS( path, "\\", "/", 0 );
#endif
	while( DString_MatchMBS( path, " ^ %.%. / ", NULL, NULL ) ){
		if( DString_MatchMBS( base, " [^/] + ( / | ) $ ", NULL, NULL ) ){
			DString_ChangeMBS( path, " ^ %.%. / ", "", 1 );
			DString_ChangeMBS( base, " [^/] + ( / |) $ ", "", 0 );
		}else{
			DString_Delete( base );
			return;
		}
	}
	if( DString_MatchMBS( path, " ^ %.%. $ ", NULL, NULL ) ){
		if( DString_MatchMBS( base, " [^/] + ( / | ) $ ", NULL, NULL ) ){
			DString_Clear( path );
			DString_ChangeMBS( base, " [^/] + ( / |) $ ", "", 0 );
		}
	}
	if( base->size && path->size ){
		if( base->mbs[ base->size-1 ] != '/' && path->mbs[0] != '/' )
			DString_InsertChar( path, '/', 0 );
		DString_Insert( path, base, 0, 0, 0 );
	}else if( base->size && path->size == 0 ){
		DString_Assign( path, base );
	}
	DString_ChangeMBS( path, "/ %. /", "/", 0 );
	DString_Delete( base );
}
void DaoVmSpace_SearchPath( DaoVmSpace *self, DString *fname, int type, int check )
{
	daoint i;
	char *p;
	DString *path;

	DString_ToMBS( fname );
	DString_ChangeMBS( fname, "/ %s* %. %s* /", "/", 0 );
	DString_ChangeMBS( fname, "[^%./] + / %. %. /", "", 0 );
	/* erase the last '/' */
	if( fname->size && fname->mbs[ fname->size-1 ] =='/' ){
		fname->size --;
		fname->mbs[ fname->size ] = 0;
	}

	/* C:\dir\source.dao; /home/...  */
	if( fname->size >1 && ( fname->mbs[0]=='/' || fname->mbs[1]==':' ) ) return;

	while( ( p = strchr( fname->mbs, '\\') ) !=NULL ) *p = '/';

	path = DString_Copy( self->pathWorking );

	/* ./source.dao; ../../source.dao */
	if( strstr( fname->mbs, "./" ) !=NULL || strstr( fname->mbs, "../" ) !=NULL ){

		if( self->pathLoading->size ){
			DString_Assign( path, self->pathLoading->items.pString[0] );
			if( path->size ==0 ) goto FreeString;
		}else if( self->pathWorking->size==0 ) goto FreeString;

		Dao_MakePath( path, fname );
		goto FreeString;
	}

	for( i=0; i<self->pathLoading->size; i++){
		DString_Assign( path, self->pathLoading->items.pString[i] );
		if( path->size > 0 && path->mbs[ path->size-1 ] != '/' ) DString_AppendMBS( path, "/" );
		DString_Append( path, fname );
		/*
		   printf( "%s %s\n", self->pathLoading->items.pString[i]->mbs, path->mbs );
		 */
		if( TestPath( self, path, type ) ){
			DString_Assign( fname, path );
			goto FreeString;
		}
	}
	if( path->size > 0 && path->mbs[ path->size -1 ] != '/' ) DString_AppendMBS( path, "/" );
	DString_Append( path, fname );
	/* printf( "%s %s\n", path->mbs, path->mbs ); */
	if( ! check || TestPath( self, path, type ) ){
		DString_Assign( fname, path );
		goto FreeString;
	}
	for( i=0; i<self->pathSearching->size; i++){
		DString_Assign( path, self->pathSearching->items.pString[i] );
		DString_AppendMBS( path, "/" );
		DString_Append( path, fname );
		/*
		   printf( "%s %s\n", self->pathSearching->items.pString[i]->mbs, path->mbs );
		 */
		if( TestPath( self, path, type ) ){
			DString_Assign( fname, path );
			goto FreeString;
		}
	}
FreeString:
	DString_Delete( path );
}
void DaoVmSpace_SetPath( DaoVmSpace *self, const char *path )
{
	char *p;
	DString_SetMBS( self->pathWorking, path );
	while( ( p = strchr( self->pathWorking->mbs, '\\') ) !=NULL ) *p = '/';
}
/* Make path only relative to the current loading path or working path: */
void DaoVmSpace_MakePath( DaoVmSpace *self, DString *path )
{
	DString *wpath = self->pathWorking;

	if( path->size == 0 ) return;
	/* C:\dir\source.dao; /home/...  */
	if( path->size > 1 && (path->mbs[0] == '/' || path->mbs[1] == ':') ) return;

	if( self->pathLoading->size ) wpath = self->pathLoading->items.pString[0];
	if( path->mbs[0] == '.' ){
		Dao_MakePath( wpath, path );
	}else{
		DString *tmp = DString_Copy( wpath );
		if( tmp->size > 0 && tmp->mbs[ tmp->size-1 ] != '/' ) DString_AppendMBS( tmp, "/" );
		DString_Append( tmp, path );
		DString_Assign( path, tmp );
		DString_Delete( tmp );
	}
}
void DaoVmSpace_AddPath( DaoVmSpace *self, const char *path )
{
	DString *tmp, *pstr;
	char *p;

	if( path == NULL || path[0] == '\0' ) return;

	pstr = DString_New(1);
	DString_SetMBS( pstr, path );
	while( ( p = strchr( pstr->mbs, '\\') ) !=NULL ) *p = '/';

	DaoVmSpace_MakePath( self, pstr );

	if( pstr->mbs[pstr->size-1] == '/' ) DString_Erase( pstr, pstr->size-1, 1 );

	if( Dao_IsDir( pstr->mbs ) ){
		tmp = self->pathWorking;
		self->pathWorking = pstr;
		if( DString_FindMBS( pstr, "modules/auxlib", 0 ) == (pstr->size-14) ) self->hasAuxlibPath = 1;
		if( DString_FindMBS( pstr, "modules/syslib", 0 ) == (pstr->size-14) ) self->hasSyslibPath = 1;
		DArray_PushFront( self->pathSearching, pstr );
		DString_AppendMBS( pstr, "/addpath.dao" );
		if( TestFile( self, pstr ) ){
			/* the namespace may have got no chance to reduce its reference count: */
			DaoNamespace *ns = DaoVmSpace_LoadDaoModuleExt( self, pstr, NULL, 0 );
			GC_IncRC( ns );
			GC_DecRC( ns );
		}
		self->pathWorking = tmp;
	}
	DString_Delete( pstr );
	/*
	   for(i=0; i<self->pathSearching->size; i++ )
	   printf( "%s\n", self->pathSearching->items.pString[i]->mbs );
	 */
}
void DaoVmSpace_DelPath( DaoVmSpace *self, const char *path )
{
	DString *pstr;
	char *p;
	int i, id = -1;

	pstr = DString_New(1);
	DString_SetMBS( pstr, path );
	while( ( p = strchr( pstr->mbs, '\\') ) !=NULL ) *p = '/';

	DaoVmSpace_MakePath( self, pstr );

	if( pstr->mbs[pstr->size-1] == '/' ) DString_Erase( pstr, pstr->size-1, 1 );

	for(i=0; i<self->pathSearching->size; i++ ){
		if( DString_Compare( pstr, self->pathSearching->items.pString[i] ) == 0 ){
			id = i;
			break;
		}
	}
	if( id >= 0 ){
		DString *pathDao = DString_Copy( pstr );
		DString *tmp = self->pathWorking;
		self->pathWorking = pstr;
		DString_AppendMBS( pathDao, "/delpath.dao" );
		if( TestFile( self, pathDao ) ){
			DaoNamespace *ns = DaoVmSpace_LoadDaoModuleExt( self, pathDao, NULL, 0 );
			GC_IncRC( ns );
			GC_DecRC( ns );
			/* id may become invalid after loadDaoModule(): */
			id = -1;
			for(i=0; i<self->pathSearching->size; i++ ){
				if( DString_Compare( pstr, self->pathSearching->items.pString[i] ) == 0 ){
					id = i;
					break;
				}
			}
		}
		DArray_Erase( self->pathSearching, id, 1 );
		DString_Delete( pathDao );
		self->pathWorking = tmp;
	}
	DString_Delete( pstr );
}
const char* DaoVmSpace_CurrentWorkingPath( DaoVmSpace *self )
{
	return self->pathWorking->mbs;
}
const char* DaoVmSpace_CurrentLoadingPath( DaoVmSpace *self )
{
	if( self->pathLoading->size ==0 ) return NULL;
	return self->pathLoading->items.pString[0]->mbs;
}

extern DaoTypeBase vmpTyper;

extern DaoTypeBase DaoFdSet_Typer;

extern void DaoInitLexTable();

static void DaoConfigure_FromFile( const char *name )
{
	double number;
	int i, ch, isnum, isint, integer=0, yes;
	FILE *fin = fopen( name, "r" );
	DaoToken *tk1, *tk2;
	DString *mbs;
	DArray *tokens;
	if( fin == NULL ) return;
	mbs = DString_New(1);
	tokens = DArray_New( D_TOKEN );
	while( ( ch=getc(fin) ) != EOF ) DString_AppendChar( mbs, ch );
	fclose( fin );
	DString_ToLower( mbs );
	DaoToken_Tokenize( tokens, mbs->mbs, 1, 0, 0 );
	i = 0;
	while( i < tokens->size ){
		tk1 = tokens->items.pToken[i];
		/* printf( "%s\n", tk1->string->mbs ); */
		if( tk1->type == DTOK_IDENTIFIER ){
			if( i+2 >= tokens->size ) goto InvalidConfig;
			if( tokens->items.pToken[i+1]->type != DTOK_ASSN ) goto InvalidConfig;
			tk2 = tokens->items.pToken[i+2];
			isnum = isint = 0;
			yes = -1;
			if( tk2->type >= DTOK_DIGITS_HEX && tk2->type <= DTOK_NUMBER_SCI ){
				isnum = 1;
				if( tk2->type <= DTOK_NUMBER_HEX ){
					isint = 1;
					number = integer = strtol( tk2->string->mbs, NULL, 0 );
				}else{
					number = strtod( tk2->string->mbs, NULL );
				}
			}else if( tk2->type == DTOK_IDENTIFIER ){
				if( TOKCMP( tk2, "yes" )==0 )  yes = 1;
				if( TOKCMP( tk2, "no" )==0 ) yes = 0;
			}
			if( TOKCMP( tk1, "cpu" )==0 ){
				/* printf( "%s  %i\n", tk2->string->mbs, tk2->type ); */
				if( isint == 0 ) goto InvalidConfigValue;
				daoConfig.cpu = integer;
			}else if( TOKCMP( tk1, "jit" )==0 ){
				if( yes <0 ) goto InvalidConfigValue;
				daoConfig.jit = yes;
			}else if( TOKCMP( tk1, "safe" )==0 ){
				if( yes <0 ) goto InvalidConfigValue;
				daoConfig.safe = yes;
			}else if( TOKCMP( tk1, "typedcode" )==0 ){
				if( yes <0 ) goto InvalidConfigValue;
				daoConfig.typedcode = yes;
			}else if( TOKCMP( tk1, "optimize" )==0 ){
				if( yes <0 ) goto InvalidConfigValue;
				daoConfig.optimize = yes;
			}else if( TOKCMP( tk1, "incompile" )==0 ){
				if( yes <0 ) goto InvalidConfigValue;
				daoConfig.incompile = yes;
			}else if( TOKCMP( tk1, "mbs" )==0 ){
				if( yes <0 ) goto InvalidConfigValue;
				daoConfig.mbs = yes;
			}else if( TOKCMP( tk1, "wcs" )==0 ){
				if( yes <0 ) goto InvalidConfigValue;
				daoConfig.wcs = yes;
			}else{
				goto InvalidConfigName;
			}
			i += 3;
			continue;
		}else if( tk1->type == DTOK_COMMA || tk1->type == DTOK_SEMCO ){
			i ++;
			continue;
		}
InvalidConfig :
		printf( "error: invalid configuration file format at line: %i!\n", tk1->line );
		break;
InvalidConfigName :
		printf( "error: invalid configuration option name: %s!\n", tk1->string->mbs );
		break;
InvalidConfigValue :
		printf( "error: invalid configuration option value: %s!\n", tk2->string->mbs );
		break;
	}
	DArray_Delete( tokens );
	DString_Delete( mbs );
}
static void DaoConfigure()
{
	char *daodir = getenv( "DAO_DIR" );
	DString *mbs = DString_New(1);

	DaoInitLexTable();
	daoConfig.iscgi = getenv( "GATEWAY_INTERFACE" ) ? 1 : 0;

	DString_SetMBS( mbs, DAO_DIR );
	DString_AppendMBS( mbs, "/dao.conf" );
	DaoConfigure_FromFile( mbs->mbs );
	if( daodir ){
		DString_SetMBS( mbs, daodir );
		if( daodir[ mbs->size-1 ] == '/' ){
			DString_AppendMBS( mbs, "dao.conf" );
		}else{
			DString_AppendMBS( mbs, "/dao.conf" );
		}
		DaoConfigure_FromFile( mbs->mbs );
	}
	DaoConfigure_FromFile( "./dao.conf" );
	DString_Delete( mbs );
}

#ifdef DEBUG
static void dao_FakeList_FakeList( DaoProcess *_proc, DaoValue *_p[], int _n );
static void dao_FakeList_Size( DaoProcess *_proc, DaoValue *_p[], int _n );
static void dao_FakeList_GetItem( DaoProcess *_proc, DaoValue *_p[], int _n );
static void dao_FakeList_SetItem( DaoProcess *_proc, DaoValue *_p[], int _n );

#define FakeListName "FakeList<@T<short|int|float>=int,@S=int>"

static DaoFuncItem dao_FakeList_Meths[] = 
{
	/* the names of allocators must be identical to the typer name: */
	{ dao_FakeList_FakeList, FakeListName "( size=0 )" },
	{ dao_FakeList_Size, "size( self :FakeList )=>int" },
	{ dao_FakeList_GetItem, "[]( self :FakeList<@T<short|int|float>>, index :int )=>int" },
	{ dao_FakeList_SetItem, "[]=( self :FakeList<@T<short|int|float>>, value :int, index :int )=>int" },
	{ NULL, NULL }
};
static void Dao_FakeList_Delete( void *self ){}
static DaoTypeBase FakeList_Typer = 
{ FakeListName, NULL, NULL, dao_FakeList_Meths, {0}, {0}, Dao_FakeList_Delete, NULL };
DaoTypeBase *dao_FakeList_Typer = & FakeList_Typer;

static void dao_FakeList_FakeList( DaoProcess *_proc, DaoValue *_p[], int _n )
{
  int size = _p[0]->xInteger.value;
  DaoType *retype = DaoProcess_GetReturnType( _proc );
  DaoCdata *cdata = DaoCdata_New( FakeList_Typer.core->kernel->abtype, (void*)(daoint)size );
  printf( "retype = %s\n", retype->name->mbs );
  GC_ShiftRC( retype, cdata->ctype );
  cdata->ctype = retype;
  DaoProcess_PutValue( _proc, (DaoValue*)cdata );
}
static void dao_FakeList_Size( DaoProcess *_proc, DaoValue *_p[], int _n )
{
  daoint size = (daoint) DaoCdata_GetData( & _p[0]->xCdata );
  DaoProcess_PutInteger( _proc, size );
}
static void dao_FakeList_GetItem( DaoProcess *_proc, DaoValue *_p[], int _n )
{
  DaoProcess_PutInteger( _proc, 123 );
}
static void dao_FakeList_SetItem( DaoProcess *_proc, DaoValue *_p[], int _n )
{
}
#endif

extern void DaoType_Init();

DaoType *dao_type_udf = NULL;
DaoType *dao_type_none = NULL;
DaoType *dao_type_any = NULL;
DaoType *dao_type_int = NULL;
DaoType *dao_type_float = NULL;
DaoType *dao_type_double = NULL;
DaoType *dao_type_complex = NULL;
DaoType *dao_type_long = NULL;
DaoType *dao_type_string = NULL;
DaoType *dao_type_tuple = NULL;
DaoType *dao_array_any = NULL;
DaoType *dao_list_any = NULL;
DaoType *dao_map_any = NULL;
DaoType *dao_map_meta = NULL;
DaoType *dao_routine = NULL;
DaoType *dao_type_for_iterator = NULL;
DaoType *dao_access_enum = NULL;
DaoType *dao_storage_enum = NULL;
DaoType *dao_dynclass_field = NULL;
DaoType *dao_dynclass_method = NULL;
DaoType *dao_array_types[DAO_COMPLEX+1] = {0};

/* name:string,value:any,storage:enum<>,access:enum<> */
const char *field_typename = 
"tuple<string,any>|tuple<string,any,enum<const,global,var>>|"
"tuple<string,any,enum<const,global,var>,enum<private,protected,public>>";

/* name:string,method:routine,access:enum<> */
const char *method_typename = 
"tuple<string,routine>|tuple<string,routine,enum<private,protected,public>>";


#ifdef DAO_WITH_THREAD
DaoType *dao_type_mutex = NULL;
DaoType *dao_type_condvar = NULL;
DaoType *dao_type_sema = NULL;

extern DMutex mutex_long_sharing;
extern DMutex mutex_string_sharing;
extern DMutex mutex_type_map;
extern DMutex mutex_values_setup;
extern DMutex mutex_methods_setup;
extern DMutex mutex_routines_update;
extern DMutex mutex_routine_specialize;
extern DMutex mutex_routine_specialize2;
extern DMutex dao_cdata_mutex;
extern DaoFuncItem dao_mt_methods[];
#endif

DaoType *dao_type_stream = NULL;
extern DaoFuncItem dao_std_methods[];
extern DaoFuncItem dao_io_methods[];

#include<signal.h>
void print_trace();

extern DMap *dao_cdata_bindings;
extern DHash *dao_meta_tables;

int DaoVmSpace_TryInitJIT( DaoVmSpace *self, const char *module )
{
	DString *name = DString_New(1);
	void (*init)( DaoVmSpace*, DaoJIT* );
	void *jitHandle = NULL;
	if( module ){
		DString_SetMBS( name, module );
		jitHandle = DaoLoadLibrary( name->mbs );
	}else{
		DString_SetMBS( name, "libDaoJIT" DAO_DLL_SUFFIX );
		DaoVmSpace_SearchPath( self, name, DAO_FILE_PATH, 1 );
		jitHandle = DaoLoadLibrary( name->mbs );
		if( jitHandle == NULL ){
			DString_SetMBS( name, "DaoJIT" DAO_DLL_SUFFIX );
			DaoVmSpace_SearchPath( self, name, DAO_FILE_PATH, 1 );
			jitHandle = DaoLoadLibrary( name->mbs );
		}
	}
	DString_Delete( name );
	if( jitHandle == NULL ) return 0;
	init = (DaoJIT_InitFPT) DaoFindSymbol( jitHandle, "DaoJIT_Init" );
	if( init == NULL ) return 0;
	(*init)( self, & dao_jit );
	dao_jit.Quit = (DaoJIT_QuitFPT) DaoFindSymbol( jitHandle, "DaoJIT_Quit" );
	dao_jit.Free = (DaoJIT_FreeFPT) DaoFindSymbol( jitHandle, "DaoJIT_Free" );
	dao_jit.Compile = (DaoJIT_CompileFPT) DaoFindSymbol( jitHandle, "DaoJIT_Compile" );
	dao_jit.Execute = (DaoJIT_ExecuteFPT) DaoFindSymbol( jitHandle, "DaoJIT_Execute" );
	if( dao_jit.Execute == NULL ) dao_jit.Compile = NULL;
	return dao_jit.Compile != NULL;
}


DaoVmSpace* DaoVmSpace_MainVmSpace()
{
	return mainVmSpace;
}
DaoVmSpace* DaoInit( const char *command )
{
	DString *mbs, *mbs2;
	DaoVmSpace *vms;
	DaoNamespace *ns, *ns2;
	DaoType *type, *type1, *type2, *type3, *type4;
	DaoModuleOnLoad fpter;
	char *daodir = getenv( "DAO_DIR" );
	void *handle;
	daoint i, n;

	if( mainVmSpace ) return mainVmSpace;

	dao_cdata_bindings = DHash_New(0,0);
	dao_meta_tables = DHash_New(0,0);

	/* signal( SIGSEGV, print_trace ); */
	/* signal( SIGABRT, print_trace ); */

#ifdef DAO_WITH_THREAD
	DMutex_Init( & mutex_long_sharing );
	DMutex_Init( & mutex_string_sharing );
	DMutex_Init( & mutex_type_map );
	DMutex_Init( & mutex_values_setup );
	DMutex_Init( & mutex_methods_setup );
	DMutex_Init( & mutex_routines_update );
	DMutex_Init( & mutex_routine_specialize );
	DMutex_Init( & mutex_routine_specialize2 );
	DMutex_Init( & dao_cdata_mutex );
#endif

	mbs = DString_New(1);
	setlocale( LC_CTYPE, "" );

	DaoConfigure();
	DaoType_Init();
	/*
	   printf( "number of VM instructions: %i\n", DVM_NULL );
	 */

#ifdef DEBUG
	for(i=0; i<100; i++) ObjectProfile[i] = 0;
#endif

#ifdef DAO_WITH_THREAD
	DaoInitThread();
#endif

	DaoGC_Start();

#if 0
#warning"-------------using concurrent GC by default!"
	DaoCGC_Start();
#endif

	if( daodir == NULL && command ){
		int absolute = command[0] == '/';
		int relative = command[0] == '.';
#ifdef WIN32
		absolute = isalpha( command[0] ) && command[1] == ':';
#endif
		DString_SetMBS( mbs, command );
		if( relative ){
			DString *base = DString_New(1);
			char pwd[512];
			getcwd( pwd, 511 );
			DString_SetMBS( base, pwd );
			Dao_MakePath( base, mbs );
			DString_Delete( base );
		}
		while( (i = mbs->size) && mbs->mbs[i-1] != '/' && mbs->mbs[i-1] != '\\' ) mbs->size --;
		mbs->mbs[ mbs->size ] = 0;
		daodir = (char*) dao_malloc( mbs->size + 10 );
		strncpy( daodir, "DAO_DIR=", 9 );
		strncat( daodir, mbs->mbs, mbs->size );
		putenv( daodir );
	}

	dao_type_udf = DaoType_New( "?", DAO_UDT, NULL, NULL );
	dao_type_any = DaoType_New( "any", DAO_ANY, NULL, NULL );
	dao_type_int = DaoType_New( "int", DAO_INTEGER, NULL, NULL );
	dao_type_float = DaoType_New( "float", DAO_FLOAT, NULL, NULL );
	dao_type_double = DaoType_New( "double", DAO_DOUBLE, NULL, NULL );
	dao_type_complex = DaoType_New( "complex", DAO_COMPLEX, NULL, NULL );
	dao_type_long = DaoType_New( "long", DAO_LONG, NULL, NULL );
	dao_type_string = DaoType_New( "string", DAO_STRING, NULL, NULL );
	dao_routine = DaoType_New( "routine<=>?>", DAO_ROUTINE, (DaoValue*)dao_type_udf, NULL );

	mainVmSpace = vms = DaoVmSpace_New();

	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_udf );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_any );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_int );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_float );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_double );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_complex );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_long );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_type_string );
	DaoProcess_CacheValue( vms->mainProcess, (DaoValue*) dao_routine );

	vms->safeTag = 0;
	ns = vms->nsInternal;

	dao_type_none = DaoNamespace_MakeValueType( ns, dao_none_value );
	dao_type_for_iterator = DaoParser_ParseTypeName( "tuple<valid:int,iterator:any>", ns, NULL );
	dao_access_enum = DaoNamespace_MakeEnumType( ns, "private,protected,public" );
	dao_storage_enum = DaoNamespace_MakeEnumType( ns, "const,global,var"  );
	dao_dynclass_field = DaoParser_ParseTypeName( field_typename, ns, NULL );
	dao_dynclass_method = DaoParser_ParseTypeName( method_typename, ns, NULL );

	DString_SetMBS( dao_type_for_iterator->name, "for_iterator" );
	DaoNamespace_AddType( ns, dao_type_for_iterator->name, dao_type_for_iterator );

	dao_array_any = DaoParser_ParseTypeName( "array<any>", ns, NULL );
	dao_list_any = DaoParser_ParseTypeName( "list<any>", ns, NULL );
	dao_map_any = DaoParser_ParseTypeName( "map<any,any>", ns, NULL );
	dao_map_meta = DaoParser_ParseTypeName( "map<string,any>", ns, NULL );

	dao_type_tuple = DaoNamespace_MakeType( ns, "tuple<...>", DAO_TUPLE, NULL, NULL, 0 );
	dao_type_tuple->variadic = 1;

	dao_array_types[DAO_NONE] = dao_array_any;
	dao_array_types[DAO_INTEGER] = DaoNamespace_MakeType( ns, "array", DAO_ARRAY, NULL, & dao_type_int, 1 );
	dao_array_types[DAO_FLOAT]   = DaoNamespace_MakeType( ns, "array", DAO_ARRAY, NULL, & dao_type_float, 1 );
	dao_array_types[DAO_DOUBLE]  = DaoNamespace_MakeType( ns, "array", DAO_ARRAY, NULL, & dao_type_double, 1 );
	dao_array_types[DAO_COMPLEX] = DaoNamespace_MakeType( ns, "array", DAO_ARRAY, NULL, & dao_type_complex, 1 );

#ifdef DEBUG
	DaoNamespace_TypeDefine( ns, "int", "short" );
	DaoNamespace_WrapType( vms->nsInternal, dao_FakeList_Typer, 1 );
	DaoNamespace_TypeDefine( ns, "FakeList<short>", "FakeList<int>" );
#endif

#ifdef DAO_WITH_NUMARRAY
	DaoNamespace_SetupType( vms->nsInternal, & numarTyper );
#endif

	DaoNamespace_SetupType( vms->nsInternal, & stringTyper );
	DaoNamespace_SetupType( vms->nsInternal, & longTyper );
	DaoNamespace_SetupType( vms->nsInternal, & comTyper );
	DaoNamespace_SetupType( vms->nsInternal, & listTyper );
	DaoNamespace_SetupType( vms->nsInternal, & mapTyper );

	GC_ShiftRC( dao_type_complex, comTyper.core->kernel->abtype );
	GC_ShiftRC( dao_type_long, longTyper.core->kernel->abtype );
	GC_ShiftRC( dao_type_string, stringTyper.core->kernel->abtype );
	comTyper.core->kernel->abtype = dao_type_complex;
	longTyper.core->kernel->abtype = dao_type_long;
	stringTyper.core->kernel->abtype = dao_type_string;

	ns2 = DaoVmSpace_GetNamespace( vms, "io" );
	DaoNamespace_AddConstValue( ns, "io", (DaoValue*) ns2 );
	dao_type_stream = DaoNamespace_WrapType( ns2, & streamTyper, 0 );
	GC_ShiftRC( dao_type_stream, vms->stdioStream->ctype );
	vms->stdioStream->ctype = dao_type_stream;
	DaoNamespace_WrapFunctions( ns2, dao_io_methods );
	DaoNamespace_AddConstValue( ns2, "stdio", (DaoValue*) vms->stdioStream );

	dao_default_cdata.ctype = DaoNamespace_WrapType( vms->nsInternal, & defaultCdataTyper, 0 );
	dao_default_cdata.ctype->cdatatype = DAO_CDATA_PTR;
	GC_IncRC( dao_default_cdata.ctype );

	DaoException_Setup( vms->nsInternal );

#ifdef DAO_WITH_CONCURRENT
	ns2 = DaoVmSpace_GetNamespace( vms, "mt" );
	DaoNamespace_AddConstValue( ns, "mt", (DaoValue*) ns2 );
	dao_type_mutex = DaoNamespace_WrapType( ns2, & mutexTyper, 0 );
	dao_type_condvar = DaoNamespace_WrapType( ns2, & condvTyper, 0 );
	dao_type_sema = DaoNamespace_WrapType( ns2, & semaTyper, 0 );
	DaoNamespace_SetupType( ns2, & futureTyper );
	DaoNamespace_WrapFunctions( ns2, dao_mt_methods );
#endif
	DaoNamespace_SetupType( vms->nsInternal, & vmpTyper );

	ns2 = DaoVmSpace_GetNamespace( vms, "std" );
	DaoNamespace_AddConstValue( ns, "std", (DaoValue*) ns2 );
	DaoNamespace_WrapFunctions( ns2, dao_std_methods );

	DaoNamespace_AddParent( vms->mainNamespace, vms->nsInternal );

	DaoVmSpace_InitPath( vms );

#ifdef DAO_WITH_MODULES
	if( (n = strlen(DAO_WITH_MODULES)) ){
		mbs2 = DString_New(1);
		DString_Clear( mbs );
		handle = DaoOpenDLL( NULL );
		for(i=0; i<n; i++){
			char ch = DAO_WITH_MODULES[i];
			if( ch == ',' || i == (n-1) ){
				if( ch != ',' ) DString_AppendChar( mbs, ch );
				if( mbs->size == 0 || mbs2->size == 0 ) continue;
				DString_InsertMBS( mbs2, "Dao", 0, 0, 3 );
				DString_AppendMBS( mbs2, "_OnLoad" );
				fpter = (DaoModuleOnLoad) DaoGetSymbolAddress( handle, mbs2->mbs );
				if( fpter ){
					DString_AppendMBS( mbs, DAO_DLL_SUFFIX );
					DaoVmSpace_AddVirtualModule( vms, mbs->mbs, fpter );
				}else{
					fprintf( stderr, "WARNING: failed to embed module \"%s\"!\n", mbs->mbs );
				}
				DString_Clear( mbs );
				DString_Clear( mbs2 );
			}else if( ch == ':' ){
				DString_Assign( mbs2, mbs );
			}else{
				DString_AppendChar( mbs, ch );
			}
		}
		DString_Delete( mbs2 );
	}
#endif

	/*
	   printf( "initialized...\n" );
	 */
	DString_Delete( mbs );
	vms->safeTag = 1;
	return vms;
}
extern DaoType* DaoParser_ParseTypeName( const char *type, DaoNamespace *ns, DaoClass *cls );
extern DaoType *simpleTypes[ DAO_ARRAY ];
void DaoQuit()
{
	int i;
	DNode *it = NULL;

	/* TypeTest(); */
#ifdef DAO_WITH_CONCURRENT
	DaoCallServer_Stop();
#endif

	if( daoConfig.iscgi ) return;

	GC_DecRC( dao_default_cdata.ctype );
	dao_default_cdata.ctype = NULL;

	DaoVmSpace_DeleteData( mainVmSpace );
	for(i=0; i<DAO_ARRAY; i++){
		GC_DecRC( simpleTypes[i] );
		simpleTypes[i] = NULL;
	}
	DaoGC_Finish();
#ifdef DEBUG
	for(it=DMap_First(mainVmSpace->nsModules); it; it=DMap_Next(mainVmSpace->nsModules,it) ){
		printf( "Warning: namespace/module \"%s\" is not collected with reference count %i!\n",
				((DaoNamespace*)it->value.pValue)->name->mbs, it->value.pValue->xBase.refCount );
	}
#endif
	DaoVmSpace_Delete( mainVmSpace );
	DMap_Delete( dao_cdata_bindings );
	DMap_Delete( dao_meta_tables );
	dao_cdata_bindings = NULL;
	dao_meta_tables = NULL;
	mainVmSpace = NULL;
	mainProcess = NULL; 
	if( dao_jit.Quit ){
		dao_jit.Quit();
		dao_jit.Quit = NULL;
		dao_jit.Free = NULL;
		dao_jit.Compile = NULL;
		dao_jit.Execute = NULL;
	}
#ifdef DAO_WITH_THREAD
	DMutex_Destroy( & mutex_long_sharing );
	DMutex_Destroy( & mutex_string_sharing );
	DMutex_Destroy( & mutex_type_map );
	DMutex_Destroy( & mutex_values_setup );
	DMutex_Destroy( & mutex_methods_setup );
	DMutex_Destroy( & mutex_routines_update );
	DMutex_Destroy( & mutex_routine_specialize );
	DMutex_Destroy( & mutex_routine_specialize2 );
	DMutex_Destroy( & dao_cdata_mutex );
#endif
}
DaoNamespace* DaoVmSpace_FindModule( DaoVmSpace *self, DString *fname )
{
	DaoNamespace* ns = DaoVmSpace_FindNamespace( self, fname );
	if( ns ) return ns;
	DaoVmSpace_CompleteModuleName( self, fname );
	return DaoVmSpace_FindNamespace( self, fname );
}
DaoNamespace* DaoVmSpace_LoadModule( DaoVmSpace *self, DString *fname )
{
	DaoNamespace *ns = NULL;
#if 0
	printf( "modtype = %i\n", modtype );
#endif
	switch( DaoVmSpace_CompleteModuleName( self, fname ) ){
	case DAO_MODULE_DAO_O : ns = DaoVmSpace_LoadDaoByteCode( self, fname, 1 ); break;
	case DAO_MODULE_DAO_S : ns = DaoVmSpace_LoadDaoAssembly( self, fname, 1 ); break;
	case DAO_MODULE_DAO : ns = DaoVmSpace_LoadDaoModule( self, fname ); break;
	case DAO_MODULE_DLL : ns = DaoVmSpace_LoadDllModule( self, fname ); break;
	}
	return ns;
}

#ifdef UNIX
#include<dlfcn.h>
#elif WIN32
#include<windows.h>
#endif

void DaoGetErrorDLL()
{
#ifdef UNIX
	printf( "%s\n", dlerror() );
#elif WIN32
	DWORD error = GetLastError();
	LPSTR message;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, error, LANG_NEUTRAL, (LPTSTR)&message, 0, NULL );
	if( message ){
		printf( "%s\n", message );
		LocalFree( message );
	}
#endif
}

void* DaoOpenDLL( const char *name )
{
#ifdef UNIX
	void *handle = dlopen( name, RTLD_NOW | RTLD_GLOBAL );
#elif WIN32
	void *handle = LoadLibrary( name );
#endif
	if( !handle ){
		DaoGetErrorDLL();
		return 0;
	}
	return handle;
}
void* DaoGetSymbolAddress( void *handle, const char *name )
{
#ifdef UNIX
	void *sym = dlsym( handle, name );
#elif WIN32
	void *sym = (void*)GetProcAddress( (HMODULE)handle, name );
#endif
	return sym;
}
