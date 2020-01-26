#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include "sqlite3.h"


struct timeval  t1, t2, res;
struct timespec cpu_t1, cpu_t2;


static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	for(int i=0; i<argc; ++i) {
		//std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
	}
	return 0;
}


int main(int argc, const char* argv[])
{
	sqlite3    *db = NULL;
	char       *errMessage = NULL;
	std::string query;
	try {
		if(sqlite3_open("test.db", &db) != SQLITE_OK) {
			throw std::runtime_error(std::string("Can't open database: ") + sqlite3_errmsg(db));
		}
		if(sqlite3_exec(db, "DROP TABLE IF EXISTS test", NULL, NULL, &errMessage)  != SQLITE_OK) {
			throw std::runtime_error(errMessage);
		}
		if(sqlite3_exec(db, "CREATE TABLE test(id INTEGER PRIMARY KEY, name VARCHAR(256))", 0, 0, &errMessage) != SQLITE_OK ) {
			throw std::runtime_error(errMessage);
		}
		if(sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS idx_test_name ON test(name)", 0, 0, &errMessage) != SQLITE_OK ) {
			throw std::runtime_error(errMessage);
		}
		query.reserve(4096*128);
		// test 1
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &cpu_t1);
		for(size_t i=0UL; i<1000UL; ++i) {
			query = "INSERT INTO test (id, name) VALUES ";
			for(size_t j=0UL; j<1000UL; ++j) {
				const std::string num = std::to_string(1000 * i + j);
				if(query.back() == ')') {
					query += ",";
				}
				query += "(";
				query += num;
				query += ",'";
				query += "Test ";
				query += num;
				query += "')";
			}
			if(sqlite3_exec(db, query.c_str(), NULL, NULL, &errMessage) != SQLITE_OK ) {
				throw std::runtime_error(errMessage);
			}
		}
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &cpu_t2);
		TIMESPEC_TO_TIMEVAL(&t1, &cpu_t1);
		TIMESPEC_TO_TIMEVAL(&t2, &cpu_t2);
		timersub(&t2, &t1, &res);
		std::cout << "insert: " << res.tv_sec << "sec " << res.tv_usec << "usec" << std::endl;
		// test 2
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &cpu_t1);
		if(sqlite3_exec(db, "SELECT id,name FROM test WHERE name = 'Test 128'", callback, NULL, &errMessage) != SQLITE_OK ) {
			throw std::runtime_error(errMessage);
		}
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &cpu_t2);
		TIMESPEC_TO_TIMEVAL(&t1, &cpu_t1);
		TIMESPEC_TO_TIMEVAL(&t2, &cpu_t2);
		timersub(&t2, &t1, &res);
		std::cout << "find: " << res.tv_sec << "sec " << res.tv_usec << "usec" << std::endl;

	} catch(const std::exception& err) {
		std::cerr << "Error: " << err.what() << std::endl;
		if(errMessage) {
			sqlite3_free(errMessage);
		}
	}
	if(db) {
		sqlite3_close(db);
	}
	return 0;
}
