#include <gtest/gtest.h>
#include "Report.h"

TEST(ReportTest, ConstructorTest) {
    Report report("Monthly", "Initial Content");

    EXPECT_EQ(report.getType(), "Monthly");
    EXPECT_EQ(report.getContent(), "Initial Content");
}

TEST(ReportTest, UpdateReportTest) {
    Report report("Monthly", "Initial Content");
    report.updateReport("Weekly", "Updated Content");

    EXPECT_EQ(report.getType(), "Weekly");
    EXPECT_EQ(report.getContent(), "Updated Content");
}

