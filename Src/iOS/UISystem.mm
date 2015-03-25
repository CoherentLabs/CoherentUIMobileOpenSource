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

#if !defined(COHERENT_UNITY3D_BUILD) || defined(FORCE_COMPILE_FILE)
#include <UISystem.h>

#include <iOS/ViewImpl.h>
#include <ViewListener.h>
#include <iOS/ResourceResponseImpl.h>
#include <iOS/ViewObserver.h>
#include <iOS/ManagedReferences.h>

#include <MobileCoreServices/UTType.h>

#import <iOS/NSData+Base64.h>
#import <CommonCrypto/CommonDigest.h>

@interface CoUIURLResponse : NSHTTPURLResponse
@property (nonatomic) NSInteger statusCode;
@end
@implementation CoUIURLResponse
{}
@end

@interface CoUIProtocol : NSURLProtocol
-(id)initWithRequest:(NSURLRequest*)request cachedResponse:(NSCachedURLResponse*)cachedResponse client:(id<NSURLProtocolClient>)client;
+(BOOL)canInitWithRequest:(NSURLRequest*)request;
-(void)readComplete:(NSData*)data;
@end

namespace Coherent { namespace UI {

class UISystemDefaultLogHandler : public Logging::ILogHandler
{
public:
	virtual ~UISystemDefaultLogHandler() {}

	virtual void WriteLog(Logging::Severity severity, const char* message, size_t length) override
	{
		NSString* type;
		switch(severity)
		{
			case Logging::Trace:
				type = @"TRACE";
				break;
			case Logging::Debug:
				type = @"DEBUG";
				break;
			case Logging::Info:
				type = @"INFO";
				break;
			case Logging::Warning:
				type = @"WARNING";
				break;
			case Logging::AssertFailure:
				type = @"ASSERT";
				break;
			case Logging::Error:
				type = @"ERROR";
				break;
			default:
				break;
		}
		
		NSLog(@"Coherent UI %@: %s", type, message);
	}
	
	virtual void Assert(const char* message) override
	{
		NSLog(@"Coherent UI ASSERT: %s", message);
		assert(false);
	}
};

class LoggingScope
{
public:
	LoggingScope()
		: m_SuppliedLogHandler(nullptr)
	{}
	
	void Initialize(Logging::Severity severity, Logging::ILogHandler* handler)
	{
		m_SuppliedLogHandler = handler;
		bool ownLogHandler = (m_SuppliedLogHandler == nullptr);
		if (ownLogHandler)
		{
			handler = new UISystemDefaultLogHandler();
		}
#if defined(COHERENT_DOT_NET)
		else
		{
			Coherent::UI::ManagedReferences::AddReference(m_SuppliedLogHandler);
		}
#endif
		
#if defined(COHERENT_UNITY3D_BUILD) && !defined(COHERENT_PLATFORM_IOS)
		handler = new UnityLogHandlerDecorator(handler, ownLogHandler);
		ownLogHandler = true;
#endif
		
		Coherent::Logging::Initialize(severity, handler, ownLogHandler);
	}
	
	~LoggingScope()
	{
		Coherent::Logging::Uninitialize();
#if defined(COHERENT_DOT_NET)
		if (m_SuppliedLogHandler)
		{
			Coherent::UI::ManagedReferences::ReleaseReference(m_SuppliedLogHandler);
		}
#endif
	}
private:
	Logging::ILogHandler* m_SuppliedLogHandler;
};
	
class DefaultFileHandler : public FileHandler
{
public:
	virtual ~DefaultFileHandler() {}
	
	virtual void ReadFile(const CharType* url, ResourceResponse* response)
	{
		NSString* strURL = CreateNSStringFrom(url);
		NSString* filename = [strURL substringFromIndex:6];
		NSString* directory = [filename stringByDeletingLastPathComponent];
		
		NSURL* theUrl = [NSURL URLWithString:filename];
		NSString* comp = [theUrl lastPathComponent];
		NSString* strippedFilename = [comp stringByDeletingPathExtension];
		NSString* extFile = [[theUrl lastPathComponent] pathExtension];

		NSString* bundlePath = [[NSBundle mainBundle]
		pathForResource:strippedFilename ofType:extFile inDirectory:directory];
		NSData* data = [NSData dataWithContentsOfFile:bundlePath];
		
		if(data) {
			auto respData = response->GetBuffer([data length]);
			
			memcpy(respData, [data bytes], [data length]);
			
			response->SignalSuccess();
		} else {
			response->SignalFailure();
		}
	}
};
	
class System : public UISystem, public ViewObserver
{
public:
	System();
	virtual ~System();
	
	bool Initialize(const char* license, const SystemSettings& settings, EventListener* listener, Logging::Severity severity, Logging::ILogHandler* logHandler, FileHandler* fileHandler);
	
	virtual bool Uninitialize() override;
	virtual void Update() override;
	virtual void CreateView(const ViewInfo& info, const CharType* path, ViewListenerBase* listener) override;
	
	virtual void OnViewDestroyed(unsigned int viewId) override;
	
	void PushRequestRead(const CustomString& url, CoUIProtocol* protocol);
	void RequestComplete(CoUIProtocol* protocol, ResourceResponseImpl* response);
		
private:
	// Views are always looked up by iterating the vector, never indexed
	std::vector<ViewImpl*> m_ActiveViews;
	unsigned int m_NextViewId;
	
	EventListener* m_Listener;
	SystemSettings m_Settings;
	
	LoggingScope m_Logging;
	
	FileHandler* m_FileHandler;
	bool m_OwnsFileHandler;
	
	bool m_HasNotifiedReady;

	std::queue<std::pair<CustomString, CoUIProtocol*>> m_RequestedReads;
};

System* g_System = nullptr;
	
System::System()
	: m_NextViewId(0)
	, m_Listener(nullptr)
	, m_HasNotifiedReady(false)
	, m_FileHandler(nullptr)
	, m_OwnsFileHandler(false)
{}

System::~System()
{}
	
bool System::Initialize(const char* license, const SystemSettings& settings, EventListener* listener, Logging::Severity severity, Logging::ILogHandler* logHandler, FileHandler* fileHandler)
{
	m_Logging.Initialize(severity, logHandler);
		
	m_Settings = settings;
	m_Listener = listener;
	
	m_FileHandler = fileHandler;
	if(!m_FileHandler) {
		m_FileHandler = new DefaultFileHandler;
		m_OwnsFileHandler = true;
	}
	
	[NSURLProtocol registerClass:[CoUIProtocol class]];

	const bool isDefaultLogHandler = logHandler == nullptr;
	
	COHERENT_INFO("Initializing Coherent UI ver. ", COHERENT_UI_MAJOR_VERSION, ".", COHERENT_UI_MINOR_VERSION, ".", COHERENT_UI_PATCH_VERSION, ".", COHERENT_UI_BUILD_VERSION);
	COHERENT_DEBUG("Is using default log handler {",
				   isDefaultLogHandler ? "yes" : "no", "}");
    
    if(settings.iOS_UseURLCache) {
        NSURLCache* sharedCache = [[NSURLCache alloc] initWithMemoryCapacity:settings.iOS_URLMemoryCacheSize diskCapacity:settings.iOS_URLDiskCacheSize diskPath:@"coui_urlcache"];
        [NSURLCache setSharedURLCache:sharedCache];
    }
    
	return true;
}

bool System::Uninitialize()
{
	COHERENT_INFO("Uninitializing Coherent UI");
	
	[NSURLProtocol unregisterClass:[CoUIProtocol class]];
	
	if(m_OwnsFileHandler) {
		delete m_FileHandler;
		m_FileHandler = nullptr;
	}
	
	// Destory all remaining views (CAUTION - we delete the views from the collection as we iterate)
	auto viewsCopy = m_ActiveViews;
	std::for_each(viewsCopy.begin(), viewsCopy.end(), [](ViewImpl* view) { view->Destroy(); });
	
	COHERENT_INFO("Coherent UI Uninitialized");
	
	g_System = nullptr;
	delete this;
	return true;
}
	
void System::Update()
{
	if(!m_HasNotifiedReady) {
		if(m_Listener) {
			m_Listener->SystemReady();
		}
		m_HasNotifiedReady = true;
	}

	// request all pending reads
	while(!m_RequestedReads.empty())
	{
		auto& request = m_RequestedReads.front();
		ResourceResponseImpl* response(new ResourceResponseImpl(std::bind(&System::RequestComplete, this, request.second, std::placeholders::_1)));
	
		COHERENT_DEBUG("Requesting read of resource: ", request.first);
	
		m_FileHandler->ReadFile(request.first.c_str(), response);
		
		m_RequestedReads.pop();
	}
	
	std::for_each(m_ActiveViews.begin(), m_ActiveViews.end(), [](ViewImpl* view) { view->Update(); } );
}
	
void System::CreateView(const ViewInfo& info, const CharType* path, ViewListenerBase* listener)
{
	COHERENT_DEBUG("Starting view creation with options:");
	COHERENT_DEBUG("Parent {", (int)info.Width, "}");
	COHERENT_DEBUG("X {", info.X, "}");
	COHERENT_DEBUG("Y {", info.Y, "}");
	COHERENT_DEBUG("Width {", info.Width, "}");
	COHERENT_DEBUG("Height {", info.Height, "}");
	COHERENT_DEBUG("SupportClickThrough {", info.SupportClickThrough, "}");
	COHERENT_DEBUG("IsTransparent {", info.IsTransparent, "}");
	COHERENT_DEBUG("Path {", path ? path : reinterpret_cast<const CharType*>(L""), "}");
	
	ViewImpl* newView = new ViewImpl(m_NextViewId++, info, listener, this);
	
	if(!newView->Initialize(path)) {
		delete newView;
		
		ViewError error = { VE_FailedCreation, "Unable to create view" };
		listener->OnError(error);
		return;
	}

	m_ActiveViews.push_back(newView);
	
	COHERENT_INFO("Created view ", m_NextViewId-1);
}
	
void System::OnViewDestroyed(unsigned int viewId)
{
	auto it = std::find_if(m_ActiveViews.begin(), m_ActiveViews.end(), [viewId](ViewImpl* view) { return viewId == view->GetId(); });
	
	if(it == m_ActiveViews.end()) {
		COHERENT_WARNING("Destroying unregistered view!");
		return;
	}
	
	auto theView = *it;
	
	COHERENT_INFO("Destroyed view ", theView->GetId());
	
	m_ActiveViews.erase(it);
	
	delete theView;
}
	
void System::PushRequestRead(const CustomString& url, CoUIProtocol* protocol)
{
	m_RequestedReads.push(std::make_pair(url, protocol));
}

void System::RequestComplete(CoUIProtocol* protocol, ResourceResponseImpl* response)
{
	if(response->HasFailed()) {
		[protocol readComplete:nil];
	} else {
		[protocol readComplete:response->GetData()];
	}
	
	delete response;
}

}}

@implementation CoUIProtocol
{
	NSURLRequest* m_Request;
	NSCachedURLResponse* m_CachedResponse;
	id<NSURLProtocolClient> m_Client;
	
	Coherent::CustomString m_UnescapedURL;
}
-(id)initWithRequest:(NSURLRequest*)request cachedResponse:(NSCachedURLResponse*)cachedResponse client:(id<NSURLProtocolClient>)client
{
	self = [super initWithRequest:request cachedResponse:cachedResponse client:client];
	m_Request = request;
	m_CachedResponse = cachedResponse;
	m_Client = client;
	
	return self;
}

+(BOOL)canInitWithRequest:(NSURLRequest*)request
{
	NSURL* url = request.URL;
	
	if([[url scheme] isEqualToString:@"coui"]) {
		return YES;
	}
	return NO;
}

+(NSURLRequest*)canonicalRequestForRequest:(NSURLRequest*)request
{
	return request;
}

-(NSString*)getMIMETypeForExtension:(NSString*)extension
{
	CFStringRef cfExt = (__bridge_retained CFStringRef)extension;
	CFStringRef type = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, cfExt, NULL);
	CFRelease(cfExt);
	
	NSString* mimeType = (__bridge_transfer NSString*) UTTypeCopyPreferredTagWithClass(type, kUTTagClassMIMEType);
	
	if(type != NULL) {
		CFRelease(type);
	}
	return mimeType;
}

-(void)startLoading
{
	m_UnescapedURL = Coherent::CreateFromNSString([[m_Request.URL absoluteString] stringByReplacingPercentEscapesUsingEncoding:NSASCIIStringEncoding]);
	Coherent::UI::g_System->PushRequestRead(m_UnescapedURL, self);
}

-(void)stopLoading
{
}

- (void)readComplete:(NSData*)data
{
	if (data)
	{
		COHERENT_DEBUG("Read of resource: ",
					   m_UnescapedURL,
					   " successful; size: ",
					   [data length]);

		NSString* filename
			= [[m_Request URL] lastPathComponent];
		NSString* extension = [[filename lastPathComponent] pathExtension];
		NSString* mimeType = [self getMIMETypeForExtension:extension];
		NSDictionary* headers = nil;
		if (mimeType == nil && [@"css" isEqualToString:extension])
		{
			mimeType = @"text/css";
		}
		if (mimeType != nil)
		{
			headers = [[NSDictionary alloc]
				initWithObjectsAndKeys:mimeType, @"Content-Type", nil];
		}
		else
		{
			headers = [[NSDictionary alloc]
				initWithObjectsAndKeys:@"text/plain",
									   @"Content-Type",
									   @"UTF-8",
									   @"Content-Encoding",
									   nil];
		}

		CoUIURLResponse* response
			= [[CoUIURLResponse alloc] initWithURL:[[self request] URL]
										statusCode:200
									   HTTPVersion:@"HTTP/1.1"
									  headerFields:headers];

		[[self client] URLProtocol:self
				didReceiveResponse:response
				cacheStoragePolicy:NSURLCacheStorageNotAllowed];
		[[self client] URLProtocol:self didLoadData:data];
		[[self client] URLProtocolDidFinishLoading:self];
	}
	else
	{
		COHERENT_DEBUG("Read of resource: ", m_UnescapedURL, " failed");
		NSError* error = [[NSError alloc] initWithDomain:NSOSStatusErrorDomain
													code:404
												userInfo:nil];
		[[self client] URLProtocol:self didFailWithError:error];
	}
}

@end

bool CheckUISDKVersionExact(int sdkVersion)
{
	return sdkVersion == COHERENT_UI_SDK_VER;
}

bool CheckUISDKVersionCompatible(int sdkVersion)
{
	// After masking with this, the version has its build number ignored
	const int COUI_SDK_VER_COMPATIBILITY_MASK = ~((1 << COHERENT_BITS_PER_VERSION) - 1);

	return (sdkVersion & COUI_SDK_VER_COMPATIBILITY_MASK) == (COHERENT_UI_SDK_VER & COUI_SDK_VER_COMPATIBILITY_MASK);
}

Coherent::UI::UISystem* InitializeUISystem(  int sdkVersion
										   , const char* licenseKey
										   , const Coherent::UI::SystemSettings& settings
										   , Coherent::UI::EventListener* listener
										   , Coherent::Logging::Severity logSeverity
										   , Coherent::Logging::ILogHandler* logHandler
										   , Coherent::UI::FileHandler* fileHandler
										   , void*
										   )
{
	if (!CheckUISDKVersionCompatible(sdkVersion))
	{
		NSLog(@"SDK Binary version (%d) is not compatible with the initialization one (%d)!", COHERENT_UI_SDK_VER, sdkVersion);
		return nullptr;
	}
	else if (!CheckUISDKVersionExact(sdkVersion))
	{
		NSLog(@"SDK Binary version (%d) is compatible with the initialization one (%d) but does not match!", COHERENT_UI_SDK_VER, sdkVersion);
	}
	

	auto system = new Coherent::UI::System;
	if(!system->Initialize(licenseKey, settings, listener, logSeverity, logHandler, fileHandler))
	{
		system->Uninitialize();
		return nullptr;
	}
	Coherent::UI::g_System = system;
	return system;
}
#endif
