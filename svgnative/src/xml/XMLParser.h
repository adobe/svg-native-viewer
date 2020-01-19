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

#pragma once

#include <memory>
#include <string>
#ifdef MSXML
#include <cwchar>
#define SVG_C_CHAR const wchar_t*
#define SVG_STRING std::wstring
#else
#define SVG_C_CHAR const char*
#define SVG_STRING std::string
#endif

namespace SVGNative
{
namespace xml
{
    struct Attribute
    {
        Attribute(bool aFound, const SVG_STRING& aValue)
            : found{aFound}
            , value{aValue}
        {}
        bool found{false};
        SVG_STRING value{};
    };

    class XMLNode {
    public:

        virtual SVG_C_CHAR GetName() const = 0;
        virtual SVG_C_CHAR GetValue() const = 0;

        virtual std::unique_ptr<XMLNode> GetFirstNode() const = 0;
        virtual std::unique_ptr<XMLNode> GetNextSibling() const = 0;

        virtual Attribute GetAttribute(SVG_C_CHAR, SVG_C_CHAR nsPrefix = nullptr) const = 0;

        virtual ~XMLNode() {}
    };

    class XMLDocument {
    public:
        static std::unique_ptr<XMLDocument> CreateXMLDocument(const char*  documentString);
        virtual std::unique_ptr<XMLNode> GetFirstNode() const = 0;

        virtual ~XMLDocument() {}
    };
} // namespace xml

} // namespace SVGNative
