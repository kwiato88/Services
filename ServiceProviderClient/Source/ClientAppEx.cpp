#include <iostream>

#include "SockSocketUtils.hpp"
#include "ServiceProviderClient.hpp"

enum class Result
{
	InConclusive,
	Success,
	Failure
};

class Indent
{
public:
	Indent() = default;
	Indent(std::size_t p_level) : level(p_level) {}
	std::string operator()() const
	{
		std::string res;
		for(std::size_t i = 0; i < level; ++i)
			res += "  ";
		return res;
	}
	Indent& operator++()
	{
		++level;
		return *this;
	}
	Indent operator++(int)
	{
		Indent tmp = *this;
		++level;
		return tmp;
	}
	Indent& operator--()
	{
		--level;
		return *this;
	}
	Indent operator--(int)
	{
		Indent tmp = *this;
		--level;
		return tmp;
	}
private:
	std::size_t level = 0;
};

class Failure
{
public:
	Failure() = default;
	Failure(std::size_t p_line) : lines{std::string("Line:") + std::to_string(p_line)} {}
	Failure(const std::string& p_failure) : lines{p_failure} {}
	Failure(const std::vector<std::string>& p_failures) : lines{p_failures} {}
	Failure& append(const std::string& p_line)
	{
		lines.push_back(p_line);
		return *this;
	}
	Failure& append(const Failure& p_failure)
	{
		lines.insert(lines.end(), p_failure.lines.begin(), p_failure.lines.end());
		return *this;
	}
	std::string print(Indent indent) const
	{
		std::string res;
		for(const auto& l : lines)
			res += indent() + l + "\n";
		return res;
	}
	template <typename T>
	static std::string toString(const T& val)
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	}
private:
	std::vector<std::string> lines;
};

class MismatchFailure
{
public:
	template <typename T>
	MismatchFailure& expected(const T& exp)
	{
		lines.push_back(std::string("Expected: ") + Failure::toString(exp));
		return *this;
	}
	template <typename T>
	MismatchFailure& actual(const T& act)
	{
		lines.push_back(std::string("Actual:   ") + Failure::toString(act));
		return *this;
	}
	Failure get() const
	{
		return Failure{lines};
	}
private:
	std::vector<std::string> lines;
};

class SameFailure
{
public:
	template <typename T>
	SameFailure& value(const T& val)
	{
		lines.push_back(std::string("Value: ") + Failure::toString(val));
		return *this;
	}
	Failure get() const
	{	
		return Failure{"Expected different values but got the same"}.append(Failure{lines});
	}
private:
	std::vector<std::string> lines;
};

class Test
{
	public:
		Test(const char* p_name) : name(p_name) {}
		virtual ~Test() {}
		std::string getName() const
		{
			return name;
		}
		Result getResult() const
		{
			return res;
		}
		std::string printResult() const
		{
			if (res == Result::Success)
				return "Success";
			else if (res == Result::Failure)
				return "Failure";
			else
				return "InConclusive";
		}
		void setRes(Result p_res)
		{
			if(res == Result::Failure)
				return;
			res = p_res;
		}
		void addFailure(const std::string& p_failure, std::size_t p_line)
		{
			failures.push_back(Failure(p_line).append(p_failure));
			setRes(Result::Failure);
		}
		void addFailure(const Failure& p_failure, std::size_t p_line)
		{
			failures.push_back(Failure(p_line).append(p_failure));
			setRes(Result::Failure);
		}
		void addFailure(const std::string& p_failure)
		{
			failures.push_back(Failure(p_failure));
			setRes(Result::Failure);
		}
		void printFailures() const
		{
			Indent ind{1};
			for(const auto& f : failures)
				std::cout << f.print(ind) << std::endl;
		}
		virtual void test() = 0;
		void run()
		{
			std::cout << "\nRun      " << name << std::endl;
			try
			{
				test();
				setRes(Result::Success);
			}
			catch (std::exception& e)
			{
				addFailure(std::string("Unexpected exception: ") + e.what());
			}
			catch (...)
			{
				addFailure("Unknown exception");
			}
			std::cout << "Finished " << name << "\nStatus   " << printResult() << std::endl;
			printFailures();
		}
	private:
		Result res = Result::InConclusive;
		std::vector<Failure> failures;
		std::string name;
};

class Status
{
public:
	void testRun()
	{
		++numOfTests;
	}
	void testFinished(const std::string& p_name, Result p_res)
	{
		if(p_res == Result::Failure)
			failures.push_back(p_name);
	}
	void summary()
	{
		std::cout << "\n\nSummary\n";
		std::cout << "  Run:  " << numOfTests << std::endl;
		std::cout << "  Fail: " << failures.size() << std::endl;
		for(const auto& f : failures)
			std::cout << "    " << f << std::endl;
	}
private:
	std::size_t numOfTests = 0;
	std::vector<std::string> failures;
};

#define TEST(testName) class Test_##testName : public Test { public: Test_##testName() : Test(#testName) {} void test() override; }; \
	void Test_##testName::test()

#define RUN_TEST(nameName) do { summary.testRun(); Test_##nameName t; t.run(); summary.testFinished(t.getName(), t.getResult()); } while(false)

#define THROWS(expression) \
	try { \
		expression; \
		addFailure(std::string(#expression) + " should throw but didn't", __LINE__); \
	} catch (...) { \
	}

#define IS_EQ(expectedVal, actualVal) do { \
	if(expectedVal != actualVal) \
		addFailure(MismatchFailure{}.expected(expectedVal).actual(actualVal).get(), __LINE__); \
	} while(false)

#define IS_NOT_EQ(expectedVal, actualVal) do { \
	if(expectedVal == actualVal) \
		addFailure(SameFailure{}.value(expectedVal).get(), __LINE__); \
	} while(false)

#define IS_TRUE(expression) do { \
	if(!(expression)) \
		addFailure(std::string(#expression) + " expected to be true. Actual false", __LINE__); \
	} while(false)

#define IS_FALSE(expression) do { \
	if(expression) \
		addFailure(std::string(#expression) + " expected to be false. Actual true", __LINE__); \
	} while(false)

#define MAIN_START Status summary
#define MAIN_END summary.summary()

TEST(DummyPass)
{
	std::cout << "Dummy" << std::endl;
}
TEST(Fail)
{
	IS_EQ(1, 2);
	IS_NOT_EQ(1, 1);
	IS_TRUE(1==2);
	int a=1, b=2;
	IS_FALSE(a != b);
}
TEST(Pass)
{
	IS_EQ(1, 1);
	IS_NOT_EQ(1, 2);
	IS_TRUE(1 == 1);
	int a=1, b=2;
	IS_FALSE(a==b);
}
TEST(FailException)
{
	std::cout << "DummyFException" << std::endl;
	throw std::runtime_error("Dummy");
}
TEST(PassException)
{
	std::cout << "pass1" << std::endl;
	THROWS(throw std::exception());
	std::cout << "pass2" << std::endl;
}

void dum() {}

TEST(FailExpectThrowNoThrow)
{
	std::cout << "fail1" << std::endl;
	THROWS(dum());
	std::cout << "fail2" << std::endl;
}

TEST(getUnregisteredServiceShouldFail)
{
	Networking::ServiceProviderClient provider;
	THROWS(provider.getServiceAddr("MyService"));
}

TEST(shouldAssignDifferentAddrWhenRequestedIsnotAvailable)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService", "127.0.0.1", "1111");
	auto actualAddr = provider.getServiceAddr("MyService");
	IS_NOT_EQ(assigned.host, "");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);

	std::cout << "MyService assigned to [" << assigned.host << ":" << assigned.port << "]" << std::endl;
}

TEST(registerAlreadyRegisteredServiceShouldFail)
{
	Networking::ServiceProviderClient provider;
	THROWS(provider.setServiceAddr("MyService", "127.0.0.1", "50005"));
}

TEST(shouldAssignDifferentAddrWhenRequestedAlreadyAssigned)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService2", "127.0.0.1", "50001");
	auto actualAddr = provider.getServiceAddr("MyService2");
	IS_NOT_EQ(assigned.host, "50001");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(shouldAssignDifferentAddrWhenRequestedAlreadyUsed)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService3", "127.0.0.1", "50000");
	auto actualAddr = provider.getServiceAddr("MyService3");
	IS_NOT_EQ(assigned.host, "50000");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(registerServiceOnFreeAddrShouldSucceed)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService4", "127.0.0.1", "50100");
	auto actualAddr = provider.getServiceAddr("MyService4");
	IS_EQ(assigned.port, "50100");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(registerServiceWithoutPreferredAddrShouldSucceed)
{
	Networking::ServiceProviderClient provider;
	auto assigned = provider.setServiceAddr("MyService5", "", "");
	auto actualAddr = provider.getServiceAddr("MyService5");
	IS_NOT_EQ(assigned.host, "");
	IS_NOT_EQ(assigned.port, "");
	IS_EQ(assigned.host, actualAddr.host);
	IS_EQ(assigned.port, actualAddr.port);
}

TEST(shouldRemoveService)
{
	Networking::ServiceProviderClient provider;
	provider.removeServiceAddr("MyService");
	THROWS(provider.getServiceAddr("MyService"));
}

TEST(removeNotExistingServiceShouldNotThrow)
{
	Networking::ServiceProviderClient provider;
	provider.removeServiceAddr("MyService10");
}

TEST(removeAllServices)
{
	Networking::ServiceProviderClient provider;
	provider.removeServiceAddr("MyService2");
	provider.removeServiceAddr("MyService3");
	provider.removeServiceAddr("MyService4");
	provider.removeServiceAddr("MyService5");
}

TEST(stopProvider)
{
	Networking::ServiceProviderClient provider;
	provider.stop();
}

int main()
{
	
	MAIN_START;

	RUN_TEST(DummyPass);
	RUN_TEST(FailException);
	RUN_TEST(PassException);
	RUN_TEST(FailExpectThrowNoThrow);
	RUN_TEST(Fail);
	RUN_TEST(Pass);

	//TODO: extract tests framework

	sock::init();
	RUN_TEST(getUnregisteredServiceShouldFail);
	RUN_TEST(shouldAssignDifferentAddrWhenRequestedIsnotAvailable);
	RUN_TEST(registerAlreadyRegisteredServiceShouldFail);
	RUN_TEST(shouldAssignDifferentAddrWhenRequestedAlreadyAssigned);
	RUN_TEST(shouldAssignDifferentAddrWhenRequestedAlreadyUsed);
	RUN_TEST(registerServiceOnFreeAddrShouldSucceed);
	RUN_TEST(registerServiceWithoutPreferredAddrShouldSucceed);
	RUN_TEST(shouldRemoveService);
	RUN_TEST(removeNotExistingServiceShouldNotThrow);
	RUN_TEST(removeAllServices);
	RUN_TEST(stopProvider);
	sock::cleanup();

	MAIN_END;
}
