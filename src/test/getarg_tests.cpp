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
    ResetArgs("-ANN");
    BOOST_CHECK(GetBoolArg("-ANN"));
    BOOST_CHECK(GetBoolArg("-ANN", false));
    BOOST_CHECK(GetBoolArg("-ANN", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ANNo"));
    BOOST_CHECK(!GetBoolArg("-ANNo", false));
    BOOST_CHECK(GetBoolArg("-ANNo", true));

    ResetArgs("-ANN=0");
    BOOST_CHECK(!GetBoolArg("-ANN"));
    BOOST_CHECK(!GetBoolArg("-ANN", false));
    BOOST_CHECK(!GetBoolArg("-ANN", true));

    ResetArgs("-ANN=1");
    BOOST_CHECK(GetBoolArg("-ANN"));
    BOOST_CHECK(GetBoolArg("-ANN", false));
    BOOST_CHECK(GetBoolArg("-ANN", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noANN");
    BOOST_CHECK(!GetBoolArg("-ANN"));
    BOOST_CHECK(!GetBoolArg("-ANN", false));
    BOOST_CHECK(!GetBoolArg("-ANN", true));

    ResetArgs("-noANN=1");
    BOOST_CHECK(!GetBoolArg("-ANN"));
    BOOST_CHECK(!GetBoolArg("-ANN", false));
    BOOST_CHECK(!GetBoolArg("-ANN", true));

    ResetArgs("-ANN -noANN");  // -ANN should win
    BOOST_CHECK(GetBoolArg("-ANN"));
    BOOST_CHECK(GetBoolArg("-ANN", false));
    BOOST_CHECK(GetBoolArg("-ANN", true));

    ResetArgs("-ANN=1 -noANN=1");  // -ANN should win
    BOOST_CHECK(GetBoolArg("-ANN"));
    BOOST_CHECK(GetBoolArg("-ANN", false));
    BOOST_CHECK(GetBoolArg("-ANN", true));

    ResetArgs("-ANN=0 -noANN=0");  // -ANN should win
    BOOST_CHECK(!GetBoolArg("-ANN"));
    BOOST_CHECK(!GetBoolArg("-ANN", false));
    BOOST_CHECK(!GetBoolArg("-ANN", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ANN=1");
    BOOST_CHECK(GetBoolArg("-ANN"));
    BOOST_CHECK(GetBoolArg("-ANN", false));
    BOOST_CHECK(GetBoolArg("-ANN", true));

    ResetArgs("--noANN=1");
    BOOST_CHECK(!GetBoolArg("-ANN"));
    BOOST_CHECK(!GetBoolArg("-ANN", false));
    BOOST_CHECK(!GetBoolArg("-ANN", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ANN", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ANN", "eleven"), "eleven");

    ResetArgs("-ANN -bar");
    BOOST_CHECK_EQUAL(GetArg("-ANN", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ANN", "eleven"), "");

    ResetArgs("-ANN=");
    BOOST_CHECK_EQUAL(GetArg("-ANN", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ANN", "eleven"), "");

    ResetArgs("-ANN=11");
    BOOST_CHECK_EQUAL(GetArg("-ANN", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ANN", "eleven"), "11");

    ResetArgs("-ANN=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ANN", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ANN", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ANN", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ANN", 0), 0);

    ResetArgs("-ANN -bar");
    BOOST_CHECK_EQUAL(GetArg("-ANN", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ANN=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ANN", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ANN=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ANN", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ANN");
    BOOST_CHECK_EQUAL(GetBoolArg("-ANN"), true);

    ResetArgs("--ANN=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ANN", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noANN");
    BOOST_CHECK(!GetBoolArg("-ANN"));
    BOOST_CHECK(!GetBoolArg("-ANN", true));
    BOOST_CHECK(!GetBoolArg("-ANN", false));

    ResetArgs("-noANN=1");
    BOOST_CHECK(!GetBoolArg("-ANN"));
    BOOST_CHECK(!GetBoolArg("-ANN", true));
    BOOST_CHECK(!GetBoolArg("-ANN", false));

    ResetArgs("-noANN=0");
    BOOST_CHECK(GetBoolArg("-ANN"));
    BOOST_CHECK(GetBoolArg("-ANN", true));
    BOOST_CHECK(GetBoolArg("-ANN", false));

    ResetArgs("-ANN --noANN");
    BOOST_CHECK(GetBoolArg("-ANN"));

    ResetArgs("-noANN -ANN"); // ANN always wins:
    BOOST_CHECK(GetBoolArg("-ANN"));
}

BOOST_AUTO_TEST_SUITE_END()
