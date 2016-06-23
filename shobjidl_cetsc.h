// shobjidl_cetsc.h
//

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Tue Mar 08 17:16:10 2005
 */
/* Compiler settings for .\shobjidl.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
	#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
	#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
	#include "windows.h"
	#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __shobjidl_h__
	#define __shobjidl_h__

	/* Forward Declarations */

	#ifndef __ITaskbarList_FWD_DEFINED__
	#define __ITaskbarList_FWD_DEFINED__
	typedef interface ITaskbarList ITaskbarList;
	#endif 	/* __ITaskbarList_FWD_DEFINED__ */


	#ifndef __ITaskbarList2_FWD_DEFINED__
	#define __ITaskbarList2_FWD_DEFINED__
	typedef interface ITaskbarList2 ITaskbarList2;
	#endif 	/* __ITaskbarList2_FWD_DEFINED__ */

	#ifndef __TaskbarList_FWD_DEFINED__
	#define __TaskbarList_FWD_DEFINED__

	#ifdef __cplusplus
	typedef class TaskbarList TaskbarList;
	#else
	typedef struct TaskbarList TaskbarList;
	#endif /* __cplusplus */

	#endif 	/* __TaskbarList_FWD_DEFINED__ */


	/* header files for imported files */
	#include "objidl.h"
	#include "oleidl.h"
	#include "oaidl.h"
	#include "shtypes.h"
	#include "servprov.h"

	#ifdef __cplusplus
	extern "C"{
	#endif 


	/* interface __MIDL_itf_shobjidl_0125 */
	/* [local] */ 

	//-------------------------------------------------------------------------
	//
	// ITaskbarList interface
	//
	//
	// [Member functions]
	//
	// ITaskbarList::HrInit()
	//   This function must be called first to validate use of other members.
	//
	// ITaskbarList::AddTab(hwnd)
	//   This function adds a tab for hwnd to the taskbar.
	//
	// ITaskbarList::DeleteTab(hwnd)
	//   This function deletes a tab for hwnd from the taskbar.
	//
	// ITaskbarList::ActivateTab(hwnd)
	//   This function activates the tab associated with hwnd on the taskbar.
	//
	// ITaskbarList::SetActivateAlt(hwnd)
	//   This function marks hwnd in the taskbar as the active tab
	//
	//-------------------------------------------------------------------------


	extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0125_v0_0_c_ifspec;
	extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0125_v0_0_s_ifspec;

	#ifndef __ITaskbarList_INTERFACE_DEFINED__
	#define __ITaskbarList_INTERFACE_DEFINED__

	/* interface ITaskbarList */
	/* [object][uuid] */ 


	EXTERN_C const IID IID_ITaskbarList;

	#if defined(__cplusplus) && !defined(CINTERFACE)
    
		MIDL_INTERFACE("56FDF342-FD6D-11d0-958A-006097C9A090")
		ITaskbarList : public IUnknown
		{
		public:
			virtual HRESULT STDMETHODCALLTYPE HrInit( void) = 0;
        
			virtual HRESULT STDMETHODCALLTYPE AddTab( 
				/* [in] */ HWND hwnd) = 0;
        
			virtual HRESULT STDMETHODCALLTYPE DeleteTab( 
				/* [in] */ HWND hwnd) = 0;
        
			virtual HRESULT STDMETHODCALLTYPE ActivateTab( 
				/* [in] */ HWND hwnd) = 0;
        
			virtual HRESULT STDMETHODCALLTYPE SetActiveAlt( 
				/* [in] */ HWND hwnd) = 0;
        
		};
    
	#else 	/* C style interface */

		typedef struct ITaskbarListVtbl
		{
			BEGIN_INTERFACE
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
				ITaskbarList __RPC_FAR * This,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
			ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
				ITaskbarList __RPC_FAR * This);
        
			ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
				ITaskbarList __RPC_FAR * This);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HrInit )( 
				ITaskbarList __RPC_FAR * This);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddTab )( 
				ITaskbarList __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteTab )( 
				ITaskbarList __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ActivateTab )( 
				ITaskbarList __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetActiveAlt )( 
				ITaskbarList __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			END_INTERFACE
		} ITaskbarListVtbl;

		interface ITaskbarList
		{
			CONST_VTBL struct ITaskbarListVtbl __RPC_FAR *lpVtbl;
		};

	#ifdef COBJMACROS


	#define ITaskbarList_QueryInterface(This,riid,ppvObject)	\
		(This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

	#define ITaskbarList_AddRef(This)	\
		(This)->lpVtbl -> AddRef(This)

	#define ITaskbarList_Release(This)	\
		(This)->lpVtbl -> Release(This)


	#define ITaskbarList_HrInit(This)	\
		(This)->lpVtbl -> HrInit(This)

	#define ITaskbarList_AddTab(This,hwnd)	\
		(This)->lpVtbl -> AddTab(This,hwnd)

	#define ITaskbarList_DeleteTab(This,hwnd)	\
		(This)->lpVtbl -> DeleteTab(This,hwnd)

	#define ITaskbarList_ActivateTab(This,hwnd)	\
		(This)->lpVtbl -> ActivateTab(This,hwnd)

	#define ITaskbarList_SetActiveAlt(This,hwnd)	\
		(This)->lpVtbl -> SetActiveAlt(This,hwnd)

	#endif /* COBJMACROS */


	#endif 	/* C style interface */



	HRESULT STDMETHODCALLTYPE ITaskbarList_HrInit_Proxy( 
		ITaskbarList __RPC_FAR * This);


	void __RPC_STUB ITaskbarList_HrInit_Stub(
		IRpcStubBuffer *This,
		IRpcChannelBuffer *_pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD *_pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ITaskbarList_AddTab_Proxy( 
		ITaskbarList __RPC_FAR * This,
		/* [in] */ HWND hwnd);


	void __RPC_STUB ITaskbarList_AddTab_Stub(
		IRpcStubBuffer *This,
		IRpcChannelBuffer *_pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD *_pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ITaskbarList_DeleteTab_Proxy( 
		ITaskbarList __RPC_FAR * This,
		/* [in] */ HWND hwnd);


	void __RPC_STUB ITaskbarList_DeleteTab_Stub(
		IRpcStubBuffer *This,
		IRpcChannelBuffer *_pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD *_pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ITaskbarList_ActivateTab_Proxy( 
		ITaskbarList __RPC_FAR * This,
		/* [in] */ HWND hwnd);


	void __RPC_STUB ITaskbarList_ActivateTab_Stub(
		IRpcStubBuffer *This,
		IRpcChannelBuffer *_pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD *_pdwStubPhase);


	HRESULT STDMETHODCALLTYPE ITaskbarList_SetActiveAlt_Proxy( 
		ITaskbarList __RPC_FAR * This,
		/* [in] */ HWND hwnd);


	void __RPC_STUB ITaskbarList_SetActiveAlt_Stub(
		IRpcStubBuffer *This,
		IRpcChannelBuffer *_pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD *_pdwStubPhase);



	#endif 	/* __ITaskbarList_INTERFACE_DEFINED__ */


	#ifndef __ITaskbarList2_INTERFACE_DEFINED__
	#define __ITaskbarList2_INTERFACE_DEFINED__

	/* interface ITaskbarList2 */
	/* [object][uuid] */ 


	EXTERN_C const IID IID_ITaskbarList2;

	#if defined(__cplusplus) && !defined(CINTERFACE)
    
		MIDL_INTERFACE("602D4995-B13A-429b-A66E-1935E44F4317")
		ITaskbarList2 : public ITaskbarList
		{
		public:
			virtual HRESULT STDMETHODCALLTYPE MarkFullscreenWindow( 
				/* [in] */ HWND hwnd,
				/* [in] */ BOOL fFullscreen) = 0;
        
		};
    
	#else 	/* C style interface */

		typedef struct ITaskbarList2Vtbl
		{
			BEGIN_INTERFACE
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
				ITaskbarList2 __RPC_FAR * This,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
			ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
				ITaskbarList2 __RPC_FAR * This);
        
			ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
				ITaskbarList2 __RPC_FAR * This);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HrInit )( 
				ITaskbarList2 __RPC_FAR * This);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddTab )( 
				ITaskbarList2 __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteTab )( 
				ITaskbarList2 __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ActivateTab )( 
				ITaskbarList2 __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetActiveAlt )( 
				ITaskbarList2 __RPC_FAR * This,
				/* [in] */ HWND hwnd);
        
			HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MarkFullscreenWindow )( 
				ITaskbarList2 __RPC_FAR * This,
				/* [in] */ HWND hwnd,
				/* [in] */ BOOL fFullscreen);
        
			END_INTERFACE
		} ITaskbarList2Vtbl;

		interface ITaskbarList2
		{
			CONST_VTBL struct ITaskbarList2Vtbl __RPC_FAR *lpVtbl;
		};

    

	#ifdef COBJMACROS


	#define ITaskbarList2_QueryInterface(This,riid,ppvObject)	\
		(This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

	#define ITaskbarList2_AddRef(This)	\
		(This)->lpVtbl -> AddRef(This)

	#define ITaskbarList2_Release(This)	\
		(This)->lpVtbl -> Release(This)


	#define ITaskbarList2_HrInit(This)	\
		(This)->lpVtbl -> HrInit(This)

	#define ITaskbarList2_AddTab(This,hwnd)	\
		(This)->lpVtbl -> AddTab(This,hwnd)

	#define ITaskbarList2_DeleteTab(This,hwnd)	\
		(This)->lpVtbl -> DeleteTab(This,hwnd)

	#define ITaskbarList2_ActivateTab(This,hwnd)	\
		(This)->lpVtbl -> ActivateTab(This,hwnd)

	#define ITaskbarList2_SetActiveAlt(This,hwnd)	\
		(This)->lpVtbl -> SetActiveAlt(This,hwnd)


	#define ITaskbarList2_MarkFullscreenWindow(This,hwnd,fFullscreen)	\
		(This)->lpVtbl -> MarkFullscreenWindow(This,hwnd,fFullscreen)

	#endif /* COBJMACROS */


	#endif 	/* C style interface */



	HRESULT STDMETHODCALLTYPE ITaskbarList2_MarkFullscreenWindow_Proxy( 
		ITaskbarList2 __RPC_FAR * This,
		/* [in] */ HWND hwnd,
		/* [in] */ BOOL fFullscreen);


	void __RPC_STUB ITaskbarList2_MarkFullscreenWindow_Stub(
		IRpcStubBuffer *This,
		IRpcChannelBuffer *_pRpcChannelBuffer,
		PRPC_MESSAGE _pRpcMessage,
		DWORD *_pdwStubPhase);



	#endif 	/* __ITaskbarList2_INTERFACE_DEFINED__ */

	EXTERN_C const CLSID CLSID_TaskbarList;

	#ifdef __cplusplus

	class DECLSPEC_UUID("56FDF344-FD6D-11d0-958A-006097C9A090")
	TaskbarList;
	#endif

	#ifdef __cplusplus
	}
	#endif

#endif