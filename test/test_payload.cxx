

#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>
#include <payload.h>
#include <glog/logging.h>

#include "utils.h"

#include "json.h"
using json = nlohmann::json;


TEST(payload, creation)
{

    auto payload = Payload::fromString("Foobar");

    EXPECT_EQ("Foobar", payload.toString());

}



