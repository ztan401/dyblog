#include <clickhouse/base/socket.h>
#include <clickhouse/client.h>
#include <clickhouse/error_codes.h>
#include <clickhouse/types/type_parser.h>
#include <cmath>
#include <iostream>
#include <stdexcept>

void test_client() {
    using namespace clickhouse;
    // Initialize client connection.
    Client client(ClientOptions()
                          .SetHost("localhost")
                          .SetPort(9000)
                          .SetUser("default")
                          .SetPassword("default")
                          .SetPingBeforeQuery(true));


    // Create a table.
    client.Execute("CREATE DATABASE IF NOT EXISTS test;");
    client.Execute("CREATE TABLE IF NOT EXISTS test.numbers (id UInt64, name String) ENGINE = Memory");

    // Insert some values.
    {
        Block block;

        auto id = std::make_shared<ColumnUInt64>();
        id->Append(1);
        id->Append(7);

        auto name = std::make_shared<ColumnString>();
        name->Append("one");
        name->Append("seven");

        block.AppendColumn("id", id);
        block.AppendColumn("name", name);

        client.Insert("test.numbers", block);
    }

    // Select values inserted in the previous step.
    client.Select("SELECT id, name FROM test.numbers", [](const Block &block) {
        for (size_t i = 0; i < block.GetRowCount(); ++i) {
            std::cout << block[0]->As<ColumnUInt64>()->At(i) << " "
                      << block[1]->As<ColumnString>()->At(i) << "\n";
        }
    });

    //    // Delete table.
    //    client.Execute("DROP TABLE test.numbers");
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    test_client();
    return 0;
}
