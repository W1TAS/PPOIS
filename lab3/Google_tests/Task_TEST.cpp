#include <gtest/gtest.h>
#include "Task.h"

TEST(TaskTest, ConstructorTest) {
    Task task("Complete project documentation", "2024-12-01");

    EXPECT_EQ(task.getDescription(), "Complete project documentation");
    EXPECT_EQ(task.getDeadline(), "2024-12-01");
    EXPECT_FALSE(task.isCompleted());
}

TEST(TaskTest, SetDescriptionTest) {
    Task task("Complete project documentation", "2024-12-01");
    task.setDescription("Review project documentation");

    EXPECT_EQ(task.getDescription(), "Review project documentation");
}

TEST(TaskTest, SetDeadlineTest) {
    Task task("Complete project documentation", "2024-12-01");
    task.setDeadline("2024-12-05");

    EXPECT_EQ(task.getDeadline(), "2024-12-05");
}

TEST(TaskTest, SetCompletedTest) {
    Task task("Complete project documentation", "2024-12-01");
    task.setCompleted(true);

    EXPECT_TRUE(task.isCompleted());
}

