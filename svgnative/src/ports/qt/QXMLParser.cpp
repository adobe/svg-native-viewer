/*
Copyright 2025 Adobe. All rights reserved.
Copyright 2025 KATO Kanryu All rights reserved.
Developed and contributed by KATO Kanryu.

This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "../../xml/XMLParser.h"
#include "svgnative/Config.h"

#include <QDebug>
#include <QXmlStreamReader>
#include <QMap>
#include <map>
#include <stack>
#include <string>
#include <string.h>

namespace SVGNative
{
namespace xml
{
/**
 * XMLNode implemented using the Qt API.
 * This allows you to parse XML without any external libraries.
 */
class QXMLNode final : public XMLNode {
public:
    QXMLNode()
    {}

    const char* GetName() const override
    {
        return mName.c_str();
    }

    const char* GetValue() const override
    {
        // Not implemented.
        return nullptr;
    }

    std::unique_ptr<XMLNode> GetFirstNode() override
    {
        return std::move(mChild);
    }

    std::unique_ptr<XMLNode> GetNextSibling() override
    {
        return std::move(mSibling);
    }

    Attribute GetAttribute(const char* attrName, const char* xmlNSPrefix) const override
    {
        auto it = mAttributes.find(attrName);
        if (it != mAttributes.end())
            return {true, it->second.c_str()};
        if (xmlNSPrefix)
        {
            std::string newAttrName{xmlNSPrefix};
            newAttrName.append(":");
            newAttrName.append(attrName);
            it = mAttributes.find(newAttrName);
            if (it != mAttributes.end())
                return {true, it->second.c_str()};
        }
        return {false, {}};
    }

private:
    friend class QXMLDocument;

    std::string mName;
    std::map<std::string, std::string> mAttributes;
    std::unique_ptr<QXMLNode> mChild;
    std::unique_ptr<QXMLNode> mSibling;
};

/**
 * XMLDocument implemented using the Qt API.
 * This allows you to parse XML without any external libraries.
 */
class QXMLDocument final : public XMLDocument {
public:
    static std::unique_ptr<XMLDocument> CreateXMLDocument(const char* documentString)
    {
        auto newDocument = new QXMLDocument(documentString);
        return std::unique_ptr<XMLDocument>(newDocument);
    }

    ~QXMLDocument()
    {
    }

    std::unique_ptr<XMLNode> GetFirstNode() override
    {
        return std::move(mRootNode);
    }

private:
    QXMLDocument(const char* documentString)
    {
        QAnyStringView view(documentString);
        QXmlStreamReader reader(view);
        QXmlStreamReader::TokenType tp = reader.readNext();
        while(!reader.atEnd()){
            if(reader.isStartElement()) {
                auto node = createNode(reader);
                if (mXMLNodeStack.empty()) {
                    mRootNode = std::unique_ptr<QXMLNode>(node);
                }
                else
                {
                    if (mStartNodeCalled)
                        mXMLNodeStack.top()->mChild = std::unique_ptr<QXMLNode>(node);
                    else
                        mPreviousSilbingXMLNode->mSibling = std::unique_ptr<QXMLNode>(node);
                }
                mXMLNodeStack.push(node);
                mStartNodeCalled = true;
            }
            if(reader.isEndElement()) {
                mPreviousSilbingXMLNode = mXMLNodeStack.top();
                mXMLNodeStack.pop();
                mStartNodeCalled = false;
            }
            reader.readNext();
        }
    }

    QXMLNode* createNode(QXmlStreamReader& reader) {
        auto node = new QXMLNode{};
        node->mName = reader.name().toUtf8().constData();
        for (QXmlStreamAttribute att : reader.attributes()){
            node->mAttributes[att.name().toUtf8().constData()] = att.value().toUtf8().constData();
        }
        return node;
    }

private:
    bool mStartNodeCalled{false};

    // These members have no ownership of the pointers.
    std::stack<QXMLNode*> mXMLNodeStack;
    QXMLNode* mPreviousSilbingXMLNode{};

    std::unique_ptr<QXMLNode> mRootNode;
};

std::unique_ptr<XMLDocument> XMLDocument::CreateXMLDocument(const char* documentString)
{
    return QXMLDocument::CreateXMLDocument(documentString);
}
} // namespace xml
} // namespace SVGNative
