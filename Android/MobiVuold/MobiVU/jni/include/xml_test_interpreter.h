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
#ifndef XML_TEST_INTERPRETER_H_INCLUDED
#define XML_TEST_INTERPRETER_H_INCLUDED

#ifndef TEST_RESULT_H
#include "test_result.h"
#endif

#ifndef UNIT_TEST_XML_WRITER_H_INCLUDED
#include "unit_test_xml_writer.h"
#endif

//a simple interpreter which returns an xml report of testing
class xml_test_interpreter
{
    protected:
        void add_problems(UnitTest_XMLWriter & writer, _STRING type, const _VECTOR(test_problem, unit_test_allocator)& vect) const;
        _STRING problem_string(const test_problem& problem) const;
    public:
        //returns the interpretation of a test result
        _STRING interpretation(const test_result& result, _STRING executable_name) const;
        //returns a summary that reports that the tests failed to run.
        _STRING unexpected_termination_interpretation(_STRING executable_name) const;
};

#endif
