#include"dao_greeting.h"

#ifdef __cplusplus
extern "C"{
#endif

/*  greeting.h */


static DaoNumItem dao_AutobindTest_Nums[] =
{
  { NULL, 0, 0 }
};
static void dao_AutobindTest_AutobindTest( DaoContext *_ctx, DValue *_p[], int _n );

static DaoFuncItem dao_AutobindTest_Meths[] = 
{
  { dao_AutobindTest_AutobindTest, "AutobindTest(  )=>AutobindTest" },
  { NULL, NULL }
};
static void Dao_AutobindTest_Delete( void *self )
{
	delete (AutobindTest*) self;
}

static DaoTypeBase AutobindTest_Typer = 
{ "AutobindTest", NULL, 
  dao_AutobindTest_Nums,
  dao_AutobindTest_Meths, 
  { 0 },
  { 0 },
  Dao_AutobindTest_Delete,
  NULL
};
DaoTypeBase DAO_DLL_GREETING *dao_AutobindTest_Typer = & AutobindTest_Typer;
static void dao_AutobindTest_AutobindTest( DaoContext *_ctx, DValue *_p[], int _n )
{
	AutobindTest *self = Dao_AutobindTest_New();
	DaoContext_PutCData( _ctx, self, dao_AutobindTest_Typer );
}

/*  greeting.h */


static DaoNumItem dao_Bool_Nums[] =
{
  { NULL, 0, 0 }
};

static DaoFuncItem dao_Bool_Meths[] = 
{
  { NULL, NULL }
};
static void Dao_Bool_Delete( void *self )
{
	free( self );
}

static DaoTypeBase Bool_Typer = 
{ "Bool", NULL, 
  dao_Bool_Nums,
  dao_Bool_Meths, 
  { 0 },
  { 0 },
  Dao_Bool_Delete,
  NULL
};
DaoTypeBase DAO_DLL_GREETING *dao_Bool_Typer = & Bool_Typer;

/*  greeting.h */


static DaoNumItem dao_Greeting_Nums[] =
{
  { NULL, 0, 0 }
};
static void dao_Greeting_Greeting( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_DoGreeting__Greeting( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_DoGreeting( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_PrintMessage( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_SetMessage( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_TestGreeting( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_TestNull( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_VirtWithDefault__Greeting( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Greeting_VirtWithDefault( DaoContext *_ctx, DValue *_p[], int _n );

static DaoFuncItem dao_Greeting_Meths[] = 
{
  { dao_Greeting_Greeting, "Greeting( msg : string=\'\' )=>Greeting" },
  { dao_Greeting_DoGreeting, "DoGreeting( self : Greeting, name : string )" },
  { dao_Greeting_PrintMessage, "PrintMessage( self : Greeting )" },
  { dao_Greeting_SetMessage, "SetMessage( self : Greeting, msg : string )" },
  { dao_Greeting_TestGreeting, "TestGreeting( self : Greeting, g : Greeting, name : string )" },
  { dao_Greeting_TestNull, "TestNull( self : Greeting, _cp0 : Greeting_Null )=>Greeting_Null" },
  { dao_Greeting_VirtWithDefault, "VirtWithDefault( self : Greeting, g : Greeting =0 )" },
  { NULL, NULL }
};
static void Dao_Greeting_Delete( void *self )
{
	delete (Greeting*) self;
}
static int Dao_Greeting_DelTest( void *self0 )
{
	Greeting *self = (Greeting*) self0;
	return (self!=GetGreetingObject());
}

static DaoTypeBase Greeting_Typer = 
{ "Greeting", NULL, 
  dao_Greeting_Nums,
  dao_Greeting_Meths, 
  { 0 },
  { 0 },
  Dao_Greeting_Delete,
  Dao_Greeting_DelTest
};
DaoTypeBase DAO_DLL_GREETING *dao_Greeting_Typer = & Greeting_Typer;
/* greeting.h */
static void dao_Greeting_Greeting( DaoContext *_ctx, DValue *_p[], int _n )
{
  char* msg= (char*) DString_GetMBS( _p[0]->v.s );
	DaoCxx_Greeting *_self = DaoCxx_Greeting_New( msg );
	DaoContext_PutResult( _ctx, (DaoBase*) _self->cdata );
}
/* greeting.h */
static void dao_Greeting_DoGreeting__Greeting( DaoContext *_ctx, DValue *_p[], int _n )
{
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  char* name= (char*) DString_GetMBS( _p[1]->v.s );
  self->Greeting::DoGreeting( name );
}
/* greeting.h */
static void dao_Greeting_DoGreeting( DaoContext *_ctx, DValue *_p[], int _n )
{
  if( DaoCData_OwnData( _p[0]->v.cdata ) ){
    dao_Greeting_DoGreeting__Greeting( _ctx, _p, _n );
    return;
  }
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  char* name= (char*) DString_GetMBS( _p[1]->v.s );
  self->DoGreeting( name );
}
/* greeting.h */
static void dao_Greeting_PrintMessage( DaoContext *_ctx, DValue *_p[], int _n )
{
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  self->Greeting::PrintMessage(  );
}
/* greeting.h */
static void dao_Greeting_SetMessage( DaoContext *_ctx, DValue *_p[], int _n )
{
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  char* msg= (char*) DString_GetMBS( _p[1]->v.s );
  self->Greeting::SetMessage( msg );
}
/* greeting.h */
static void dao_Greeting_TestGreeting( DaoContext *_ctx, DValue *_p[], int _n )
{
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  Greeting* g= (Greeting*) DaoCData_CastData( _p[1]->v.cdata, dao_Greeting_Typer );
  char* name= (char*) DString_GetMBS( _p[2]->v.s );
  self->Greeting::TestGreeting( g, name );
}
/* greeting.h */
static void dao_Greeting_TestNull( DaoContext *_ctx, DValue *_p[], int _n )
{
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  Greeting::Null* _cp0= (Greeting::Null*) DaoCData_CastData( _p[1]->v.cdata, dao_Greeting_Null_Typer );
  Greeting::Null _TestNull = self->Greeting::TestNull( *_cp0 );
  DaoContext_PutCData( _ctx, (void*)new Greeting::Null( _TestNull ), dao_Greeting_Null_Typer );
}
/* greeting.h */
static void dao_Greeting_VirtWithDefault__Greeting( DaoContext *_ctx, DValue *_p[], int _n )
{
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  Greeting* g= (Greeting*) DaoCData_CastData( _p[1]->v.cdata, dao_Greeting_Typer );
  if(_n<=1) self->Greeting::VirtWithDefault(  );
  else self->Greeting::VirtWithDefault( *g );
}
/* greeting.h */
static void dao_Greeting_VirtWithDefault( DaoContext *_ctx, DValue *_p[], int _n )
{
  if( DaoCData_OwnData( _p[0]->v.cdata ) ){
    dao_Greeting_VirtWithDefault__Greeting( _ctx, _p, _n );
    return;
  }
  Greeting* self= (Greeting*) DaoCData_CastData( _p[0]->v.cdata, dao_Greeting_Typer );
  Greeting* g= (Greeting*) DaoCData_CastData( _p[1]->v.cdata, dao_Greeting_Typer );
  if(_n<=1) self->VirtWithDefault(  );
  else self->VirtWithDefault( *g );
}

/*  greeting.h */


static DaoNumItem dao_Greeting2_Nums[] =
{
  { NULL, 0, 0 }
};
static void dao_Greeting2_Greeting2( DaoContext *_ctx, DValue *_p[], int _n );

static DaoFuncItem dao_Greeting2_Meths[] = 
{
  { dao_Greeting2_Greeting2, "Greeting2(  )=>Greeting2" },
  { NULL, NULL }
};
static void Dao_Greeting2_Delete( void *self )
{
	delete (Greeting2*) self;
}
void* dao_cast_Greeting2_to_Greeting( void *data )
{
	return (Greeting*)(Greeting2*)data;
}

static DaoTypeBase Greeting2_Typer = 
{ "Greeting2", NULL, 
  dao_Greeting2_Nums,
  dao_Greeting2_Meths, 
  { dao_Greeting_Typer, 0 },
  { dao_cast_Greeting2_to_Greeting,0 },
  Dao_Greeting2_Delete,
  NULL
};
DaoTypeBase DAO_DLL_GREETING *dao_Greeting2_Typer = & Greeting2_Typer;
static void dao_Greeting2_Greeting2( DaoContext *_ctx, DValue *_p[], int _n )
{
	DaoCxx_Greeting2 *self = DaoCxx_Greeting2_New();
	DaoContext_PutResult( _ctx, (DaoBase*) self->cdata );
}

/*  greeting.h */


static DaoNumItem dao_Greeting_Null_Nums[] =
{
  { NULL, 0, 0 }
};

static DaoFuncItem dao_Greeting_Null_Meths[] = 
{
  { NULL, NULL }
};
static void Dao_Greeting_Null_Delete( void *self )
{
	delete (Greeting::Null*) self;
}

static DaoTypeBase Greeting_Null_Typer = 
{ "Greeting_Null", NULL, 
  dao_Greeting_Null_Nums,
  dao_Greeting_Null_Meths, 
  { 0 },
  { 0 },
  Dao_Greeting_Null_Delete,
  NULL
};
DaoTypeBase DAO_DLL_GREETING *dao_Greeting_Null_Typer = & Greeting_Null_Typer;

/*  greeting.h */


static void dao_Test_GETF_index( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Test_SETF_index( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Test_GETF_value( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Test_SETF_value( DaoContext *_ctx, DValue *_p[], int _n );
static DaoNumItem dao_Test_Nums[] =
{
  { NULL, 0, 0 }
};
static void dao_Test_Test( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_Test_Print( DaoContext *_ctx, DValue *_p[], int _n );

static DaoFuncItem dao_Test_Meths[] = 
{
  { dao_Test_GETF_index, ".index( self : Test )=>int" },
  { dao_Test_SETF_index, ".index=( self : Test, index : int )" },
  { dao_Test_GETF_value, ".value( self : Test )=>double" },
  { dao_Test_SETF_value, ".value=( self : Test, value : double )" },
  { dao_Test_Test, "Test(  )=>Test" },
  { dao_Test_Print, "Print( self : Test )" },
  { NULL, NULL }
};
static void Dao_Test_Delete( void *self )
{
	delete (CxxNS::Test*) self;
}

static DaoTypeBase Test_Typer = 
{ "Test", NULL, 
  dao_Test_Nums,
  dao_Test_Meths, 
  { 0 },
  { 0 },
  Dao_Test_Delete,
  NULL
};
DaoTypeBase DAO_DLL_GREETING *dao_Test_Typer = & Test_Typer;
static void dao_Test_GETF_index( DaoContext *_ctx, DValue *_p[], int _n )
{
  CxxNS::Test *self = (CxxNS::Test*)DaoCData_CastData(_p[0]->v.cdata,dao_Test_Typer);
  DaoContext_PutInteger( _ctx, (int) self->index );
}
static void dao_Test_SETF_index( DaoContext *_ctx, DValue *_p[], int _n )
{
  CxxNS::Test *self = (CxxNS::Test*)DaoCData_CastData(_p[0]->v.cdata,dao_Test_Typer);
  self->index = (int) _p[1]->v.i;
}
static void dao_Test_GETF_value( DaoContext *_ctx, DValue *_p[], int _n )
{
  CxxNS::Test *self = (CxxNS::Test*)DaoCData_CastData(_p[0]->v.cdata,dao_Test_Typer);
  DaoContext_PutDouble( _ctx, (double) self->value );
}
static void dao_Test_SETF_value( DaoContext *_ctx, DValue *_p[], int _n )
{
  CxxNS::Test *self = (CxxNS::Test*)DaoCData_CastData(_p[0]->v.cdata,dao_Test_Typer);
  self->value = (double) _p[1]->v.d;
}
static void dao_Test_Test( DaoContext *_ctx, DValue *_p[], int _n )
{
	CxxNS::Test *self = CxxNS::Dao_Test_New();
	DaoContext_PutCData( _ctx, self, dao_Test_Typer );
}
/* greeting.h */
static void dao_Test_Print( DaoContext *_ctx, DValue *_p[], int _n )
{
  CxxNS::Test* self= (CxxNS::Test*) DaoCData_CastData( _p[0]->v.cdata, dao_Test_Typer );
  self->Test::Print(  );
}

/*  greeting.h */


static DaoNumItem dao_otto_Nums[] =
{
  { NULL, 0, 0 }
};
static void dao_otto_otto( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_otto_geta( DaoContext *_ctx, DValue *_p[], int _n );
static void dao_otto_test( DaoContext *_ctx, DValue *_p[], int _n );

static DaoFuncItem dao_otto_Meths[] = 
{
  { dao_otto_otto, "otto( b : int=123 )=>otto" },
  { dao_otto_geta, "geta( self : otto )=>int" },
  { dao_otto_test, "test( self : otto, value : otto )=>otto" },
  { NULL, NULL }
};
static void Dao_otto_Delete( void *self )
{
	delete (otto*) self;
}

static DaoTypeBase otto_Typer = 
{ "otto", NULL, 
  dao_otto_Nums,
  dao_otto_Meths, 
  { 0 },
  { 0 },
  Dao_otto_Delete,
  NULL
};
DaoTypeBase DAO_DLL_GREETING *dao_otto_Typer = & otto_Typer;
/* greeting.h */
static void dao_otto_otto( DaoContext *_ctx, DValue *_p[], int _n )
{
  int b= (int) _p[0]->v.i;
	DaoCxx_otto *_self = DaoCxx_otto_New( b );
	DaoContext_PutResult( _ctx, (DaoBase*) _self->cdata );
}
/* greeting.h */
static void dao_otto_geta( DaoContext *_ctx, DValue *_p[], int _n )
{
  otto* self= (otto*) DaoCData_CastData( _p[0]->v.cdata, dao_otto_Typer );
  int _geta = self->otto::geta(  );
  DaoContext_PutInteger( _ctx, (int) _geta );
}
/* greeting.h */
static void dao_otto_test( DaoContext *_ctx, DValue *_p[], int _n )
{
  if( DaoCData_GetObject( _p[0]->v.cdata ) == NULL ){
    DaoContext_RaiseException( _ctx, DAO_ERROR, "call to protected method" );
    return;
  }
  DaoCxx_otto *self = (DaoCxx_otto*) DaoCData_CastData( _p[0]->v.cdata, dao_otto_Typer );
  otto* value= (otto*) DaoCData_CastData( _p[1]->v.cdata, dao_otto_Typer );
  otto _test = self->DaoWrap_test( *value );
  DaoContext_PutCData( _ctx, (void*)new otto( _test ), dao_otto_Typer );
}

/*  greeting.h */


static DaoNumItem dao_otto2_Nums[] =
{
  { NULL, 0, 0 }
};
static void dao_otto2_otto2( DaoContext *_ctx, DValue *_p[], int _n );

static DaoFuncItem dao_otto2_Meths[] = 
{
  { dao_otto2_otto2, "otto2(  )=>otto2" },
  { NULL, NULL }
};
static void Dao_otto2_Delete( void *self )
{
	delete (otto2*) self;
}
void* dao_cast_otto2_to_otto( void *data )
{
	return (otto*)(otto2*)data;
}

static DaoTypeBase otto2_Typer = 
{ "otto2", NULL, 
  dao_otto2_Nums,
  dao_otto2_Meths, 
  { dao_otto_Typer, 0 },
  { dao_cast_otto2_to_otto,0 },
  Dao_otto2_Delete,
  NULL
};
DaoTypeBase DAO_DLL_GREETING *dao_otto2_Typer = & otto2_Typer;
static void dao_otto2_otto2( DaoContext *_ctx, DValue *_p[], int _n )
{
	DaoCxx_otto2 *self = DaoCxx_otto2_New();
	DaoContext_PutResult( _ctx, (DaoBase*) self->cdata );
}

/*  greeting.h */

/*  greeting.h */

#ifdef __cplusplus
}
#endif

