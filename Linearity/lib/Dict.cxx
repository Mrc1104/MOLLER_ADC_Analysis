// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME Dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "StreamingDecode.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_tDataSamples(void *p = nullptr);
   static void *newArray_tDataSamples(Long_t size, void *p);
   static void delete_tDataSamples(void *p);
   static void deleteArray_tDataSamples(void *p);
   static void destruct_tDataSamples(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::tDataSamples*)
   {
      ::tDataSamples *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::tDataSamples >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("tDataSamples", ::tDataSamples::Class_Version(), "DataSmpl.h", 8,
                  typeid(::tDataSamples), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::tDataSamples::Dictionary, isa_proxy, 4,
                  sizeof(::tDataSamples) );
      instance.SetNew(&new_tDataSamples);
      instance.SetNewArray(&newArray_tDataSamples);
      instance.SetDelete(&delete_tDataSamples);
      instance.SetDeleteArray(&deleteArray_tDataSamples);
      instance.SetDestructor(&destruct_tDataSamples);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::tDataSamples*)
   {
      return GenerateInitInstanceLocal(static_cast<::tDataSamples*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::tDataSamples*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr tDataSamples::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *tDataSamples::Class_Name()
{
   return "tDataSamples";
}

//______________________________________________________________________________
const char *tDataSamples::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::tDataSamples*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int tDataSamples::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::tDataSamples*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *tDataSamples::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::tDataSamples*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *tDataSamples::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::tDataSamples*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void tDataSamples::Streamer(TBuffer &R__b)
{
   // Stream an object of class tDataSamples.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(tDataSamples::Class(),this);
   } else {
      R__b.WriteClassBuffer(tDataSamples::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_tDataSamples(void *p) {
      return  p ? new(p) ::tDataSamples : new ::tDataSamples;
   }
   static void *newArray_tDataSamples(Long_t nElements, void *p) {
      return p ? new(p) ::tDataSamples[nElements] : new ::tDataSamples[nElements];
   }
   // Wrapper around operator delete
   static void delete_tDataSamples(void *p) {
      delete (static_cast<::tDataSamples*>(p));
   }
   static void deleteArray_tDataSamples(void *p) {
      delete [] (static_cast<::tDataSamples*>(p));
   }
   static void destruct_tDataSamples(void *p) {
      typedef ::tDataSamples current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::tDataSamples

namespace ROOT {
   static TClass *vectorlEunsignedsPintgR_Dictionary();
   static void vectorlEunsignedsPintgR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPintgR(void *p = nullptr);
   static void *newArray_vectorlEunsignedsPintgR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPintgR(void *p);
   static void deleteArray_vectorlEunsignedsPintgR(void *p);
   static void destruct_vectorlEunsignedsPintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned int>*)
   {
      vector<unsigned int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned int>", -2, "vector", 389,
                  typeid(vector<unsigned int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned int>) );
      instance.SetNew(&new_vectorlEunsignedsPintgR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPintgR);
      instance.SetDelete(&delete_vectorlEunsignedsPintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPintgR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<unsigned int>","std::vector<unsigned int, std::allocator<unsigned int> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<unsigned int>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<unsigned int>*>(nullptr))->GetClass();
      vectorlEunsignedsPintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPintgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned int> : new vector<unsigned int>;
   }
   static void *newArray_vectorlEunsignedsPintgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned int>[nElements] : new vector<unsigned int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPintgR(void *p) {
      delete (static_cast<vector<unsigned int>*>(p));
   }
   static void deleteArray_vectorlEunsignedsPintgR(void *p) {
      delete [] (static_cast<vector<unsigned int>*>(p));
   }
   static void destruct_vectorlEunsignedsPintgR(void *p) {
      typedef vector<unsigned int> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned int>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = nullptr);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 389,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<double>","std::vector<double, std::allocator<double> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr))->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete (static_cast<vector<double>*>(p));
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] (static_cast<vector<double>*>(p));
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   // Registration Schema evolution read functions
   int RecordReadRules_Dict() {
      return 0;
   }
   static int _R__UNIQUE_DICT_(ReadRules_Dict) = RecordReadRules_Dict();R__UseDummy(_R__UNIQUE_DICT_(ReadRules_Dict));
} // namespace ROOT
namespace {
  void TriggerDictionaryInitialization_Dict_Impl() {
    static const char* headers[] = {
"StreamingDecode.h",
nullptr
    };
    static const char* includePaths[] = {
"/home/mrc/Programs/ROOT-Cern/include/",
"/home/mrc/Documents/MollerADC/coda3-decoder/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$DataSmpl.h")))  __attribute__((annotate("$clingAutoload$StreamingDecode.h")))  tDataSamples;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "StreamingDecode.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"tDataSamples", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_Dict() {
  TriggerDictionaryInitialization_Dict_Impl();
}
