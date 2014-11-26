#include "Poco/Exception.h"
#include "Poco/RegularExpression.h"
#include "Poco/StreamCopier.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPCookie.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>

using Poco::RegularExpression;
using Poco::RegularExpressionException;
using Poco::StreamCopier;
using Poco::SharedPtr;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPSClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPCookie;
using Poco::Net::HTMLForm;
using Poco::Net::SSLManager;
using Poco::Net::Context;
using Poco::Net::ConsoleCertificateHandler;
using Poco::Net::InvalidCertificateHandler;
using Poco::Net::NameValueCollection;
using Poco::Util::Application;
using Poco::Util::OptionSet;
using Poco::Util::Option;
using Poco::Util::HelpFormatter;



class HTMLRegex : public Poco::Util::Application
{

public:
	HTMLRegex() : _helpRequested(false)
	{

	}

	~HTMLRegex()
	{

	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration();
		Application::initialize(self);
	}

	void uninitialize()
	{
		Application::uninitialize();
	}

	void defineOptions(OptionSet& options)
	{
		options.addOption(
			Option("help", "h", "display help")
			.required(false)
			.repeatable(false)
			);
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		Application::handleOption(name, value);

		if (name == "help")
			_helpRequested = true;
	}

	void displayHelp()
	{
		HelpFormatter helpformatter(options());
		helpformatter.setCommand(commandName());
		helpformatter.setUsage("OPTIONS");
		helpformatter.setHeader("An application that talks to the library");
		helpformatter.format(std::cout);
	}

	void getPage()
	{
		HTTPClientSession session("www.google.co.uk");
		HTTPRequest request(HTTPRequest::HTTP_GET, "/?gfe_rd=cr&amp;ei=XvtsVM3dBMjH8gfox4GgDw", HTTPMessage::HTTP_1_1);
		HTTPResponse response;

		session.sendRequest(request);
		std::istream& rs = session.receiveResponse(response);

		int statusCode = response.getStatus();

		poco_information_f1(logger(), "Status %d", statusCode);

		std::vector<HTTPCookie> cookies;
		response.getCookies(cookies);
		for (HTTPCookie cookie : cookies)
		{
			poco_information_f1(logger(), "Cookie %s", cookie.toString());
		}

		//StreamCopier::copyStream(rs, std::cout);
	}

	void getLoans()
	{
		HTTPSClientSession session("capitadiscovery.co.uk");
		//session.setProxy("localhost", 8888);

		NameValueCollection cookies;

		fetchLoginPage(session, cookies);
		submitLoginPage(session, cookies);
		fetchAccountPage(session, cookies);
	}

	void fetchLoginPage(HTTPSClientSession& clientSession, NameValueCollection& cookies)
	{
		HTTPRequest request(HTTPRequest::HTTP_GET, "/royalgreenwich/login?message=borrowerservices_notloggedin&referer=https%3A%2F%2Fcapitadiscovery.co.uk%2Froyalgreenwich%2Faccount", HTTPMessage::HTTP_1_1);
		request.setCookies(cookies);
		HTTPResponse response;

		std::ostream& ostr = clientSession.sendRequest(request);
		std::istream& rs = clientSession.receiveResponse(response);

		int statusCode = response.getStatus();

		poco_information_f1(logger(), "Status %d", statusCode);

		std::vector<HTTPCookie> newCookies;
		response.getCookies(newCookies);
		for (HTTPCookie cookie : newCookies)
		{
			poco_information_f1(logger(), "Cookie %s", cookie.toString());
			if (cookies.has(cookie.getName()))
			{
				cookies.set(cookie.getName(), cookie.getValue());
			}
			else
			{
				cookies.add(cookie.getName(), cookie.getValue());
			}
		}
	}

	void submitLoginPage(HTTPSClientSession& clientSession, NameValueCollection& cookies)
	{
		HTTPRequest request(HTTPRequest::HTTP_POST, "/royalgreenwich/sessions", HTTPMessage::HTTP_1_1);
		request.setCookies(cookies);
		HTTPResponse response;
		HTMLForm loginForm;
		loginForm.add("barcode", "28028005913354");
		loginForm.add("pin", "3347");
		loginForm.prepareSubmit(request);

		std::ostream& ostr = clientSession.sendRequest(request);
		loginForm.write(ostr);
		std::istream& rs = clientSession.receiveResponse(response);

		int statusCode = response.getStatus();

		poco_information_f1(logger(), "Status %d", statusCode);

		std::vector<HTTPCookie> newCookies;
		response.getCookies(newCookies);
		for (HTTPCookie cookie : newCookies)
		{
			poco_information_f1(logger(), "Cookie %s", cookie.toString());
			if (cookies.has(cookie.getName()))
			{
				cookies.set(cookie.getName(), cookie.getValue());
			}
			else
			{
				cookies.add(cookie.getName(), cookie.getValue());
			}
		}
	}

	void fetchAccountPage(HTTPSClientSession& clientSession, NameValueCollection& cookies)
	{
		HTTPRequest request(HTTPRequest::HTTP_GET, "/royalgreenwich/account", HTTPMessage::HTTP_1_1);
		request.setCookies(cookies);
		HTTPResponse response;

		std::ostream& ostr = clientSession.sendRequest(request);
		std::istream& rs = clientSession.receiveResponse(response);

		int statusCode = response.getStatus();

		poco_information_f1(logger(), "Status %d", statusCode);

		std::vector<HTTPCookie> newCookies;
		response.getCookies(newCookies);
		for (HTTPCookie cookie : newCookies)
		{
			poco_information_f1(logger(), "Cookie %s", cookie.toString());
			if (cookies.has(cookie.getName()))
			{
				cookies.set(cookie.getName(), cookie.getValue());
			}
			else
			{
				cookies.add(cookie.getName(), cookie.getValue());
			}
		}
		StreamCopier::copyStream(rs, std::cout);
	}

	int main(const std::vector<std::string>& args)
	{
		SharedPtr<InvalidCertificateHandler> ptrCert = new ConsoleCertificateHandler(false);
		Context::Ptr ptrContext = new Context(Context::CLIENT_USE, "", "", "rootcert.pem", Context::VERIFY_RELAXED, 9);
		SSLManager::instance().initializeClient(0, ptrCert, ptrContext);
		logger().information("Library app start");

		if (_helpRequested)
		{			displayHelp();
		}
		else
		{
			getLoans();
		}

		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};

void ExtractData(std::string& tableRow)
{
	RegularExpression regex(R"(<img.*>\s*(\w+.*\w+)\s*</a>.*<span.*>(.*)</span>.*<td.*>(.*)</td>)", 
		RegularExpression::RE_MULTILINE | RegularExpression::RE_DOTALL | RegularExpression::RE_UNGREEDY);
	//RegularExpression regex(R"(<img.*>(.*)</a>)", RegularExpression::RE_MULTILINE | RegularExpression::RE_DOTALL);
	std::vector<std::string> strings;

	regex.split(tableRow, strings);

	for (int i = 1; i < strings.size(); i++)
	{
		std::cout << strings[i] << std::endl;
	}
}

int main(int argc, char** argv)
{
	// parse an HTML document and print the generated tags

	if (argc < 2)
	{
		std::cout << "usage: " << argv[0] << ": <htmlfile>" << std::endl;
		return 1;
	}

	std::ifstream infile(argv[1], std::ios_base::in);
	std::string fileContents((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

	//std::cout << fileContents;
	std::string matchText;
	int numMatches = 0;
	RegularExpression::Match match;
	std::string::size_type htmlOffset = 0;

	try
	{
		std::string reg = R"(<tr.*tr>)";
		//std::string reg = R"(</?\w+((\s+\w+(\s*=\s*(?:".*?"|'.*?'|[^'">\s]+))?)+\s*|\s*)/?>)";
		//std::string reg = R"(</?\w+>)";
		RegularExpression regex(reg, RegularExpression::RE_EXTENDED | RegularExpression::RE_MULTILINE | RegularExpression::RE_DOTALL | RegularExpression::RE_UNGREEDY);

		while (regex.match(fileContents, htmlOffset, match))
		{
			htmlOffset = match.offset + match.length;
			std::cout << "*****************************************************************************" << std::endl;
			std::string matchString(fileContents, match.offset, match.length);
			//std::cout << matchString << std::endl;

			ExtractData(matchString);
		}
		std::cout << "*****************************************************************************" << std::endl;

	}
	catch (RegularExpressionException& ree)
	{
		std::cout << ree.message();
	}

	return 0;
}

