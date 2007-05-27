#!/usr/bin/env io

conn := PostgreSQL Connection clone setDatabase("iotest") open
excpt := try(
  try(
    conn execute("CREATE TABLE test_table (id int4, name text)")
  )
  id := 42
  name := "Test entry"
  conn executeTemplate("INSERT INTO test_table (id, name) VALUES (#{id}, #{name})")
  result := conn queryTemplate("SELECT * FROM test_table WHERE id=#{id}")
  result columnNames println
  result rawRows println
  result println
  result first at("name") println
  conn execute("DROP TABLE test_table")
  conn queryTemplate("SELECT now()::date AS result") first at("result") println
)
conn close
excpt catch(PostgreSQL DbError,
  excpt pgsqlMessage print
  excpt pass
) pass
