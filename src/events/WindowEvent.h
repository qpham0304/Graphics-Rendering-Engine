#pragma once

#include "./Event.h"

class WindowEvent : public Event
{
	virtual EventType GetEventType() const override;
	virtual const char* GetName() const override;
	virtual std::string ToString() const override;
};