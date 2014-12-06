#include "stdafx.h"
#include "BoostLog.h"
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/empty_deleter.hpp>

void LogInit()
{
	boost::shared_ptr< logging::core > core = logging::core::get();
	logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
	logging::core::get()->add_global_attribute("RecordID", attrs::counter< unsigned int >());
	logging::core::get()->set_filter
		(
		logging::trivial::severity >= logging::trivial::debug
		);

	boost::shared_ptr< sinks::text_file_backend > backend =
		boost::make_shared< sinks::text_file_backend >(
		keywords::file_name = "pscada_%5N.log",                                          
		keywords::rotation_size = 5 * 1024 * 1024,   
		keywords::open_mode = std::ios::app,
		keywords::format = "[%timestamp%]: %message%"
		);
	backend->auto_flush(true);

	// Wrap it into the frontend and register in the core.
	// The backend requires synchronization in the frontend.
	typedef sinks::synchronous_sink< sinks::text_file_backend > sink_t;
	boost::shared_ptr< sink_t > sink(new sink_t(backend));
	sink->set_formatter
		(
		/*expr::format("<record id=\"%1%\" timestamp=\"%2%\">%3%</record>")
		% expr::attr< unsigned int >("RecordID")
		%logging::trivial::severity
		% expr::attr< boost::posix_time::ptime >("TimeStamp")
		% expr::xml_decor[ expr::stream << expr::smessage ] */           
		expr::stream
		<< expr::attr< boost::posix_time::ptime >("TimeStamp")
		<< ": <" << logging::trivial::severity
		<< "> " << expr::smessage
		);
		typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
		boost::shared_ptr< text_sink > sink_console = boost::make_shared< text_sink >();

	boost::shared_ptr< std::ostream > stream(&std::clog, logging::empty_deleter());
	sink_console->locked_backend()->add_stream(stream);
	sink_console->set_formatter
		(
		expr::stream
		<< expr::attr< boost::posix_time::ptime >("TimeStamp")
		<< ": <" << logging::trivial::severity
		<< "> " << expr::smessage
		);

	core->add_sink(sink);
	core->add_sink(sink_console);
}