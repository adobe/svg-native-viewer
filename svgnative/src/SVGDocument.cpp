/*
Copyright 2014 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "SVGDocument.h"
#include "SVGDocumentImpl.h"
#include "SVGRenderer.h"
#ifdef STYLE_SUPPORT
#include "StyleSheet/Document.h"
#include "StyleSheet/Parser.h"
#endif

#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/detail/xml_parser_read_rapidxml.hpp>
#include <stdexcept>
#include <stdlib.h>
#include <string>

using namespace boost::property_tree::detail::rapidxml;

namespace SVGNative
{
std::unique_ptr<SVGDocument> SVGDocument::CreateSVGDocument(const char* s, std::shared_ptr<SVGRenderer> renderer)
{
    try
    {
        auto realSVGDoc = std::unique_ptr<SVGDocumentImpl>(new SVGDocumentImpl(renderer));
        realSVGDoc->mXMLDocument.parse<0>((char*)s); // 0 means default parse flags
        realSVGDoc->TraverseSVGTree();
        realSVGDoc->mXMLDocument.clear();

        auto retval = new SVGDocument();
        retval->mDocument = std::move(realSVGDoc);

        return std::unique_ptr<SVGDocument>(retval);
    }
    catch (...)
    {
    }

    return nullptr;
}

SVGDocument::SVGDocument() {}

SVGDocument::~SVGDocument() {}

void SVGDocument::Render()
{
    ColorMap colorMap;
    mDocument->Render(colorMap, mDocument->mViewBox[2], mDocument->mViewBox[3]);
}

void SVGDocument::Render(float width, float height)
{
    ColorMap colorMap;
    mDocument->Render(colorMap, width, height);
}

void SVGDocument::Render(const ColorMap& colorMap)
{
    mDocument->Render(colorMap, mDocument->mViewBox[2], mDocument->mViewBox[3]);
}

void SVGDocument::Render(const ColorMap& colorMap, float width, float height) { mDocument->Render(colorMap, width, height); }

std::int32_t SVGDocument::Width() const { return static_cast<std::int32_t>(mDocument->mViewBox[2]); }

std::int32_t SVGDocument::Height() const { return static_cast<std::int32_t>(mDocument->mViewBox[3]); }

SVGRenderer* SVGDocument::Renderer() const { return mDocument->mRenderer.get(); }

#ifdef STYLE_SUPPORT
void SVGDocument::AddCustomCSS(const StyleSheet::CssDocument* cssDocument) { mDocument->AddCustomCSS(cssDocument); }

void SVGDocument::ClearCustomCSS() { mDocument->ClearCustomCSS(); }
#endif
} // namespace SVGNative
