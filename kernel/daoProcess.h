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

#ifndef DAO_PROCESS_H
#define DAO_PROCESS_H

#include"daoVmcode.h"
#include"daoType.h"
#include"time.h"
#include"stdlib.h"

#define DVM_MAKE_OBJECT (1<<6)
#define DVM_SPEC_RUN (1<<7)

#define DVM_MAX_TRY_DEPTH 16

struct DaoVmFrame
{
	ushort_t    entry;     /* entry code id */
	ushort_t    state;     /* context state */
	ushort_t    returning; /* return register id */
	ushort_t    depth;
	ushort_t    ranges[DVM_MAX_TRY_DEPTH][2];

	DaoContext *context;
	DaoVmFrame *prev;
	DaoVmFrame *next;
	DaoVmFrame *rollback;
};

struct DaoVmProcess
{
	DAO_DATA_COMMON;

	DaoVmFrame *firstFrame; /* the first frame, never active */
	DaoVmFrame *topFrame; /* top call frame */

	DaoVmSpace *vmSpace;
	DaoType    *abtype; /* for coroutine */

	DValue    returned;
	DVarray  *parResume;/* for coroutine */
	DVarray  *parYield;
	DVarray  *exceptions;

	char pauseType;
	char status;
	char stopit;

	DaoFuture *future;

	DString *mbstring;
	DMap    *mbsRegex; /* <DString*,DString*> */
	DMap    *wcsRegex; /* <DString*,DString*> */
};

/* Create a new virtual machine process */
DaoVmProcess* DaoVmProcess_New( DaoVmSpace *vms );
void DaoVmProcess_Delete( DaoVmProcess *self );

/* Push a routine into the calling stack of the VM process, new context is created */
void DaoVmProcess_PushRoutine( DaoVmProcess *self, DaoRoutine *routine );
/* Push an initialized context into the calling stack of the VM process */
void DaoVmProcess_PushContext( DaoVmProcess *self, DaoContext *context );
DaoContext* DaoVmProcess_MakeContext( DaoVmProcess *self, DaoRoutine *routine );
void DaoVmProcess_PopContext( DaoVmProcess *self );

int DaoVmProcess_Call( DaoVmProcess *self, DaoMethod *f, DValue *o, DValue *p[], int n );
/* Execute from the top of the calling stack */
int DaoVmProcess_Execute( DaoVmProcess *self );
int DaoVmProcess_ExecuteSection( DaoVmProcess *self, int entry );

DaoVmProcess* DaoVmProcess_Create( DaoContext *ctx, DValue *par[], int N );

/* Resume a coroutine */
/* coroutine.yeild( a, b, ... ); store object a,b,... in "DaoList *list"
 * 
 * param = coroutine.resume( corout, a, b, ... ); pass "DValue par[]" as a,b,...
 * they become addition result from yeild().
 */
int DaoVmProcess_Resume( DaoVmProcess *self, DValue *par[], int N, DaoList *list );
void DaoVmProcess_Yield( DaoVmProcess *self, DValue *par[], int N, DaoList *list );

void DaoVmProcess_PrintException( DaoVmProcess *self, int clear );

DValue DaoVmProcess_MakeConst( DaoVmProcess *self );


typedef struct DaoJIT DaoJIT;
typedef void (*DaoJIT_InitFPT)( DaoVmSpace*, DaoJIT* );
typedef void (*DaoJIT_QuitFPT)();
typedef void (*DaoJIT_FreeFPT)( DaoRoutine *routine );
typedef void (*DaoJIT_CompileFPT)( DaoRoutine *routine );
typedef void (*DaoJIT_ExecuteFPT)( DaoContext *context, int jitcode );

struct DaoJIT
{
	void (*Quit)();
	void (*Free)( DaoRoutine *routine );
	void (*Compile)( DaoRoutine *routine );
	void (*Execute)( DaoContext *context, int jitcode );
};

extern struct DaoJIT dao_jit;

#endif
