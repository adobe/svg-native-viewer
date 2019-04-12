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

#import "SVGNSView.h"
#import "Document.h"

#include "svgnative/ports/cg/CGSVGRenderer.h"

@implementation SVGNSView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
	
	Document* d = [[[self window] windowController] document];
	SVGNative::SVGDocument* doc = [d getSVGDocument];
	if (!doc)
		return;
	
	NSGraphicsContext* nsGraphicsContext = [NSGraphicsContext currentContext];
	CGContextRef ctx = (CGContextRef) [nsGraphicsContext CGContext];
	SVGNative::CGSVGRenderer* renderer = static_cast<SVGNative::CGSVGRenderer*>(doc->Renderer());

	CGRect r(dirtyRect);
	
	CGAffineTransform m = {1.0, 0.0, 0.0, -1.0, 0.0, r.size.height};
	CGContextConcatCTM(ctx, m);

	renderer->SetGraphicsContext(ctx);
	
	doc->Render(r.size.width, r.size.height);
	
	renderer->ReleaseGraphicsContext();
}

@end
