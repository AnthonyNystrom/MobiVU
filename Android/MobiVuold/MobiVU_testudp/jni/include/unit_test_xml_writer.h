/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */
#ifndef UNIT_TEST_XML_WRITER_H_INCLUDED
#define UNIT_TEST_XML_WRITER_H_INCLUDED

/**
 * @name UnitTest_XMLWriter Class for creating XML documents.
**/
class UnitTest_XMLWriter
{
    private:
        _STRING document;
        _VECTOR(_STRING, unit_test_allocator) attributes;
        _VECTOR(_STRING, unit_test_allocator) openTags;
        bool tagOpen;  // is the current xml tag open?

        _STRING escape(_STRING value);
    public:
        UnitTest_XMLWriter() :
                document("<?xml version=\"1.0\" encoding=\"UTF-8\" \?>"), tagOpen(false)
        {}

        ~UnitTest_XMLWriter() {}

        /**
         * Creates a new tag by name.
         * @return an id that an be used to close the tag
        **/
        unsigned start(_STRING name);

        /**
         * Creates new attribute to be used in the next created tag.
         * @param name Name of the attribute
         * @param value Value of attribute
        **/
        void addAttribute(_STRING name, _STRING value);

        /**
         * Creates new attribute to be used in the next created tag.
         * @param name Name of the attribute
         * @param value Value of attribute
        **/
        void addAttribute(_STRING name, int value);

        /**
         * Close the last opened tag
        **/
        void close();

        /**
         * Closes tags
         * @param id Closes the tags up to and including the tag identified by id.
        **/
        void close(unsigned id);

        /**
         * Create a new element whose child is a text node.
         * @param name Name of tag
         * @param value Text content to be enclosed by tag.
        **/
        void element(_STRING name, _STRING value);

        /**
         * @return the xml document as a string.
        **/
        _STRING to_str();
};

#endif //UNIT_TEST_XML_WRITER_H_INCLUDED

