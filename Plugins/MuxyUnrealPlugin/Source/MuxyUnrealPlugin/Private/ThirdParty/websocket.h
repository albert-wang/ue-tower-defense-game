#pragma once
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

struct WebsocketConnectionImpl;

class WebsocketConnection
{
public:
	WebsocketConnection(const std::string& host, uint16_t port);
	~WebsocketConnection();

	// Noncopyable
	WebsocketConnection(const WebsocketConnection&) = delete;
	WebsocketConnection& operator=(const WebsocketConnection&) = delete;
	WebsocketConnection(WebsocketConnection&&) = delete;
	WebsocketConnection& operator=(const WebsocketConnection&&) = delete;

	// This function will block
	void send(const nlohmann::json& json);

	// This function never returns.
	void run();

	void onMessage(std::function<void(nlohmann::json)> cb);
private:
	void read();

	WebsocketConnectionImpl* impl;
};