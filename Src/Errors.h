/*
Copyright (c) 2012-2015, Coherent Labs AD
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of Coherent UI nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

namespace Coherent
{
namespace UI
{
 
/// View-related error codes
///
enum ViewErrorType
{
	VE_Success, ///< Indicates no error 
	VE_FailedCreation, ///< Indicates failure to create a view 
	VE_FailedRenderingCreation, ///< Indicates failure to create the rendering resources associated with a view 
	VE_ClickThroughFailed, ///< Indicates an error during click-through a query 
	VE_InvalidPath, ///< Indicates that the view was redirected to an invalid path 
	VE_ChildCompositionFailed, ///< Indicates that the composition of a child widget failed
	VE_InvalidCall, ///< Indicates a call on a type of view that is not called
	VE_FrameAlreadyInFlight, ///< Indicates that a frame is already in-flight
	VE_FrameNotReady, ///< Indicates that a frame is still being rendered
	VE_FrameNotRequeted, ///< Indicates that a new frame was not requested
	VE_AbnormalTermination, ///< Indicates an abnormal termination of the view
	VE_QueryNotReady, ///< Indicates that the mouse query hasn't finished yet
	VE_Count
};

/// System-related error-codes
///
enum SystemErrorType
{
	SE_Success,	///< Indicates no error 
	SE_InternalError, ///< Indicates a serious system internal error 
	SE_InvalidKey, ///< Indicates that the user supplied an invalid activation key for this version of the product
	SE_MissingComponent, ///< Indicates that a component required by Coherent UI is missing
	SE_HostCrash, ///< Indicates that the CoherentUI host process has crashed
	SE_HostStartError, ///< Indicates that the Coherent UI host process has failed to start
	SE_Count
};

/// Encapsulates a view-related error
///
struct ViewError
{
	ViewErrorType ErrorCode; ///< Indicates an error code 
	const char* Error; ///< Error description 
};

/// Encapsulates a system-related error
///
struct SystemError
{
	SystemErrorType ErrorCode;	///< Indicates an error code 
	const char* Error;			///< Error description
	int OSError;				///< The error reported by the operating system. Set only if ErrorCode is SE_HostStartError.
};

enum ScriptCallErrorType
{
	SCE_Success,		///< Indicates that the call was successful
	SCE_ArgumentType,	///< Indicates that a script provided argument has different type than the one expected
	SCE_NoSuchMethod,	///< Indicates that there is no such method 
	SCE_NoResult,		///< Indicates that there is no result for this call
};

enum DownloadErrorType
{
	DET_Success, ///<Indicates everything went fine
	DET_DownloadOperationFailed, ///<Indicates the download operation failed

	DET_Count
};

}
}