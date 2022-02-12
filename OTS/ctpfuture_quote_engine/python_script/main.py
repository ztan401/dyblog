from pprint import pprint

with open("table.sql") as fp:
    data = fp.readlines()

sql_list = []
insert_list = []
for i, row in zip(range(len(data)), data):
    if "String," in row:
        name = row.split("String")[0]
        name = name.replace(" ", "")
        sql = """auto {} = std::make_shared<clickhouse::ColumnString>();\n""".format(name)
        sql_list.append(sql)
        sql = """{0}->Append(tick_data.{0});\n""".format(name)
        sql_list.append(sql)
        sql = """block.AppendColumn("{0}"  , {0});\n""".format(name)
        insert_list.append(sql)

    if "Float64," in row:
        name = row.split("Float64")[0]
        name = name.replace(" ", "")
        sql = """auto {} = std::make_shared<clickhouse::ColumnFloat64>();\n""".format(name)
        sql_list.append(sql)
        sql = """{0}->Append(tick_data.{0});\n""".format(name)
        sql_list.append(sql)
        sql = """block.AppendColumn("{0}"  , {0});\n""".format(name)
        insert_list.append(sql)

    if "Int64," in row:
        name = row.split("Int64")[0]
        name = name.replace(" ", "")
        sql = """auto {} = std::make_shared<clickhouse::ColumnInt64>();\n""".format(name)
        sql_list.append(sql)
        sql = """{0}->Append(tick_data.{0});\n""".format(name)
        sql_list.append(sql)
        sql = """block.AppendColumn("{0}"  , {0});\n""".format(name)
        insert_list.append(sql)


pprint(sql_list)
with open("output.cpp", "w+") as fp:
    fp.writelines(sql_list)
    fp.writelines("\n")
    fp.writelines("\n")
    fp.writelines("\n")
    fp.writelines(insert_list)
    fp.close()