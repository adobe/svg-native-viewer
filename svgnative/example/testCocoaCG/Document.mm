/*
Copyright 2019 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#import "Document.h"
#include "svgnative/ports/cg/CGSVGRenderer.h"
#include "svgnative/SVGDocument.h"

#include <stdexcept>

@interface Document ()

@end

@implementation Document

- (instancetype)init {
	self = [super init];
	arrayLock = [NSLock new] ;
	if (self)
		mSVGDocument = nil;
	return self;
}

- (void) dealloc
{
	[arrayLock lock];
	if (mSVGDocument)
	{
		delete mSVGDocument;
		mSVGDocument = nil;
	}
	[arrayLock unlock];
}

+ (BOOL)autosavesInPlace {
	return YES;
}


- (NSString *)windowNibName {
	// Override returning the nib file name of the document
	// If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
	return @"Document";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController {
	[super windowControllerDidLoadNib:aController];
	// Add any code here that needs to be executed once the windowController has loaded the document's window.
	
	if (!mSVGDocument)
		return;
	
	NSSize size;
	size.height = mSVGDocument->Height();
	size.width = mSVGDocument->Width();
	
	[[aController window] setContentSize:size];
	
	NSRect allBounds= [aController window].frame;
	
	allBounds.origin.x = 0;
	allBounds.origin.y = 0;
	allBounds.size.width = mSVGDocument->Width();
	allBounds.size.height = mSVGDocument->Height();
	[[aController window] setFrame:allBounds display:YES animate:TRUE];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError {
	// Insert code here to write your document to data of the specified type. If outError != NULL, ensure that you create and set an appropriate error if you return nil.
	// Alternatively, you could remove this method and override -fileWrapperOfType:error:, -writeToURL:ofType:error:, or -writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.
	[NSException raise:@"UnimplementedMethod" format:@"%@ is unimplemented", NSStringFromSelector(_cmd)];
	return nil;
}


- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError {
	// Insert code here to read your document from the given data of the specified type. If outError != NULL, ensure that you create and set an appropriate error if you return NO.
	// Alternatively, you could remove this method and override -readFromFileWrapper:ofType:error: or -readFromURL:ofType:error: instead.
	// If you do, you should also override -isEntireFileLoaded to return NO if the contents are lazily loaded.
	std::shared_ptr<SVGNative::CGSVGRenderer> renderer = std::make_shared<SVGNative::CGSVGRenderer>();
	NSString* myString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
	mSVGDocument = SVGNative::SVGDocument::CreateSVGDocument([myString UTF8String], renderer).release();

	return YES;
}

- (SVGNative::SVGDocument*)getSVGDocument
{
	return mSVGDocument;
}

- (void)Lock
{
	[arrayLock lock];
}

- (void)Unlock
{
	[arrayLock unlock];
}

@end
