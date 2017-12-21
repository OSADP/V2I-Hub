/*
 * PluginStatus.cpp
 *
 *  Created on: Apr 24, 2017
 *      Author: gmb
 */

#include "TmxControl.h"

#define LIST_QUERY "\
	SELECT IVP.plugin.id, name, description, version, coalesce(enabled, -1), \
	path, exeName, manifestName, \
	maxMessageInterval, commandLineParameters \
	FROM IVP.plugin \
	LEFT JOIN IVP.installedPlugin ON IVP.plugin.id = IVP.installedPlugin.pluginId"

#define STATE_QUERY "\
	SELECT name, `key`, value \
	FROM IVP.pluginStatus \
	LEFT JOIN IVP.plugin ON IVP.pluginStatus.pluginId = IVP.plugin.id"

#define MAX_UPDATE_STMT "\
	UPDATE IVP.installedPlugin \
	SET maxMessageInterval = ?"

#define ARGS_UPDATE_STMT "\
	UPDATE IVP.installedPlugin \
	SET commandLineParameters = ?"

#define MESSAGE_ACTIVITY_QUERY "\
	SELECT IVP.plugin.name, IVP.messageType.type, IVP.messageType.subtype, IVP.messageActivity.id, count, lastReceivedTimestamp, averageInterval \
	FROM IVP.messageActivity \
	INNER JOIN IVP.plugin ON IVP.messageActivity.pluginId = IVP.plugin.id \
	INNER JOIN IVP.messageType ON IVP.messageActivity.messageTypeId = IVP.messageType.id"

#define EVENT_LOG_QUERY "\
	SELECT IVP.eventLog.id, IVP.eventLog.logLevel, IVP.eventLog.source, IVP.eventLog.description, IVP.eventLog.timestamp \
	FROM IVP.eventLog"

#define EVENT_LOG_DELETE "\
	DELETE FROM IVP.eventLog"

#define USER_INFO_QUERY "\
	SELECT IVP.user.id, IVP.user.username, IVP.user.password, IVP.user.accessLevel \
	FROM IVP.user"

#define SYSTEM_CONFIG_QUERY "\
	SELECT IVP.systemConfigurationParameter.key, IVP.systemConfigurationParameter.value, IVP.systemConfigurationParameter.defaultValue \
	FROM IVP.systemConfigurationParameter \
	ORDER BY IVP.systemConfigurationParameter.key"

#define INSTALLED_PLUGINS_QUERY "\
	SELECT IVP.plugin.name, IVP.plugin.version, IVP.plugin.description \
	FROM IVP.plugin \
	ORDER BY IVP.plugin.name"

using namespace std;
using namespace sql;
using namespace tmx;
using namespace tmx::utils;

namespace tmxctl {

bool TmxControl::list(pluginlist &plugins, ...)
{
	string query = add_constraint(LIST_QUERY, plugins, "IVP.plugin.id");

	try
	{
		PLOG(logDEBUG1) << "Executing query " << query;

		_output.get_storage().get_tree().clear();

		DbConnection conn = _pool.Connection();
		unique_ptr<Statement> stmt(conn.Get()->createStatement());
		unique_ptr<ResultSet> rs(stmt->executeQuery(query));

		while (rs->next())
		{
			int id = rs->getInt(1);
			string name = rs->getString(2).asStdString();
			string description = rs->getString(3).asStdString();
			string version = rs->getString(4).asStdString();
			int enabled = rs->getInt(5);
			string path = rs->getString(6).asStdString();
			string exe = rs->getString(7).asStdString();
			string manifest = rs->getString(8).asStdString();
			int maxInt = rs->getInt(9);
			string args = rs->getString(10).asStdString();

			PLOG(logDEBUG) << name << "," << description << "," << version << "," << enabled << "," << maxInt << "," << args;

			message_path_type keyPath(name, ATTRIBUTE_PATH_CHARACTER);
			message_path_type key;
			key = keyPath;
			key /= message_path_type("id", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, to_string(id));

			key = keyPath;
			key /= message_path_type("description", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, description);

			key = keyPath;
			key /= message_path_type("version", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, version);

			key = keyPath;
			key /= message_path_type("enabled", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, !enabled ? "Disabled" : enabled > 0 ? "Enabled" : "External");

			if (enabled < 0)
				continue;

			key = keyPath;
			key /= message_path_type("path", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, path);

			key = keyPath;
			key /= message_path_type("exeName", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, exe);

			key = keyPath;
			key /= message_path_type("manifest", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, manifest);

			key = keyPath;
			key /= message_path_type("maxMessageInterval", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, to_string(maxInt));

			key = keyPath;
			key /= message_path_type("commandLineParameters", ATTRIBUTE_PATH_CHARACTER);
			_output.store(key, args);
		}
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

bool TmxControl::state(pluginlist &plugins, ...)
{
	string query = add_constraint(STATE_QUERY, plugins);
	query += " AND `key` <> ''";
	query += " AND substr(name, 1, 8) <> 'ivpcore.'";

	try
	{
		PLOG(logDEBUG) << "Executing query " << query;

		_output.get_storage().get_tree().clear();
		DbConnection conn = _pool.Connection();
		unique_ptr<Statement> stmt(conn.Get()->createStatement());
		unique_ptr<ResultSet> rs(stmt->executeQuery(query));

		while (rs->next())
		{
			string name = rs->getString(1).asStdString();
			string key = rs->getString(2).asStdString();
			string val = rs->getString(3).asStdString();

			message_path_type keyPath(name, ATTRIBUTE_PATH_CHARACTER);
			keyPath /= message_path_type(key, ATTRIBUTE_PATH_CHARACTER);

			_output.store(keyPath, val);
		}
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

bool TmxControl::max_message_interval(pluginlist &plugins, ...)
{
	if (!checkPerm())
		return false;

	string query = add_constraint(MAX_UPDATE_STMT, plugins);

	try
	{
		uint32_t val = (*_opts)["max-message-interval"].as<uint32_t>();

		PLOG(logDEBUG1) << "Executing query (?1 = " << val << ")" << query;

		DbConnection conn = _pool.Connection();
		unique_ptr<PreparedStatement> stmt(conn.Get()->prepareStatement(query));
		stmt->setUInt(1, val);
		return stmt->execute();
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}
}

bool TmxControl::plugin_log_level(pluginlist &plugins, ...)
{
	string key = "LogLevel";
	string value = (*_opts)["plugin-log-level"].as<string>();

	boost::any k(key);
	boost::any val(value);
	(*_opts).insert(make_pair("key", boost::program_options::variable_value(k, false)));
	(*_opts).insert(make_pair("value", boost::program_options::variable_value(val, false)));

	return this->set(plugins);
}

bool TmxControl::plugin_log_output(pluginlist &plugins, ...)
{
	string key = "LogOutput";
	string value = (*_opts)["plugin-log-output"].as<string>();

	boost::any k(key);
	boost::any val(value);
	(*_opts).insert(make_pair("key", boost::program_options::variable_value(k, false)));
	(*_opts).insert(make_pair("value", boost::program_options::variable_value(val, false)));

	return this->set(plugins);
}

bool TmxControl::args(pluginlist &plugins, ...)
{
	if (!checkPerm())
		return false;

	string query = add_constraint(ARGS_UPDATE_STMT, plugins);
	string val = (*_opts)["args"].as<string>();

	try
	{
		PLOG(logDEBUG1) << "Executing query (?1 = " << val << ")" << query;

		DbConnection conn = _pool.Connection();
		unique_ptr<PreparedStatement> stmt(conn.Get()->prepareStatement(query));
		stmt->setString(1, val);
		return stmt->execute();
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

bool TmxControl::messages(pluginlist &plugins, ...)
{
	string query = add_constraint(MESSAGE_ACTIVITY_QUERY, plugins);
	query += " ORDER BY IVP.plugin.name, IVP.messageType.type, IVP.messageType.subtype, IVP.messageActivity.id";

	try
	{
		PLOG(logDEBUG1) << "Executing query " << query;

		_output.get_storage().get_tree().clear();

		DbConnection conn = _pool.Connection();
		unique_ptr<Statement> stmt(conn.Get()->createStatement());
		unique_ptr<ResultSet> rs(stmt->executeQuery(query));

		message payload;

		while (rs->next())
		{
			string name = rs->getString(1).asStdString();
			string type = rs->getString(2).asStdString();
			string subtype = rs->getString(3).asStdString();
			string id = rs->getString(4);
			string count = rs->getString(5);
			string time = rs->getString(6).asStdString();
			string interval = rs->getString(7);

			//PLOG(logDEBUG) << name << "," << type << "," << subtype << "," << id << "," << count << "," << time  << "," << interval;

			message_tree_type tmpTree;
			tmpTree.put("id", id);
			tmpTree.put("type", type);
			tmpTree.put("subtype", subtype);
			tmpTree.put("count", count);
			tmpTree.put("lastTimestamp", time);
			tmpTree.put("averageInterval", interval);

			message tmpSubTree;
			tmpSubTree.set_contents(tmpTree);

			payload.add_array_element(name, tmpSubTree);
		}
		_output = payload.get_container();
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

bool TmxControl::events(pluginlist &, ...)
{
	string query = EVENT_LOG_QUERY;
	if (_opts->count("eventTime") > 0 && (*_opts)["eventTime"].as<string>() != "")
	{
		query += " WHERE IVP.eventLog.timestamp > '";
		query += (*_opts)["eventTime"].as<string>();
		query += "'";
	}
	query += " ORDER BY IVP.eventLog.timestamp";

	try
	{
		PLOG(logDEBUG1) << "Executing query " << query;

		_output.get_storage().get_tree().clear();

		DbConnection conn = _pool.Connection();
		unique_ptr<Statement> stmt(conn.Get()->createStatement());
		unique_ptr<ResultSet> rs(stmt->executeQuery(query));

		message payload;

		while (rs->next())
		{
			string id = rs->getString(1).asStdString();
			string level = rs->getString(2).asStdString();
			string source = rs->getString(3).asStdString();
			string description = rs->getString(4);
			string timestamp = rs->getString(5);

			message_tree_type tmpTree;
			tmpTree.put("id", id);
			tmpTree.put("level", level);
			tmpTree.put("source", source);
			tmpTree.put("description", description);
			tmpTree.put("timestamp", timestamp);

			message tmpSubTree;
			tmpSubTree.set_contents(tmpTree);

			payload.add_array_element("EventLog", tmpSubTree);
		}
		_output = payload.get_container();
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

bool TmxControl::system_config(pluginlist &, ...)
{
	string query = SYSTEM_CONFIG_QUERY;

	try
	{
		PLOG(logDEBUG1) << "Executing query " << query;

		_output.get_storage().get_tree().clear();

		DbConnection conn = _pool.Connection();
		unique_ptr<Statement> stmt(conn.Get()->createStatement());
		unique_ptr<ResultSet> rs(stmt->executeQuery(query));

		message payload;

		while (rs->next())
		{
			string key = rs->getString(1);
			string value = rs->getString(2);
			string defaultValue = rs->getString(3);

			message_tree_type tmpTree;
			tmpTree.put("name", key);
			tmpTree.put("value", value);
			tmpTree.put("defaultValue", defaultValue);

			message tmpSubTree;
			tmpSubTree.set_contents(tmpTree);

			payload.add_array_element("SystemConfig", tmpSubTree);
		}
		_output = payload.get_container();
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

bool TmxControl::clear_event_log(pluginlist &, ...)
{
	string query = EVENT_LOG_DELETE;
	try
	{
		PLOG(logDEBUG1) << "Executing query " << query;

		_output.get_storage().get_tree().clear();

		DbConnection conn = _pool.Connection();
		unique_ptr<PreparedStatement> stmt(conn.Get()->prepareStatement(query));
		stmt->executeUpdate();
		//unique_ptr<Statement> stmt(conn.Get()->createStatement());
		//unique_ptr<ResultSet> rs(stmt->executeQuery(query));
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

bool TmxControl::user_info()
{
	string query = USER_INFO_QUERY;
	if (_opts->count("username") == 0 || (*_opts)["username"].as<string>() == "")
		return false;
	query += " WHERE IVP.user.username = '";
	query += (*_opts)["username"].as<string>();
	query += "'";

	try
	{
		PLOG(logDEBUG1) << "Executing query " << query;

		_output.get_storage().get_tree().clear();

		DbConnection conn = _pool.Connection();
		unique_ptr<Statement> stmt(conn.Get()->createStatement());
		unique_ptr<ResultSet> rs(stmt->executeQuery(query));

		message payload;

		while (rs->next())
		{
			string id = rs->getString(1).asStdString();
			string username = rs->getString(2);
			string password = rs->getString(3);
			string accessLevel = rs->getString(4).asStdString();

			message_tree_type tmpTree;
			tmpTree.put("id", id);
			tmpTree.put("username", username);
			tmpTree.put("password", password);
			tmpTree.put("accessLevel", accessLevel);

			message tmpSubTree;
			tmpSubTree.set_contents(tmpTree);

			payload.add_array_element("UserInfo", tmpSubTree);
		}
		_output = payload.get_container();
	}
	catch (exception &ex)
	{
		PLOG(logERROR) << TmxException(ex);
		return false;
	}

	return true;
}

} /* namespace tmxctl */

