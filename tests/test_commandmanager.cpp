#include <gtest/gtest.h>
#include "../core/commands/CommandManager.h"

TEST(CommandManagerTest, HistoryIsEmptyInitially)
{
    CommandManager manager;
    EXPECT_TRUE(manager.history().isEmpty());
}

TEST(CommandManagerTest, LogAddsSingleMessage)
{
    CommandManager manager;
    manager.log("Created Box");

    ASSERT_EQ(manager.history().size(), 1);
    EXPECT_EQ(manager.history().first(), QString("Created Box"));
}

TEST(CommandManagerTest, LogPreservesInsertionOrder)
{
    CommandManager manager;
    manager.log("Created Box");
    manager.log("Moved Box_1");
    manager.log("Deleted Box_1");

    ASSERT_EQ(manager.history().size(), 3);
    EXPECT_EQ(manager.history().at(0), QString("Created Box"));
    EXPECT_EQ(manager.history().at(1), QString("Moved Box_1"));
    EXPECT_EQ(manager.history().at(2), QString("Deleted Box_1"));
}

TEST(CommandManagerTest, HistoryReturnsAllMessages)
{
    CommandManager manager;
    manager.log("Import");
    manager.log("Export");

    const QStringList history = manager.history();

    ASSERT_EQ(history.size(), 2);
    EXPECT_TRUE(history.contains("Import"));
    EXPECT_TRUE(history.contains("Export"));
}
