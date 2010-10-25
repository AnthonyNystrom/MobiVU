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
#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#ifndef TEST_PROBLEM_H
#include "test_problem.h"
#endif


//a class to accumulate the results of tests.

class test_result
{
    protected:
        // Name of the test
        _STRING m_name;

        //the accumulated errors
        _VECTOR(test_problem, unit_test_allocator) m_errors;

        //the accumulated failures
        _VECTOR(test_problem, unit_test_allocator) m_failures;

        //results for subtests
        _VECTOR(test_result, unit_test_allocator) m_subresults;

        //the accumulated number of successes (including subtests)
        int m_success_count;


    public:
        //set the name of the test
        void set_name(_STRING name);
        //get the name of the test
        _STRING get_name(void) const;
        //add an error (problem in testing or error in test framework)
        void add_error(const test_problem& new_error);
        //add a failure (test for truth or equality failed)
        void add_failure(const test_problem& new_error);
        //add a success
        void add_success(void);
        //add a subtest result
        void add_result(const test_result& result);
        //delete the contents of the result
        void delete_contents(void);

        //accessorts for the errors and failures in the test result
        const _VECTOR(test_problem, unit_test_allocator)& errors(void) const;
        const _VECTOR(test_problem, unit_test_allocator)& failures(void) const;
        const _VECTOR(test_result, unit_test_allocator)& subresults(void) const;

        int success_count(void) const;
        int total_test_count(void) const;

};


#endif
