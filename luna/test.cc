//
// Created by xi on 18-11-15.
//

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

BOOST_AUTO_TEST_CASE(testMain)
{
    bool ret = true;
    BOOST_CHECK(ret);
}