/*
Copyright 2020 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "xml/XMLParser.h"

//#include "msxml2.h"
#import "msxml3.dll"
#include <locale>
#include <codecvt>
#include <string>

namespace SVGNative
{
namespace xml
{
    class MSXMLNode final : public XMLNode {
    public:
        MSXMLNode(MSXML2::IXMLDOMElementPtr node)
            : mNode{node}
        {
        }

        ~MSXMLNode()
        {
            mNode.Release();
        }

        SVG_C_CHAR GetName() const override
        {
            return mNode->nodeName;
        }

        SVG_C_CHAR GetValue() const override
        {
            return mNode->Gettext();
        }

        std::unique_ptr<XMLNode> GetFirstNode() const override
        {
            auto newNode = new MSXMLNode{mNode->firstChild};
            return std::unique_ptr<MSXMLNode>(newNode);
        }

        std::unique_ptr<XMLNode> GetNextSibling() const override
        {
            auto newNode = new MSXMLNode{mNode->nextSibling};
            return std::unique_ptr<MSXMLNode>(newNode);
        }

        Attribute GetAttribute(SVG_C_CHAR attrName, SVG_C_CHAR) const override
        {
            if (auto attribute = mNode->getAttributeNode(attrName))
            {
                auto value = attribute->nodeValue;
                value.ChangeType(VT_BSTR);
                return {true, value.bstrVal};
            }
            return {false, {}};
        }
    private:
        MSXML2::IXMLDOMElementPtr mNode{};
    };

    class MSXMLDocument final : public XMLDocument {
    public:
        static std::unique_ptr<XMLDocument> CreateXMLDocument(const char* documentString)
        {
            auto newDocument = new MSXMLDocument(documentString);
            return std::unique_ptr<XMLDocument>(newDocument);
        }

        MSXMLDocument(const char* documentString)
        {
            mDocument.CreateInstance(__uuidof(MSXML2::DOMDocument30));
            static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
            auto documentWString = converter.from_bytes(documentString);
            mDocument->loadXML(documentWString.c_str());
        }

        ~MSXMLDocument()
        {
            mDocument.Release();
        }

        std::unique_ptr<XMLNode> GetFirstNode() const override
        {
            if (!mDocument->documentElement)
                return nullptr;

            auto newNode = new MSXMLNode{mDocument->documentElement};
            return std::unique_ptr<XMLNode>(newNode);
        }
    private:
        MSXML2::IXMLDOMDocumentPtr mDocument{};
    };

    std::unique_ptr<XMLDocument> XMLDocument::CreateXMLDocument(const char* documentString)
    {
        return MSXMLDocument::CreateXMLDocument(documentString);
    }
} // namespace xml
} // namespace SVGNative
