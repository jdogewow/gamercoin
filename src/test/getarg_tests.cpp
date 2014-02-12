#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-GMR");
    BOOST_CHECK(GetBoolArg("-GMR"));
    BOOST_CHECK(GetBoolArg("-GMR", false));
    BOOST_CHECK(GetBoolArg("-GMR", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-GMRo"));
    BOOST_CHECK(!GetBoolArg("-GMRo", false));
    BOOST_CHECK(GetBoolArg("-GMRo", true));

    ResetArgs("-GMR=0");
    BOOST_CHECK(!GetBoolArg("-GMR"));
    BOOST_CHECK(!GetBoolArg("-GMR", false));
    BOOST_CHECK(!GetBoolArg("-GMR", true));

    ResetArgs("-GMR=1");
    BOOST_CHECK(GetBoolArg("-GMR"));
    BOOST_CHECK(GetBoolArg("-GMR", false));
    BOOST_CHECK(GetBoolArg("-GMR", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noGMR");
    BOOST_CHECK(!GetBoolArg("-GMR"));
    BOOST_CHECK(!GetBoolArg("-GMR", false));
    BOOST_CHECK(!GetBoolArg("-GMR", true));

    ResetArgs("-noGMR=1");
    BOOST_CHECK(!GetBoolArg("-GMR"));
    BOOST_CHECK(!GetBoolArg("-GMR", false));
    BOOST_CHECK(!GetBoolArg("-GMR", true));

    ResetArgs("-GMR -noGMR");  // -GMR should win
    BOOST_CHECK(GetBoolArg("-GMR"));
    BOOST_CHECK(GetBoolArg("-GMR", false));
    BOOST_CHECK(GetBoolArg("-GMR", true));

    ResetArgs("-GMR=1 -noGMR=1");  // -GMR should win
    BOOST_CHECK(GetBoolArg("-GMR"));
    BOOST_CHECK(GetBoolArg("-GMR", false));
    BOOST_CHECK(GetBoolArg("-GMR", true));

    ResetArgs("-GMR=0 -noGMR=0");  // -GMR should win
    BOOST_CHECK(!GetBoolArg("-GMR"));
    BOOST_CHECK(!GetBoolArg("-GMR", false));
    BOOST_CHECK(!GetBoolArg("-GMR", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--GMR=1");
    BOOST_CHECK(GetBoolArg("-GMR"));
    BOOST_CHECK(GetBoolArg("-GMR", false));
    BOOST_CHECK(GetBoolArg("-GMR", true));

    ResetArgs("--noGMR=1");
    BOOST_CHECK(!GetBoolArg("-GMR"));
    BOOST_CHECK(!GetBoolArg("-GMR", false));
    BOOST_CHECK(!GetBoolArg("-GMR", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-GMR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GMR", "eleven"), "eleven");

    ResetArgs("-GMR -bar");
    BOOST_CHECK_EQUAL(GetArg("-GMR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GMR", "eleven"), "");

    ResetArgs("-GMR=");
    BOOST_CHECK_EQUAL(GetArg("-GMR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GMR", "eleven"), "");

    ResetArgs("-GMR=11");
    BOOST_CHECK_EQUAL(GetArg("-GMR", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-GMR", "eleven"), "11");

    ResetArgs("-GMR=eleven");
    BOOST_CHECK_EQUAL(GetArg("-GMR", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-GMR", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-GMR", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-GMR", 0), 0);

    ResetArgs("-GMR -bar");
    BOOST_CHECK_EQUAL(GetArg("-GMR", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-GMR=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-GMR", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-GMR=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-GMR", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--GMR");
    BOOST_CHECK_EQUAL(GetBoolArg("-GMR"), true);

    ResetArgs("--GMR=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-GMR", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noGMR");
    BOOST_CHECK(!GetBoolArg("-GMR"));
    BOOST_CHECK(!GetBoolArg("-GMR", true));
    BOOST_CHECK(!GetBoolArg("-GMR", false));

    ResetArgs("-noGMR=1");
    BOOST_CHECK(!GetBoolArg("-GMR"));
    BOOST_CHECK(!GetBoolArg("-GMR", true));
    BOOST_CHECK(!GetBoolArg("-GMR", false));

    ResetArgs("-noGMR=0");
    BOOST_CHECK(GetBoolArg("-GMR"));
    BOOST_CHECK(GetBoolArg("-GMR", true));
    BOOST_CHECK(GetBoolArg("-GMR", false));

    ResetArgs("-GMR --noGMR");
    BOOST_CHECK(GetBoolArg("-GMR"));

    ResetArgs("-noGMR -GMR"); // GMR always wins:
    BOOST_CHECK(GetBoolArg("-GMR"));
}

BOOST_AUTO_TEST_SUITE_END()
