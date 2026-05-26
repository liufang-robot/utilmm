#include <boost/test/unit_test.hpp>

#include "testsuite.hh"
#include <utilmm/configfile/shell_expand.hh>

#include <cstdlib>
#include <string>

BOOST_AUTO_TEST_CASE( test_expansion )
{
    setenv("UTILMM_TEST_HOME", "/tmp/utilmm-test-home", 1);
    BOOST_REQUIRE_EQUAL(
        utilmm::shell_expand("prefix-$UTILMM_TEST_HOME-suffix"),
        "prefix-/tmp/utilmm-test-home-suffix");
}
