/*

Copyright (C) 2010-2013 KWARC Group <kwarc.info>

This file is part of MathWebSearch.

MathWebSearch is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MathWebSearch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MathWebSearch.  If not, see <http://www.gnu.org/licenses/>.

*/
/**
 * @brief XmlParser Utils API
 * @file XmlParser.hpp
 * @date 07 May 2014
 */
#ifndef _CRAWLER_PARSER_XMLPARSER_HPP
#define _CRAWLER_PARSER_XMLPARSER_HPP

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <cinttypes>
#include <functional>
#include <string>

#include "common/utils/compiler_defs.h"
#include "common/types/IdDictionary.hpp"

namespace crawler {
namespace parser {

/**
 * @brief parse HTML or XML document
 * @param content document data
 * @param size document size
 * @return xml document handler on success
 *
 * @throw runtime_error if parsing fails
 */
xmlDocPtr parseDocument(const char *content, int size);

typedef std::function<void (xmlNode*)> ProcessXmlNodeCallback;
class XmlNodeProcessor {
 public:
    virtual void foundResults(int count) {
        UNUSED(count);
    }
    virtual void processXmlNode(xmlNode* node) = 0;
};

/**
 * @brief run xpath and process matching results
 * @param xpath string in XPath syntax
 * @param xmlNodeProcessor
 */
void processXpathResults(xmlDoc* doc,
                         const std::string& xpath,
                         XmlNodeProcessor* xmlNodeProcessor);
void processXpathResults(xmlDoc *doc,
                         const std::string &xpath,
                         ProcessXmlNodeCallback processXmlNodeCallback);

std::string getEscapedXmlFromNode(xmlDoc* doc, xmlNode* node);

std::string getXmlFromNode(xmlDoc* doc, xmlNode* node);

typedef common::types::IdDictionary<std::string, uint32_t> MathIdDictionary;

std::string escapeXml(xmlDoc* doc, const std::string& xml);
std::string getTextFromNode(xmlNode* node,
                            MathIdDictionary* encodedMathIds = nullptr);

std::string getTextByXpath(xmlDoc* doc,
                           const std::string& xpath,
                           MathIdDictionary* encodedMathIds = nullptr,
                           xmlNode* contextNode = nullptr);

xmlNode* getNodeByXpath(xmlDoc* doc,
                        const std::string& xpath,
                        xmlNode* contextNode = nullptr);

}  // namespace parser
}  // namespace crawler

#endif  // _CRAWLER_PARSER_XMLPARSER_HPP
